/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_TRIANGLE_H
#define NIWA_RAYTRACE_OBJECTS_TRIANGLE_H

#include "niwa/math/vec3f.h"
#include "niwa/math/packed_vec3f.h"

#include <setjmp.h>
#include <xmmintrin.h>

namespace niwa {
    namespace raytrace {
        class packed_ray3f;
        class ray3f;
        class HitInfo;

        namespace objects {
            /**
             * Acceleration structure for ray-triangle intersection.
             *
             * Must be allocated at 16-byte boundaries.
             */
            class Triangle {
            public:
                /**
                 * Creates an initialized default triangle.
                 */
                Triangle();

                /**
                 * Creates an accelerated triangle given vertices.
                 */
                explicit Triangle(math::vec3f vertices[3]);

                __forceinline void raytrace(
                    ray3f const& ray, HitInfo& hitInfo, bool& hitFound) const;

                __forceinline void raytraceShadow(
                    ray3f const& ray, float cutoffDistance, jmp_buf& jump) const;

            private:
                /**
                 * Position of a corner vertex.
                 */
                math::vec3f position_; // position of a "corner" vertex

                /**
                 * Unit surface normal (outwards).
                 */
                math::vec3f normal_;

                /**
                 * Two dimensions defining the projection plane.
                 */
                int projectionDimensions_[2];

                /**
                 * Components of the inverse matrix that maps
                 * projected triangle coordinates to barycentric coordinates.
                 */
                float matrix_[2*2]; // row-major format
            };
        }
    }
}

#include "Triangle.inl"

#endif