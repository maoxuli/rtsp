// **********************************************************************
// 
// Copyright (c) 2010, The PPEngine project authors.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions 
// are met:
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

#include "TcpClient.h"

namespace pputil 
{
    
    TcpClient::TcpClient(ReceiveCallback* receiveCallback)
    : TcpConnection(receiveCallback)
    {

    }
    
    TcpClient::~TcpClient()
    {

    }
    
    bool TcpClient::connect(const std::string& host, unsigned short port)
    {
        if(_fd == INVALID_SOCKET)
        {
            try
            {
                _fd = createTcpSocket();
            }
            catch(SocketException& ex)
            {
                std::cout << ex.toString() << "\n";
                return false;
            }
        }
        assert(_fd != INVALID_SOCKET);
        
        sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
		addr.sin_addr.s_addr = inet_addr(host.c_str());
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        
        // Try to connect to server
        try
        {
            if(!doConnect(_fd, addr))
            {
                return false;
            }
        }
        catch(SocketException& ex)
        {
            std::cout << ex.toString() << "\n";
            return false;
        }
        
        // Start receiving
        receive();
        
        std::cout << "TcpClient::connect() OK.\n";
        return true;
    }

}
