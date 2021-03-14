#include "il2cpp-appdata.h"

typedef Index__Enum PokemonIndex;
typedef Index__Enum_9 FixedPokemonIndex;

struct Starter {
    Index__Enum pokemonId;
    Index__Enum_9 fixedPokemonId;
};

#define RTDX_STARTER_COUNT 16
extern "C" Starter rtdx_starters[RTDX_STARTER_COUNT];
