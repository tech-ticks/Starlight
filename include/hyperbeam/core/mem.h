#pragma once

#include <cstdint>
#include <rtdx/mem.h>

namespace hb {

    inline void* malloc(size_t size) { return rtdx_malloc(size); }
    inline void free(void* data) { rtdx_free(data); }
    inline void* memalign(size_t alignment, size_t size) { return rtdx_memalign(alignment, size); }
    inline void memcpy(void* dest, void const* src, size_t count) { rtdx_memcpy(dest, src, count); }
    inline void memset(void* dest, int value, size_t count) { rtdx_memset(dest, value, count); }

}
