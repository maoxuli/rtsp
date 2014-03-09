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

#ifndef PPUTIL_SOCKET_H
#define PPUTIL_SOCKET_H

#include <pputil/Config.h>
#include <pputil/Exception.h>

#ifdef _WIN32
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma comment(lib, "ws2_32.lib")
typedef int ssize_t;
#else
#   include <unistd.h>
#   include <fcntl.h>
#   include <sys/socket.h>
#   include <sys/poll.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#endif

#ifdef _WIN32
typedef int socklen_t;
#endif

#ifndef _WIN32
#   define SOCKET int
#   define SOCKET_ERROR -1
#   define INVALID_SOCKET -1
#endif

#ifndef SHUT_RD
#   define SHUT_RD 0
#endif

#ifndef SHUT_WR
#   define SHUT_WR 1
#endif

#ifndef SHUT_RDWR
#   define SHUT_RDWR 2
#endif

#ifndef NETDB_INTERNAL
#   define NETDB_INTERNAL -1
#endif

#ifndef NETDB_SUCCESS
#   define NETDB_SUCCESS 0
#endif

namespace pputil
{  
    class PPUTIL_API SocketException : public SyscallException
    {
    public:
        SocketException(const std::string& file, int line, int error);
        virtual ~SocketException() throw();
        
        virtual std::string toString() const;
        
    protected:
        
    };
    
    PPUTIL_API SOCKET createTcpSocket();
    PPUTIL_API SOCKET createUdpSocket();
    PPUTIL_API void closeSocket(SOCKET);
    PPUTIL_API void cloaseSocketNoException(SOCKET);
            
    PPUTIL_API void doBind(SOCKET, struct sockaddr_in&);
    PPUTIL_API void doListen(SOCKET, int);
    PPUTIL_API SOCKET doAccept(SOCKET);
    PPUTIL_API bool doConnect(SOCKET, struct sockaddr_in&);
    PPUTIL_API void doFinishConnect(SOCKET);
            
    PPUTIL_API int getSocketError();
    PPUTIL_API bool interrupted();
    PPUTIL_API bool acceptInterrupted();
    PPUTIL_API bool noBuffers();
    PPUTIL_API bool wouldBlock();
    PPUTIL_API bool timedout();
    PPUTIL_API bool connectFailed();
    PPUTIL_API bool connectionRefused();
    PPUTIL_API bool connectInProgress();
    PPUTIL_API bool connectionLost();
    PPUTIL_API bool notConnected();
    PPUTIL_API bool noMoreFds(int);

    PPUTIL_API void setBlock(SOCKET, bool);
    PPUTIL_API void setTcpNoDelay(SOCKET);
    PPUTIL_API void setKeepAlive(SOCKET);
    PPUTIL_API void setReuseAddress(SOCKET, bool);
    PPUTIL_API int getSendBufferSize(SOCKET);
    PPUTIL_API void setSendBufferSize(SOCKET, int);
    #ifndef _WIN32_WCE
    PPUTIL_API void setRecvBufferSize(SOCKET, int);
    PPUTIL_API int getRecvBufferSize(SOCKET);
    #endif
    PPUTIL_API int setTcpBufSize(SOCKET fd, int sz);

    PPUTIL_API int compareAddress(const struct sockaddr_in& addr1, const struct sockaddr_in& addr2);
    PPUTIL_API std::string fdToString(SOCKET);
    PPUTIL_API void fdToLocalAddress(SOCKET, struct sockaddr_in&);
    PPUTIL_API bool fdToRemoteAddress(SOCKET, struct sockaddr_in&);
    PPUTIL_API std::string addrToString(const struct sockaddr_in&);
}

#endif
