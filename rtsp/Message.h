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

#ifndef RTSP_MESSAGE_H
#define RTSP_MESSAGE_H

#include <pputil/Buffer.h>

namespace rtsp
{
	// A message header line
	class MessageHeader
	{
	private:
		MessageHeader();

	public:
		MessageHeader(const std::string& key);
		MessageHeader(const std::string& key, const std::string& value);

		std::string  key() const;
		std::string  value() const;
		void  setValue(const std::string& value);

	protected:
		std::string _key;
		std::string _value;
	};

	typedef std::vector<MessageHeader*> MessageHeaderSeq;

	enum MESSAGE_TYPE {UNKNOWN_MESSAGE, REQUEST_MESSAGE, RESPONSE_MESSAGE};

	class Message
	{
	private:
		bool operator==(const Message& msg) const;
		bool operator!=(const Message& msg) const;
		const HttpMessage& operator=(const Message& msg); 

	public:
		Message();
		virtual ~Message();

		virtual MESSAGE_TYPE type() const;
		virtual std::string dump() const = 0;
		virtual Buffer* toBuffer() const = 0;

		std::string version() const
		void setVersion(const std::string& version);

		std::string header(const std::string& key) const;
		void setHeader(const std::string& key, const std::string& value);
		void setHeader(const MessageHeader& header);
		void removeHeader(const std::string& key);

		// Total header length for key/val pairs (incl. ": " and CRLF)
		// but NOT separator CRLF
		size_t headerLen() const;
		size_t headerCount() const;
		MessageHeader* header(size_t index) const;

		// Body section
		size_t bodyLen() const;
		byte* body() const;
		void setBody(byte* buf, size_t len);

	protected:
		std::string		_protocol;		// "HTTP", "RTSP"
		std::string		_version;       // "1.0", "1.1", "2.0"
		HttpHeaderSeq	_headers;
		size_t			_bodyLen;
		byte*			_body;
	};

	class RequestMessage : public Message
	{
	public:
		RequestMessage();
		RequestMessage(const std::string& version);
		virtual ~RequestMessage();

		virtual MESSAGE_TYPE type() const;
		virtual std::string dump() const;
		virtual Buffer* toBuffer() const;

		std::string	method() const;
		void setMethod(const std::string& method);

		std::string url() const;
		void setUrl(const std::string& url);

	protected:
		std::string	_method;
		std::string _url;
	};

	class ResponseMessage : public Message
	{
	public:
		ResponseMessage();
		ResponseMessage(const std::string& version);
		virtual ~ResponseMessage();

		virtual MESSAGE_TYPE type() const;
		virtual std::string dump() const;
		virtual Buffer* toBuffer() const;

		int code() const;
		std::string reason() const;
		void setStatus(int code, const std::string& reason = "");

	protected:
		virtual std::string code2reason() = 0;
	};
}

#endif 

