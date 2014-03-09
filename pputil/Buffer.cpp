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

#include "Buffer.h"

namespace pputil 
{
    
    OutOfBoundsException::OutOfBoundsException(const std::string& file, int line)
    : Exception(file, line)
    {
        
    }
    
    OutOfBoundsException::~OutOfBoundsException() throw()
    {
        
    }
    
    std::string OutOfBoundsException::toString() const
    {
        std::ostringstream oss;
        oss << Exception::toString();
        oss << "Out of bounds error/";
        return oss.str();
    }
    
    //////////////////////////////////////////////////////////////////////
    
    // Buffer size if dynamic, limit is max size
    Buffer::Buffer(size_t limit)
    : _container(limit)
    {
        
    }
    
    Buffer::~Buffer()
    {
        
    }
    
    // Size of buffer, byte number of available data
    size_t Buffer::size()
    {
        return _container.readable();
    }
    
    // Expose internal memory position for external read and write
    byte* Buffer::read_pos()
    {
        return _container.read_pos();
    }
    
    void Buffer::remove(size_t n)
    {
        _container.remove(n);
    }
    
    size_t Buffer::ensure(size_t n)
    {
        return _container.reserve(n);
    }
    
    byte* Buffer::write_pos()
    {
        return _container.write_pos();
    }
    
    void Buffer::resize(size_t n)
    {
        _container.resize(n);
    }
    
    // Output data to socket
    long Buffer::send(SOCKET fd)
    {
        assert(fd != INVALID_SOCKET);
        long count = 0;
        
        while(true)
        {
            if(_container.readable() == 0)
            {
                break;
            }
            
            long len = ::send(fd, (char*)_container.read_pos(), _container.readable(), 0);
            if(len <= 0)
            {
                break;
            }
            
            _container.remove(len);
            count += len;
        }
        
        return count;
    }
    
    // Input data from socket
    long Buffer::receive(SOCKET fd)
    {
        assert(fd != INVALID_SOCKET);

        if(_container.reserve(1024) < 1024)
        {
            return -1;
        }
            
        long len = ::recv(fd, (char*)_container.write_pos(), _container.writable(), 0);

        if(len > 0)
        {
            _container.resize(len);
        }
        
        return len;
    }
    
    // Find a given character(s) in current readable data
    // Used in find the end of a string or line
    const byte* Buffer::find(const std::string& s, size_t offset) const
    {
        const byte* p = (const byte*) std::search(
              reinterpret_cast<const char*>(_container.read_pos()), 
              reinterpret_cast<const char*>(_container.write_pos()), s.begin(), s.end());
        
        return p == _container.write_pos() ? NULL : p;
    }
    
    //
    // Peek operations get data from available data
    // Particular data type and offset determine the bytes that will be read
    // Data is not removed from the buffer after peek
    // Peek operations does not change the size of buffer
    //
    
    // Peek with particular data type 
    int8_t Buffer::peek8(size_t offset) const
    {
        int8_t v = 0;            // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 1);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        v = *(_container.read_pos() + offset);

        return v;
    }
    
    uint8_t Buffer::peek8u(size_t offset) const
    {
        uint8_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 1);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        v = *(_container.read_pos() + offset);
        
