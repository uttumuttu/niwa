/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "ScopedAppender.h"

#include "Logger.h"

namespace niwa {
    namespace logging {
        __declspec(dllexport) ScopedAppender::ScopedAppender(IAppender* appender) 
            : appender_(appender) {
            Logger::addAppender(appender_);
        }

        __declspec(dllexport) ScopedAppender::~ScopedAppender() {
            Logger::removeAppender(appender_);
        }
    }
}
