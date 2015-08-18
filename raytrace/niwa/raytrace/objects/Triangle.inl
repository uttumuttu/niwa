/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_RAYTRACE_OBJECTS_TRIANGLE_INL
#define NIWA_RAYTRACE_OBJECTS_TRIANGLE_INL

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/packed_ray3f.h"
#include "niwa/raytrace/constants.h"

namespace niwa {
    namespace raytrace {
        namespace objects {
            void Triangle::raytrace(ray3f const& ray, HitInfo& hitInfo, bool& hitFound) const {
                static const float BARYCENTRIC_EPSILON = 1e-3f;

                // <p+d*t-v|n>=0
                // <p-v|n> + t<d|n> = 0
                // t = -<p-v|n> / <d|n>

                const float denum =  math::vec3f::dot(ray.getDirection(), normal_);
                if(denum >= 0) {
                    // backface culling
                    return;
                }

                const math::vec3f relativePosition = ray.getPosition() - position_;

                const float numer = -math::vec3f::dot(relativePosition, normal_);
                if(numer > 0) {
                    // backface culling
                    return;
                }

                const float distance = numer / denum;
                if(distance < constants::DISTANCE_EPSILON 
                    || (hitFound && distance >= hitInfo.distance())) {
                        return;
                }

                const math::vec3f relativeHitPosition =
                    relativePosition + ray.getDirection() * distance;

                float x = relativeHitPosition[projectionDimensions_[0]];
                float y = relativeHitPosition[projectionDimensions_[1]];

                float u = matrix_[0] * x + matrix_[1] * y;
                if(u < -BARYCENTRIC_EPSILON || u > 1+BARYCENTRIC_EPSILON) {
                    return;
                }

                float v = matrix_[2] * x + matrix_[3] * y;
                if(v < -BARYCENTRIC_EPSILON || u+v > 1+BARYCENTRIC_EPSILON) {
                    return;
                }

                hitFound = true;

                hitInfo.setValues(
                    distance,
                    position_ + relativeHitPosition,
                    normal_,
                    Material::createDiffuse(graphics::Spectrum(.5f,0,.5f)));
            }

            void Triangle::raytraceShadow(ray3f const& ray, float cutoffDistance, jmp_buf& jump) const {
                static const float BARYCENTRIC_EPSILON = 1e-3f;

                // <p+d*t-v|n>=0
                // <p-v|n> + t<d|n> = 0
                // t = -<p-v|n> / <d|n>

                const float denum =  math::vec3f::dot(ray.getDirection(), normal_);
                if(denum >= 0) {
                    // backface culling
                    return;
                }

                const math::vec3f relativePosition = ray.getPosition() - position_;

                const float numer = -math::vec3f::dot(relativePosition, normal_);
                if(numer > 0) {
                    // backface culling
                    return;
                }

                const float distance = numer / denum;
                if(distance < constants::DISTANCE_EPSILON
                    || distance >= cutoffDistance) {
                    return;
                }

                const math::vec3f relativeHitPosition =
                    relativePosition + ray.getDirection() * distance;

                float x = relativeHitPosition[projectionDimensions_[0]];
                float y = relativeHitPosition[projectionDimensions_[1]];

                float u = matrix_[0] * x + matrix_[1] * y;
                if(u < -BARYCENTRIC_EPSILON || u > 1+BARYCENTRIC_EPSILON) {
                    return;
                }

                float v = matrix_[2] * x + matrix_[3] * y;
                if(v < -BARYCENTRIC_EPSILON || u+v > 1+BARYCENTRIC_EPSILON) {
                    return;
                }

                longjmp(jump, 1);
            }
        }
    }
}

#endif