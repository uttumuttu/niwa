/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/demolib/IObject.h"

#include <typeinfo.h>

namespace niwa {
    namespace demolib {
        char const* IObject::METATABLE_NAME = typeid(IObject).name();

        IObject::IObject() {
            // ignored
        }

        IObject::~IObject() {
            // ignored
        }
    }
}
