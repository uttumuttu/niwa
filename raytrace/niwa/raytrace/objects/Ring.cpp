/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/objects/Ring.h"

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/Constants.h"

#include "niwa/math/vec3f.h"

using niwa::math::vec3f;

namespace {
    static inline float square(float x) {
        return x * x;
    }

    static const float height = 0.15f;
    static const float heightSquared = square(height);

    static const float innerRadius = 0.425f;
    static const float outerRadius = 0.575f;

    static const float boundingRadius = 
        sqrt(height*height + outerRadius*outerRadius) 
        + niwa::raytrace::constants::DISTANCE_EPSILON;

    static const float boundingRadiusSquared = square(boundingRadius);

    static const float innerRadiusSquared = square(innerRadius);
    static const float outerRadiusSquared = square(outerRadius);
}

namespace {
    static void loadIdentity(float* matrix) {
        for(int i=0; i<3; ++i) {
            for(int j=0; j<3; ++j) {
                matrix[i*3+j] = (i==j) ? 1.0f : 0.0f;
            }
        }
    }

    static void rotateX( vec3f& x, float rad ) {
        vec3f t = x;
        x.x = t.x;
        x.y = t.y * cos(rad) - t.z * sin(rad);
        x.z = t.y * sin(rad) + t.z * cos(rad);
    }

    static void rotateY( vec3f& x, float rad ) {
        vec3f t = x;
        x.x = t.x * cos(rad) - t.z * sin(rad);
        x.y = t.y;
        x.z = t.x * sin(rad) + t.z * cos(rad);
    }

    static void rotateZ( vec3f& x, float rad ) {
        vec3f t = x;
        x.x = t.x * cos(rad) - t.y * sin(rad);
        x.y = t.x * sin(rad) + t.y * cos(rad);
        x.z = t.z;
    }

    static inline float dotxyz(vec3f const& a, vec3f const& b) {
        return a.x*b.x + a.y*b.y + a.z*b.z;
    }

    static inline float dotxz(vec3f const& a, vec3f const& b) {
        return a.x*b.x + a.z*b.z;
    }

    /**
     * @param a Must be non-negative.
     *
     * @return The smaller positive solution to a quadratic equation, or zero
     *         if no solution exists.
     */
    static inline float quadraticMin( float a, float b, float c )    {
        float D = b*b-4*a*c;

        if( D >= 0 ) {
            if( a != 0 ) {
                //    (-b-sqrt(D))/(2.0*a) > 0    // a > 0
                //<=>  -b-sqrt(D) > 0  
                //<=>  -b > sqrt(D)  
                //<=>   b < 0, b*b > D  

                if( b < 0 && b*b > D ) {
                    return (-b-sqrt(D))/(2.0f*a);
                }
            }
        }
        return 0;
    }

    /**
     * @param a Must be non-negative.
     *
     * @return The larger positive solution to a quadratic equation, or zero
     *         if no solution exists.
     */
    static inline float quadraticMax( float a, float b, float c ) {
        float D = b*b-4*a*c;

        if( D >= 0 ) {
            if( a != 0 ) {
                //    (-b+sqrt(D))/(2.0*a) > 0  // a > 0
                //<=>  -b+sqrt(D) > 0 )
                //<=>     sqrt(D) > b )
                //<=>    b < 0 || D > b*b

                if( b < 0 || D > b*b ) {
                    return (-b+sqrt(D))/(2.0f*a);
                }
            }
        }
        return 0;
    }

    static vec3f multiply(float const* mat, vec3f const& vec) {
        return vec3f(
            mat[0] * vec.x + mat[1] * vec.y + mat[2] * vec.z,
            mat[3] * vec.x + mat[4] * vec.y + mat[5] * vec.z,
            mat[6] * vec.x + mat[7] * vec.y + mat[8] * vec.z);
    }

    static vec3f multiplyTranspose(float const* mat, vec3f const& vec) {
        return vec3f(
            mat[0] * vec.x + mat[3] * vec.y + mat[6] * vec.z,
            mat[1] * vec.x + mat[4] * vec.y + mat[7] * vec.z,
            mat[2] * vec.x + mat[5] * vec.y + mat[8] * vec.z);
    }
}

namespace niwa {
    using graphics::Spectrum;

    namespace raytrace {
        namespace objects {
            Ring::Ring() : spherePosition_(0,0,0) {
                loadIdentity(rotation_);
            }

