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

#include "StringUtil.h"

namespace pputil 
{
  
    //
    // Trim white space (" \t\r\n")
    //
    std::string trim(const std::string& s)
    {
        static const std::string delim = " \t\r\n";
        std::string::size_type beg = s.find_first_not_of(delim);
        if(beg == std::string::npos)
        {
            return "";
        }
        else
        {
            return s.substr(beg, s.find_last_not_of(delim) - beg + 1);
        }
    }
        
    std::string toLower(const std::string& s)
    {
        std::string result;
        for(unsigned int i = 0; i < s.length(); ++ i)
        {
            result += tolower(static_cast<unsigned char>(s[i]));
        }
        return result;
    }

    std::string toUpper(const std::string& s)
    {
        std::string result;
        for(unsigned int i = 0; i < s.length(); ++ i)
        {
            result += toupper(static_cast<unsigned char>(s[i]));
        }
        return result;
    }
        
    StringSeq splitString(const std::string& s, const std::string& delim)
    {
        StringSeq result;
        if(s.empty())
        {
            return result;
        }
        
        std::string::size_type start = 0;
        std::string::size_type end = s.find(delim, start);
        
        while(true)
        {
            if(end == std::string::npos)
            {
                result.push_back(s.substr(start));
                break;
            }
            else
            {
                result.push_back(s.substr(start, end - start));
                start = end + 1;
                end = s.find(delim, start);
            }
        }

        return result;
    }
    
    StringSeq parseArgs(int argc, const char* argv[])
    {
        StringSeq args;
        for(int i=0; argv[i] != 0; i++)
        {
            std::string value = argv[i];
            args.push_back(value);
        }
        return args;
    }
    
}

