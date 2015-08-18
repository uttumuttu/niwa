/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include <windows.h>

#include "QuaternionJulia.h"
#include "Morphing.h"

#include "niwa/demolib/ObjectProvider.h"
#include "niwa/demolib/ObjectProviderRegistry.h"

using namespace niwa::demolib;

namespace {
    template <typename T>
    static IObject* arglessFactory() {
        return new T();
    }

    template <typename T>
    static IObject* factory(CheckableArguments const& args) {
        return new T(args);
    }

    static ObjectProvider juliaProvider("quaternion_julia", arglessFactory<QuaternionJulia>);
    static ObjectProvider morphingProvider("morphing", factory<Morphing>);
}

extern "C" __declspec(dllexport) void registerProviders(
        ObjectProviderRegistry& registry) {
    registry.registerProvider(juliaProvider);
    registry.registerProvider(morphingProvider);
}

extern "C" __declspec(dllexport) void unregisterProviders(
        ObjectProviderRegistry& registry) {
    registry.unregisterProvider(juliaProvider);
    registry.unregisterProvider(morphingProvider);
}
