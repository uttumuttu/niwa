/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/system/IParallelizer.h"

namespace niwa {
    namespace system {
        IParallelizer::~IParallelizer() {
            // ignored
        }

        IParallelizer::ICallback::~ICallback() {
            // ignored
        }
    }
}