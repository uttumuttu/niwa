/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Module.h"

#include "IVoidInjectProvider.h"

namespace niwa {
    namespace inject {
        using namespace binder;

        Module::~Module() {
            std::map<Key, IVoidInjectProvider*>::iterator it = bindings_.begin();

            for(; it != bindings_.end(); ++it) {
                IVoidInjectProvider* provider = it->second;
                delete provider;
            }
        }

        BindingBuilder Module::bind(std::type_info const& type) {
            return BindingBuilder(type, bindings_);
        }

        void* Module::newVoidInstance(std::type_info const& type, Injector const& injector) const {
            Key key(type);

            std::map<Key, IVoidInjectProvider*>::const_iterator it = bindings_.find(Key(type));
            
            if(it != bindings_.end()) {
                IVoidInjectProvider* provider = it->second;

                return provider->newVoidInstance(injector);
            } else {
                return 0;
            }
        }
    }
}