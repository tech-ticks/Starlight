#pragma once

#include "il2cpp-appdata.h"
#include <hyperbeam/core.h>
#include <rtdx/misc.h>

namespace hb::reflect {

    static struct AssemblyCache {
        bool populated;
        Assembly__Array* allAssemblies;
        Assembly* unityEngineAssembly;
        Assembly* gameAssembly;
    } assemblyCache;

    void populateAssemblyCache() {
        if (assemblyCache.populated) {
            return;
        }

        auto allAssemblies = AppDomain_GetAssemblies_1(AppDomain_get_CurrentDomain(nullptr), nullptr);
        for (size_t i = 0; i < allAssemblies->max_length; i++) {
            auto assembly = allAssemblies->vector[i];

            // Cache commonly used assemblies
            auto assemblyName = AssemblyName_get_Name(Assembly_GetName_1(assembly, nullptr), nullptr);
            if (hb::stringsEqual(assemblyName, hb::createCSharpString("UnityEngine"))) {
                assemblyCache.unityEngineAssembly = assembly;
            }
            if (hb::stringsEqual(assemblyName, hb::createCSharpString("Assembly-CSharp"))) {
                assemblyCache.unityEngineAssembly = assembly;
            }
        }
        assemblyCache.allAssemblies = allAssemblies;
    }

    Assembly__Array* getAllAssemblies() {
        populateAssemblyCache();
        return assemblyCache.allAssemblies;
    }

    String* createAssemblyQualifiedName(const char* namespaceName, const char* typeName, const char* assemblyName) {
        char* buffer;

        if (namespaceName) {
            const char* formatString = "%s.%s, %s";
            int size = rtdx_snprintf(nullptr, 0, formatString, namespaceName, typeName, assemblyName) + 1;
            buffer = (char*) alloca(size);
            rtdx_snprintf(buffer, size, formatString, namespaceName, typeName, assemblyName);
        } else {
            const char* formatString = "%s, %s";
            int size = rtdx_snprintf(nullptr, 0, formatString, typeName, assemblyName) + 1;
            buffer = (char*) alloca(size);
            rtdx_snprintf(buffer, size, formatString, typeName, assemblyName);
        }

        return hb::createCSharpString(buffer);
    }

    Type* getUnityType(const char* namespaceName, const char* typeName) {
        auto assemblyQualifiedName = createAssemblyQualifiedName(namespaceName, typeName, "UnityEngine");
        return Type_GetType_2(assemblyQualifiedName, nullptr);
    }

    Type* getUnityType(Il2CppClass* klass) {
        return getUnityType(klass->namespaze, klass->name);
    }

    Type* getUnityUIType(const char* namespaceName, const char* typeName) {
        auto assemblyQualifiedName = createAssemblyQualifiedName(namespaceName, typeName, "UnityEngine.UI");
        return Type_GetType_2(assemblyQualifiedName, nullptr);
    }

    Type* getUnityUIType(Il2CppClass* klass) {
        return getUnityUIType(klass->namespaze, klass->name);
    }

    Type* getAssemblyCSharpType(const char* namespaceName, const char* typeName) {
        auto assemblyQualifiedName = createAssemblyQualifiedName(namespaceName, typeName, "Assembly-CSharp");
        return Type_GetType_2(assemblyQualifiedName, nullptr);
    }

    Type* getAssemblyCSharpType(Il2CppClass* klass) {
        return getAssemblyCSharpType(klass->namespaze, klass->name);
    }

    Type* getType(const char* assemblyName, const char* namespaceName, const char* typeName) {
        auto assemblyQualifiedName = createAssemblyQualifiedName(namespaceName, typeName, assemblyName);
        return Type_GetType_2(assemblyQualifiedName, nullptr);
    }

}
