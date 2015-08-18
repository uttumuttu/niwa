/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "BindingBuilder.h"

#include "../IVoidInjectProvider.h"

namespace niwa {
    namespace inject {
        namespace binder {
            BindingBuilder::BindingBuilder(
                    std::type_info const& type,
                    std::map<Key, IVoidInjectProvider*>& bindings)
                : type_(type), bindings_(bindings) {
                // ignored
            }

            void BindingBuilder::to(IVoidInjectProvider* provider) {
                Key key(type_);

                std::map<Key, IVoidInjectProvider*>::iterator it = bindings_.find(key);

                if(it != bindings_.end()) {
                    IVoidInjectProvider* oldProvider = it->second;
                    delete oldProvider;
                }

                bindings_[key] = provider;
            }
        }
    }
}
