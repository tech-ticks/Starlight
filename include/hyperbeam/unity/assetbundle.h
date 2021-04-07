#pragma once

#include "il2cpp-appdata.h"
#include <hyperbeam/core.h>
#include <hyperbeam/reflect.h>
#include <rtdx/misc.h>

typedef Object_1 UnityObject;

namespace hb::unity {

    class AssetBundleWrapper final: private NonCopyable {

    public:
        explicit AssetBundleWrapper(const char* name) {
            const char* formatString = "hyperbeam_romfs:/Data/StreamingAssets/ab/%s";

            int size = rtdx_snprintf(nullptr, 0, formatString, name) + 1;
            char* buffer = (char*) alloca(size);
            rtdx_snprintf(buffer, size, formatString, name);

            String* str = hb::createCSharpString(buffer);
            assetBundle = AssetBundle_LoadFromFile(str, nullptr);
        }

        ~AssetBundleWrapper() {
            AssetBundle_Unload(assetBundle, true, nullptr);
            assetBundle = nullptr;
            
        }

        UnityObject* loadAsset(const char* name) {
            return AssetBundle_LoadAsset(assetBundle,
                hb::createCSharpString(name),
                hb::reflect::getUnityType((Il2CppClass*) Object_1__TypeInfo),
                AssetBundle_LoadAsset__MethodInfo);
        }

        AssetBundle* assetBundle;

    };

}
