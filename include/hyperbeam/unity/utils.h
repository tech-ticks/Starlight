#pragma once

#include "il2cpp-appdata.h"

typedef Object_1 UnityObject;
typedef Component_1 UnityComponent;

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

    inline Transform* findTransform(Transform* parentTransform, const char* childName) {
        return Transform_Find(parentTransform, hb::createCSharpString(childName), nullptr);
    }

    template<typename T>
    T* getComponent(UnityComponent* sourceComponent, Type* type) {
        return (T*) Component_1_GetComponent(sourceComponent, type, nullptr);
    }

}
