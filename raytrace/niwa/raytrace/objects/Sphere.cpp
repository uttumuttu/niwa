/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/Sphere.h"

#include "niwa/raytrace/Constants.h"
#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/packed_ray3f.h"

#include "niwa/math/Constants.h"

#include <xmmintrin.h>

namespace niwa {
    using math::vec3f;
    using math::packed_vec3f;
    
    namespace raytrace {
        namespace objects {
            struct Sphere::Sse {
                Sse() : squareRadius(_mm_set_ps1(1.0f)) {
                    // ignored
                }

                packed_vec3f position;

                __m128 squareRadius;
            };

            Sphere::Sphere() 
                : position_(0,0,0), radius_(1), squareRadius_(1), inverseRadius_(1) {
                sse_ = new (_mm_malloc(sizeof(Sse),16)) Sse();
            }

            Sphere::~Sphere() {
                sse_->~Sse();
                _mm_free(sse_);
            }

            vec3f const& Sphere::getPosition() const {
                return position_;
            }

            float Sphere::getRadius() const {
                return radius_;
            }

            void Sphere::setPosition(vec3f const& position) {
                position_ = position;

                sse_->position = packed_vec3f(position);
            }

            void Sphere::setRadius(float radius) {
                radius_ = radius;

                squareRadius_ = radius_ * radius_;

                inverseRadius_ = 1 / radius_;

                sse_->squareRadius = _mm_set_ps1(squareRadius_);
            }

            void Sphere::setMaterial(Material const& material) {
                outsideMaterial_ = material;
                insideMaterial_ = material;

                insideMaterial_.setRefractiveIndex(0);
            }

            // Let:
            //     t = intersection time,
            //     r = radius,
            //     p = position of the ray relative to the sphere,
            //     d = direction of the ray.

            // norm(p+d*t)^2 = r^2
            // <=> p'*p + 2*p'*d*t + d'*d*t^2 = r^2
            // <=> a*t^2 + b*t + c = 0,

            // where
            //    a = d'*d = 1,  // since d(norm) = 1
            //    b = 2*p'*d,
            //    c = p'*p - r^2.

            // Instead of using the traditional
            // formula for quadratic equation, namely
            //
            //    D = b^2 - 4ac = b^2 - 4c
            //    t = [-b += sqrt(D)] / (2*a) = [-b += sqrt(D)] / 2,
            //
            // well use an equivalent solution
            //
            //    b = p'*d,
            //    c = p'*p - r^2,
            //
            //    D = b^2 - c
            //    t = [-b += sqrt(D)].

            bool Sphere::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                vec3f p( ray.getPosition() - position_ );

                vec3f const& d = ray.getDirection();

                float b = vec3f::dot(p,d);
                float c = vec3f::dot(p,p) - squareRadius_;

                float D = b*b - c;

                if(D >= 0) {
                    float d = sqrt(D);

                    float t0 = -d - b; // t0 <= t1

                    if(t0 >= constants::DISTANCE_EPSILON) {
                        vec3f hitPosition( 
                            ray.getPosition() + ray.getDirection() * t0);

                        // We are outside the sphere: normal outwards.

                        vec3f hitNormal( (hitPosition - position_) * inverseRadius_ );

                        hitInfo.setValues(
                            t0,
                            hitPosition,
                            hitNormal,
                            outsideMaterial_);

                        return true;
                    }
                    
                    float t1 =  d - b;
                    
                    if(t1 >= constants::DISTANCE_EPSILON) {
                        vec3f hitPosition = 
                            ray.getPosition() + ray.getDirection() * t1;

                        // We are inside the sphere: normal inwards.

                        vec3f hitNormal( (hitPosition - position_) / -inverseRadius_ );

                        hitInfo.setValues(
                            t1,
                            hitPosition,
                            hitNormal,
                            insideMaterial_);

                        return true;
                    }
                }

                return false;
            }

            bool Sphere::raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* /*light*/) const {
                vec3f p( ray.getPosition() - position_ );

                vec3f const& d = ray.getDirection();

                float b = vec3f::dot(p,d);
                float c = p.squareLength() - squareRadius_;

                float b2 = b*b;

                if(b2 < c) {
                    return false;
                } else {
                    float d = sqrt(b2 - c);

                    float t0 = -d - b; // t0 <= t1

                    if(t0 >= constants::DISTANCE_EPSILON && t0 < cutoffDistance) {
                        return true;
                    } else {
                        float t1 =  d - b;

                        return t1 >= constants::DISTANCE_EPSILON && t1 < cutoffDistance;
                    }
                }
           }

            __m128 Sphere::raytraceShadow(
                packed_ray3f const& ray, __m128 cutoffDistance, ILight const* /*light*/) const {
                packed_vec3f pos(ray.getPosition() - sse_->position);

                packed_vec3f const& dir = ray.getDirection();

                __m128 b = pos.dot(dir);
                __m128 c = _mm_sub_ps(pos.squareLength(), sse_->squareRadius);

                __m128 D = _mm_sub_ps(_mm_mul_ps(b,b), c);

                __m128 andMask = _mm_cmpge_ps(D, _mm_setzero_ps());

                __m128 d = _mm_sqrt_ps(D);

                __m128 t0 = _mm_sub_ps(_mm_setzero_ps(), _mm_add_ps(d,b));
                __m128 t1 = _mm_sub_ps(d, b);

                // t0 <= t1

                __m128 orMask1 = _mm_and_ps(
                    _mm_cmpge_ps(t0, constants::PACKED_DISTANCE_EPSILON),
                    _mm_cmplt_ps(t0, cutoffDistance));

                __m128 orMask2 = _mm_and_ps(
                    _mm_cmpge_ps(t1, constants::PACKED_DISTANCE_EPSILON),
                    _mm_cmplt_ps(t1, cutoffDistance));

                return _mm_and_ps(andMask, _mm_or_ps(orMask1, orMask2));
           }
        }
    }
}