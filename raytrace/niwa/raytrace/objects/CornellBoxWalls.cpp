/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/CornellBoxWalls.h"

#include "niwa/raytrace/Constants.h"

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"

#include "niwa/math/vec3f.h"

namespace niwa {
    using graphics::Spectrum;

    namespace raytrace {
        namespace objects {
            using math::vec3f;
            using math::vec3i;

            using constants::DISTANCE_EPSILON;

            CornellBoxWalls::CornellBoxWalls() 
                : defaultMaterial_(Material::createDiffuse(Spectrum(.3f,.3f,.3f))),
                leftWall_(Material::createDiffuse(Spectrum(.1f,.9f,.0f))),
                rightWall_(Material::createDiffuse(Spectrum(.9f,.1f,.0f))) {
                // ignored
            }

#define UNROLL_FOR_2(IDX, BODY) {const int IDX=0; BODY;} {const int IDX=1; BODY;}

            bool CornellBoxWalls::raytrace(ray3f const& ray, HitInfo& hitInfo) const {
                static const int neighbors[3][2] = {{1,2},{0,2},{0,1}};

                vec3f const& p = ray.getPosition();
                vec3f const& d = ray.getDirection();

                vec3f const& inv = ray.getDirectionInverses();

                vec3i const& signs = ray.getSigns();

                vec3f hitPosition = vec3f::createUninitialized();

                for(int i=0; i<3; ++i) {
                    const int sign = signs[i] * 2 - 1;

                    const float fSign = static_cast<float>(sign);

                    // For efficiency we only compute the
                    // negated time at this point.
                    const float negT = (p[i] + fSign) * inv[i];

                    UNROLL_FOR_2(j, {
                        const int k = neighbors[i][j];

                        hitPosition[k] = p[k] - d[k] * negT;

                        if(hitPosition[k] * hitPosition[k] > (1 + DISTANCE_EPSILON)) {
                            continue;
                        }
                    })

                    hitPosition[i] = p[i] - d[i] * negT;

                    vec3f normal(0,0,0);

                    normal[i] = fSign;

                    Material const*  mat = &defaultMaterial_;

                    if(i == 0) {
                        if(sign == 1) {
                            mat = &rightWall_;
                        } else {
                            mat = &leftWall_;
                        }
                    }

                    hitInfo.setValues(
                        -negT, hitPosition, normal, *mat);

                    return true;
                }

                return false;
            }

            bool CornellBoxWalls::raytraceShadow(
                    ray3f const&, float, ILight const*) const {
                // Since we assume the walls to be the boundary of the scene,
                // it never casts shadows.
                return false;
            }

            __m128 CornellBoxWalls::raytraceShadow(
                packed_ray3f const&, __m128, ILight const*) const {
                // Since we assume the walls to be the boundary of the scene,
                // it never casts shadows.
                return _mm_setzero_ps();
            }
        }
    }
}