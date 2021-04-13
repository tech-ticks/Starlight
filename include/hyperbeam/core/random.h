#pragma once

#include "il2cpp-appdata.h"

namespace hb {

    int randomIntInRange(int minInclusive, int maxExclusive) {
        return Random_1_RandomRangeInt(minInclusive, maxExclusive, nullptr);
    }

}
