/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/SquareLight.h"

#include "niwa/raytrace/Constants.h"
#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/Hemisphere.h"
#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/packed_ray3f.h"

#include "niwa/math/Constants.h"

#include "niwa/math/vec2f.h"

#include "niwa/random/VanDerCorput.h"
#include "niwa/random/Halton.h"
#include "niwa/random/EvenlySpacedSequence.h"
#include "niwa/random/Lcg.h"

#include <algorithm>

#include <boost/shared_array.hpp>

#define DEFAULT_SAMPLE_COUNT 32

#define USE_PACKED_RAYS 1

namespace niwa {
    using graphics::Spectrum;

    using math::constants::PI_F;

    using math::vec2f;
    using math::vec3f;
    using math::packed_vec3f;

    namespace raytrace {
        namespace objects {
            struct SquareLight::Sse {
                Sse() : basis1QuarticMagnitude(_mm_setzero_ps()),
                    basis2QuarticMagnitude(_mm_setzero_ps()) {
                    // ignored
                }

                math::packed_vec3f position;
                math::packed_vec3f normal;
                math::packed_vec3f basis1;
                math::packed_vec3f basis2;

                __m128 basis1QuarticMagnitude;
                __m128 basis2QuarticMagnitude;
            };

            SquareLight::SquareLight()
                : sampleCount_(DEFAULT_SAMPLE_COUNT),
                  basis1SquareMagnitude_(0),
                  basis2SquareMagnitude_(0),
                  area_(0) {
                sse_ = new (_mm_malloc(sizeof(Sse), 16)) Sse();
            }

            SquareLight::~SquareLight() {
                sse_->~Sse();
                _mm_free(sse_);
            }

            void SquareLight::setSampleCount(size_t sampleCount) {
                sampleCount_ = sampleCount;
            }

            void SquareLight::setPosition(vec3f const& position) {
                position_ = position;

                sse_->position = packed_vec3f(position_);
            }

            void SquareLight::setBasis1(vec3f const& basis1) {
                basis1_ = basis1;
     
                updateBases();
                updateRadiance();
            }

            void SquareLight::setBasis2(vec3f const& basis2) {
                basis2_ = basis2;

                updateBases();
                updateRadiance();
            }

            void SquareLight::updateBases() {
                basis1SquareMagnitude_ = vec3f::dot(basis1_, basis1_);
                basis2SquareMagnitude_ = vec3f::dot(basis2_, basis2_);

                sse_->basis1QuarticMagnitude = _mm_set_ps1(
                    basis1SquareMagnitude_ * basis1SquareMagnitude_);

                sse_->basis2QuarticMagnitude = _mm_set_ps1(
                    basis2SquareMagnitude_ * basis2SquareMagnitude_);

                sse_->basis1 = packed_vec3f(basis1_);
                sse_->basis2 = packed_vec3f(basis2_);

                // We assume that the basis vectors are orthogonal.
                area_ = sqrt(
                    basis1SquareMagnitude_ * basis2SquareMagnitude_) * 4.0f;

                normal_ = vec3f::cross(basis1_, basis2_);
                normal_.normalize();

                sse_->normal = packed_vec3f(normal_);
            }

            void SquareLight::setPower(Spectrum const& power) {
                power_ = power;

                updateRadiance();
            }

            vec3f const& SquareLight::getPosition() const {
                return position_;
            }

            vec3f const& SquareLight::getBasis1() const {
                return basis1_;
            }

            vec3f const& SquareLight::getBasis2() const {
                return basis2_;
            }

            size_t SquareLight::getSampleCount() const {
                return sampleCount_;
            }

            bool SquareLight::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                vec3f p( ray.getPosition() - position_ );

                // N = surface normal, p = relative position, d = ray direction.

                // dot(N, p + d*t) = 0

                // <=> dot(N,p) + dot(N,d) * t = 0

                float num = -vec3f::dot(normal_, p);
                float den =  vec3f::dot(normal_, ray.getDirection());

                if(den == 0) {
                    return false;
                }

                float t = num / den;

