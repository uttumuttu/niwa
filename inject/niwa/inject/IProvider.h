/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_IPROVIDER_H
#define NIWA_INJECT_IPROVIDER_H

namespace niwa {
    namespace inject {
        template <typename T>
        class IProvider {
        public:
            virtual ~IProvider();

            virtual T* newInstance() const = 0;
        };
    }
}

#include "IProvider.inl"

#endif