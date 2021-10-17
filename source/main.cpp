// The compiler on macOS complains without this
#define __arm64__

#include "il2cpp-appdata.h"

#include <hyperbeam/core.h> // Includes the new and delete operators and more

#if !defined(NDEBUG)
    #define ENABLE_LOGGING
#endif

// Note that there's only a header file for logging.
// Using a seperate translation unit messes things up and leads to crashes for some reason.
// TODO: investigate why this is happening
#include <hyperbeam/logging.h>
#include <hyperbeam/fs.h>
#include <hyperbeam/message.h>
#include <rtdx/starters.h>

#define PRESET_INDEX_FIXED 10

#define ORGANIZATION_MEMBER_NUM 3
#define ORGANIZATION_GUEST_NUM 8
#define ORGANIZATION_NAME_LENGTH 32

// The game crashes without a zero memory area at the start of the executable. Not sure why, maybe the stack isn't set up?
// TODO: Related to this? https://github.com/switchbrew/libnx/blob/master/nx/source/runtime/init.c#L38
char reserve_[0x100000];

struct DefaultStarters {
    uint16_t playerIndex;
    int16_t playerGender;
    uint16_t partnerIndex;
    int16_t partnerGender;
};

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
    bool overrideDefaultStarters = false;
    DefaultStarters defaultStarters;
} state;

void hookGroundManagerOnEnable(GroundManager* thisPtr) {
    // Called early during game initialization. Use this hook for custom initialization logic.

    // Restore the instruction that was replaced
    __asm("MOV X19, X0");

    // Initialize the TCP log server (connect with "python scripts/logClient.py [Switch IP]")
    LOG_INIT();
    LOG("Logger initialized.\n");

    if (!hb::fs::mountRomFs()) {
        LOG("Failed to mount RomFS!\n");
        return;
    }

    // Read fixed preset teams ("organizations") from a file
    char* organizationsFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/fixed_organization.bin");
    if (organizationsFile) {
        hb::memcpy(&state.organizationCount, organizationsFile, sizeof(uint32_t));
        state.organizations = new CustomOrganization[state.organizationCount];
        hb::memcpy(state.organizations, organizationsFile + sizeof(uint32_t),
            sizeof(CustomOrganization) * state.organizationCount);

        delete organizationsFile;
    } else {
        LOG("Failed to read fixed_organization.bin!\n");
    }

    char* startersFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/starters.bin");
    if (startersFile) {
        hb::memcpy(rtdx_starters, startersFile, sizeof(Starter) * RTDX_STARTER_COUNT);
        delete startersFile;
    } else {
        LOG("Failed to read starters.bin!\n");
    }

    LOG("Starter list: ");
    for (int i = 0; i < RTDX_STARTER_COUNT; i++) {
        LOGF("%d, ", rtdx_starters[i].pokemonId);
    }
    LOG("\n");

    char* defaultStartersFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/default_starters.bin");
    if (startersFile) {
        hb::memcpy(&state.defaultStarters, defaultStartersFile, sizeof(DefaultStarters));
        state.overrideDefaultStarters = true;
        delete defaultStartersFile;
    } else {
        LOG("Failed to read default_starters.bin, will fall back to hardcoded default starters.\n");
        state.defaultStarters.playerIndex = (uint16_t) Index__Enum::PIKACHUU; // Pikachu
        state.defaultStarters.partnerIndex = (uint16_t) Index__Enum::FUSHIGIDANE; // Bulbasaur
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
        LOG("Failed to read town_npc_database.json!\n");
    }

    // Restore the old value of X0
    __asm ("MOV X0, %[value]" : [value] "=r" (thisPtr));
}

