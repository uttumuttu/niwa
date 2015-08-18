/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "IAppender.h"

namespace niwa {
    namespace logging {
        __declspec(dllexport) IAppender::~IAppender() {
            // ignored
        }
    }
}