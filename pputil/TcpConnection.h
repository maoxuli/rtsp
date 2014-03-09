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

#ifndef PPUTIL_TCP_CONNECTION_H
#define PPUTIL_TCP_CONNECTION_H

#include <pputil/Config.h>
#include <pputil/Connection.h>
#include <pputil/Buffer.h>
#include <pputil/Socket.h>
#include <IceUtil/Thread.h>
#include <deque>

using namespace IceUtil;

namespace pputil
{
    //
    // A recv thread recv data from socket into buffer and callback to user
    // A send thread send data in buffer to socket
    //    
    class PPUTIL_API TcpConnection : public Connection
    {
    public:
        TcpConnection(ReceiveCallback* receiveCallback); // As client side connection
        TcpConnection(SOCKET fd, ReceiveCallback* receiveCallback); // As server side connection
        virtual ~TcpConnection();
            
        // Connection interface
        
        virtual void close();
        virtual bool isAlive();
    
        // Start receiving
        virtual bool receive();
        
        // Send       
        virtual long send(const std::string& s);
        virtual long send(byte* bytes, size_t n);
        virtual long send(Buffer* buffer);
        
        virtual bool asynSend(const std::string& s);
        virtual bool asynSend(byte* bytes, size_t n);
        virtual bool asynSend(Buffer* buffer);
    
    protected:
        // State
        bool _running;
        bool _closing;
        Mutex _mutex;
        
        // Callback
        ReceiveCallback* _receiveCallback;
        
        virtual void onReceive();
        
        // Buffers
        Buffer* _inBuffer;
        Buffer* _outBuffer;
        
        // Socket
        SOCKET _fd;
        
        // For select
        fd_set _fds;
        fd_set _readFds;
        fd_set _writeFds;
        struct timeval _timeout;
        
        virtual bool input();
        virtual bool output();
        
    protected:
        // Input thread
        class InputThread : public Thread
        {
        public:
            InputThread(TcpConnection* connection)
            : _connection(connection)
            {
            }
            
            virtual ~InputThread()
            {
            }
          
            virtual void run()
            {
                while(_connection != NULL && _connection->input())
                {
                    
                }
            }
            
        private:
            TcpConnection* _connection;
        };
        
        friend class InputThread; 
        
        typedef IceUtil::Handle<InputThread> InputThreadPtr;
        InputThreadPtr _inputThread;
        
    protected:
        // Output thread
        class OutputThread : public Thread
        {
        public:
            OutputThread(TcpConnection* connection)
            : _connection(connection)
            {
            }
            
            virtual ~OutputThread()
            {

            }
            
            virtual void run()
            {
                while(_connection != NULL && _connection->output())
                {
                    
                }
            }
            
        private:
            TcpConnection* _connection;
        };
        
        friend class OutputThread;
        
        typedef IceUtil::Handle<OutputThread> OutputThreadPtr;
        OutputThreadPtr _outputThread;
    };
}

#endif
