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

#include "Server.h"
#include "ThreadException.h"

namespace pputil
{
    
    Server::Server(bool passive)
    : _passive(passive)
    , _running(false)
    {

    }
    
    Server::~Server()
    {
        if(_running)
        {
            std::cout << "Server is not shutdown before delete.\n";
            shutdown();	// Note: this call local shutdown rather than that in derived
        }        
    }
    
    bool Server::activate()
    {
        Mutex::Lock lock(_mutex);
        
        if(!_running)
        {
            _running = doActivate();
        }

        return _running;
    }
    
    bool Server::doActivate()
    {
        return startThread();
    }
    
    void Server::shutdown()
    {
        {
            Mutex::Lock lock(_mutex);
            
            if(!_running)
            {
                return;
            }
            _running = false;
        }
        
        return doShutdown();
    }
    
    void Server::doShutdown()
    {
        stopThread();
    }
    
    // Schedule process
    // return true to indicate the server will keep running
    // return false to indicate the server stops running
    bool Server::run()
    {        
        Mutex::Lock lock(_mutex);
        if(!_running)
        {
            return false;
        }
        
        return doRun();
    }
    
    bool Server::doRun()
    {
        return true;
    }
    
    bool Server::startThread()
    {
        if(_passive)
        {
            try
            {
                if(!_thread)
                {
                    _thread = new ServerThread(this);
                }
                assert(_thread);
                _thread->start();
                return true;
            }
            catch(ThreadException& ex)
            {
                return false;
            }
        }
        
        return true;
    }
    
    void Server::stopThread()
    {
        if(_thread)
        {
            _thread->getThreadControl().join();
        }
    }
    
}
