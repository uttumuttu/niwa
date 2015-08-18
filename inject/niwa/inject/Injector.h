/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_INJECT_INJECTOR_H
#define NIWA_INJECT_INJECTOR_H

#include <typeinfo>

namespace niwa {
    namespace inject {
        class Module;

        class Injector {
        public:
            /**
             * @param module Transfers ownership.
             */
            static Injector* create(Module* module);

            ~Injector();

            template <typename T>
            T* newInstance() const;

        private:
            void* newVoidInstance(std::type_info const& type) const;

        private:
            explicit Injector(Module const* module);

        private: // prevent copying
            Injector(Injector const&);
            Injector& operator = (Injector const&);

        private:
            Module const* module_;
        };
    }
}

#include "Injector.inl"

#endif