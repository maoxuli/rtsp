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

#include "TcpServer.h"

namespace pputil
{
    
    TcpServer::TcpServer(unsigned short port, bool passive)
    : Server(passive)
    , _port(port)
    , _backlog(SOMAXCONN)
    , _fd(INVALID_SOCKET)
    , _receiveCallback(this)
    , _connectCallback(this)
    {
        memset(&_timeout, 0, sizeof(_timeout));
        _timeout.tv_sec = 1;
        _timeout.tv_usec = 0;
    }
    
    TcpServer::TcpServer(unsigned short port, ReceiveCallback* receiveCallback, 
                             ConnectCallback* connectCallback, bool passive)
    : Server(passive)
    , _port(port)
    , _backlog(SOMAXCONN)
    , _fd(INVALID_SOCKET)
    , _receiveCallback(receiveCallback)
    , _connectCallback(connectCallback)
    {
        memset(&_timeout, 0, sizeof(_timeout));
        _timeout.tv_sec = 1;
        _timeout.tv_usec = 0;
    }
    
    TcpServer::~TcpServer()
    {
        if(_fd != INVALID_SOCKET)
        {
            closeSocket(_fd);
            _fd = INVALID_SOCKET;
        }
    }
    
    unsigned short TcpServer::port()
    {
        return _port;
    }
    
    bool TcpServer::doActivate()
    {
        // Start listening
        try
        {
            if(_fd == INVALID_SOCKET)
            {
                _fd = createTcpSocket();
            }
            assert(_fd != INVALID_SOCKET);
            
            sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = INADDR_ANY;
            addr.sin_port = htons(_port);
            
            doBind(_fd, addr);
            doListen(_fd, SOMAXCONN);
        }
        catch(SocketException& ex)
        {
            std::cout << ex.toString() << "\n";
            return false;
        }
        
        return startThread();
    }
    
    void TcpServer::doShutdown()
    {
        stopThread();
        
        // Close socket
        if(_fd != INVALID_SOCKET)
        {
            closeSocket(_fd);
            _fd = INVALID_SOCKET;
        }
        
        // Clear Connections
        for(std::vector<TcpConnection*>::iterator it = _connections.begin(); it != _connections.end(); ++it)
        {
            TcpConnection* p = *it;
            assert(p != NULL);
            if(p != NULL)
            {
                p->close();
                delete p;
            }
        }
        _connections.clear();
    }
    
    bool TcpServer::doRun()
    {        
        // Select to accept conn
        FD_ZERO(&_fds);
        FD_SET(_fd, &_fds);
        
        int rc = select(_fd + 1, &_fds, NULL, NULL, &_timeout);
        if (rc < 0 )
        {

        }
        else if (FD_ISSET(_fd, &_fds))
        {
            SOCKET fd = doAccept(_fd);
            if(fd != INVALID_SOCKET)
            {
                // Accept a new connection
                TcpConnection* pConn = createConnection(fd);
                if(pConn == NULL)
                {
                    closeSocket(fd);
                    fd = INVALID_SOCKET;
                }
                
                _connections.push_back(pConn);
                
                pConn->receive();
                
                // Connect callback
                if(_connectCallback != NULL) 
                {
                    _connectCallback->onConnect(pConn);
                }
            }
        }
        
        // Monitor and close zombie connections
        for(std::vector<TcpConnection*>::iterator it = _connections.begin(); it != _connections.end(); )
        {
            TcpConnection* p = *it;
            if(!p->isAlive())
            {
                p->close();
                it = _connections.erase(it);
                delete p;
            }
            else
            {
                ++it;
            }
        }
        
        return true;
    }
    
    TcpConnection* TcpServer::createConnection(SOCKET fd)
    {
        TcpConnection* pConn = new TcpConnection(fd, _receiveCallback);
        return pConn;
    }
    
}
