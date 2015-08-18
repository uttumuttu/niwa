/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_IPROVIDER_INL
#define NIWA_INJECT_IPROVIDER_INL

namespace niwa {
    namespace inject {
        template <typename T>
        IProvider<T>::~IProvider() {
            // ignored
        }
    }
}

#endif