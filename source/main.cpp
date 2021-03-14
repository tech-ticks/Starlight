// The compiler on macOS complains without this
#define __arm64__

#include "il2cpp-appdata.h"

#include <hyperbeam/core.h> // Includes hb::RegisterStore, the new and delete operators and more

// Note that there's only a header file for logging.
// Using a seperate translation unit messes things up and leads to crashes for some reason.
// TODO: investigate why this is happening
#define ENABLE_LOGGING
#include <hyperbeam/logging.h>
#include <hyperbeam/fs.h>

#define PRESET_INDEX_FIXED 10

#define ORGANIZATION_MEMBER_NUM 3
#define ORGANIZATION_GUEST_NUM 8
#define ORGANIZATION_NAME_LENGTH 32

char reserve_[0x100000]; // The game crashes without a zero memory area at the start of the executable. Not sure why, maybe the stack isn't set up?

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

struct alignas(4) OrganizationMember {
    uint16_t index;
    bool locked;
};

struct OrganizationGuest {
    uint16_t index;
    uint16_t type;
};

struct CustomOrganization {
    char name[ORGANIZATION_NAME_LENGTH];
    OrganizationMember members[ORGANIZATION_MEMBER_NUM];
    OrganizationGuest guests[ORGANIZATION_GUEST_NUM];
};

static struct GlobalState {
    uint32_t organizationCount = 0;
    CustomOrganization* organizations;
} state;

void hookGameDataConstructor(GameData* thisPtr) {
    // Called early during game initialization. Use this hook for custom initialization logic.

    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // hb::RegisterStore automatically saves registers that must be preserved and restores them in the
    // destructor when the struct goes out of scope (at the end of the function). This pattern makes
    // writing safe hooks easier, but note that this is much slower than manually restoring the registers, 
    // so don't use hb::RegisterStore in hot loops.
    hb::RegisterStore registerStore;

    // Initialize the TCP log server (connect with "python scripts/logClient.py [Switch IP]")
    LOG_INIT();
    LOG("Logger initialized.\n");

    if (!hb::fs::mountRomFs()) {
        LOG("Failed to mount RomFS!");
        return;
    }

    // Read fixed preset teams ("organizations") from a file
    char* organizationsFile = (char*) hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/fixed_organization.bin");
    if (organizationsFile) {
        hb::memcpy(&state.organizationCount, organizationsFile, sizeof(uint32_t));
        state.organizations = new CustomOrganization[state.organizationCount];
        hb::memcpy(state.organizations, organizationsFile + sizeof(uint32_t),
            sizeof(CustomOrganization) * state.organizationCount);

        delete organizationsFile;
    } else {
        LOG("Failed to read fixed_organization.bin!");
    }

    if (!hb::fs::unmountRomFs()) {
        LOG("Failed to unmount RomFS!");
    }
}

void customPegasusActDatabaseStaticConstructor() {
    // Read actor database from a file
    if (!hb::fs::mountRomFs()) {
        LOG("Failed to mount RomFS!");
        return;
    }

    uint32_t actorCount = 0;
    CustomActorData* actors;

    char* actorDatabaseFile = (char*) hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/actor_database.bin");
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

    if (!hb::fs::unmountRomFs()) {
        LOG("Failed to unmount RomFS!");
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

Organization_OrganizationType__Enum customGetOrganizationType(String* organizationSymbol) {
    // The return value affects the result of Organization_GetOrganizationType
    // since this function is branched into with "B" instead of "BL".

    // Find the organization that matches the symbol string
    uint32_t index;
    for (index = 0; index < state.organizationCount; index++) {
        auto name = hb::createCSharpString(state.organizations[index].name);
        if (hb::stringsEqual(organizationSymbol, name)) {
            break;
        }
    }

    if (index < state.organizationCount) {
        // hb::cSharpStringToTempCharPtr creates a temporary buffer on the stack so it's deleted at the end of
        // the function, no need to delete it manually.
        LOGF("Resolved organization \"%s\" with index %d\n", hb::cSharpStringToTempCharPtr(organizationSymbol), index);
        return (Organization_OrganizationType__Enum) index;
    } else {
        LOGF("WARNING: Unknown organization symbol \"%s\".\n", hb::cSharpStringToTempCharPtr(organizationSymbol));
        return Organization_OrganizationType__Enum::FREE;
    }
}

void customClearFixedPartyOrganization(Organization* thisPtr, bool resetFreeMember) {
    // Apply the organization members that were loaded from the file
    size_t organizationType = (size_t) thisPtr->type_;
    for (uint32_t i = 0; i < ORGANIZATION_MEMBER_NUM; i++) {
        auto member = state.organizations[organizationType].members[i];

        Bind_Organization_SetOrganizationMemberLocked(i, member.locked, nullptr);
        if (!resetFreeMember || member.locked) {
            LOGF("Set member %d (locked: %d) to slot %d.\n", member.index, member.locked, i);
            Bind_Organization_SetPresetMember(PRESET_INDEX_FIXED, i, member.index, 0, nullptr);
        }
    }

    LOGF("Applied member Pokémon to organization type %d.\n", organizationType);
}

void hookDungeonLoopMoveNext(DungeonParameter* dungeonParameter) {
    // The DungeonParameter pointer is passed as an argument since it's stored in X0 at this point
    // This is always called since the check for Gengar is disabled in the patch.
    // No need to use hb::RegisterStore here because we're replacing another method call.

    // Shared method; see comment on list constructor above
    auto organizationInstance = (Organization*) Singleton_1_NativeMessageWindowCtrl__get_Instance((MethodInfo*) Singleton_1_Organization__get_Instance__MethodInfo);
    // Get the current organization index
    size_t organizationType = (size_t) organizationInstance->type_;
    
    // Add all guests that were previously read from the file
    for (uint32_t i = 0; i < ORGANIZATION_GUEST_NUM; i++) {
        auto guest = state.organizations[organizationType].guests[i];
        if (guest.index != (uint16_t) FixedWarehouseId__Enum::NULL_1) {
            DungeonParameter_AddExtraFixedPokemon(dungeonParameter, (FixedWarehouseId__Enum) guest.index,
                (ExtraPokemonType__Enum) guest.type, nullptr);
        }
    }

    LOGF("Added guest Pokémon for organization type %d.\n", organizationType);
}

int main(int argc, char** argv) {
    // Do nothing, implementing main() is required for compilation
}
