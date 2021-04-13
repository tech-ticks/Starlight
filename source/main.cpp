// The compiler on macOS complains without this
#define __arm64__

#include "il2cpp-appdata.h"

#include <hyperbeam/core.h> // Includes the new and delete operators and more

// Note that there's only a header file for logging.
// Using a seperate translation unit messes things up and leads to crashes for some reason.
// TODO: investigate why this is happening
#define ENABLE_LOGGING
#include <hyperbeam/logging.h>
#include <hyperbeam/fs.h>
#include <hyperbeam/reflect.h>
#include <hyperbeam/unity.h>
#include <rtdx/starters.h>

#include "SentryDutyMinigame.h"

// The game crashes without a zero memory area at the start of the executable. Not sure why, maybe the stack isn't set up?
char reserve_[0x100000];

struct CustomActorData {
    char16_t symbolName[32];
    char16_t debugName[32];
    uint16_t pokemonIndex;
    uint16_t formType;
    bool isFemale;
    uint8_t partyId;
    uint16_t warehouseId;
    int specialNameHash;
};

static SentryDutyMinigame* sentryDuty = nullptr;
static bool interruptSleep = false; // Whether the next "Sleep" command should be interrupted (after the minigame)

void hookGroundManagerOnEnable(GroundManager* thisPtr) {
    // Called early during game initialization. Use this hook for custom initialization logic.

    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // Initialize the TCP log server (connect with "python scripts/logClient.py [Switch IP]")
    LOG_INIT();
    LOG("Logger initialized.\n");

    if (!hb::fs::mountRomFs()) {
        LOG("Failed to mount RomFS!");
        return;
    }

    auto npcDatabase = thisPtr->groundTonwNpcDatabase_; // sic

    // GroundTownNpcDatabase is a Unity ScriptableObject, so we can simply overwrite its data from a JSON file
    auto json = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/town_npc_database.json");
    if (json) {
        auto jsonString = hb::createCSharpString(json);
        JsonUtility_FromJsonOverwrite(jsonString, (Object*) npcDatabase, JsonUtility_FromJsonOverwrite__MethodInfo);
        delete json;

        LOG("Loaded the town NPC database.\n");
    } else {
        LOG("Failed to load the town NPC database!\n");
    }

    // Uncomment this code to dump the NPC database to JSON:
    // auto json = JsonUtility_ToJson_1((Object*) npcDatabase, true, JsonUtility_ToJson_1__MethodInfo);
    // LOG(hb::cSharpStringToTempCharPtr(json));
    // LOG("\n");
}

void hookGameObjectManagerUpdate() {
    // Called on every frame

    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // If the minigame is being played, update it
    if (sentryDuty) {
        sentryDuty->update();
        if (sentryDuty->isFinished()) {
            sentryDuty->destroy();
            delete sentryDuty;
            sentryDuty = nullptr;
            interruptSleep = true;
            LOG("Destoyed the minigame.\n");
        }
    }
}

void hookSpecialFuncMoveNext(ActCommand_SpecialFunc_callAction_d_4* thisPtr) {
    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    int specialFuncType = (int) thisPtr->__4__this->specialFuncType;
    LOGF("SpecialFunc call: %d\n", specialFuncType);

    if (specialFuncType == 3000) {
        // Spawn the minigame if SpecialFunc 3000 is called in a script
        sentryDuty = new SentryDutyMinigame();
    }

     // Restore the old value of X0
    __asm ("MOV X0, %[value]" : [value] "=r" (thisPtr));
}

void hookSleepMoveNext(ActCommand_Sleep_callAction_d_1* thisPtr) {
    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // The game would normally continue executing the script after our SpecialFunc, causing the script
    // to keep playing during the minigame.
    // This can be circumvented by adding a really long "Sleep" command after the SpecialFunc command and
    // setting the elapsed time to the sleep duration when the minigame is finished, continuing the script.

    if (interruptSleep) {
        thisPtr->_nowT_5__2 = thisPtr->__4__this->timeSec;
        interruptSleep = false;
        LOG("Interrupted sleep.\n");
    }

     // Restore the old value of X0
    __asm ("MOV X0, %[value]" : [value] "=r" (thisPtr));
}

