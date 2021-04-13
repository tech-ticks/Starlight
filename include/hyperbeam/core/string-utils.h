#pragma once

#include <cstdint>
#include <alloca.h>
#include <hyperbeam/core/mem.h>
#include "il2cpp-appdata.h"

namespace hb {

    inline String* createCSharpString(const char* rawString) {
        // Converts regular strings
        return String_CreateString(nullptr, (int8_t*) rawString, nullptr);
    }

    inline String* createCSharpString(const char16_t* rawString) {
        // Converts wide strings
        return String_CreateString_2(nullptr, (uint16_t*) rawString, nullptr);
    }

    inline bool stringsEqual(String* a, String* b) {
        return String_op_Equality(a, b, nullptr);
    }

    __attribute__((always_inline)) inline char* cSharpStringToTempCharPtr(String* cSharpString) {
        auto il2CppString = reinterpret_cast<Il2CppString*>(cSharpString);

        // This function is inlined, so the memory from alloca() is freed at the end of the caller function
        char* string = (char*) alloca(il2CppString->length + 1);
        hb::memset(string, 0, il2CppString->length + 1);
        char* origChars = (char*) il2CppString->chars;

        // Convert wide string to regular string in a cheap and bad way
        for (int i = 0; i < il2CppString->length; i++) {
            string[i] = origChars[i*2];
            if (string[i] > 127) {
                string[i] = '_';
            }
        }
        return string;
    }

    String* combineStrings(String* a, String* b) {
        auto cppStrA = reinterpret_cast<Il2CppString*>(a);
        auto cppStrB = reinterpret_cast<Il2CppString*>(b);

        int totalLength = (cppStrA->length + cppStrB->length) * 2 + 2;
        auto newTempStr = (char*) alloca(totalLength);
        newTempStr[totalLength - 1] = 0;
        newTempStr[totalLength - 2] = 0;
        hb::memcpy(newTempStr, cppStrA, cppStrA->length * 2);
        hb::memcpy(newTempStr + cppStrA->length * 2, cppStrB, cppStrB->length * 2);

        return createCSharpString(newTempStr);
    }

}
