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

#ifndef PPUTIL_TCP_SERVER_H
#define PPUTIL_TCP_SERVER_H

#include <pputil/Config.h>
#include <pputil/Server.h>
#include <pputil/TcpConnection.h>
#include <pputil/Socket.h>

namespace pputil
{
    //
    // Start TCP listener
    // Accept connections, start its receiving, call back to user
    // Manage connections as a list, monitor and close zombie connections
    // Driven by internal thread or external thread
    //
    
    class PPUTIL_API TcpServer : public Server
    {
    public:
        TcpServer(unsigned short port, bool passive = true);
        TcpServer(unsigned short port, ReceiveCallback* receiveCallback, 
                  ConnectCallback* connectCallback = NULL, bool passive = true);
        virtual ~TcpServer();
        
        unsigned short port();
        
    protected:
        // Override to Server
        virtual bool doActivate();
        virtual void doShutdown();
        virtual bool doRun();
        
    protected:
        // TCP port and listener
        unsigned short _port;
        int _backlog;
        SOCKET _fd;
        
        // Callback
        ReceiveCallback* _receiveCallback;
        ConnectCallback* _connectCallback;
        
        // Selector
        fd_set _fds;
        struct timeval _timeout;
        
        // Create a connection
        virtual TcpConnection* createConnection(SOCKET fd);
        
        // TCP connections
        std::vector<TcpConnection*> _connections;
    };
}

#endif