            bool Ring::raytrace(ray3f const& ray, HitInfo& hit) const {
                Spectrum hitColor;

                vec3f hitNormal;

                float distance;
                
                if( traceRing(ray, hitColor, hitNormal, distance) ) {
                    vec3f hitPosition = ray.getPosition() + ray.getDirection() * distance;

                    hit.setValues(
                        distance, 
                        hitPosition, 
                        multiplyTranspose(rotation_, hitNormal),
                        Material::createSpecular(hitColor));

                    return true;
                } else {
                    return false;
                }
            }

            void Ring::computeRotation( float timeSeconds ) {
                float t = timeSeconds * .5f;

                spherePosition_ = vec3f(
                    sin(t)*0.25f, -0.0f, cos(t)*0.25f );

                vec3f bases[3];

                for(int i=0; i<3; ++i) {
                    bases[i][i] = 1;
                }

                for(int i=0; i<3; ++i) {
                    rotateX(bases[i], t*3);
                    rotateY(bases[i], t*2);
                    rotateZ(bases[i], t*1);
                }

                for(int i=0; i<3; ++i) {
                    for(int j=0; j<3; ++j) {
                        rotation_[i*3+j] = bases[j][i];
                    }
                }
            }

            /**
             * @return No hit: 0; hit with outer cylinder or caps: 1; hit with inner cylinder (shadow test): 2.
             */
            int Ring::traceRing(
                    ray3f const& ray, Spectrum& color,
                    vec3f& normal, float& distance) const {
                // Notes: cylinder direction is defined to be (0,1,0).

                vec3f rayp = ray.getPosition() - spherePosition_;

                // Rotation-invariant bounding sphere check.
                {
                    vec3f const& rayd = ray.getDirection();

                    float b = vec3f::dot(rayd, rayp);
                    float c = vec3f::dot(rayp, rayp) - boundingRadiusSquared;

                    if( b*b < c ) return 0;
                }

                rayp = multiply(rotation_, rayp);
                vec3f rayd = multiply(rotation_, ray.getDirection());

                if( rayd.y != 0 ) // check caps
                {
                    // (rayp.y+rayd.y*t) = h
                    // (rayp.y+rayd.y*t) = -h
                    // t =  (h-rayp.y)/rayd.y
                    // t = (-h-rayp.y)/rayd.y
                    
                    if( rayp.y > height && rayd.y < 0 )
                    {
                        distance = (height - rayp.y) / rayd.y;

                        vec3f p = rayp + rayd * distance;

                        if( dotxz(p,p) >= innerRadiusSquared &&
                            dotxz(p,p) <= outerRadiusSquared )
                        {
                            normal = vec3f(0,1,0);
                            color = Spectrum(1,0.7f,0.5f);
                            return 1;
                        }
                    }

                    if( rayp.y < -height && rayd.y > 0 )
                    {
                        distance = (-height - rayp.y)/rayd.y;

                        vec3f p = rayp + rayd * distance;

                        if( dotxz(p,p) >= innerRadiusSquared &&
                            dotxz(p,p) <= outerRadiusSquared )
                        {
                            normal = vec3f(0,-1,0);
                            color = Spectrum(1,0.7f,0.5f);
                            return 1;
                        }
                    }
                }

                // Coefficients for cylindral quadratic equation.
                float a = dotxz(rayd, rayd);
                float b = dotxz(rayp, rayd)*2;
                float c = dotxz(rayp, rayp);

                // check cylinders

                if( dotxz(rayp,rayp) > outerRadiusSquared ) {
                    distance = quadraticMin(a,b,c - outerRadiusSquared);

                    if( distance > 0 ) {
                        float py = rayp.y + rayd.y * distance;

                        if( py*py <= heightSquared ) {
                            normal = vec3f(
                                rayp.x + rayd.x * distance, 0,
                                rayp.z + rayd.z * distance);

                            normal.normalize();

                            color = Spectrum(1,1,0.7f);
                            return 1;
                        }
                    }
                }

                distance = quadraticMax(a,b,c - innerRadiusSquared);

                if( distance > 0 ) {
                    float py = rayp.y + rayd.y * distance;

                    if( py*py <= heightSquared ) {
                        normal = vec3f(
                            rayp.x + rayd.x * distance, 0,
                            rayp.z + rayd.z * distance);

                        normal.normalize();

                        color = Spectrum(1,0.9f,0.8f);
                        return 2;
                    }
                }
                
                return 0;
            }
        }
    }
}