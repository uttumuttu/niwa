/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_INJECTOR_INL
#define NIWA_INJECT_INJECTOR_INL

namespace niwa {
    namespace inject {
        template <typename T>
        T* Injector::newInstance() const {
            void* result = newVoidInstance(typeid(T));

            return static_cast<T*>(result);
        }
    }
}

#endif 