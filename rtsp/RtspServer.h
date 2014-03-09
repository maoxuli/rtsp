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

#ifndef RTSP_RTSP_SERVER_H
#define RTSP_RTSP_SERVER_H

#include <rtsp/RtspMessage.h>
#include <rtsp/RtspConnection.h>
#include <tcp/TcpServer.h>

namespace rtsp 
{
    //
    // RTSP server is extension of a TCP server
    // Handle RTSP requests from multiple RtspConnections
    // 
    class RtspServer : public TcpServer
    {
    public:
        
        RtspServer(unsigned short port, bool passive = true);
        virtual ~RtspServer();
        
        // Receive a request from a RtspConnection
        void onRequest(RtspRequest* msg, RtspConnection* conn);
        
    protected:
        
        // Override to clear sessions when shutdown
        virtual void doShutdown();
        
        // Override to run sessions
        virtual bool doRun();
        
        // Override to create RtspConnection
        virtual TcpConnection* createConnection(SOCKET fd);
        
    protected:

        // RTSP serve handle RTSP messages received over multiple RTSP connections
        void OnDescribeRequest(RtspRequest* msg, RtspConnection* conn);
        void OnOptionsRequest(RtspRequest* msg, RtspConnection* conn);
        void OnSetupRequest(RtspRequest* msg, RtspConnection* conn);
        void OnGetParamRequest(RtspRequest* msg, RtspConnection* conn);
        void OnSetParamRequest(RtspRequest* msg, RtspConnection* conn);
        void OnPlayRequest(RtspRequest* msg, RtspConnection* conn);
        void OnPauseRequest(RtspRequest* msg, RtspConnection* conn);
        void OnTeardownRequest(RtspRequest* msg, RtspConnection* conn);

        // RtspServer need to know some media info
        // that is bound to medias managed by application
        virtual std::string mediaSDP(const std::string& mid) = 0;

    protected:
        
        // Session based stream managements
        std::vector<RtspSession*> _sessions;

        // Session ID, automatically increased with new session
        unsigned int _sid;

        // A session is identified with sid
        RtspSession* findSession(const std::string& sid);

        // A RTSP server is resided between media and player
        // A session is linked to a player with session ID
        // A session is linked to a media with media ID
        void removeSession(const std::string& sid);
        void removeMedia(const std::string& mid);

        // RTSPSession is linked to client player with RTSP session ID (sid)
        // RTSPSession is linked to local media with media ID (mid)
        // Medias are managed by application
        virtual RtspSession* createSession(const std::string& sid, const std::string& mid) = 0;
    };
}

#endif 
