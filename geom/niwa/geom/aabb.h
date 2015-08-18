/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GEOM_AABB_H
#define NIWA_GEOM_AABB_H

#include "niwa/math/vec3f.h"

namespace niwa {
    namespace geom {
        /**
         * Axis-aligned bounding box (AABB).
         *
         * The volume covered by the AABB is defined
         * as the closed volume spanned by its
         * extrema.
         */
        class aabb {
        public:
            /**
             * Creates an AABB around the
             * given vector.
             */
            explicit aabb(math::vec3f const& vector);

            aabb(math::vec3f const& minPosition, math::vec3f const& maxPosition);

            void extendToFit(math::vec3f const& vector);

            void extendDimensionsBy(math::vec3f const& vector);

            void extendDimensionsBy(float value);

            void extendToFit(aabb const& bounds);

            /**
             * @return The minimum coordinates of the AABB.
             */
            math::vec3f const& minPosition() const;

            /**
             * @return The maximum coordinates of the AABB.
             */
            math::vec3f const& maxPosition() const;

            /**
             * @return The center position of the AABB.
             */
            math::vec3f center() const;

            /**
             * @return The dimensions of the AABB.
             */
            math::vec3f dimensions() const;

            /**
             * @return A two-element array where
             *         the first element is the minimum position,
             *         and the second element is the maximum position.
             */
            math::vec3f const* getExtrema() const;

            /**
             * @return Whether the position is inside
             *         the closed volume spanned by the AABB extrema.
             */
            bool contains(math::vec3f const& position) const;

            /**
             * @return The closest point in the AABB to the
             *         position.
             */
            math::vec3f closestPositionTo(math::vec3f const& position) const;

            float shortestDistanceTo(math::vec3f const& position) const;

        private:
            math::vec3f minPosition_;
            math::vec3f maxPosition_;
        };
    }
}

#endif