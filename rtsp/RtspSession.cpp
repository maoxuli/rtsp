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

#include "RtspSession.h"

namespace rtsp
{

    RtspSession::RtspSession(const std::string& sid, const std::string& mid)
    : m_sid(sid)
    , m_mid(mid)
    , m_state(INIT)
    {

    }

    RtspSession::~RtspSession()
    {
        std::cout << "RtspSession::~RtspSession() " << m_sid << "\n";
    }

    void RtspSession::close()
    {
        
    }
        
    std::string RtspSession::sid()
    {
        return m_sid;
    }

    std::string RtspSession::mid()
    {
        return m_mid;
    }

    std::string RtspSession::streamsInfo()
    {
        std::ostringstream oss;

        for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
        {
            RtspStream* p = *it;
            if(p != NULL)
            {
                oss << "url=rtsp://127.0.0.1/" << m_mid << "/" << p->name() << ";seq=" << p->seq(false) << ";rtptime=0";
            }

            if(it != m_streams.end())
            {
                oss << ",";
            }
        }

        return oss.str();
    }

    RtspStream* RtspSession::findStream(const std::string& name)
    {
        for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); ++it)
        {
            RtspStream* p = *it;
            if(p != NULL && p->name() == name)
            {
                return p;
            }
        }

        return NULL;
    }

    void RtspSession::removeStream(const std::string& name)
    {
        // Delete existing stream
        for(std::vector<RtspStream*>::iterator it = m_streams.begin(); it != m_streams.end(); )
        {
            RtspStream* p = *it;
            if(p != NULL && p->name() == name)
            {
                delete p;
                it = m_streams.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    // RTSession must be inited sucessfully
    // Based on port, util bind a local address successfully
    bool RtspSession::setupStream(const std::string& name, unsigned short& serverPort, unsigned short clientPort) 
    {
        // Remove existing stream
        removeStream(name);

        // Add a new stream
        RTPStream* pStream = new RTPStream(name);
        if(pStream != NULL && pStream->init(serverPort, clientPort))
        {
            m_streams.push_back(pStream);
            m_state = READY;

            return true;
        }

        return false;
    }

    bool RtspSession::setupStream(const std::string& name, RtspConnection* connection)
    {
        // Remove existing stream
        removeStream(name);

        // Add a new stream
        TCPStream* pStream = new TCPStream(name);
        if(pStream != NULL && pStream->init(connection))
        {
            m_streams.push_back(pStream);
            m_state = READY;

            return true;
        }

        return false;
    }

    void RtspSession::play()
    {
        m_state = PLAYING;
    }

    void RtspSession::pause()
    {
        m_state = READY;
    }

    void RtspSession::teardown()
    {
        m_state = INIT;
    }

}
