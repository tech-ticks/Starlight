// Memory functions from the main executable (the functions from libnx don't seem to work)
#pragma once

// The compiler on macOS complains without this
#define __arm64__

#include <cstdint>
#include <cstddef>

extern "C" int rtdx_vsnprintf(char * s, size_t n, const char* format, va_list arg);
extern "C" int rtdx_snprintf (char * s, size_t n, const char * format, ...);
