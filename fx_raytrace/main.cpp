/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include <windows.h>

#include "RaytraceBindings.h"
#include "RaytraceEffect.h"

#include "niwa/demolib/ObjectProvider.h"
#include "niwa/demolib/ObjectProviderRegistry.h"

using namespace niwa::demolib;

namespace {
    template <typename T>
    static IObject* factory(CheckableArguments const& args) {
        return new T(args);
    }

    static ObjectProvider providers[] = {
        ObjectProvider("raytrace_effect", factory<RaytraceEffect>),
        ObjectProvider("sphere", factory<SphereBinding>),
        ObjectProvider("cornell_box_walls", factory<CornellBoxWallsBinding>),
        ObjectProvider("mesh", factory<MeshBinding>),
        ObjectProvider("square_light", factory<SquareLightBinding>),
        ObjectProvider("camera", factory<CameraBinding>)
    };
}

extern "C" __declspec(dllexport) void registerProviders(
        ObjectProviderRegistry& registry) {
    size_t n = sizeof(providers) / sizeof(ObjectProvider);

    for(size_t i=0; i<n; ++i) {
        registry.registerProvider(providers[i]);
    }
}

extern "C" __declspec(dllexport) void unregisterProviders(
        ObjectProviderRegistry& registry) {
    size_t n = sizeof(providers) / sizeof(ObjectProvider);

    for(size_t i=n-1; i>=0; --i) {
        registry.unregisterProvider(providers[i]);
    }
}
