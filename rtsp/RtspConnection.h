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

#ifndef RTSP_RTSP_CONNECTION_H
#define RTSP_RTSP_CONNECTION_H

#include <pputil/TcpConnection.h>

namespace rtsp
{
    // 
    // RtspConnection is extension of TcpConnection
    // Handle RTSP packet and forward to RtspServer
    //
    
    class RtspServer;
    
    class RtspConnection : public pputil::TcpConnection
    {
    public:
        RtspConnection(); // For client 
        RtspConnection(pputil::SOCKET fd, RtspServer* server); // For server
        virtual ~RtspConnection();

        // For client, send request to server
        bool sendRequest(RtspRequest* msg);
        
        // For server, send response to client
        // send interleaved data to client
        bool sendResponse(RtspResponse* msg);
        bool sendData(byte* b, size_t n);
        
    private:
        // From Connection
        virtual void onReceive();
            
        // Parse packet
        void readType();
        void readData();
        void readInitial();
        void readHeader();
        void readBody();
        void onMessage();
        
        // For client, receive data and response
        virtual onData(Buffer* buffer) = 0;
        virtual onResponse(RtspResponse* msg) = 0;

    private:
        // Owner RTSP server
        RtspServer* _server;

        // State Machine to receive RTSP message
        enum RTSP_MESSAGE_STATE
        {
            RMS_READY,        // Ready to receive a new packet
            RMS_READ_DATA,    // Reading interleaved raw data packet
            RMS_READ_INITIAL, // Reading initial line of request or response
            RMS_READ_HEADER,  // Reading header lines
            RMS_READ_BODY,    // Reading body
            RMS_READ_OK       // Complete a message packet
        };

        RTSP_MESSAGE_STATE _state;

        // Incoming Message packet
        Message* _message;  // RtspRequest or RtspResponse message
        size_t _bodyLen;	// Length of body
    };
}

#endif 
