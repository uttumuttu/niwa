/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/TraceableLevelSet.h"

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"

#include "niwa/levelset/ILevelSet.h"

#include "niwa/math/vec3f.h"

namespace {
    /**
     * This distance epsilon is deliberately defined
     * independently from niwa::raytrace::constants::DISTANCE_EPSILON,
     * since level sets have their unique needs for distance thresholds,
     * not governed only by roundoff errors but by convergence as well.
     */
    static const double LEVEL_SET_DISTANCE_EPSILON = 1e-3;

    static const int MAX_ITERATIONS = 64;
}

using boost::shared_ptr;

namespace niwa {
    using math::vec3f;
    using levelset::ILevelSet;

    namespace raytrace {
        namespace objects {
            TraceableLevelSet::TraceableLevelSet(
                    shared_ptr<ILevelSet> levelSet, 
                    Material const& material) 
                : levelSet_(levelSet), material_(material) {
                // ignored
            }

            TraceableLevelSet::~TraceableLevelSet() {
                // ignored
            }

            /**
             * Implements a simple ray marching algorithm
             * for ray tracing signed distance fields.
             */
            bool TraceableLevelSet::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                vec3f position = ray.getPosition();
                vec3f direction = ray.getDirection();

                float distanceTraveled = 0;

                for(int i=0; i<MAX_ITERATIONS; ++i) {
                    float safeDistance = levelSet_->value(position);

                    if(safeDistance <= LEVEL_SET_DISTANCE_EPSILON
                        && distanceTraveled >= LEVEL_SET_DISTANCE_EPSILON) {
                        vec3f gradient = levelSet_->gradient(position);

                        if(vec3f::dot(direction, gradient) < 0) {
                            hitInfo.setValues(
                                distanceTraveled,
                                position,
                                gradient,
                                material_);

                            return true;
                        }
                    }

                    position += direction * safeDistance;

                    distanceTraveled += safeDistance;
                }

                return false;
            }
        }
    }
}