                if(t >= constants::DISTANCE_EPSILON) {
                    vec3f relativeHitPosition( p + ray.getDirection() * t );

                    if( fabs(vec3f::dot(relativeHitPosition, basis1_)) 
                        <= basis1SquareMagnitude_
                        && fabs(vec3f::dot(relativeHitPosition, basis2_)) 
                           <= basis2SquareMagnitude_ ) {
                        vec3f hitPosition( relativeHitPosition + position_ );

                        if( vec3f::dot(p, normal_) < 0 ) {
                            // behind the light
                            hitInfo.setValues(
                                t, hitPosition, -normal_, 
                                Material::createBlack());
                        } else {
                            // in front of the light
                            hitInfo.setValues(
                                t, hitPosition, normal_, 
                                Material::createEmitting(Spectrum(1,1,1)*.6f));
                        }

                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return false;
                }
            }

            bool SquareLight::raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const {
                if(this == light) {
                    // Convex light cannot cast shadows with itself.
                    return false;
                }

                vec3f p( ray.getPosition() - position_ );

                float num = -vec3f::dot(normal_, p);
                float den =  vec3f::dot(normal_, ray.getDirection());

                float t = num / den;

                if(t >= constants::DISTANCE_EPSILON && t < cutoffDistance) {
                    // If den==0, t will be either
                    // positive or negative infinity, or NaN;
                    // none of the three possibilities will get
                    // us into this block, so we never check for den==0 explicitly.

                    vec3f relativeHitPosition( p + ray.getDirection() * t );

                    return fabs(vec3f::dot(relativeHitPosition, basis1_)) 
                        <= basis1SquareMagnitude_
                        && fabs(vec3f::dot(relativeHitPosition, basis2_)) 
                        <= basis2SquareMagnitude_;
                } else {
                    return false;
                }
            }

