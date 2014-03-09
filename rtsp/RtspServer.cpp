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

#include "RtspServer.h"

namespace rtsp
{

    RtspServer::RtspServer(unsigned short port, bool passive)
    : TcpServer(port, passive)
    {

    }

    RtspServer::~RtspServer()
    {
        if(_sessions.size() > 0)
        {
            std::cout << "Rtsp Server is not shutdown before delete.\n";
            shutdown();	
        }
    }
    
    // Override to clear sessions when shutdown
    void RtspServer::doShutdown()
    {
        // Shutdown Tcp Server
        TcpServer::doShutdown();
        
        // Clear RtspSessions
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); ++it)
        {
            RtspSession* p = *it;
            assert(p != NULL);;
            if(p != NULL)
            {
                p->close();
                delete p;
            }
        }
        _sessions.clear();
    }

    // Override to run sessions
    bool RtspServer::doRun()
    {
        // Schedule streaming over sessions
        // If a session work failed, close the session
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
        {
            RtspSession* p = *it;
            assert(p != NULL);

            if(!p->run())
            {
                p->close();
                delete p;
                it = _sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
        
        // Run tcp server
        return TcpServer::doRun();
    }
    
    TcpConnection* RtspServer::createConnection(SOCKET fd)
    {
        return new RtspConnection(fd, this);
    }

    void RtspServer::removeSession(const std::string& sid)
    {
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
        {
            RtspSession* p = *it;
            if(p != NULL && p->sid() == sid)
            {
                p->close();
                delete p;
                it = _sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // Close all sessions related to the given media
    void RtspServer::removeMedia(const std::string& mid)
    {
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
        {
            RtspSession* p = *it;
            if(p != NULL && p->mid() == mid)
            {
                p->close();
                delete p;
                it = _sessions.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    RtspSession* RtspServer::findSession(const std::string& sid)
    {
        for(std::vector<RtspSession*>::iterator it = _sessions.begin(); it != _sessions.end(); )
        {
            RtspSession* p = *it;
            if(p != NULL && p->sid() == sid)
            {
                return p;
            }
        }
        
        return NULL;
    }
    
    // Receive Rtsp request
    void onRequest(RtspRequest* msg, RtspConnection* conn)
    {
        assert(msg != NULL);
        assert(conn != NULL);
        assert(msg->type() == REQUEST_MESSAGE);

        switch(msg->method())
        {
            case VERB_OPTIONS:  OnOptionsRequest( msg, conn );	break;
            case VERB_DESCRIBE: OnDescribeRequest( msg, conn );	break;
            case VERB_GETPARAM: OnGetParamRequest( msg, conn );	break;
            case VERB_SETPARAM: OnSetParamRequest( msg, conn );	break;
            case VERB_PAUSE:    OnPauseRequest( msg, conn );	break;
            case VERB_PLAY:     OnPlayRequest( msg, conn );		break;
            case VERB_SETUP:    OnSetupRequest( msg, conn );	break;
            case VERB_TEARDOWN: OnTeardownRequest( msg, conn );	break;
            default: assert(false);                             break;
        }
    }

    // Query supported commands
    // Not affect session state
    void RtspServer::OnOptionsRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        RtspResponse *pResponse	= new RtspResponse();

        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));

        pResponse->setHeader("Server", "Helix Server Version 9.0.8.1427 (linux-2.2-libc6-i586-server) (RealServer compatible)");
        pResponse->setHeader("Public", "OPTIONS, DESCRIBE, SETUP, GET_PARAMETER, SET_PARAMETER, PLAY, PAUSE, TEARDOWN");
        
        pResponse->setHeader("RealChallenge1", "d12f6756d0027a12ee0afbfd64a5cedd");

        conn->sendResponse(pResponse);
        delete pResponse;
    }

    // Query SDP of media
    // Not affect session state
    void RtspServer::OnDescribeRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        // URL: rtsp://127.0.0.1:9960/3012
        std::string url = pmsg->url(); 
        int midStartPos = url.rfind("/");
        std::string mid = url.substr(midStartPos + 1);
        
        // SDP
        std::string sdp = mediaSDP(mid);
        assert(!sdp.empty());

        std::ostringstream oss;
        oss << sdp.size();
        std::string sdpLen = oss.str();

        // Response
        RtspResponse *pResponse	= new RtspResponse();
        
        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));    
        pResponse->setHeader("Content-base", url);
        pResponse->setHeader("Content-type","application/sdp");
        pResponse->setHeader("Content-length",sdpLen);
        pResponse->setBody((byte*)sdp.c_str(),sdp.size());

        conn->sendResponse(pResponse);
        delete pResponse;
    }

    // Rtsp SETUP request
    // Negotiation to establish a session and the streams in the session
    void RtspServer::OnSetupRequest(RtspRequest* pmsg, RtspConnection* conn)
    {	
        assert(pmsg != NULL);
        assert(conn != NULL);

        // URL: rtsp://127.0.0.1:9960/3201/(rtx/audio/video)
        std::string url = pmsg->url(); 

        // mid and stream name
        int urlEndPos = url.rfind("/");
        std::string streamName = url.substr(urlEndPos + 1);
        url = url.erase(urlEndPos);
        int midStartPos = url.rfind("/");
        std::string mid = url.substr(midStartPos + 1);

        // Session
        std::string sid = pmsg->header("Session");
        RtspSession* pSession = NULL;

        // Session exist already
        if(!sid.empty())
        {
            pSession = findSession(sid);
            if(pSession != NULL)
            {
                assert(mid == pSession->mid());
            }
        }

        // Create new session
        if(pSession == NULL)
        {
            m_sid ++;
            std::ostringstream oss;
            oss << m_sid;
            sid = oss.str();

            // Create new session
            pSession = createSession(sid, mid);
            if(pSession != NULL)
            {
                m_sessions.push_back(pSession);
            }
        }

        assert(pSession != NULL);

        // Setup stream for session
        unsigned short serverPort = m_port + 10;

        unsigned short  clientPort = 0;
        int nPorts = 0;
        std::string strTran = pmsg->header( "Transport" );
        CRequestTransportHdr rqtHdr(strTran);
        rqtHdr.GetBasePort(&clientPort, &nPorts );

        if( rqtHdr.CanUDP() )
        {
            // Over UDP
            pSession->setupStream(streamName, serverPort, clientPort);
        }
        else
        {	
            // Over TCP
            pSession->setupStream(streamName, conn);		
        }

        // Response
        RtspResponse *pResponse	= new RtspResponse();

        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq",pmsg->header("CSeq"));
        pResponse->setHeader("Date","Thu, 15 Dec 2005 03:00:04 GMT");
        pResponse->setHeader("Session", sid);

        // Stream name: rtx, audio, video 
        if( streamName == "rtx" ) 
        {
            pResponse->setHeader("RealChallenge3","d67b8f21bf272fd5020e9fbb08428cfa4f213d09,sdr=abcdabcd");

            std::ostringstream oss;
            oss << "RTP/AVP/UDP;unicast;server_port=" << serverPort << "-" << serverPort + 1 
                << ";client_port=" << clientPort << "-" << clientPort + 1 << ";ssrc=f2bde83e;mode=PLAY";
            pResponse->setHeader("Transport",oss.str());
        } 
        else if(streamName == "audio" )
        {
            pResponse->setHeader("RealChallenge3","d67b8f21bf272fd5020e9fbb08428cfa4f213d09,sdr=abcdabcd");
            pResponse->setHeader("Transport","RTP/AVP/TCP;unicast;interleaved=2-3;ssrc=bedf8d08;mode=PLAY");
        }
        else if(streamName == "video" ) 
        {
            pResponse->setHeader("Transport","RTP/AVP/TCP;unicast;interleaved=4-5;ssrc=bedf8d2d;mode=PLAY");
        }
        else
        {
            assert(false);
        }

        conn->sendResponse(pResponse);
        delete pResponse;
    }


    // Get parameter of stream 
    void RtspServer::OnGetParamRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);
     
        std::string sid = pmsg->header("Session");
        
        RtspResponse* pResponse = new RtspResponse();
        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));
        pResponse->setHeader("Session", sid);

        conn->sendResponse(pResponse);
        delete pResponse; 
    }

    // Set parameter of stream
    void RtspServer::OnSetParamRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        std::string sid = pmsg->header("Session");

        RtspResponse *	pResponse	= new RtspResponse();

        std::string ping = pmsg->header("Ping");
        if( ping.empty())
            pResponse->setStatus(200);
        else
            pResponse->setStatus( 451 );

        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));
        pResponse->setHeader("Session", sid);

        conn->sendResponse( pResponse );
        delete pResponse;
    }

    // Start to play
    void RtspServer::OnPlayRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        std::string sid = pmsg->header("Session");

        // Start position
        int nStartTime = -1;
        std::string strTime = pmsg->header("Range");
        if( !strTime.empty() )
        {
            strTime = strTime.substr(4);
            int nPos = strTime.find("-");
            std::string strStartTime = strTime.substr(0, nPos);
            nStartTime = atoi(strStartTime.c_str());
        }

        // Seek to pos
        RtspSession* pSession = findSession(sid);
        assert(pSession != NULL);
        if(pSession != NULL)
        {
            pSession->seek(nStartTime);
        }

        // RTP Info
        std::string rtpInfo;
        if(pSession != NULL)
        {
            rtpInfo = pSession->streamsInfo();
        }

        // Response
        RtspResponse* pResponse	= new RtspResponse();

        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));
        pResponse->setHeader("Session", sid);
        pResponse->setHeader("RTP-Info",rtpInfo);

        conn->sendResponse(pResponse);
        delete pResponse;

        // Play
        if(pSession != NULL)
        {
            pSession->play(); 
        }
    }

    void RtspServer::OnPauseRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        std::string sid = pmsg->header("Session");

        // Pause
        RtspSession* pSession = findSession(sid);
        assert(pSession != NULL);
        if(pSession != NULL)
        {
            pSession->pause();
        }

        // Response
        RtspResponse* pResponse	= new RtspResponse();
        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));
        pResponse->setHeader("Session",sid);

        conn->sendResponse(pResponse);
        delete pResponse;
    }

    // Close session
    void RtspServer::OnTeardownRequest(RtspRequest* pmsg, RtspConnection* conn)
    {
        assert(pmsg != NULL);
        assert(conn != NULL);

        std::string sid = pmsg->header("Session");

        // Stop and remove
        RtspSession* pSession = findSession(sid);
        assert(pSession != NULL);
        if(pSession != NULL)
        {
            pSession->teardown();
            removeSession(sid);
        }

        // Response
        RtspResponse* pResponse	= new RtspResponse();
        pResponse->setStatus(200);
        pResponse->setVersion("1.0");
        pResponse->setHeader("CSeq", pmsg->header("CSeq"));
        pResponse->setHeader("Session",sid);

        conn->sendResponse(pResponse);
        delete pResponse;
    }

}