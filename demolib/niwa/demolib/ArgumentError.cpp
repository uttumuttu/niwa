/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "ArgumentError.h"

namespace niwa {
    namespace demolib {
        ArgumentError::ArgumentError(std::string const& message) : message_(message) {
            // ignored
        }

        std::string const& ArgumentError::getMessage() const {
            return message_;
        }
    }
}