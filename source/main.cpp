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

static uint32_t organizationCount = 0;
static CustomOrganization* organizations;

void hookGameDataConstructor(GameData* thisPtr, void* methodInfo) {
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

    if (hb::fs::mountRomFs()) {
        // Read fixed preset teams ("organizations") from a file
        char* file = (char*) hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/fixed_organization.bin");
        if (file) {
            hb::memcpy(&organizationCount, file, sizeof(uint32_t));
            organizations = new CustomOrganization[organizationCount];
            hb::memcpy(organizations, file + sizeof(uint32_t),
                sizeof(CustomOrganization) * organizationCount);

            delete file;
        } else {
            LOG("Failed to read fixed_organization.bin!");
        }
        hb::fs::unmountRomFs();
    } else {
        LOG("Failed to mount RomFS!");
    }

    // The previous state is restored automatically
}

Organization_OrganizationType__Enum customGetOrganizationType(String* organizationSymbol) {
    // The return value affects the result of Organization_GetOrganizationType
    // since this function is branched into with "B" instead of "BL".

    // Find the organization that matches the symbol string
    uint32_t index;
    for (index = 0; index < organizationCount; index++) {
        auto name = hb::createCSharpString(organizations[index].name);
        if (hb::stringsEqual(organizationSymbol, name)) {
            break;
        }
    }

    if (index < organizationCount) {
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
        auto member = organizations[organizationType].members[i];

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

    // The function is called NativeMessageWindowCtrl__get_Instance because it's shared between
    // multiple classes. The MethodInfo must be passed to shared functions.
    // (see https://katyscode.wordpress.com/2021/01/14/il2cppinspector-tutorial-working-with-code-in-il2cpp-dll-injection-projects/)
    auto organizationInstance = (Organization*) Singleton_1_NativeMessageWindowCtrl__get_Instance(Singleton_1_Organization__get_Instance__MethodInfo);
    // Get the current organization index
    size_t organizationType = (size_t) organizationInstance->type_;
    
    // Add all guests that were previously read from the file
    for (uint32_t i = 0; i < ORGANIZATION_GUEST_NUM; i++) {
        auto guest = organizations[organizationType].guests[i];
        if (guest.index != (uint16_t) FixedWarehouseId__Enum::NULL_1) {
            DungeonParameter_AddExtraFixedPokemon(dungeonParameter, (FixedWarehouseId__Enum) guest.index, (ExtraPokemonType__Enum) guest.type, nullptr);
        }
    }

    LOGF("Added guest Pokémon for organization type %d.\n", organizationType);
}

int main(int argc, char** argv) {
    // Do nothing, implementing main() is required for compilation
}
