/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_IVOIDINJECTPROVIDER_H
#define NIWA_INJECT_IVOIDINJECTPROVIDER_H

namespace niwa {
    namespace inject {
        class Injector;

        class IVoidInjectProvider {
        public:
            virtual ~IVoidInjectProvider();

            virtual void* newVoidInstance(Injector const& injector) const = 0;
        };
    }
}

#endif