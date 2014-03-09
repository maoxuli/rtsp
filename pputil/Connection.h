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

#ifndef PPUTIL_CONNECTION_H
#define PPUTIL_CONNECTION_H

#include <pputil/Config.h>
#include <pputil/Buffer.h>

namespace pputil 
{
    class PPUTIL_API Connection
    {
    public:
        virtual void close() = 0;
        virtual bool isAlive() = 0;
        
        virtual bool receive() = 0; 
        
        virtual long send(const std::string& s) = 0;
        virtual long send(byte* bytes, size_t n) = 0;
        virtual long send(Buffer* buffer) = 0;
        
        virtual bool asynSend(const std::string& s) = 0;
        virtual bool asynSend(byte* bytes, size_t n) = 0;
        virtual bool asynSend(Buffer* buffer) = 0;
    };
    
    class PPUTIL_API ConnectCallback
    {
    public:
        virtual void onConnect(Connection* conn) = 0;
        virtual void onDisconnect(Connection* conn) = 0;
    };
    
    class PPUTIL_API ReceiveCallback
    {
    public:
        virtual void onReceive(Buffer* buffer, Connection* conn) = 0;
    };
    

}
#endif