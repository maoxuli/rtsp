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

#include "Socket.h"

#if defined(_WIN32)
#  include <winsock2.h>
#  include <ws2tcpip.h>
#elif defined(__APPLE__) || defined(__FreeBSD__)
#  include <ifaddrs.h>
#  include <net/if.h>
#else
#  include <sys/ioctl.h>
#  include <net/if.h>
#  ifdef __sun
#    include <sys/sockio.h>
#  endif
#endif

namespace pputil 
{ 
    SocketException::SocketException(const std::string& file, int line, int error)
    : SyscallException(file, line, error)
    {
        
    }
    
    SocketException::~SocketException() throw()
    {
        
    }
    
    std::string SocketException::toString() const
    {
        std::ostringstream oss;
        oss << SyscallException::toString();
        oss << "Socket Error/";
        return oss.str();
    }
    
    //////////////////////////////////////////////////////////////////////////////

    SOCKET createTcpSocket()
    {
        SOCKET fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(fd == INVALID_SOCKET)
        {
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
        setTcpNoDelay(fd);
        setKeepAlive(fd);
        
        return fd;
    }

    SOCKET createUdpSocket()
    {
        SOCKET fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(fd == INVALID_SOCKET)
        {
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }

        return fd;
    }

    void closeSocket(SOCKET fd)
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        if(closesocket(fd) == SOCKET_ERROR)
        {
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        WSASetLastError(error);
    #else
        int error = errno;
        if(close(fd) == SOCKET_ERROR)
        {
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        errno = error;
    #endif
    }

    void closeSocketNoException(SOCKET fd)
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        closesocket(fd);
        WSASetLastError(error);
    #else
        int error = errno;
        close(fd);
        errno = error;
    #endif
    }

    void doBind(SOCKET fd, struct sockaddr_in& addr)
    {
        if(bind(fd, reinterpret_cast<struct sockaddr*>(&addr), int(sizeof(addr))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
        socklen_t len = static_cast<socklen_t>(sizeof(addr));
    #ifdef NDEBUG
        getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
    #else
        int ret = getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len);
        assert(ret != SOCKET_ERROR);
    #endif
    }
        
    void doListen(SOCKET fd, int backlog)
    {
    repeatListen:
        if(::listen(fd, backlog) == SOCKET_ERROR)
        {
            if(interrupted())
            {
                goto repeatListen;
            }
            
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    SOCKET doAccept(SOCKET fd)
    {
    #ifdef _WIN32                   
        SOCKET ret;
    #else       
        int ret;    
    #endif 
        
    repeatAccept:
        if((ret = ::accept(fd, 0, 0)) == INVALID_SOCKET)
        {
            if(acceptInterrupted())
            {
                goto repeatAccept;
            }
            
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
        setTcpNoDelay(ret);
        setKeepAlive(ret);
        
        return ret;
    }
     
    bool doConnect(SOCKET fd, struct sockaddr_in& addr)
    {
    repeatConnect:
        if(::connect(fd, reinterpret_cast<struct sockaddr*>(&addr), int(sizeof(sockaddr_in))) == SOCKET_ERROR)
        {
            if(interrupted())
            {
                goto repeatConnect;
            }
            
            if(connectInProgress())
            {
                return false;
            }
            
            closeSocketNoException(fd);
            
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
        #if defined(__linux)
        //
        // Prevent self connect (self connect happens on Linux when a client tries to connect to
        // a server which was just deactivated if the client socket re-uses the same ephemeral
        // port as the server).
        //
        struct sockaddr_in localAddr;
        fdToLocalAddress(fd, localAddr);
        if(compareAddress(addr, localAddr) == 0)
        {
            SocketException ex(__FILE__, __LINE__);
            throw ex;
        }
        #endif
        
        return true;
    }

    void doFinishConnect(SOCKET fd)
    {
        //
        // Note: we don't close the socket if there's an exception. It's the responsability
        // of the caller to do so.
        //
        
        //
        // Strange windows bug: The following call to Sleep() is
        // necessary, otherwise no error is reported through
        // getsockopt.
        //
    #ifdef _WIN32
        Sleep(0);
    #endif
        
        int val;
        socklen_t len = static_cast<socklen_t>(sizeof(int));
        if(getsockopt(fd, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&val), &len) == SOCKET_ERROR)
        {
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
        if(val > 0)
        {
    #ifdef _WIN32
            WSASetLastError(val);
    #else
            errno = val;
    #endif

            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        
    #if defined(__linux)
        //
        // Prevent self connect (self connect happens on Linux when a client tries to connect to
        // a server which was just deactivated if the client socket re-uses the same ephemeral
        // port as the server).
        //
        struct sockaddr_in localAddr;
        fdToLocalAddress(fd, localAddr);
        struct sockaddr_in remoteAddr;
        if(!fdToRemoteAddress(fd, remoteAddr) && compareAddress(remoteAddr, localAddr) == 0)
        {
            SocketException ex(__FILE__, __LINE__);
            throw ex;
        }
    #endif
    }

    int getSocketError()
    {
    #ifdef _WIN32
        return WSAGetLastError();
    #else
        return errno;
    #endif
    }
        
    bool interrupted()
    {
    #ifdef _WIN32
        return WSAGetLastError() == WSAEINTR;
    #else
    #   ifdef EPROTO
        return errno == EINTR || errno == EPROTO;
    #   else
        return errno == EINTR;
    #   endif
    #endif
    }

    bool acceptInterrupted()
    {
        if(interrupted())
        {
            return true;
        }
        
    #ifdef _WIN32
        int error = WSAGetLastError();
        return error == WSAECONNABORTED ||
        error == WSAECONNRESET ||
        error == WSAETIMEDOUT;
    #else
        return errno == ECONNABORTED ||
        errno == ECONNRESET ||
        errno == ETIMEDOUT;
    #endif
    }
        
    bool noBuffers()
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        return error == WSAENOBUFS ||
               error == WSAEFAULT;
    #else
        return errno == ENOBUFS;
    #endif
    }

    bool wouldBlock()
    {
    #ifdef _WIN32
        return WSAGetLastError() == WSAEWOULDBLOCK;
    #else
        return errno == EAGAIN || errno == EWOULDBLOCK;
    #endif
    }

    bool timedout()
    {
    #ifdef _WIN32
        return WSAGetLastError() == WSAETIMEDOUT;
    #else
        return errno == EAGAIN || errno == EWOULDBLOCK;
    #endif
    }

    bool connectFailed()
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        return error == WSAECONNREFUSED ||
               error == WSAETIMEDOUT ||
               error == WSAENETUNREACH ||
               error == WSAEHOSTUNREACH ||
               error == WSAECONNRESET ||
               error == WSAESHUTDOWN ||
               error == WSAECONNABORTED;
    #else
        return errno == ECONNREFUSED ||
               errno == ETIMEDOUT ||
               errno == ENETUNREACH ||
               errno == EHOSTUNREACH ||
               errno == ECONNRESET ||
               errno == ESHUTDOWN ||
               errno == ECONNABORTED;
    #endif
    }

    bool connectionRefused()
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        return error == WSAECONNREFUSED;
    #else
        return errno == ECONNREFUSED;
    #endif
    }

    bool connectInProgress()
    {
    #ifdef _WIN32
        return WSAGetLastError() == WSAEWOULDBLOCK;
    #else
        return errno == EINPROGRESS;
    #endif
    }

    bool connectionLost()
    {
    #ifdef _WIN32
        int error = WSAGetLastError();
        return error == WSAECONNRESET ||
               error == WSAESHUTDOWN ||
               error == WSAENOTCONN ||
               error == WSAECONNABORTED;
    #else
        return errno == ECONNRESET ||
               errno == ENOTCONN ||
               errno == ESHUTDOWN ||
               errno == ECONNABORTED ||
               errno == EPIPE;
    #endif
    }

    bool notConnected()
    {
    #ifdef _WIN32
        return WSAGetLastError() == WSAENOTCONN;
    #elif defined(__APPLE__) || defined(__FreeBSD__)
        return errno == ENOTCONN || errno == EINVAL;
    #else
        return errno == ENOTCONN;
    #endif
    }

    bool noMoreFds(int error)
    {
    #ifdef _WIN32
        return error == WSAEMFILE;
    #else
        return error == EMFILE || error == ENFILE;
    #endif
    }

    void setBlock(SOCKET fd, bool block)
    {
        if(block)
        {
    #ifdef _WIN32
            unsigned long arg = 0;
            if(ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
            {
                closeSocketNoException(fd);
                SocketException ex(__FILE__, __LINE__, WSAGetLastError());
                throw ex;
            }
    #else
            int flags = fcntl(fd, F_GETFL);
            flags &= ~O_NONBLOCK;
            
            if(fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
            {
                closeSocketNoException(fd);
                SocketException ex(__FILE__, __LINE__, errno);
                throw ex;
            }
    #endif
        }
        else
        {
    #ifdef _WIN32
            unsigned long arg = 1;
            if(ioctlsocket(fd, FIONBIO, &arg) == SOCKET_ERROR)
            {
                closeSocketNoException(fd);
                SocketException ex(__FILE__, __LINE__, WSAGetLastError());
                throw ex;
            }
    #else
            int flags = fcntl(fd, F_GETFL);
            flags |= O_NONBLOCK;
            if(fcntl(fd, F_SETFL, flags) == SOCKET_ERROR)
            {
                closeSocketNoException(fd);
                SocketException ex(__FILE__, __LINE__, errno);
                throw ex;
            }
    #endif
        }
    }

    void setTcpNoDelay(SOCKET fd)
    {
        int flag = 1;
        if(setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }
        
    void setKeepAlive(SOCKET fd)
    {
        int flag = 1;
        if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    void setReuseAddress(SOCKET fd, bool reuse)
    {
        int flag = reuse ? 1 : 0;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, int(sizeof(int))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    int getSendBufferSize(SOCKET fd)
    {
        int sz;
        socklen_t len = sizeof(sz);
        if(getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&sz, &len) == SOCKET_ERROR || len != sizeof(sz))
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        return sz;
    }

    void setSendBufferSize(SOCKET fd, int sz)
    {
        if(setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&sz, int(sizeof(int))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    #ifndef _WIN32_WCE
    void setRecvBufferSize(SOCKET fd, int sz)
    {
        if(setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&sz, int(sizeof(int))) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    int getRecvBufferSize(SOCKET fd)
    {
        int sz;
        socklen_t len = sizeof(sz);
        if(getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&sz, &len) == SOCKET_ERROR || len != sizeof(sz))
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
        return sz;
    }
    #endif // _WIN32_WCE

    int setTcpBufSize(SOCKET fd, int sz)
    {
        assert(fd != INVALID_SOCKET);
        
        int sizeRequested = sz;
        
        //
        // By default, on Windows we use a 64KB buffer size. On Unix
        // platforms, we use the system defaults.
        //
        if(sizeRequested <= 0)
        {
    #ifdef _WIN32
            sizeRequested = 64 * 1024;
    #else
            sizeRequested = 0;
    #endif
        }
        
        int sizeSet = sz;
        
    #ifndef _WIN32_WCE
        //
        // Sockect option is not available on CE to set receive buffer size.
        //
        
        //
        // Try to set the receive buffer size. The kernel will silently adjust
        // the size to an acceptable value. Then read the size back to
        // get the size that was actually set.
        //
        setRecvBufferSize(fd, sizeRequested);
        sizeSet = getRecvBufferSize(fd);
    #endif //_WIN32_WCE
        
        //
        // Try to set the send buffer size. The kernel will silently adjust
        // the size to an acceptable value. Then read the size back to
        // get the size that was actually set.
        //
        setSendBufferSize(fd, sizeRequested);
        sizeSet = getSendBufferSize(fd);
        
        return sizeSet;
    }

        
    int compareAddress(const struct sockaddr_in& addr1, const struct sockaddr_in& addr2)
    {
        if(addr1.sin_family < addr2.sin_family)
        {
            return -1;
        }
        else if(addr2.sin_family < addr1.sin_family)
        {
            return 1;
        }

        if(addr1.sin_port < addr2.sin_port)
        {
            return -1;
        }
        else if(addr2.sin_port < addr1.sin_port)
        {
            return 1;
        }

        if(addr1.sin_addr.s_addr < addr2.sin_addr.s_addr)
        {
            return -1;
        }
        else if(addr2.sin_addr.s_addr < addr1.sin_addr.s_addr)
        {
            return 1;
        }

        return 0;
    }

    std::string fdToString(SOCKET fd)
    {
        if(fd == INVALID_SOCKET)
        {
            return "<closed>";
        }

        struct sockaddr_in localAddr;
        fdToLocalAddress(fd, localAddr);

        struct sockaddr_in remoteAddr;
        bool peerNotConnected = fdToRemoteAddress(fd, remoteAddr);

        std::string s;
        s += "local address = ";
        s += addrToString(localAddr);
        if(peerNotConnected)
        {
            s += "\nremote address = <not connected>";
        }
        else
        {
            s += "\nremote address = ";
            s += addrToString(remoteAddr);
        }
        return s;
    }

    void fdToLocalAddress(SOCKET fd, struct sockaddr_in& addr)
    {
        socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
        if(getsockname(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == SOCKET_ERROR)
        {
            closeSocketNoException(fd);
            SocketException ex(__FILE__, __LINE__, getSocketError());
            throw ex;
        }
    }

    bool fdToRemoteAddress(SOCKET fd, struct sockaddr_in& addr)
    {
        bool peerNotConnected = false;
        socklen_t len = static_cast<socklen_t>(sizeof(struct sockaddr_in));
        if(getpeername(fd, reinterpret_cast<struct sockaddr*>(&addr), &len) == SOCKET_ERROR)
        {
            if(notConnected())
            {
                peerNotConnected = true;
            }
            else
            {
                closeSocketNoException(fd);
                SocketException ex(__FILE__, __LINE__, getSocketError());
                throw ex;
            }
        }
        return peerNotConnected;
    }

    std::string addrToString(const struct sockaddr_in& addr)
    {
        std::string s;
        // To be done ...
        return s;
    }
    
}
