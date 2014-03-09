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

#ifndef PPUTIL_SERVER_H
#define PPUTIL_SERVER_H

#include <pputil/Config.h>
#include <IceUtil/Thread.h>
#include <IceUtil/RecMutex.h>

using namespace IceUtil;

namespace pputil 
{
    //
    // Framework of a single thread server
    // passive == true: Driven by internal thread 
    // passive == false: Driven by external thread via run()
    //
    class PPUTIL_API Server
    {
    public:
        Server(bool passive = true);
        virtual ~Server();
        
        // Start and stop server
        // Actual actions are done in doActivate() and doShutdown()
        bool activate();
        void shutdown();
        
        // Schedule running of server
        // Actual actions are done in doRun()
        bool run();
        
    protected:
        // Overridable
        virtual bool doActivate();
        virtual void doShutdown();
        virtual bool doRun();
        
        // Start and stop internal driving thread
        // Do nothing when passive == false
        bool startThread();
        void stopThread();
        
        // State
        bool _passive;
        bool _running;
        Mutex _mutex;
        
    protected:
        // Internal driving thread
        // Acting when m_passive = false
        class ServerThread : public Thread
        {
        public:
            ServerThread(Server* server)
            : _server(server)
            {
                assert(_server != NULL);
            }
            
            virtual ~ServerThread() 
            { 
            };
            
            virtual void run()
            {
                while(_server->run())
                {
                }
            }
            
        private:
            Server* _server;
        };
        
        friend class ServerThread;
        
        typedef IceUtil::Handle<ServerThread> ServerThreadPtr;
        ServerThreadPtr _thread;
    };
}

#endif
