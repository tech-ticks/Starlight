#pragma once

namespace hb {
    
    struct RegisterStore final {
        // Saves all registers that must be preserved and restores them when an instance
        // of this struct goes out of scope.

        RegisterStore() {
            __asm("MOV %[var], X0"  : [var] "=r" (x0));
            __asm("MOV %[var], X1"  : [var] "=r" (x1));
            __asm("MOV %[var], X2"  : [var] "=r" (x2));
            __asm("MOV %[var], X3"  : [var] "=r" (x3));
            __asm("MOV %[var], X4"  : [var] "=r" (x4));
            __asm("MOV %[var], X5"  : [var] "=r" (x5));
            __asm("MOV %[var], X6"  : [var] "=r" (x6));
            __asm("MOV %[var], X7"  : [var] "=r" (x7));
            __asm("MOV %[var], X8"  : [var] "=r" (x8));
            __asm("MOV %[var], X9"  : [var] "=r" (x9));
            __asm("MOV %[var], X10" : [var] "=r" (x10));
            __asm("MOV %[var], X11" : [var] "=r" (x11));
            __asm("MOV %[var], X12" : [var] "=r" (x12));
            __asm("MOV %[var], X13" : [var] "=r" (x13));
            __asm("MOV %[var], X14" : [var] "=r" (x14));
            __asm("MOV %[var], X15" : [var] "=r" (x15));
        };

        ~RegisterStore() {
            __asm("MOV X0,  %[var]" : [var] "=r" (x0));
            __asm("MOV X1,  %[var]" : [var] "=r" (x1));
            __asm("MOV X2,  %[var]" : [var] "=r" (x2));
            __asm("MOV X3,  %[var]" : [var] "=r" (x3));
            __asm("MOV X4,  %[var]" : [var] "=r" (x4));
            __asm("MOV X5,  %[var]" : [var] "=r" (x5));
            __asm("MOV X6,  %[var]" : [var] "=r" (x6));
            __asm("MOV X7,  %[var]" : [var] "=r" (x7));
            __asm("MOV X8,  %[var]" : [var] "=r" (x8));
            __asm("MOV X9,  %[var]" : [var] "=r" (x9));
            __asm("MOV X10, %[var]" : [var] "=r" (x10));
            __asm("MOV X11, %[var]" : [var] "=r" (x11));
            __asm("MOV X12, %[var]" : [var] "=r" (x12));
            __asm("MOV X13, %[var]" : [var] "=r" (x13));
            __asm("MOV X14, %[var]" : [var] "=r" (x14));
            __asm("MOV X15, %[var]" : [var] "=r" (x15));
        }

        RegisterStore(const RegisterStore&) = delete;
        RegisterStore& operator=(const RegisterStore&) = delete;

        unsigned long x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15;
    };

}
