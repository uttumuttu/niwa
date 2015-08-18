/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "StreamAppender.h"

#include "niwa/logging/Level.h"
#include "niwa/logging/Logger.h"

namespace niwa {
    namespace logging {
        namespace appenders {
            __declspec(dllexport) StreamAppender::StreamAppender(std::ostream& stream) : stream_(stream) {
                // ignored
            }

            void StreamAppender::append(Logger const& logger,
                    Level const& level, std::string const& line) {
                stream_ << logger.getName() << " - " << level.getName() << " - " << line << std::endl;
            }
        }
    }
}