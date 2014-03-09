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

#ifndef RTSP_RTSP_STREAM_H
#define RTSP_RTSP_STREAM_H

#include <rtsp/UdpSocket.h>
#include <rtsp/RtspConnection.h>

namespace rtsp
{
	// 
	// RTSP server using a session to keep state of a presentation
	// A RTSPSession using one or more streams to send media to client
	// The stream may be RTP based or TCP based
	//
	class RtspStream
	{
	public:
		RtspStream(const std::string& name);
		virtual ~RtspStream();

		// Configuration name of the stream
		// SDP and SETUP of RTSP request
		std::string name();

		// update == true, get current seq and generate new one
		// update == false, peek current seq only
		unsigned int seq(bool update = true);

		// Data send interface
		virtual bool sendData(byte* b, size_t n) = 0;

	protected:
		std::string _name;
		unsigned int _seq;
	};

	class RtpStream : public RtspStream
	{
	public:
		RtpStream(const std::string& name);
		virtual ~RtpStream();

		bool init(unsigned short& serverPort, unsigned short clientPort);
		virtual bool sendData(byte* b, size_t n);

	private:
		UdpSocket _rtpSocket;
		UdpSocket _rtcpSocket;
	};


	class TcpStream : public RtspStream
	{
	public:
		TcpStream(const std::string& name);
		virtual ~TcpStream();

		bool init(RtspConnection* conn);
		virtual bool sendData(byte* b, size_t n);

	private:
		RtspConnection* _connection;
	};

}

#endif 
