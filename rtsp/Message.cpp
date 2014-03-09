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

#include "RtspMessage.h"

namespace rtsp
{

    MessageHeader::MessageHeader(const std::string& key)
    : _key(key)
    {

    }

    MessageHeader::MessageHeader(const std::string& key, const std::string& value)
    : _key(key)
    , _value(value)
    {

    }

    std::string MessageHeader::key() const
    {
        return _key;
    }

    std::string MessageHeader::value() const
    {
        return _value;
    }

    void MessageHeader::setValue(const std::string& value)
    {
       _value = value;
    }

    /////////////////////////////////////////////////////////////////////////

    Message::Message() 
    {
        _protocol = "HTTP";
        _version = "1.0";
        _bodyLen = 0;
        _body = NULL;
    }

    Message::Message(const std::string& version)
    {
        _protocol = "HTTP";
        _version = version;
        _bodyLen = 0;
        _body = NULL;
    }

    Message::~Message()
    {
        for(MessageHeaderSeq::iterator it = _headers.begin(); it != _headers.end(); ++it)
        {
            MessageHeader* pHeader = *it;
            if(pHeader != NULL)
            {
                delete pHeader;
            }
        }
        _headers.clear();

        if(_body != NULL)
        {
            delete[] _body; 
            _body = NULL;
            _bodyLen = 0;
        }
    }

    MESSAGE_TYPE Message::type() const
    {
        return UNKNOWN_MESSAGE;
    }

    std::string Message::version() const
    {
        return _version;
    }

    void Message::setVersion(const std::string& version)
    {
        _version = version;
    }

    std::string Message::header(const std::string& key) const
    {
        std::string value;
        MessageHeaderSeq::const_iterator it(_headers.begin());
        while(it != _headers.end())
        {
            MessageHeader* pHeader = *it;
            if(key == pHeader->key())
            {
                value = pHeader->value();
                break;
            }
            ++it;
        }

        return value;
    }

    void Message::setHeader(const std::string& key, const std::string& value)
    {
        MessageHeaderSeq::iterator it(_headers.begin());
        while(it != _headers.end())
        {
            MessageHeader* pHeader = *it;
            if(key == pHeader->key())
            {
                pHeader->setValue(value);
                return;
            }
            ++it;
        }

        _headers.push_back(new MessageHeader(key, value));
    }

    void Message::setHeader(const MessageHeader& header)
    {
        MessageHeaderSeq::iterator it(_headers.begin());
        while(it != _headers.end())
        {
            MessageHeader* pHeader = *it;
            if(header.key() == pHeader->key())
            {
                pHeader->setValue(header.value());
                return;
            }
            ++it;
        }

        _headers.push_back(new MessageHeader(header));
    }

