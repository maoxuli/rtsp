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

#ifndef PPUTIL_BUFFER_H
#define PPUTIL_BUFFER_H

#include <pputil/Config.h>
#include <pputil/Socket.h>

namespace pputil
{
    //
    // Exceptions throwed by Buffer's operations 
    //
    class PPUTIL_API OutOfBoundsException : public Exception
    {
    public:
        OutOfBoundsException(const std::string& file, int line);
        virtual ~OutOfBoundsException() throw();
        
        virtual std::string toString() const;
        
    protected:
        
    };
    
    //
    // A Buffer is a block of readable data reside in consecutive bytes in memory
    // Interface:
    // size()
    // peekXxxx()
    // updateXxxx()
    // readXxxx()
    // 
    // writeXxxx()
    // ensure(n)
    // resize(n)
    // 
    // 1. Read from front
    // 2. Write to the tail
    // 3. Peek or update in a random position
    // 
    class PPUTIL_API Buffer
    {
    public:
        // Buffer size is dynamic, limit is the max size        
        Buffer(size_t limit = 0);
        virtual ~Buffer();
        
        // Size of buffer, bytes number of available data
        size_t size();
        
        //
        // Peek operations get data from available data
        // Particular data type and offset determine the bytes that will be read
        // Data is not removed from the buffer after peek
        // Peek operations does not change the size of buffer
        //
        
        // Peek with particular data type 
        int8_t peek8(size_t offset = 0) const;
        uint8_t peek8u(size_t offset = 0) const;
        
        int16_t peek16(size_t offset = 0) const;
        uint16_t peek16u(size_t offset = 0) const;
        
        int32_t peek32(size_t offset = 0) const;
        uint32_t peek32u(size_t offset = 0) const;
        
        int64_t peek64(size_t offset = 0) const;
        uint64_t peek64u(size_t offset = 0) const;
        
        bool peekBool(size_t offset = 0) const;    
        float peekFloat(size_t offset = 0) const;
        double peekDouble(size_t offset = 0) const;
        
        std::string peekString(size_t offset = 0) const;
        
        //
        // Update operations modify available data
        // Particular data type and offset determine the bytes that will be modified
        // Update operations does not change the size of buffer
        //
        
        // Update with particular data type
        bool update8(int8_t v, size_t offset = 0);
        bool update8u(uint8_t v, size_t offset = 0);
        
        bool update16(int16_t v, size_t offset = 0);
        bool update16u(uint16_t v, size_t offset = 0);
        
        bool update32(int32_t v, size_t offset = 0);
        bool update32u(uint32_t v, size_t offset = 0);
        
        bool update64(int64_t v, size_t offset = 0);
        bool update64u(uint64_t v, size_t offset = 0);
        
        bool updateBool(bool v, size_t offset = 0);
        bool updateFloat(float v, size_t offset = 0);
        bool updateDouble(double v, size_t offset = 0);
        
        bool updateString(const std::string& s, size_t offset = 0);
        
        // Update with a byte array
        bool updateBlob(byte* b, size_t n, size_t offset = 0);
        
        //
        // Read operations get data from the front of available data
        // The data will be removed from buffer after read
        // Read operations will change the size of buffer
        //
        
        // Read with particular data type
        int8_t read8();
        uint8_t read8u();
        
        int16_t read16();
        uint16_t read16u();
        
        int32_t read32();
        uint32_t read32u();
        
        int64_t read64();
        uint64_t read64u();
        
        bool readBool();
        float readFloat();
        double readDouble();
        
        std::string readString();
        std::string readLine();
        
        // Read into a bytes array
        bool readBlob(byte* b, size_t n);
    
        //
        // Write operatons only append data to the end of available data
        // The buffer size will be changed after write operations 
        //

        // Write with particular data type 
        bool write8(int8_t v);
        bool write8u(uint8_t v);
        
        bool write16(int16_t v);
        bool write16u(uint16_t v);
        
        bool write32(int32_t v);
        bool write32u(uint32_t v);
        
        bool write64(int64_t v);
        bool write64u(uint64_t v);
        
