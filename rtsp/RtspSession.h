// **********************************************************************
//
// Copyright (c) 2011, PPEngine
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
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

#ifndef RTSP_RTSP_SESSION_H
#define RTSP_RTSP_SESSION_H

#include <rtsp/RtspConnection.h>
#include <rtsp/RtspStream.h>

namespace rtsp
{
    // 
    // RTSP server using a session to keep state of a presentation
    // A RtspSession using one or more streams to send media to client
    // The stream may be RTP based or TCP based
    //
    
    class RtspSession
    {  
    public:
        RtspSession(const std::string& sid, const std::string& mid);
        virtual ~RtspSession();

        // Close session
        void close();
        
        // Identifier
        std::string sid();
        std::string mid();

        // Get all streams info	(for RTSP response)	
        std::string streamsInfo();

        // find a stream 
        MediaStream* findStream(const std::string& name);

        // Delet a stream
        void removeStream(const std::string& name);

        // Setup a stream
        bool setupStream(const std::string& name, unsigned short& serverPort, unsigned short clientPort);
        bool setupStream(const std::string& name, RtspConnection* conn);

        // Seek to a position, return actual result position 
        // With -1 to return current position
        virtual int seek(int pos = -1) = 0;

        // Playing control
        virtual void play();
        virtual void pause();
        virtual void teardown();
            
        // Driven by external thread
        virtual bool run() = 0;

    protected:
        // Identifier
        std::string m_sid;
        std::string m_mid;

        // State of session
        enum {INIT, READY, PLAYING};
        int m_state;

        // One or more streams
        std::vector<RtspStream*> m_streams;			
    };
}

#endif 
