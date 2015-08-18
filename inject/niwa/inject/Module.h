/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_MODULE_H
#define NIWA_INJECT_MODULE_H

#include "binder/BindingBuilder.h"
#include "Key.h"

#include <typeinfo>
#include <map>

namespace niwa {
    namespace inject {
        class Injector;
        class IVoidInjectProvider;

        class Module {
        public:
            virtual ~Module();

            virtual void configure() = 0;

        protected:
            template <typename T>
            binder::BindingBuilder bind();

            binder::BindingBuilder bind(std::type_info const& type);

        private:
            void* newVoidInstance(std::type_info const& type, Injector const& injector) const;

        private:
            std::map<Key, IVoidInjectProvider*> bindings_; // elements owned

            friend class Injector;
        };
    }
}

#include "Module.inl"

#endif