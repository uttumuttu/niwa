/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include <windows.h>

#include "ClothEffect.h"
#include "RigidBodyEffect.h"

#include "niwa/demolib/ObjectProvider.h"
#include "niwa/demolib/ObjectProviderRegistry.h"

using namespace niwa::demolib;

namespace {
    template <typename T>
    static IObject* factory() {
        return new T();
    }

    ObjectProvider providers[2] = {
        ObjectProvider("cloth_effect", factory<ClothEffect>),
        ObjectProvider("rigid_body_effect", factory<RigidBodyEffect>)
    };
}

extern "C" __declspec(dllexport) void registerProviders(
        ObjectProviderRegistry& registry) {
    for(size_t i=0; i<sizeof(providers) / sizeof(ObjectProvider); ++i) {
        registry.registerProvider(providers[i]);
    }
}

extern "C" __declspec(dllexport) void unregisterProviders(
        ObjectProviderRegistry& registry) {
    for(size_t i=sizeof(providers) / sizeof(ObjectProvider) -1; i >= 0; --i) {
        registry.unregisterProvider(providers[i]);
    }
}