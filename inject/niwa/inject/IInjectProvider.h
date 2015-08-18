/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_IINJECTPROVIDER_H
#define NIWA_INJECT_IINJECTPROVIDER_H

#include "IVoidInjectProvider.h"

namespace niwa {
    namespace inject {
        template <typename T>
        class IInjectProvider : public IVoidInjectProvider {
        public:
            virtual ~IInjectProvider();

            virtual T* newInstance(Injector const& injector) const = 0;

            void* newVoidInstance(Injector const& injector) const;
        };
    }
}

#include "IInjectProvider.inl"

#endif