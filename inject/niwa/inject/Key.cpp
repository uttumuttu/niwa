/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Key.h"

namespace niwa {
    namespace inject {
        Key::Key(std::type_info const& type) : type_(&type) {
            // ignored
        }

        bool Key::operator < (Key const& rhs) const {
            return type_->before(*rhs.type_) != 0;
        }
    }
}