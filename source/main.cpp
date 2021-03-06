// The compiler on macOS complains without this
#define __arm64__

#include <cstddef>

//#include "il2cpp-appdata.h"

#include <stdio.h>

void patchMain() {
    //CharacterModel_SetScarfVisible(nullptr, true, nullptr);
    printf("test patch");
}

int main(int argc, char** argv) {
    printf("test");
}
