#pragma once

#include <new>
#include <hyperbeam/core/mem.h>

void* operator new(size_t size) {
    return hb::malloc(size);
}

void operator delete(void* ptr) noexcept {
    hb::free(ptr);
}
