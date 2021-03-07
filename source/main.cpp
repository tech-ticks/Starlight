// The compiler on macOS complains without this
#define __arm64__

#include <cstddef>

//#include "il2cpp-appdata.h"

#include <stdio.h>
#include <math.h>

#define PRESET_INDEX_FIXED 10

static int y = 0;

void Bind_Organization_SetPresetMember(int32_t presetIndex, int32_t memberId, int32_t warehouseId, int32_t formIndex, void * method);

int other(int x) {
    return x + y + 4 % 23;
}

void patchMain() {
    // Restore the instruction that was replaced
    __asm ("MOV X19, X0");

    // Add Jirachi to all fixed organizations
    Bind_Organization_SetPresetMember(PRESET_INDEX_FIXED, 2, (int) /*FixedWarehouseId__Enum::JIRAACHI*/0x0000001c, 0, nullptr);

    /*int x;
    for (unsigned long i = 0; i < 1000000; i++) {
        x += i / 200 % 256 + 4 + other(x) + other(x + 6);
        for (int j = 0; j < 1000; j++) {
            x += j + other(x+j) + other(j);
        }
    }
    y = x;*/
}

int main(int argc, char** argv) {
    printf("test");
}
