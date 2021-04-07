#pragma once

#include "il2cpp-appdata.h"

typedef Object_1 UnityObject;

namespace hb::unity {

    inline UnityObject* instantiate(UnityObject* original) {
        return Object_1_Instantiate(original, nullptr);
    }

    inline void destroy(UnityObject* object) {
        Object_1_Destroy_1(object, nullptr);
    }

    inline void destroyDelayed(UnityObject* object, float delay) {
        Object_1_Destroy(object, delay, nullptr);
    }

}
