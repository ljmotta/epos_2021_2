// EPOS System/Application Containers and Dynamic Memory Allocators Declarations

#ifndef __system_h
#define __system_h

#include <utility/string.h>
#include <utility/heap.h>
#include <system/info.h>
#include <memory.h>

__BEGIN_SYS

class Application
{
    friend class Init_Application;
    friend void * ::malloc(size_t);
    friend void ::free(void *);

private:
    static void init();

private:
    static char _preheap[sizeof(Heap)];
    static Heap * _heap;
};

class System
{
    friend class Init_System;                                                   // for _heap
    friend class Init_Application;                                              // for _heap with multiheap = false
    friend void CPU::Context::load() const volatile;
    friend void * ::malloc(size_t);						// for _heap
    friend void ::free(void *);							// for _heap
    friend void * ::operator new(size_t, const EPOS::System_Allocator &);	// for _heap
    friend void * ::operator new[](size_t, const EPOS::System_Allocator &);	// for _heap
    friend void ::operator delete(void *);					// for _heap
    friend void ::operator delete[](void *);					// for _heap
    friend void * shared_memory();

public:
    static System_Info * const info() { assert(_si); return _si; }
    static Segment * _shared_memory;

private:
    static void init();

private:
    static System_Info * _si;
    static char _preheap[(Traits<System>::multiheap ? sizeof(Segment) : 0) + sizeof(Heap)];
    static Segment * _heap_segment;
    static Heap * _heap;
};

__END_SYS

extern "C"
{
    // Standard C Library allocators
    inline void * malloc(size_t bytes) {
        __USING_SYS;
        if(Traits<System>::multiheap)
            return Application::_heap->alloc(bytes);
        else
            return System::_heap->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
        void * ptr = malloc(n * bytes);
        memset(ptr, 0, n * bytes);
        return ptr;
    }

    inline void free(void * ptr) {
        __USING_SYS;
        if(Traits<System>::multiheap)
            Heap::typed_free(ptr);
        else
            Heap::untyped_free(System::_heap, ptr);
    }
}

inline void * shared_memory() {
    __USING_SYS;
    return Address_Space(MMU::current()).attach(System::_shared_memory);
}

// C++ dynamic memory allocators and deallocators
inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes);
}

inline void * operator new(size_t bytes, const EPOS::System_Allocator & allocator) {
    return _SYS::System::_heap->alloc(bytes);
}

inline void * operator new[](size_t bytes, const EPOS::System_Allocator & allocator) {
    return _SYS::System::_heap->alloc(bytes);
}

// Delete cannot be declared inline due to virtual destructors
void operator delete(void * ptr);
void operator delete[](void * ptr);
void operator delete(void * ptr, size_t bytes);
void operator delete[](void * ptr, size_t bytes);

#endif