void customPegasusActDatabaseStaticConstructor() {
    // Read actor database from a file

    uint32_t actorCount = 0;
    CustomActorData* actors;

    char* actorDatabaseFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/actor_database.bin");
    if (actorDatabaseFile) {
        hb::memcpy(&actorCount, actorDatabaseFile, sizeof(uint32_t));
        actors = new CustomActorData[actorCount];
        hb::memcpy(actors, actorDatabaseFile + sizeof(uint32_t),
            sizeof(CustomActorData) * actorCount);

        delete actorDatabaseFile;
    } else {
        LOG("Failed to read actor_database.bin!");
        return;
    }

    // IL2CPP lazily initializes method metadata, so the methods that need to be accessed here are not loaded yet.
    // Call the initialization function manually to make sure that they are loaded.
    // The parameter values can be found by looking for calls to il2cpp_initialize_method_metadata()
    // before objects are constructed with il2cpp_object_new() in a reverse-engineering tool.
    // TODO: is there a less "magic" way to do this? this is probably done automatically when calling methods with Reflection?
    il2cpp_initialize_method_metadata(0x508e);
    il2cpp_initialize_method_metadata(0x369);

    // Create a new System.Collections.Generic.List<PegasusActDatabase.ActorData>
    auto actorDataList = (List_1_PegasusActDatabase_ActorData_ *) il2cpp_object_new(List_1_PegasusActDatabase_ActorData___TypeInfo);
    // The constructor is called List_1_System_IO_Directory_SearchData___ctor because it's shared between
    // multiple classes. The MethodInfo must be passed to shared functions.
    // (see https://katyscode.wordpress.com/2021/01/14/il2cppinspector-tutorial-working-with-code-in-il2cpp-dll-injection-projects/)
    List_1_System_IO_Directory_SearchData___ctor((List_1_System_IO_Directory_SearchData_ *) actorDataList, 
        List_1_PegasusActDatabase_ActorData___ctor__MethodInfo);
    
    for (uint32_t i = 0; i < actorCount; i++) {
        auto& actorData = actors[i];
        
        auto cSharpActor = (PegasusActDatabase_ActorData*) il2cpp_object_new(PegasusActDatabase_ActorData__TypeInfo);
        cSharpActor->symbolName = hb::createCSharpString(actorData.symbolName);
        cSharpActor->debug_name = hb::createCSharpString(actorData.debugName);
        cSharpActor->raw_pokemonIndex = (Index__Enum) actorData.pokemonIndex;
        cSharpActor->raw_formType = (FormType__Enum) actorData.formType;
        cSharpActor->bIsFemale = actorData.isFemale;
        cSharpActor->opt_partyId = (PegasusActDatabase_ActorData_PartyID__Enum) actorData.partyId;

        // Create a PokemonWarehouseId instance
        auto warehouseId = (PokemonWarehouseId*) il2cpp_object_new(PokemonWarehouseId__TypeInfo);
        PokemonWarehouseId__ctor_2(warehouseId, (FixedWarehouseId__Enum) actorData.warehouseId, nullptr);
        cSharpActor->opt_warehouseId = warehouseId;

        // Create a TextId instance
        auto specialName = (TextId*) il2cpp_object_new(TextId__TypeInfo);
        TextId__ctor_2(specialName, actorData.specialNameHash, nullptr);
        cSharpActor->opt_specialName = specialName;

        // Add to list
        List_1_System_IO_Directory_SearchData__Add((List_1_System_IO_Directory_SearchData_ *) actorDataList,
            (Directory_SearchData*) cSharpActor, List_1_PegasusActDatabase_ActorData__Add__MethodInfo);
    }

    // Save the list in the static actorDataList_ field
    PegasusActDatabase__TypeInfo->static_fields->actorDataList_ = actorDataList;
    delete actors;

    LOGF("Initialized the actor database with %d actors.\n", actorCount);
}

int main(int argc, char** argv) {
    // Do nothing, implementing main() is required for compilation
}