        return v;
    }
    
    int16_t Buffer::peek16(size_t offset) const
    {
        int16_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 2);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    uint16_t Buffer::peek16u(size_t offset) const
    {
        uint16_t v = 0;          // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 2);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    int32_t Buffer::peek32(size_t offset) const
    {
        int32_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 4);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    uint32_t Buffer::peek32u(size_t offset) const
    {
        int32_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 4);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    int64_t Buffer::peek64(size_t offset) const
    {
        int64_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 8);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    uint64_t Buffer::peek64u(size_t offset) const
    {
        int64_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 8);
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    bool Buffer::peekBool(size_t offset) const
    {
        int8_t v = peek8(offset);
        return v != 0;
    }
    
    float Buffer::peekFloat(size_t offset) const
    {
        float v = 0;            // Value
        size_t n = sizeof(v);   // Bytes number
        assert(n == 4);         
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#endif
        
        return v;
    }
    
    double Buffer::peekDouble(size_t offset) const
    {
        double v = 0;           // Value
        size_t n = sizeof(v);   // Bytes number
        assert(n == 8);         
        
        if(_container.readable() < n + offset)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos() + offset;
#ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++; 
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
#else
        byte* dest = reinterpret_cast<byte*>(&v);
#  if defined(__arm__) && defined(__linux)
        dest[4] = *src++;
        dest[5] = *src++;
        dest[6] = *src++;
        dest[7] = *src++;
        dest[0] = *src++;
        dest[1] = *src++;
        dest[2] = *src++;
        dest[3] = *src;
#  else
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
#  endif
#endif
        
        return v;
    }
    
    std::string Buffer::peekString(size_t offset) const
    {
        std::string v;
        
        size_t i = offset;
        uint32_t n = peek8u(i);
        i += 1;
        if(n == 255)
        {
            n = peek32u(i);
            i += 4;
        }
        
        if(n > 0)
        {
            if(_container.readable() < n + i)
            {
                throw OutOfBoundsException(__FILE__, __LINE__);
            }
            
            const char* p = reinterpret_cast<const char*>(_container.read_pos() + i);
            std::string(p, p + n).swap(v);
        }
        
        return v;
    }
    
    //
    // Read operations get data from the begining of current readable data
    // The data will be removed from buffer after read
    // Read operations will change the size of buffer
    //
    
    // Read with particular data type    
    int8_t Buffer::read8()
    {
        int8_t v = 0;            // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 1);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        v = *_container.read_pos();
        
        _container.remove(n);
        return v;
    }
    
    uint8_t Buffer::read8u()
    {
        uint8_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 1);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        v = *_container.read_pos();
        
        _container.remove(n);
        return v;
    }
    
    int16_t Buffer::read16()
    {
        int16_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 2);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    uint16_t Buffer::read16u()
    {
        uint16_t v = 0;          // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 2);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    int32_t Buffer::read32()
    {
        int32_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 4);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    uint32_t Buffer::read32u()
    {
        uint32_t v = 0;          // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 4);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    int64_t Buffer::read64()
    {
        int64_t v = 0;           // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 8);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    uint64_t Buffer::read64u()
    {
        uint64_t v = 0;          // result value
        size_t n = sizeof(v);    // bytes number
        assert(n == 8);
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    bool Buffer::readBool()
    {
        int8_t v = read8();
        return v != 0;
    }
    
    float Buffer::readFloat()
    {
        float v = 0;            // Value
        size_t n = sizeof(v);   // Bytes number
        assert(n == 4);         
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.remove(n);
        return v;
    }
    
    double Buffer::readDouble()
    {
        double v = 0;           // Value
        size_t n = sizeof(v);   // Bytes number
        assert(n == 8);         
        
        if(_container.readable() < n)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        const byte* src = _container.read_pos();
    #ifdef PP_BIG_ENDIAN
        byte* dest = reinterpret_cast<byte*>(&v) + n - 1;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest-- = *src++;
        *dest = *src;
    #else
        byte* dest = reinterpret_cast<byte*>(&v);
    #  if defined(__arm__) && defined(__linux)
        dest[4] = *src++;
        dest[5] = *src++;
        dest[6] = *src++;
        dest[7] = *src++;
        dest[0] = *src++;
        dest[1] = *src++;
        dest[2] = *src++;
        dest[3] = *src;
    #  else
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #  endif
    #endif
        
        _container.remove(n);
        return v;
    }
    
    std::string Buffer::readString()
    {
        std::string v;
        
        uint32_t n = read8u();
        if(n == 255)
        {
            n = read32u();
        }
        
        if(n > 0)
        {
            if(_container.readable() < n)
            {
                throw OutOfBoundsException(__FILE__, __LINE__);
            }
            
            const char* p = reinterpret_cast<const char*>(_container.read_pos());
            std::string s(p, p + n);
            v = s;
            _container.remove(n);
        }
        
        return v;
    }
    
    std::string Buffer::readLine()
    {
        std::string v;
        const char* p = reinterpret_cast<const char*>(_container.read_pos());
        const char* n = reinterpret_cast<const char*>(find("\n"));
        
        if(n == NULL)
        {
            throw OutOfBoundsException(__FILE__, __LINE__);
        }
        
        std::string s(p, n);
        if(s.size() > 0 && *s.rbegin() == '\r')
        {
            s.erase(s.rbegin());
        }
        v = s;
        _container.remove(n - p);
        
        return v;
    }
    
    // Read into a bytes array
    bool Buffer::readBlob(byte* b, size_t n)
    {
        if(_container.readable() < n)
        {
            return false;
        }
        
        memcpy(b, _container.read_pos(), n);
        
        _container.remove(n);
        return true;
    }
    
    //
    // Write operatons only append data to the end of current readable data
    // The buffer size will be changed after write operations 
    //
    
    // Write with particular data type     
    bool Buffer::write8(int8_t v)
    {
        size_t n = sizeof(v);
        assert(n == 1);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        *_container.write_pos() = v;
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write8u(uint8_t v)
    {
        size_t n = sizeof(v);
        assert(n == 1);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        *_container.write_pos() = v;
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write16(int16_t v)
    {
        size_t n = sizeof(v);
        assert(n == 2);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write16u(uint16_t v)
    {
        size_t n = sizeof(v);
        assert(n == 2);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write32(int32_t v)
    {
        size_t n = sizeof(v);
        assert(n == 4);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write32u(uint32_t v)
    {
        size_t n = sizeof(v);
        assert(n == 4);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write64(int64_t v)
    {
        size_t n = sizeof(v);
        assert(n == 8);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::write64u(uint64_t v)
    {
        size_t n = sizeof(v);
        assert(n == 8);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::writeBool(bool v)
    {
        return write8(static_cast<int8_t>(v));
    }
    
    bool Buffer::writeFloat(float v)
    {
        size_t n = sizeof(v);
        assert(n == 4);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::writeDouble(double v)
    {
        size_t n = sizeof(v);
        assert(n == 8);
        
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        byte* dest = _container.write_pos();
    #ifdef PP_BIG_ENDIAN
        const byte* src = reinterpret_cast<const byte*>(&v) + n - 1;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest++ = *src--;
        *dest = *src;
    #else
        const byte* src = reinterpret_cast<const byte*>(&v);
    #  if defined(__arm__) && defined(__linux)
        dest[4] = *src++;
        dest[5] = *src++;
        dest[6] = *src++;
        dest[7] = *src++;
        dest[0] = *src++;
        dest[1] = *src++;
        dest[2] = *src++;
        dest[3] = *src;
    #  else
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest++ = *src++;
        *dest = *src;
    #  endif
    #endif
        
        _container.resize(n);
        return true;
    }
    
    bool Buffer::writeString(const std::string& v)
    {
        size_t n = v.size();

        if(n > 254)
        {
            if(_container.reserve(5 + n) < (5 + n))
            {
                return false;
            }
            
            write8u(255);
            write32u((uint32_t)n);
        }
        else
        {
            if(_container.reserve(1 + n) < (1 + n))
            {
                return false;
            }
            
            write8u(n);
        }
        
        if(n > 0)
        {
            memcpy(_container.write_pos(), v.data(), n);
            _container.resize(n);
        }
        
        return true;
    }
    
    // Write with a byte array
    bool Buffer::writeBlob(byte* b, size_t n)
    {
        if(_container.reserve(n) < n)
        {
            return false;
        }
        
        memcpy(_container.write_pos(), b, n);
        
        _container.resize(n);
        return true;
    }

}
