/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_INJECT_IINJECTPROVIDER_INL
#define NIWA_INJECT_IINJECTPROVIDER_INL

namespace niwa {
    namespace inject {
        template <typename T>
        IInjectProvider<T>::~IInjectProvider() {
            // ignored
        }

        template <typename T>
        void* IInjectProvider<T>::newVoidInstance(Injector const& injector) const {
            T* result = newInstance(injector);

            return static_cast<void*>(result);
        }
    }
}

#endif