void customPegasusActDatabaseStaticConstructor() {
    uint32_t actorCount = 0;
    CustomActorData* actors;

    // Read actor database from a file
    char* actorDatabaseFile = hb::fs::readEntireFile("hyperbeam_romfs:/Data/StreamingAssets/custom_data/actor_database.bin");
    if (actorDatabaseFile) {
        hb::memcpy(&actorCount, actorDatabaseFile, sizeof(uint32_t));
        actors = new CustomActorData[actorCount];
        hb::memcpy(actors, actorDatabaseFile + sizeof(uint32_t), sizeof(CustomActorData) * actorCount);

        delete actorDatabaseFile;
    } else {
        LOG("Failed to read actor_database.bin!\n");
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
    List_1_PegasusActDatabase_ActorData___ctor(actorDataList, List_1_PegasusActDatabase_ActorData___ctor__MethodInfo);
    
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
    if (state.organizations == nullptr || state.organizationCount <= 0) {
        LOGF("No custom organization types present, falling back to native hardcoded values.\n");
        return Organization_GetOrganizationType(organizationSymbol, nullptr);
    }

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
     if (state.organizations == nullptr || state.organizationCount <= 0) {
        LOGF("No custom organizations present, falling back to native hardcoded values.\n");
        Organization_ClearFixedPartyOrganization(thisPtr, resetFreeMember, nullptr);
        return;
    }

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

static bool firstTimeDungeonEntered = false;

void hookDungeonLoopMoveNext(DungeonParameter* dungeonParameter) {
    // The DungeonParameter pointer is passed as an argument since it's stored in X0 at this point

    if (state.organizations != nullptr && state.organizationCount > 0) {
        auto organizationInstance = Singleton_1_Organization__get_Instance((MethodInfo*) Singleton_1_Organization__get_Instance__MethodInfo);
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

    firstTimeDungeonEntered = true;

    // Restore the old value of X0
    __asm ("MOV X0, %[value]" : [value] "=r" (dungeonParameter));
}

extern "C" void nativeInitializeWithChangeLanguage();

void hookInitializeWithChangeLanguage() {
    // Original call
    nativeInitializeWithChangeLanguage();

    Bind_PokemonWarehouse_InitializeHero((int) state.defaultStarters.playerIndex,
        (int) state.defaultStarters.playerGender, nullptr);
    Bind_PokemonWarehouse_InitializePartner((int) state.defaultStarters.partnerIndex,
        (int) state.defaultStarters.partnerGender, nullptr);

    LOGF("Initialized save game with default starters.\n");

    il2cpp_initialize_method_metadata(0x30DDu);
    il2cpp_initialize_method_metadata(0x378u);

    auto warehouse = Singleton_1_PokemonWarehouse__get_Instance(
        (MethodInfo*) Singleton_1_PokemonWarehouse__get_Instance__MethodInfo);
    
    auto heroWarehouseStatus = (PokemonWarehouseStatus*) PokemonWarehouse_GetHeroStatus(warehouse, nullptr);
    auto partnerWarehouseStatus = (PokemonWarehouseStatus*) PokemonWarehouse_GetPartnerStatus(warehouse, nullptr);

    auto heroStatus = (PokemonStatus*) PokemonWarehouseStatus_GetStatus(heroWarehouseStatus, nullptr);
    auto partnerStatus = (PokemonStatus*) PokemonWarehouseStatus_GetStatus(partnerWarehouseStatus, nullptr);

    PokemonStatus_SetName(heroStatus, hb::message::getCommonString(
        (int) TextIDHash__Enum::DEBUG_MENU__DEBUG_HERO_NAME), nullptr);
    PokemonStatus_SetName(partnerStatus, hb::message::getCommonString(
        (int) TextIDHash__Enum::DEBUG_MENU__DEBUG_PARTNER_NAME), nullptr);

    Bind_AdventureLog_SetMetPokemon(state.defaultStarters.playerIndex, nullptr);
    Bind_AdventureLog_SetMetPokemon(state.defaultStarters.partnerIndex, nullptr);
}

typedef void (*setBelly_t) (uint64_t* nativeStatusThisPtr, int belly);

void setBelly(PokemonStatus* status, int newBelly) {
    uint64_t nativePointer = StatusBase_get_NativePoineterId((StatusBase*) status, nullptr);

    // Based on decompilation of a similar function call, which is why it looks so ugly
    // I think this is calling into the vtable (?)
    auto nativeSetBelly = (setBelly_t) (*(uint64_t*)(*(uint64_t*)nativePointer + 128LL));
    nativeSetBelly((uint64_t*) nativePointer, newBelly);
}

void forceUpdateBellyInUI(PokemonStatus* status) {
    uint32_t nativeUniqueId = StatusBase_get_UniqueId((StatusBase*) status, nullptr);
    auto belly = PokemonStatus_GetBelly(status, nullptr);

    auto dungeonEngine = DungeonEngine__TypeInfo->static_fields->s_insntance;
    auto topScreen = dungeonEngine->m_dungeonTopScreen;
    auto displayStatusArray = topScreen->m_aDisplayStatus;

    auto dirtyFlagArray = topScreen->m_dirtyFlag->aStatus;
    for (int i = 0; i < (int) displayStatusArray->max_length; i++) {
        if (displayStatusArray->vector[i]->nativeUniqueId == nativeUniqueId) {
            displayStatusArray->vector[i]->belly = belly;
            dirtyFlagArray->vector[i]->bBelly = true;
        }
    }
}

void recolorBellyGauge() {
    auto dungeonEngine = DungeonEngine__TypeInfo->static_fields->s_insntance;
    auto topScreen = dungeonEngine->m_dungeonTopScreen;

    auto mainTransform = GameObject_get_transform(topScreen->m_obj, nullptr);
    for (size_t i = 0; i <  topScreen->DisplayStatusToken->max_length; i++) {
        auto token = topScreen->DisplayStatusToken->vector[i];
        auto displayObj = UIToken_FindHierarchyToken(mainTransform, token, false, nullptr);
        auto displayTransform = GameObject_get_transform(displayObj, nullptr);
        auto onakaSetObj = UIToken_FindHierarchyToken(displayTransform, hb::createCSharpString("OnakaSet"), false, nullptr);
        auto onakaSet = GameObject_get_transform(onakaSetObj, nullptr);
        auto gaugeObj = UIToken_FindHierarchyToken(onakaSet, hb::createCSharpString("Gauge"), false, nullptr);
        auto gaugeImage = (Image *) GameObject_GetComponent_148(gaugeObj, GameObject_GetComponent_20__MethodInfo);

        Color color;
        color.r = 211.f/255.f;
        color.g = 0.f;
        color.b = 1.f;
        color.a = 1.f;
        Graphic_set_color((Graphic*) gaugeImage, color, nullptr);
        
        auto borderTransform = Transform_Find(onakaSet, hb::createCSharpString("Img_Waku"), Transform_Find__MethodInfo);
        auto borderObj = Component_1_get_gameObject((Component_1*) borderTransform, nullptr);
        auto borderImage = (Image *) GameObject_GetComponent_148(borderObj, GameObject_GetComponent_20__MethodInfo);
        color.r = 183.f/255.f;
        color.g = 155.f/255.f;
        Graphic_set_color((Graphic*) borderImage, color, nullptr);
    }
}

const int BELLY_GAIN_PER_ENEMY = 600;
const int EXTRA_BELLY_DECREASE_PER_TURN = 50;
int lastPartyExperience[3] = { 0, 0, 0 };
bool partyExperienceInitialized = false;

typedef Index__Enum_5 WazaIndex;

void hookDungeonEngineUpdate(GameSystem* thisPtr) {
    // Called on every frame in a dungeon

    recolorBellyGauge(); // TODO: no reason to do this in every frame

    auto gameData = Singleton_1_GameData__get_Instance((MethodInfo*) Singleton_1_GameData__get_Instance__MethodInfo);
    auto partyList = GameData_GetPartyList(gameData, nullptr);

    for (int i = 0; i < partyList->_size && i < 3; i++) {
        auto partyStatus = (PartyStatus*) partyList->_items->vector[i];
        auto status = (PokemonStatus*) PartyStatus_GetStatus(partyStatus, nullptr);
        int experience = PokemonStatus_GetExperience(status, nullptr);        

        if (experience != lastPartyExperience[i]) {
            if (partyExperienceInitialized && lastPartyExperience[i] != 0 && PartyStatus_IsPlayer(partyStatus, nullptr)) {
                int experienceDiff = experience - lastPartyExperience[i];

                auto currentBelly = PokemonStatus_GetBelly(status, nullptr);
                auto maxBelly = PokemonStatus_GetMaxBelly(status, nullptr);
                auto newBelly = currentBelly + BELLY_GAIN_PER_ENEMY;
                if (newBelly > maxBelly) {
                    newBelly = maxBelly;
                }

                setBelly(status, newBelly);
                LOGF("Gained %d belly, experience diff: %d\n", BELLY_GAIN_PER_ENEMY, experienceDiff);
            }

            lastPartyExperience[i] = experience;
            partyExperienceInitialized = true;
        }
    }

    // auto input = (InputSystem*) Singleton_1_NativeMessageWindowCtrl__get_Instance(
    //             Singleton_1_InputSystem__get_Instance__MethodInfo);
    // if (InputSystem_IsPad_MenuGeneralYButton(input, nullptr)) {
    //     DungeonPlayerCommand_UseWaza(WazaIndex::TELEPORT, nullptr, nullptr);
    // }
}

IEnumerator* hookOnPlayerTurnInput(IEnumerator* returnVal) {
    /*auto player = (Creature*) Creature_GetPlayer(nullptr);
    auto playerStatus = (PokemonStatus*) Creature_GetStatus(player, nullptr);
    auto belly = PokemonStatus_GetBelly(playerStatus, nullptr);
    belly -= EXTRA_BELLY_DECREASE_PER_TURN;
    if (belly < 1) {
        belly = 1;
    }
    setBelly(playerStatus, belly);*/

    auto gameData = Singleton_1_GameData__get_Instance((MethodInfo*) Singleton_1_GameData__get_Instance__MethodInfo);
    auto partyList = GameData_GetPartyList(gameData, nullptr);

    for (int i = 0; i < partyList->_size && i < 3; i++) {
        auto partyStatus = (PartyStatus*) partyList->_items->vector[i];
        auto status = (PokemonStatus*) PartyStatus_GetStatus(partyStatus, nullptr);
        
        forceUpdateBellyInUI(status);
    }

    return returnVal;
}

bool customCustomShortcutCheckValidCommand(CustomShortcut* thisPtr, OptionManager_Shortcut__Enum shortcut, MethodInfo *method) {
    if ((int) shortcut == 12) { // Top button (Sneak Attack)
        auto playerCreature = (Creature*) Creature_GetPlayer(nullptr);
        if (playerCreature == nullptr) {
            return false;
        }
        auto playerCoord = Creature_GetCoord(playerCreature, nullptr);
        auto playerDir = Creature_GetDirection(playerCreature, nullptr);

        auto enemies = Creature_FindEnemies(playerCreature, 1, true, false, playerCoord, playerDir, nullptr);
        if (enemies == nullptr || enemies->_size < 1) {
            return false;
        }
        
        // The list should always contain either zero or one item
        auto enemy = (Creature*) enemies->_items->vector[0];
        auto enemyDir = Creature_GetDirection(enemy, nullptr);

        // Sneak Attack can only be performed if the player attacks from behind, so the player direction
        // must be the same as the enemy direction
        return playerDir->dir_ == enemyDir->dir_;
    }
    return false;
}

IEnumerator* hookOnPlayerTurnInputShortcutMoveNext(
    DungeonEngine_OnPlayerTurnInput_Shortcut_d_64* thisPtr, MethodInfo* method
) {
    int shortcut = thisPtr->shortcut_no;
    LOGF("input shortcut movenext %d\n", shortcut);
    if (shortcut == 12) { // Top button
        DungeonPlayerCommand_UseWaza(WazaIndex::HASAMIGUILLOTINE, nullptr, nullptr); // Use Guillotine
    }
    return nullptr;
}

void hookMessageLogManagerAdd2(MessageLogManager* thisPtr, String* str) {
    // Restore the instruction that was replaced
    __asm("MOV X20, X1");

    auto cStr = hb::cSharpStringToTempCharPtr(str);
    LOGF("From log: %s\n", cStr);

    // Restore the old value of X1
    __asm("MOV X1, %[value]" : [value] "=r" (str));
}

int main(int argc, char** argv) {
    // Do nothing, implementing main() is required for compilation
}
