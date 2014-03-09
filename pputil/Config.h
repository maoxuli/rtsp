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

#ifndef PPUTIL_CONFIG_H
#define PPUTIL_CONFIG_H

//
// Endianness
//
// Most CPUs support only one endianness, with the notable exceptions
// of Itanium (IA64) and MIPS.
//
#if defined(__i386) || defined(_M_IX86) || defined(__x86_64)  || \
defined(_M_X64) || defined(_M_IA64) || defined(__alpha__) || \
defined(__MIPSEL__)
#   define PP_LITTLE_ENDIAN
#elif defined(__sparc) || defined(__sparc__) || defined(__hppa) || \
defined(__ppc__) || defined(__powerpc) || defined(_ARCH_COM) || \
defined(__MIPSEB__)
#   define PP_BIG_ENDIAN
#else
#   error "Unknown architecture"
#endif

//
// 32 or 64 bit mode?
//
#if defined(__linux) && defined(__sparc__)
//
// We are a linux sparc, which forces 32 bit usr land, no matter 
// the architecture
//
#   define  PP_32
#elif defined(__sun) && (defined(__sparcv9) || defined(__x86_64))  || \
defined(__linux) && defined(__x86_64)                        || \
defined(__hppa) && defined(__LP64__)                         || \
defined(_ARCH_COM) && defined(__64BIT__)                     || \
defined(__alpha__)                                           || \
defined(_WIN64)
#   define PP_64
#else
#   define PP_32
#endif

#if defined(_WIN32)

#   ifndef _WIN32_WINNT
        //
        // Necessary for TryEnterCriticalSection (see Mutex.h).
        //
#       if defined(_MSC_VER) && _MSC_VER < 1500
#           define _WIN32_WINNT 0x0400
#       endif
#   elif _WIN32_WINNT < 0x0400
#       error "TryEnterCricalSection requires _WIN32_WINNT >= 0x0400"
#   endif

#   if defined(_MSC_VER) && (!defined(_DLL) || !defined(_MT))
#       error "Only multi-threaded DLL libraries can be used with this implementation!"
#   endif

#define WIN32_LEAN_AND_MEAN
#   include <windows.h>

#   ifdef _MSC_VER
//     '...' : forcing value to bool 'true' or 'false' (performance warning)
#      pragma warning( disable : 4800 )
//     ... identifier was truncated to '255' characters in the debug information
#      pragma warning( disable : 4786 )
//     'this' : used in base member initializer list
#      pragma warning( disable : 4355 )
//     class ... needs to have dll-interface to be used by clients of class ...
#      pragma warning( disable : 4251 )
//     ... : inherits ... via dominance
#      pragma warning( disable : 4250 )
//     non dll-interface class ... used as base for dll-interface class ...
#      pragma warning( disable : 4275 )
//      ...: decorated name length exceeded, name was truncated
#      pragma warning( disable : 4503 )  
#   endif

#endif

//
// Some include files we need almost everywhere.
//

#include <cassert>
#include <iostream>
#include <sstream>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdint.h>

#ifndef _WIN32
#   include <pthread.h>
#   include <errno.h>
#endif

//
// Compiler extensions to export and import symbols: see the documentation 
// for Visual C++, Sun ONE Studio 8 and HP aC++.
//
// TODO: more macros to support IBM Visual Age _Export syntax as well.
//
#if defined(__BCPLUSPLUS__) || defined(_MSC_VER) || \
    (defined(__HP_aCC) && defined(__HP_WINDLL))
#   define PP_DECLSPEC_EXPORT __declspec(dllexport)
#   define PP_DECLSPEC_IMPORT __declspec(dllimport)
#elif defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x550)
#   define PP_DECLSPEC_EXPORT __global
#   define PP_DECLSPEC_IMPORT
#else
#   define PP_DECLSPEC_EXPORT /**/
#   define PP_DECLSPEC_IMPORT /**/
#endif

//
// Let's use these extensions with PPUtil:
//
#ifdef PPUTIL_EXPORTS
#   define PPUTIL_API PP_DECLSPEC_EXPORT
#else
#   define PPUTIL_API PP_DECLSPEC_IMPORT
#endif

//
// Type define
//

namespace pputil 
{
    typedef unsigned char byte;
    
    typedef std::vector<std::string> StringSeq;    
}

#endif
