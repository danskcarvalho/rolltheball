//
//  buffer.h
//  RollerBallCore
//
//  Created by Danilo Santos de Carvalho on 21/06/13.
//  Copyright (c) 2013 Danilo Carvalho. All rights reserved.
//

#ifndef __RollerBallCore__buffer__
#define __RollerBallCore__buffer__

#include "base.h"
#include <atomic>

namespace rb {
    class buffer {
    private:
        struct buffer_internals {
        public:
            uint8_t* _bytes;
            size_t _size;
        private:
            std::atomic_ulong _refCount;
        public:
            ~buffer_internals();
            buffer_internals();
            buffer_internals(const uint8_t* bytes, size_t size);
            buffer_internals(const std::vector<uint8_t>& bytes);
            buffer_internals(const rb_string& encoded);
            void add_ref();
            void remove_ref(buffer_internals** pointer);
        };
        buffer_internals* _internals;
    public:
        ~buffer();
        buffer();
        buffer(const void* bytes, size_t size);
        buffer(const std::vector<uint8_t>& bytes);
        buffer(const rb_string& encoded);
        buffer(const buffer& other); //copy constructor
        buffer(buffer&& other); //move constructor
        buffer& operator=(const buffer& other);
        buffer& operator=(buffer&& other);
        
        size_t size() const;
        uint8_t operator[](size_t index) const;
        void copy_to_vector(std::vector<uint8_t>& bytes) const;
        void copy_to_cbuffer(void* cbuffer) const;
        const void* internal_buffer() const;
        rb_string encode() const;
    };
    
    bool operator==(const buffer& o1, const buffer& o2);
    bool operator!=(const buffer& o1, const buffer& o2);
    bool operator<(const buffer& o1, const buffer& o2);
    bool operator>(const buffer& o1, const buffer& o2);
    bool operator<=(const buffer& o1, const buffer& o2);
    bool operator>=(const buffer& o1, const buffer& o2);
}

namespace std {
    template<>
    struct hash<rb::buffer>{
        size_t operator()(const rb::buffer& obj) const;
    };
}

#endif /* defined(__RollerBallCore__buffer__) */
