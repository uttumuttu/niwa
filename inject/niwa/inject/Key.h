/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_KEY_H
#define NIWA_INJECT_KEY_H

#include <typeinfo>

namespace niwa {
    namespace inject {
        class Key {
        public:
            explicit Key(std::type_info const& type);

            bool operator < (Key const& rhs) const;

        private:
            std::type_info const* type_;

            friend class Module;
        };
    }
}

#endif