    void Message::removeHeader(const std::string&key) 
    {
        MessageHeaderSeq::iterator it(_headers.begin());
        while(it != _headers.end())
        {
            MessageHeader* pHeader = *it;
            if(key == pHeader->key())
            {
                it = _headers.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    size_t Message::headerLen() const
    {
        size_t nLen = 0;
        MessageHeaderSeq::const_iterator it(_headers.begin());
        while(it != _headers.end())
        {
            MessageHeader* pHeader = *it;
            if(pHeader != NULL)
            {
                nLen += (pHeader->key().size() + 2 + pHeader->value().size() + 2);
            }
            ++it;
        }

        return nLen;
    }

    size_t Message::headerCount() const
    {
        return _headers.size();
    }

    MessageHeader* Message::header(size_t index) const
    {
        if(index >= _headers.size())
        {
            return NULL;
        }

        return _headers.at(index);
    }

    size_t Message::bodyLen() const
    {
        return _bodyLen;
    }

    byte* Message::body() const
    {
        return _body;
    }

    void Message::setBody(byte* buf, size_t len)
    {
        if(_body != NULL)
        {
            delete[] _body; 
            _body = NULL;
        }

        _bodyLen = len;
        if(_bodyLen > 0)
        {
            _body = new byte[m_bodyLen];
            memcpy(_body, buf, _bodyLen);
        }
    }

    //////////////////////////////////////////////////////////////////////////

    RequestMessage::RequestMessage() 
    : Message()
    {
        _method = "GET";
    }

    RequestMessage::RequestMessage(const std::string& version) 
    : Message(version)
    {
        _method = "GET";
    }

    RequestMessage::~RequestMessage()
    {

    }

    MESSAGE_TYPE RequestMessage::type() const
    {
        return MESSAGE_REQUEST;
    }

    std::string RequestMessage::dump() const
    {
        // <verb> SP <url> SP <protocol/version> CRLF
        // <headers> CRLF 
        // <buf>
        std::ostringstream oss;

        oss << _method << " " << _url << " " << _protocol << "/" << _version << "\r\n";

        for(size_t i = 0; i < headerCount(); i++)
        {
            MessageHeader* pHeader = header(i);
            oss << pHeader->key() << ":" << pHeader->value() << "\r\n";
        }

        oss << "\r\n";

        oss << "Body (" << _bodyLen << "bytes)\r\n";

        return oss.str();
    }

    Buffer* RequestMessage::toBuffer() const
    {
        return NULL;
    }

    std::string RequestMessage::method() const
    {
        return _method;
    }

    void RequestMessage::setMehtod(const std::string& method)
    {
        _mehtod =  method;
    }

    std::string RequestMessage::url() const
    {
        return _url;
    }

    void RequestMessage::setUrl(const std::string& url)
    {
        _url = url;
    }

    //////////////////////////////////////////////////////////////////////////////////

    ResponseMessage::ResponseMessage() 
    : Message() 
    {
        _code = 0;
    }

    ResponseMessage::ResponseMessage(const std::string& version) 
    : Message(version)
    {
        _code = 0;
    }

    ResponseMessage::~ResponseMessage()
    {

    }

    MessageType ResponseMessage::type() const
    {
        return RESPONSE_MESSAGE;
    }

    std::string ResponseMessage::dump() const
    {
        // <protocol>/<version> code message CRLF
        // <headers> CRLF 
        // <buf>
        std::ostringstream oss;

        oss << _protocol << "/" << _version << " " << _code << " " << _reason << "\r\n";

        for(size_t i = 0; i < headerCount(); i++)
        {
            MessageHeader* pHeader = header(i);
            oss << pHeader->key() << ":" << pHeader->value() << "\r\n";
        }

        oss << "\r\n";

        oss << "Body (" << _bodyLen << "bytes)" << "\r\n";

        return oss.str();
    }

    Buffer* ResponseMessage::toBuffer()
    {
        unsigned int nCode = msg->code();
        std::string sReason = msg->message();
        size_t nHdrLen = msg->headerLen();
        size_t nBufLen = msg->bufferLen();
        
        // "RTSP/1.0" SP <code> SP <reason> CRLF
        // <headers> CRLF
        // <buf> (or terminating NULL from sprintf() if no buffer)
        size_t size = 8 + 1 + 3 + 1 + sReason.length() + 2 + nHdrLen + 2;
        size += nBufLen != 0 ? nBufLen : 1;
        
        char* buf = new char[size];
        assert(buf != NULL);
        if(buf == NULL)
        {
            std::cout << "RTSPServer send response out of memory.\n";
            return false;
        }
        
        char* p = buf;
        p += sprintf(p, "RTSP/1.0 %u %s\r\n", nCode, sReason.c_str());
        
        for(UINT n = 0; n < msg->headerCount(); n++ )
        {
            RTSPHeader* pHeader = msg->header(n);
            p += sprintf(p, "%s: %s\r\n", pHeader->key().c_str(), pHeader->value().c_str());
        }
        p += sprintf(p, "\r\n");
        
        if( nBufLen )
        {
            memcpy(p, msg->buffer(), nBufLen );
            p += nBufLen;
        }
        
        m_pOutput->Add(buf, p - buf);
        OnWrite();
        delete[] buf;

        return NULL;
    }

    int ResponseMessage::code() const
    {
        return _code;
    }

    std::string ResponseMessage::reason() const
    {
        return _reason;
    }

    void ResponseMessage::setStatus(int code, const std::string& reason)
    {
        _code = code;
        _reason = reason;

        if(_reason.empty())
        {
            _reason = code2reason(_code);
        }
    }

}