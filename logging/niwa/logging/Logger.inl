/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LOGGING_LOGGER_INL
#define NIWA_LOGGING_LOGGER_INL

namespace niwa {
    namespace logging {
        template <typename T>
        Logger::Line& Logger::Line::operator << (T const& obj) {
            stream_ << obj;
            return *this;
        }
    }
}

#endif