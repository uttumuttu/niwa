/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOGGING_IAPPENDER_H
#define NIWA_LOGGING_IAPPENDER_H

#include <string>

namespace niwa {
    namespace logging {
        class Level;
        class Logger;

        /**
         * Appends log messages to a destination
         * (e.g., std::cout or a file).
         */
        class IAppender {
        public:
            __declspec(dllexport) virtual ~IAppender();

            /**
             * Appends a single log message to the destination.
             * The method must not produce its own log messages:
             * that would lead to an infinite loop.
             *
             * @param logger The logger from which this message originated.
             *
             * @param level The level of the message; can be assumed to be
             *              in current log level.
             *
             * @param line A non-terminated line. Never NULL.
             */
            virtual void append(
                Logger const& logger,
                Level const& level,
                std::string const& line) = 0;
        };
    }
}

#endif