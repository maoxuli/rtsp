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

#include "UdpSocket.h"

namespace rtsp
{

    UdpSocket::UdpSocket()
    : m_socket(INVALID_SOCKET)
    {
    }

    UdpSocket::~UdpSocket()
    {
    }

    bool UdpSocket::init(unsigned short port)
    {
        assert(m_socket == INVALID_SOCKET );
        
        try
        {
            m_socket = pputil::createUdpSocket();

            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(port);
            addr.sin_addr.s_addr = INADDR_ANY;

            pputil::doBind(m_socket, addr);
        }
        catch(pputil::SocketException& ex)
        {
            close();
            return false;
        }

        return true;
    }

    void UdpSocket::close()
    {
        if(m_socket != INVALID_SOCKET)
        {
            pputil::closeSocket(m_socket);
            m_socket = INVALID_SOCKET;
        }
    }

    void UdpSocket::setPeer(const std::string& host, unsigned short port)
    {
        memset(&m_peer, 0, sizeof(m_peer));
        m_peer.sin_family = AF_INET;
        m_peer.sin_port = htons(port);
        m_peer.sin_addr.s_addr = inet_addr(host.c_str());
    }

    long UdpSocket::send(unsigned char* b, size_t n)
    {
        return sendto(m_socket, (const char*)b, n, 0, (sockaddr*)&m_peer, sizeof(m_peer));
    }
    
}

