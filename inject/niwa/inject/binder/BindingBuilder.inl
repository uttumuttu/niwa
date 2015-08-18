/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_BINDER_BINDINGBUILDER_INL
#define NIWA_INJECT_BINDER_BINDINGBUILDER_INL

#include "../IProvider.h"

namespace niwa {
    namespace inject {
        namespace binder {
            template <typename T>
            void BindingBuilder::to() {
                to(new T::InjectProvider());
            }

            template <typename T>
            void BindingBuilder::to(IProvider<T>* provider) {
                class Adapter : public IInjectProvider<T> {
                public:
                    explicit Adapter(IProvider<T>* provider)
                        : provider_(provider) {
                        // ignored
                    }

                    ~Adapter() {
                        delete provider_;
                    }

                    T* newInstance(Injector const& injector) const {
                        return provider_->newInstance();
                    }
                private:
                    IProvider<T>* provider_;
                };

                to(new Adapter(provider));
            }
        }
    }
}

#endif
