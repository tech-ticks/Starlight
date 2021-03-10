// The compiler on macOS complains without this
#define __arm64__

#include "il2cpp-appdata.h"

// Note that there's only a header file for logging.
// Using a seperate translation unit messes things up and leads to crashes for some reason.
// TODO: investigate why this is happening
#define ENABLE_LOGGING
#include <hyperbeam/logging.h>

#define PRESET_INDEX_FIXED 10

void hookGameDataConstructor(GameData* thisPtr, void* methodInfo) {
    // Called early during game initialization. Use this hook for custom initialization logic.

    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // Initialize the TCP log server (connect with "python scripts/logClient.py [Switch IP]")
    LOG_INIT();
    LOG("Logger initialized.\n");
    LOGF("Printing an integer: %d\n", 5);
}

void hookClearFixedPartyOrganization() {
    // Restore the instruction that was replaced
    __asm("MOV W19, W1");

    // Add Jirachi to all fixed organizations
    Bind_Organization_SetPresetMember(PRESET_INDEX_FIXED, 2, (int) FixedWarehouseId__Enum::JIRAACHI, 0, nullptr);

    LOG("Added Jirachi to the party.\n");
}

void hookDungeonLoopMoveNext(DungeonParameter* dungeonParameter) {
    // The DungeonParameter pointer is passed as an argument since it's stored in X0 at this point
    // Add Absol and Mew as guests. This is always called since the check for Gengar is disabled in the patch.
    DungeonParameter_AddExtraFixedPokemon(dungeonParameter, FixedWarehouseId__Enum::MYUU, ExtraPokemonType__Enum::GUEST, nullptr);
    DungeonParameter_AddExtraFixedPokemon(dungeonParameter, FixedWarehouseId__Enum::ABUSORU, ExtraPokemonType__Enum::GUEST, nullptr);

    LOG("Added Mew and Absol as guests.\n");
}

int main(int argc, char** argv) {
    // Do nothing on startup, implementing main() is required for compilation
}
