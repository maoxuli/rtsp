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

#include "RtspMessage.h"

namespace rtsp 
{

    RtspRequest::RtspRequest() 
    : _protocol("RTSP")
    , _method("");
    {

    }

    RtspRequest::RtspRequest(const std::string& version) 
    : RequestMessage(version)
    , _protocol("RTSP")
    , _method("");
    {
     
    }

    RtspRequest::~RtspRequest()
    {

    }

    //////////////////////////////////////////////////////////////////////////////////

    RtspResponse::RtspResponse() 
    : _protocol("RTSP")
    {

    }

    RtspResponse::RtspResponse(const std::string& version) 
    : ResponseMessage(version)
    : _protocol("RTSP")
    {

    }

    RtspResponse::~RtspResponse()
    {

    }

    // Rtsp response status code and reason map
    struct RtspResponseStatus 
    { 
        int code; 
        char* reason; 
    };

    // These must be sorted
    static RtspResponseStatus s_pRtspStatus[] =
    {
        { 100, "Continue" },

        { 200, "OK" },
        { 201, "Created" },
        { 250, "Low on Storage Space" },

        { 300, "Multiple Choices" },
        { 301, "Moved Permanently" },
        { 302, "Moved Temporarily" },
        { 303, "See Other" },
        { 304, "Not Modified" },
        { 305, "Use Proxy" },

        { 400, "Bad Request" },
        { 401, "Unauthorized" },
        { 402, "Payment Required" },
        { 403, "Forbidden" },
        { 404, "Not Found" },
        { 405, "Method Not Allowed" },
        { 406, "Not Acceptable" },
        { 407, "Proxy Authentication Required" },
        { 408, "Request Time-out" },
        { 410, "Gone" },
        { 411, "Length Required" },
        { 412, "Precondition Failed" },
        { 413, "Request Entity Too Large" },
        { 414, "Request-URI Too Large" },
        { 415, "Unsupported Media Type" },
        { 451, "Parameter Not Understood" },
        { 452, "Conference Not Found" },
        { 453, "Not Enough Bandwidth" },
        { 454, "Session Not Found" },
        { 455, "Method Not Valid in This State" },
        { 456, "Header Field Not Valid for Resource" },
        { 457, "Invalid Range" },
        { 458, "Parameter Is Read-Only" },
        { 459, "Aggregate operation not allowed" },
        { 460, "Only aggregate operation allowed" },
        { 461, "Unsupported transport" },
        { 462, "Destination unreachable" },

        { 500, "Internal Server Error" },
        { 501, "Not Implemented" },
        { 502, "Bad Gateway" },
        { 503, "Service Unavailable" },
        { 504, "Gateway Time-out" },
        { 505, "Rtsp Version not supported" },
        { 551, "Option not supported" },
        {   0, NULL }
    };

    static const size_t s_nRtspStatus = sizeof(s_pRtspStatus)/sizeof(RtspResponseStatus) - 1;

    std::string RtspResponse::code2reason(UINT code)
    {
        int hi = s_nRtspStatus;
        int lo = -1;
        int mid;
        while(hi - lo > 1)
        {
            mid = (hi + lo)/2;
            if(_code <= s_pRtspStatus[mid].code)
                hi = mid;
            else
                lo = mid;
        }

        if(_code == s_pRtspStatus[hi].code)
        {
            return s_pRtspStatus[hi].reason;
        }

        return "Unknown";
    }

}