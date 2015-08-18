/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LEVELSET_ILEVELSET_H
#define NIWA_LEVELSET_ILEVELSET_H

#include "niwa/math/vec3f.h"

namespace niwa {
    namespace levelset {
        /**
         * A level set (isosurface from an implicit function).
         *
         * A special type of level set is the signed distance field,
         * whose requirements are documented in the respective functions.
         */
        class ILevelSet {
        public:
            virtual ~ILevelSet();

            /**
             * @return Whether the implicit function underlying
             *         the level set is a (signed) distance field.
             */
            virtual bool isDistanceField() const = 0;

            /**
             * @return The value of the implicit function defining
             *         the level set at a given point. For signed distance
             *         fields, the value must be positive outwards the surface,
             *         and negative inwards the surface.
             */
            virtual float __fastcall value(math::vec3f const& position) const = 0;

            /**
             * @return The gradient of the implicit function defining
             *         the level set at a given point. For signed distance
             *         fields, the gradient must have unit length.
             */
            virtual math::vec3f __fastcall gradient(math::vec3f const& position) const = 0;
        };
    }
}

#endif