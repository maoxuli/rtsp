// **********************************************************************
//
// Copyright (c) 2011, PPEngine
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
// 
//   * Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in
//     the documentation and/or other materials provided with the
//     distribution.
// 
//   * Neither the name of Google nor the names of its contributors may
//     be used to endorse or promote products derived from this software
//     without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// **********************************************************************

#include "RtspConnection.h"
#include "RtspServer.h"

namespace rtsp
{

    RtspConnection::RtspConnection(pputil::SOCKET fd, RtspServer* server)
    : TcpConnection(fd, NULL)
    , _server(server)
    , _state(RMS_READY)
    , _message(NULL)
    , _bodyLen(0)
    {
        assert(_server != NULL);
    }

    RtspConnection::~RtspConnection()
    {
        if(_message != NULL)
        {
            delete _message;
            _message = NULL;
        }
    }
    
    // Send interleaved data
    bool RtspConnection::sendData(byte* b, size_t n)
    {
        Buffer b;
        b.write8u('$');
        b.write8u('$');
        b.write16u(n);
        b.writeBlob(b, n);
        
        return send(&b);
    }

    // Send a response message
    bool RtspConnection::sendResponse(RtspResponse* msg)
    {
        assert(msg != NULL);

    #ifdef _DEBUG
        std::cout << ">>> " << msg->dump();
    #endif
        
        Buffer b;
        msg->toBuffer(&b);
        
        return send(&b);
    }

    // Called when data is received and appended to _inBuffer
    // Parse RtspRequest, forward to RtspServer to handle
    void RtspConnection::onReceive()
    {
        switch(_state) 
        {
            case RMS_READY:
                readType();
                break;
            case RMS_READ_DATA:
                readData();
                break;
            case RMS_READ_INITIAL:
                readInitial();
                break;
            case RMS_READ_HEADER:
                readHeader();
                break;
            case RMS_READ_BODY:
                readBody();
                break;
            default:
                break;
        }
        
        if(_state == RMS_READ_OK)
        {
            onMessage();
        }
    }
      
    // First two bytes marks the type of data
    // $$ leads to a raw data packet (2 bytes dataLen + data)
    // otherwise RTSP message packet
    void RtspConnection::readType()
    {
        assert(_state == RMS_READY);
        assert(_message == NULL);
        assert(_dataLen == 0);
        
        if(_inBuffer->size() >= 1)
        {
            m_state = (_inBuffer->peek8u() == '$') ? RMS_READ_DATA : RMS_READ_INITIAL;
        }
    }

    // Read raw data packet  
    void RtspConnection::readData()
    {
        assert(_state == RMS_READ_DATA);
        assert(_inBuffer->peek8u() == '$');
        assert(_message == NULL);
        assert(_dataLen == 0);
       
        // Check data length and availability in buffer
        if(_inBuffer->size() < 4)
        {
           return;
        }

        size_t dataLen = _inBuffer->peek16u(2);
        assert(dataLen > 0);

        if(_inBuffer->size() >= dataLen + 4)
        {
           // Read data
           _inBuffer.remove(4);

           Buffer dataBuffer;
           dataBuffer.writeBlob(_inBuffer->read_pos(), dataLen);
           onData(&dataBuffer);

           _inBuffer->remove(dataLen);
           
           _state = RMS_READY;
        }
    }

    // Read initial line of RTSP message
    // Request: <verb> <url> RTSP/1.0
    // Response: RTSP/1.0 <code> <reason>
    void RtspConnection::ReadInitial()
    {
        assert(_state == RMS_READ_INITIAL);
        assert(_message == NULL);
        assert(_bodyLen == 0);
        
        try
        {
            // Read a line
            std::string initialLine = _inBuffer->readLine();
            assert(!initialLine.empty());

            // Split sring with space
            std::istringstream iss(initialLine);
            std::string sVerb;
            iss >> sVerb;

            if(sVerb.substr(0,4) == "RTSP")
            {
                // Response: RTSP/#.# <code> <reason>
                std::string sCode;
                iss >> sCode;

                RtspResponse* pmsg = new RtspResponse();
                pmsg->setStatus(atoi(sCode.c_str()));
                _message = pmsg;
            }
            else
            {
                // Request: <verb> <url> RTSP/#.#
                std::string sUrl;
                iss >> sUrl;

                RtspRequest* pmsg = new RtspRequest();
                pmsg->setMethod(sVerb);
                pmsg->setUrl( sUrl );
                m_message = pmsg;
            }
            
            _state = RMS_READ_HEADER;
        }
        catch(OutofBoundException& ex)
        {
            // There is no complete line
        }
    }

    // Read header lines
    // Read each complete line, until a blank line
    bool RtspConnection::readHeader()
    {
        assert(_state == RTSP_READ_HEADER);
        assert(_message != NULL);
        assert(_bodyLen == 0);

        // Read all complete lines
        while(true)
        {
            try
            {
                std::string line = buffer->readLine();
                
                if(line.empty())
                {
                    // Separator line of headers and body
                    std::string sBodyLen = _message->header("Content-Length");
                    _bodyLen = atoi(sBodyLen.c_str());
                    _state = _bodyLen > 0 ? RMS_READ_BODY : RMS_READ_OK;
                    break;
                }
                else if(line.size() > 512)
                {
                   // Line is too long
                   std::cout << "RtspConnection::readHeader() too long line.\n";
                }
                else
                {
                    // Key and value
                    size_t pos = line.find(":");
                    assert(pos != std::string::npos);
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    _message->setHeader(key, value);
                }
            }
            catch(OutOfBoundsException& ex)
            {
                // There is no complete line
                break;
            }
        }
    }

    void RtspConnection::readBody()
    {
        assert(_state == RMS_READ_BODY);
        assert(_message != NULL);
        assert(_bodyLen > 0);
        
        if(_inBuffer.size() >= _bodyLen)
        {
            _message->setBody(_inBuffer->read_pos(), _bodyLen);
            _inBuffer->remove(_bodyLen);
            
            _state = RMS_READ_OK;
        }
    }

    // Called when a RTSP message is parsed
    // Forward to RtspServer
    void RtspConnection::onMessage()
    {
        assert(_state = RMS_READ_OK);
        assert(_message != NULL);
        
    #ifdef _DEBUG
        std::cout << "<<< " << _message->dump(); 
    #endif
        
        switch(_message->type())
        {
            case REQUEST_MESSAGE:
            {
                assert(_server != NULL);
                RtspRequest* pmsg = dynamic_cast<RtspRequest*>(_message);
                _server->onRequest(pmsg, this);
                break;
            }
            case RESPONSE_MESSAGE:
            {
                RtspResponse* pmsg = dynamic_cast<RtspResponse*>(_message);
                onResponse(pmsg);
                break;
            }
            default:
                assert(false);
                break;
        }

        delete _message; 
        _message = NULL;
        _bodyLen = 0;
        _state = RMS_READY;
    }

}