            __m128 SquareLight::raytraceShadow(
                packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const {
                if(this == light) {
                    // Convex light cannot cast shadows with itself.
                    return _mm_setzero_ps();
                }

                packed_vec3f pos( ray.getPosition() - sse_->position );

                __m128 num = _mm_sub_ps(_mm_setzero_ps(), sse_->normal.dot(pos));
                __m128 den = sse_->normal.dot(ray.getDirection());

                __m128 t = _mm_div_ps(num, den);

                packed_vec3f relativeHitPosition( pos + ray.getDirection() * t );

                __m128 d1 = relativeHitPosition.dot(sse_->basis1);
                __m128 d2 = relativeHitPosition.dot(sse_->basis2);

                __m128 andMask1 = _mm_cmple_ps(
                    _mm_mul_ps(d1,d1), sse_->basis1QuarticMagnitude);

                __m128 andMask2 = _mm_cmple_ps(
                    _mm_mul_ps(d2,d2), sse_->basis2QuarticMagnitude);

                __m128 andMask3 = _mm_and_ps(
                    _mm_cmpge_ps(t, constants::PACKED_DISTANCE_EPSILON),
                    _mm_cmplt_ps(t, cutoffDistance));

                return _mm_and_ps(andMask1, _mm_and_ps(andMask2, andMask3));
            }

            void SquareLight::updateRadiance() {
                // For diffuse emitters we have
                // power = radiance * area * pi
                // (AGI, Equation 2.12).

                radiance_ = power_ / (getArea() * PI_F);
            }

            float SquareLight::getArea() const {
                return area_;
            }

            const vec3f SquareLight::getRelativePosition(float u, float v) const {
                return basis1_ * (2*u-1) + basis2_ * (2*v-1);
            }

            const packed_vec3f SquareLight::getRelativePosition(__m128 u, __m128 v) const {
                u = _mm_sub_ps(_mm_add_ps(u,u), _mm_set_ps1(1.0f));
                v = _mm_sub_ps(_mm_add_ps(v,v), _mm_set_ps1(1.0f));

                return sse_->basis1 * u + sse_->basis2 * v;
            }

            const Spectrum SquareLight::getPower() const {
                return power_;
            }

            const Spectrum SquareLight::sampleIrradiance(
                    vec3f const& position, vec3f const& normal,
                    ITraceable const& scene) const {
                vec3f relativePosition = position - position_;

                if( vec3f::dot(normal_, relativePosition) <= 0 ) {
                    return Spectrum(0,0,0);
                }

                // In hemispherical formulation, irradiance is computed as
                // (AGI, Equation 2.12)
                //
                //   E = \int_{hemisphere} L(x <- w) cos(N_x, w) dw. 
                //
                // In area formulation, the corresponding equation is
                //
                //   E = \int_{area} L(x <- w) V(x,y) G(x,y) dA_y,
                //
                // where
                //
                //   G(x,y) = cos(N_x, w) * cos(N_y, w) / r^2(x,y).

                // Since L(x <- w) is constant, we only integrate
                // V(x,y) * G(x,y).

                __m128 packedScore = _mm_setzero_ps();

                float score = 0;

                random::EvenlySpacedSequence param1(sampleCount_,
                    random::Lcg::global().nextd());

                random::VanDerCorput param2;

                long seed = rand();

                param1.setSeed(seed);
                param2.setSeed(seed);

#if USE_PACKED_RAYS
                const size_t packedSampleCount = sampleCount_ / 4;
#else
                const size_t packedSampleCount = 0;
#endif

                math::packed_vec3f packedPosition(position);

                math::packed_vec3f packedNormal(normal);

                math::packed_vec3f packedRelativePosition(relativePosition);

                for(size_t i=0; i<packedSampleCount; ++i) {
                    __m128 u = _mm_set_ps(
                        param1.nextf(), param1.nextf(),
                        param1.nextf(), param1.nextf());

                    __m128 v = _mm_set_ps(
                        param2.nextf(), param2.nextf(),
                        param2.nextf(), param2.nextf());

                    packed_vec3f relativeLightPosition( getRelativePosition(u, v) );

                    packed_vec3f directions(relativeLightPosition - packedRelativePosition);

                    __m128 squareDistance = directions.squareLength();

                    __m128 distance = _mm_sqrt_ps(squareDistance);

                    __m128 cutoffDistance = _mm_sub_ps(
                        distance, constants::PACKED_DISTANCE_EPSILON);

                    __m128 invDistance = _mm_rcp_ps(distance);

                    directions *= invDistance; // normalize

                    packed_ray3f packedRay(packedPosition, directions);

                    __m128 dot1 = _mm_max_ps(
                        _mm_setzero_ps(), packedNormal.dot(directions));

                    __m128 dot2 = _mm_sub_ps(
                        _mm_setzero_ps(), sse_->normal.dot(directions));

                    __m128 mask = scene.raytraceShadow(packedRay, cutoffDistance, this);

                    __m128 contribution = _mm_andnot_ps(mask, _mm_mul_ps(
                        _mm_mul_ps(dot1, dot2), 
                        _mm_mul_ps(invDistance, invDistance)));

                    packedScore = _mm_add_ps(packedScore, contribution);
                }

                for(size_t i=0; i<4; ++i) {
                    score += packedScore.m128_f32[i];
                }

                for(size_t i=packedSampleCount * 4; i<sampleCount_; ++i) {
                    float u = param1.nextf();
                    float v = param2.nextf();

                    vec3f relativeLightPosition( getRelativePosition(u, v) );

                    // Notice: direction is unnormalized; it must
                    //         be later normalized.
                    vec3f direction( relativeLightPosition - relativePosition );

                    // Notice: dot1 is computed with unnormalized direction;
                    //         it must be later divided by direction.length().
                    float dot1 = vec3f::dot(normal, direction);

                    if(dot1 < 0) {
                        continue;
                    }

                    // Notice: dot2 is computed with unnormalized direction;
                    //         it must be later divided by direction.length().
                    float dot2 = -vec3f::dot(normal_, direction);

                    // Notice that dot2 cannot be non-positive;
                    // we already checked that at the start of the function.

                    float distance = direction.length();

                    float invDistance = 1 / distance;

                    direction *= invDistance;

                    ray3f ray(position, direction);

                    if( !scene.raytraceShadow(ray, distance - constants::DISTANCE_EPSILON, this) ) {
                        dot1 *= invDistance;
                        dot2 *= invDistance;

                        float contribution = dot1 * dot2 * (invDistance * invDistance);

                        score += contribution;
                    }
                }

                // The score was Monte Carlo integrated,
                // so we must multiply by \int_{area} dA_y = area
                // and divide by sample count.

                return radiance_ * (score * getArea() / sampleCount_);
            }

            const ray3f SquareLight::samplePhoton(Spectrum& power,
                    vec2f const& positionParameter,
                    vec2f const& directionParameter) const {
                power = power_;

                return ray3f(
                    position_ + getRelativePosition(
                        positionParameter.x,
                        positionParameter.y),
                    Hemisphere::cosineWeightedDirection(
                        normal_,
                        directionParameter.x,
                        directionParameter.y));
            }
        }
    }
}