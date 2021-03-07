// The compiler on macOS complains without this
#define __arm64__

#include <cstddef>

#include "il2cpp-appdata.h"

#define PRESET_INDEX_FIXED 10

void hookClearFixedPartyOrganization() {
    // Restore the instruction that was replaced
    __asm ("MOV W19, W1");

    // Add Jirachi to all fixed organizations
    Bind_Organization_SetPresetMember(PRESET_INDEX_FIXED, 2, (int) FixedWarehouseId__Enum::JIRAACHI, 0, nullptr);
}

void hookDungeonLoopMoveNext(DungeonParameter* dungeonParameter) {
    // The DungeonParameter pointer is passed as an argument since it's stored in X0 at that point
    // Add Absol and Mew as guests. This is always called since the check for Gengar is disabled in the patch.
    DungeonParameter_AddExtraFixedPokemon(dungeonParameter, FixedWarehouseId__Enum::MYUU, ExtraPokemonType__Enum::GUEST, nullptr);
    DungeonParameter_AddExtraFixedPokemon(dungeonParameter, FixedWarehouseId__Enum::ABUSORU, ExtraPokemonType__Enum::GUEST, nullptr);
}

int main(int argc, char** argv) {
    // Do nothing on startup
}
