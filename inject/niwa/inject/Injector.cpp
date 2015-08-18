/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Injector.h"

#include "Module.h"

namespace niwa {
    namespace inject {
        Injector* Injector::create(Module* module) {
            module->configure();
            return new Injector(module);
        }

        Injector::~Injector() {
            delete module_;
        }

        Injector::Injector(Module const* module) : module_(module) {
            // ignored
        }

        void* Injector::newVoidInstance(std::type_info const& type) const {
            return module_->newVoidInstance(type, *this);
        }
    }
}
