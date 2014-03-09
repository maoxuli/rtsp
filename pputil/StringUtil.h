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

#ifndef PPUTIL_STRING_UTIL_H
#define PPUTIL_STRING_UTIL_H

#include <pputil/Config.h>

namespace pputil 
{
    //
    // Trim white space
    //
    PPUTIL_API std::string trim(const std::string& s);
    
    //
    // Functions to convert to to all lower/upper case
    //
    PPUTIL_API std::string toLower(const std::string& s);
    PPUTIL_API std::string toUpper(const std::string& s);
    
    //
    // Split a string using the given delimiters. Considers single and double quotes;
    // returns false for unbalanced quote, true otherwise.
    //
    PPUTIL_API StringSeq splitString(const std::string& s, const std::string& delim);
    
    //
    // Convert command line args to a string sequence
    //
    PPUTIL_API StringSeq parseArgs(int argc, const char* argv[]);
}

#endif
