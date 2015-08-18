/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_MODULE_INL
#define NIWA_INJECT_MODULE_INL

namespace niwa {
    namespace inject {
        template <typename T>
        binder::BindingBuilder Module::bind() {
            return bind(typeid(T));
        }
    }
}

#endif