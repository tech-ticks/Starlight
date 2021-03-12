// Memory functions from the main executable (the functions from libnx don't seem to work)
#pragma once

// The compiler on macOS complains without this
#define __arm64__

#include <cstdint>
#include <cstddef>

extern "C" void* rtdx_malloc(size_t size);
extern "C" void rtdx_free(void* data);
extern "C" void* rtdx_memalign(size_t alignment, size_t size);
extern "C" void rtdx_memcpy(void* dest, void const* src, size_t count);
extern "C" void rtdx_memset(void* dest, int value, size_t count);
