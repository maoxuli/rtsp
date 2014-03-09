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

#include "RtspStream.h"

namespace rtsp
{

    RtspStream::RtspStream(const std::string& name)
    : m_name(name)
    , m_seq(0)
    {

    }

    RtspStream::~RtspStream()
    {

    }

    std::string RtspStream::name()
    {
        return m_name;
    }

    unsigned int RtspStream::seq(bool update)
    { 
        return update ? m_seq++ : m_seq;
    }
    
    /////////////////////////////////////////////////////////////////////

    RtpStream::RtpStream(const std::string& name)
    : RtspStream(name)
    {

    }

    RtpStream::~RtpStream()
    {
        m_rtpSocket.close();
        m_rtcpSocket.close();
    }

    bool RtpStream::init(unsigned short& serverPort, unsigned short clientPort)
    {
        // Try to do until available ports
        for(int i=0; i<10; i++)
        {
            if(m_rtpSocket.init(serverPort))
            {
                if(m_rtcpSocket.init(serverPort + 1))
                {
                    break;
                }
                else
                {
                    m_rtpSocket.close();
                }
            }

            serverPort += 2;
        }

        m_rtpSocket.setPeer("127.0.0.1", clientPort);
        m_rtcpSocket.setPeer("127.0.0.1", clientPort + 1);

        return true;
    }

    bool RtpStream::sendData(byte* b, size_t n)
    {
        int len = m_rtpSocket.send(b, n);
        return len == n;
    }
    
    //////////////////////////////////////////////////////////////////////

    TcpStream::TcpStream(const std::string& name)
    : RtspStream(name)
    , m_connection(NULL)
    {

    }

    TcpStream::~TcpStream()
    {
        m_connection = NULL;
    }

    bool TcpStream::init(RtspConnection* conn)
    {
        m_connection = conn;
        return true;
    }

    bool TcpStream::sendData(byte* b, size_t n)
    {
        if(m_connection != NULL)
        {
            m_connection->sendData(b, n);
            return true;
        }

        return false;
    }

}
