/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOG_APPENDERS_STREAMAPPENDER_H
#define NIWA_LOG_APPENDERS_STREAMAPPENDER_H

#include "niwa/logging/IAppender.h"

#include <iostream>

namespace niwa {
    namespace logging {
        namespace appenders {
            /**
             * Appends log messages to a given std::ostream
             * (e.g., std::cout).
             */
            class StreamAppender : public IAppender {
            public:
                /**
                 * @param stream Does not transfer ownership.
                 */
                __declspec(dllexport) StreamAppender(
                    std::ostream& stream);

                void append(Logger const& logger,
                    Level const& level, std::string const& line);

            private: // prevent
                StreamAppender(StreamAppender const&);
                StreamAppender& operator = (StreamAppender const&);

            private:
                std::ostream& stream_;
            };
        }
    }
}

#endif