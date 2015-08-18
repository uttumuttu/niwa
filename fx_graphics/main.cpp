/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include <windows.h>

#include "ClearEffect.h"
#include "FullScreenShader.h"
#include "HardwareCaustics.h"
#include "PostprocessDemo.h"

#include "niwa/demolib/ObjectProvider.h"
#include "niwa/demolib/ObjectProviderRegistry.h"

using namespace niwa::demolib;

namespace {
    template <typename T>
    static IObject* factory(CheckableArguments const& args) {
        return new T(args);
    }

    template <typename T>
    static IObject* arglessFactory() {
        return new T();
    }

    static ObjectProvider clear("clear", factory<ClearEffect>);

    static ObjectProvider fullScreenShader("full_screen_shader", factory<FullScreenShader>);
    static ObjectProvider hardwareCaustics("hardware_caustics", factory<HardwareCaustics>);
    static ObjectProvider postprocessDemo("postprocess_demo", arglessFactory<PostprocessDemo>);
}

extern "C" __declspec(dllexport) void registerProviders(
        ObjectProviderRegistry& registry) {
    registry.registerProvider(clear);
    registry.registerProvider(fullScreenShader);
    registry.registerProvider(hardwareCaustics);
    registry.registerProvider(postprocessDemo);
}

extern "C" __declspec(dllexport) void unregisterProviders(
        ObjectProviderRegistry& registry) {
    registry.unregisterProvider(postprocessDemo);
    registry.unregisterProvider(hardwareCaustics);
    registry.unregisterProvider(fullScreenShader);
    registry.unregisterProvider(clear);
}