        bool writeBool(bool v);
        bool writeFloat(float v);
        bool writeDouble(double v);
        
        bool writeString(const std::string& s);
        
        // Write with a byte array
        bool writeBlob(byte* b, size_t n);
        
        // Expose internal memory position for external read and write
        byte* read_pos();
        void remove(size_t n);
        
        size_t ensure(size_t n);
        byte* write_pos();
        void resize(size_t n);
        
        // Input and output with socket
        long receive(SOCKET fd);
        long send(SOCKET fd);
        
    protected:
        // Find a given character(s) in available data
        // Used in find the end of a string or line
        const byte* find(const std::string& s, size_t offset = 0) const;
           
    protected:
        //
        // A buffer is implemented with a general container
        // A container has a dynamic size to hold objects with fixed size
        // Container's interface:
        // 
        // begin()
        // readable()
        // write_pos()
        // writable()
        // reserve(n)
        // shrink()
        // swap(Buffer)
        // 
        // Plain manipulations on bytes without concerns of endianness
        // Inline for performance
        //
        
        // vector    |##########################################################|-------------|
        //         begin()                    -size()-                         end()       capacity
        //
        //
        // Container |********|xxxxxxxxxxxxxxxxxxxxxxxxx|***********************|-------------|
        //                 read_pos() -readable()-  write_pos() -writable()-  reserved 
        //         begin() read_index               write_index               size()/end() capacity 
        //
        //         read_index = read_pos - begin
        //
        
        // Todo: template the class
        class Container
        {
        public:
            Container(size_t max_size = 0)
            : _max_size(max_size)
            , _read_index(0)
            , _write_index(0)
            , _v(1024)
            {
                
            }
            
            virtual ~Container()
            {
                
            }
            
            // Reset the buffer to be empty
            // To do: shrink memory occupation 
            void reset()
            {
                _read_index = 0;
                _write_index = 0; // ? Will thrink capacity?
            }
                    
            // Swap two Buffers
            void swap(Container& c)
            {
                _v.swap(c._v);
                std::swap(_read_index, c._read_index);
                std::swap(_write_index, c._write_index);
                std::swap(_max_size, c._max_size);
            }

                    
            // Iterator for read position
            byte* read_pos()
            {
                return begin() + _read_index;
            }
                    
            // Iterator for read position
            const byte* read_pos() const 
            {
                return begin() + _read_index;
            }
                    
            // Iterator for write position
            byte* write_pos()
            {
                return begin() + _write_index;
            }
                    
            // Iterator for wirte position 
            const byte* write_pos() const
            {
                return begin() + _write_index;
            }
                    
            // Return byte number that are available to read?
            size_t readable() const
            {
                return _write_index - _read_index;
            }
                    
            // Return byte number that are free to write?
            size_t writable() const 
            {
                return _v.size() - _write_index;
            }
            
            // Remove n bytes data from read position 
            void remove(size_t n)
            {
                if(readable() > n)
                {
                    _read_index += n;
                }
                else
                {
                    _read_index = 0;
                    _write_index = 0;
                }
            }
                    
            // Ensure there is n bytes space is writable
            // Automatically extend buffer is needed
            // Not extend when n == 0
            size_t reserve(size_t n = 0)
            {
                if(n == 0 || writable() >= n)
                {
                    return writable();
                }
                
                if(_max_size > 0 && (_v.size() + n) > _max_size)
                {
                    _v.resize(_max_size);
                }
                else
                {
                    _v.resize(_v.size() + n);
                }
                        
                return writable();
            }

            // Extend n bytes to data readable 
            // Note, n is not the total size of data
            size_t resize(size_t n)
            {
                _write_index += n;
                return n;
            }
                
        protected:
            // Internal use only
            byte* begin()
            {
                return &_v[0];
            }
            
            const byte* begin() const
            {
                return &_v[0];
            }
            
            // Managed with a vector of object  
            // Only support byte type at this moment
            // Todo: template
            std::vector<byte> _v;
            size_t _read_index; // Index of first readable object
            size_t _write_index; // Index of first writable object
            size_t _max_size; // Limitation of the size of the container
        };
        
        Container _container;
    };
}

#endif
