/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_BINDER_BINDINGBUILDER_H
#define NIWA_INJECT_BINDER_BINDINGBUILDER_H

#include "../Key.h"

#include <typeinfo>
#include <map>

namespace niwa {
    namespace inject {
        template <typename T>
        class IProvider;

        class IVoidInjectProvider;

        namespace binder {
            class BindingBuilder {
            public:
                explicit BindingBuilder(
                    std::type_info const& type,
                    std::map<Key, IVoidInjectProvider*>& bindings);

                template <typename T>
                void to();

                /**
                 * @param provider Transfers ownership.
                 */
                template <typename T>
                void to(IProvider<T>* provider);

            private:
                /**
                 * @param provider Transfers ownership.
                 */
                void to(IVoidInjectProvider* provider);

            private: // prevent assignment
                BindingBuilder& operator = (BindingBuilder const&);

            private:
                std::type_info const& type_;
                std::map<Key, IVoidInjectProvider*>& bindings_;
            };
        }
    }
}

#include "BindingBuilder.inl"

#endif
