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

#include "TcpConnection.h"
#include "ThreadException.h"

namespace pputil
{
  
    TcpConnection::TcpConnection(ReceiveCallback* receiveCallback)
    : _fd(INVALID_SOCKET)
    , _receiveCallback(receiveCallback)
    , _inBuffer(NULL)
    , _outBuffer(NULL)
    , _running(false)
	, _closing(false)
    {

    }
    
    TcpConnection::TcpConnection(SOCKET fd, ReceiveCallback* receiveCallback)
    : _fd(fd)
    , _receiveCallback(receiveCallback)
    , _inBuffer(NULL)
    , _outBuffer(NULL)
    , _running(false)
	, _closing(true)
    {

    }
    
    TcpConnection::~TcpConnection()
    {
        if(_fd != INVALID_SOCKET)
        {
            close();
        }
        
        if(_inBuffer != NULL)
        {
            delete _inBuffer;
            _inBuffer = NULL;
        }
        
        if(_outBuffer != NULL)
        {
            delete _outBuffer;
            _outBuffer = NULL;
        }        
    }
    
    bool TcpConnection::isAlive()
    {
        Mutex::Lock lock(_mutex);
        return _running;
    }
    
    // Close the connection 
    void TcpConnection::close()
    {     
        // Signal to threads
        {
            Mutex::Lock lock(_mutex);
            _closing = true;
        }
        
        // Wait for thread is joint
        try
        {
            if(_inputThread)
            {
                _inputThread->getThreadControl().join();
            }
            
            if(_outputThread)
            {
                _outputThread->getThreadControl().join();
            }
        }
        catch(ThreadException& ex)
        {
            std::cout << ex.toString() << "\n";
        }
        
        // Close socket
        try
        {
            if(_fd != INVALID_SOCKET)
            {
                closeSocket(_fd);
                _fd = INVALID_SOCKET;
            }
        }
        catch(SocketException& ex)
        {
            std::cerr << ex.toString() << "\n";
        }
                
        _running = false;
        std::cout << "TcpConnection::close().\n";
    }
    
    // Start receiving
    bool TcpConnection::receive()
    {
        Mutex::Lock lock(_mutex);
		_closing = false;

        // FD_SET for receive and send selecting
        FD_ZERO(&_fds);
        FD_SET(_fd, &_fds);
        
        memset(&_timeout, 0, sizeof(_timeout));
        _timeout.tv_sec = 1;
        _timeout.tv_usec = 0;
        
        // Buffer
        if(_inBuffer == NULL)
        {
            _inBuffer = new Buffer();
        };
        assert(_inBuffer != NULL);
        
        if(_outBuffer == NULL)
        {
            _outBuffer = new Buffer();
        }
        assert(_outBuffer != NULL);
        
        // Start input thread that works with the buffer
        // and callback
        try 
        {
            if(!_inputThread)
            {
                _inputThread = new InputThread(this);
            }
            assert(_inputThread);
            
            _inputThread->start();            
        } 
        catch(ThreadException& ex) 
        {
            std::cout << ex.toString() << "\n";
        }
        
        // Start output thread
        // Select on socket to send data
        try 
        {
            if(!_outputThread)
            {
                _outputThread = new OutputThread(this);
            }
            assert(_outputThread);
            
            _outputThread->start();
            
        } 
        catch(ThreadException& ex) 
        {
            std::cout << ex.toString() << "\n";
        }
        
        _running = true;
        
        return true;
    }

    // Select on socket to receive data
    // Return false will result in input thread exist. 
    // and this method will not be reentered again.
    // Later connection broken will be detected by another thread.
    // That means onDisconnect() and onReceive() can not be 
    // concurrent.    
    bool TcpConnection::input()
    {
        Mutex::Lock lock(_mutex);
        if(_closing)
        {
            return false;
        }
        
        // Select to receive on socket
        _readFds = _fds;
        
        int rc = select(_fd + 1, &_readFds, NULL, NULL, &_timeout);
        if(rc < 0)
        {
            return false;
        }
        else if(rc == 0)
        {

        }
        else if(FD_ISSET(_fd, &_readFds))
        {
            assert(_inBuffer != NULL);
            long n = _inBuffer->receive(_fd);
            if(n <= 0)
            {
                _closing = true;
                return false;
            }
            else
            {
                onReceive();
            }
        }
        
        return true;
    }
    
    void TcpConnection::onReceive()
    {
        if(_receiveCallback != NULL)
        {
            _receiveCallback->onReceive(_inBuffer, this);
        }
    }
    
    // Select on socket to send data
    bool TcpConnection::output()
    {
        Mutex::Lock lock(_mutex);
        if(_closing)
        {
            return false;

        }
        
        /*
        if(_outBuffer == NULL || _outBuffer->size() == 0)
        {
            return true;
        }
        
        // Select to send on socket
        _writeFds = _fds;
		std::cout << "TcpConnection select on socket for write\n";
        int rc = select(_fd + 1, NULL, &_writeFds, NULL, &_timeout);
        if(rc < 0)
        {
            
        }
        else if(rc == 0)
        {
            
        }
        else if(FD_ISSET(_fd, &_writeFds))
        {
            assert(_outBuffer != NULL);
            long n = _outBuffer->send(_fd); 
        }
        */
        return true;
    }
    
    long TcpConnection::send(const std::string& s)
    {

        return 0;
    }
    
    long TcpConnection::send(byte* bytes, size_t n)
    {
        if(bytes == NULL || n == 0)
        {
            return -1;
        }
        
        long len = ::send(_fd, (char*)bytes, n, 0);
        return len;
    }
    
    long TcpConnection::send(Buffer* buffer)
    {
        return buffer->send(_fd);
    }
    
    bool TcpConnection::asynSend(const std::string& s)
    {
        return false;
    }
    
    bool TcpConnection::asynSend(byte* bytes, size_t n)
    {
        if(bytes == NULL || n == 0)
        {
            return false;
        }
        
        // Mark
        
        // Try to send immediately if there is nothing in cache already
        
            // Cancel mark if send complete
        
        // Cache if mark 
        
        return true;
    }
    
    bool TcpConnection::asynSend(Buffer* buffer)
    {
        return false;
    }
    
}
