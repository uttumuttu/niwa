/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/CompositeTraceable.h"

#include "niwa/raytrace/HitInfo.h"

#include <xmmintrin.h>

using boost::shared_ptr;

namespace niwa {
    namespace raytrace {
        namespace objects {
            CompositeTraceable::CompositeTraceable() {
                // ignored
            }

            CompositeTraceable::~CompositeTraceable() {
                // ignored
            }

            void CompositeTraceable::setObjects(
                    std::vector<shared_ptr<ITraceable>> const& objects) {
                objects_ = objects;
            }

            bool CompositeTraceable::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                HitInfo candidateHit = HitInfo::createUninitialized();

                bool hitFound = false;

                const size_t n = objects_.size();

                boost::shared_ptr<ITraceable> const*const objects = &objects_[0];

                for(size_t i=0; i<n; ++i) {
                    if( objects[i]->raytrace(ray, candidateHit) ) {
                        if(!hitFound || candidateHit.distance() < hitInfo.distance()) {
                            hitInfo = candidateHit;

                            hitFound = true;
                        }
                    }
                }
                return hitFound;
            }

            bool CompositeTraceable::raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const {
                const size_t n = objects_.size();

                boost::shared_ptr<ITraceable> const*const objects = &objects_[0];

                for(size_t i=0; i<n; ++i) {
                    if( objects[i]->raytraceShadow(ray, cutoffDistance, light) ) {
                        return true;
                    }
                }
                return false;
            }

            __m128 CompositeTraceable::raytraceShadow(
                packed_ray3f const& ray, 
                const __m128 cutoffDistance, 
                ILight const* const light) const {
                __m128 mask = _mm_setzero_ps();

                const size_t n = objects_.size();

                boost::shared_ptr<ITraceable> const*const objects = &objects_[0];

                for(size_t i=0; i<n; ++i) {
                    mask = _mm_or_ps(mask,
                        objects[i]->raytraceShadow(ray, cutoffDistance, light));

                    // If all rays are found to be shadowed, return early.
                    if(mask.m128_i32[0] && mask.m128_i32[1]
                        && mask.m128_i32[2] && mask.m128_i32[3]) {
                        return mask;
                    }
                }
                return mask;
            }
        }
    }
}