/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/Hemisphere.h"

#include "niwa/math/vec3f.h"

#include "niwa/math/Constants.h"

namespace {
    static inline float square(float x) {
        return x * x;
    }

    static inline float quintic(float x1) {
        float x2 = x1*x1;
        float x4 = x2*x2;

        return x4 * x1;
    }
}

namespace niwa {
    namespace raytrace {
        using math::constants::PI_F;

        using math::vec3f;

        const vec3f Hemisphere::cosineWeightedDirection(
                vec3f const& normal, float u, float v) {
            // See AGI page 66 for derivation.

            float phi = 2 * PI_F * u;

            float sin_theta = sqrt(1-v); // = sin(acos(sqrt(v)))
            float cos_theta = sqrt(v);   // = cos(acos(sqrt(v)))

            float x = cos(phi) * sin_theta;
            float y = sin(phi) * sin_theta;
            float z = cos_theta;

            vec3f bx(1,0,0);

            if( fabs(vec3f::dot(bx, normal)) > .5f * .5f ) {
                bx = vec3f(0,1,0);
            }

            bx -= normal * vec3f::dot(normal, bx);
            bx.normalize();

            vec3f by( vec3f::cross(bx, normal) );

            return bx*x + by*y + normal*z;
        }

        const math::vec3f Hemisphere::mirrorReflection(
                math::vec3f const& normal, math::vec3f const& incomingDirection) {
            return incomingDirection - normal * (2 * vec3f::dot(incomingDirection, normal));
        }

        const float Hemisphere::fresnelCoefficient(
                math::vec3f const& normal, math::vec3f const& incidentDirection,
                float incidentRefractiveIndex,
                float excitantRefractiveIndex) {
            float n1 = incidentRefractiveIndex; // shorthands
            float n2 = excitantRefractiveIndex;

            // Fresnel coefficient at normal incidence 
            // (Wann Jensen, Equations 2.28 and 2.29)
            float f0 = square( (n1-n2) / (n1+n2) );

            float cosine = -vec3f::dot(incidentDirection, normal);

            // Wann Jensen, Equation 2.30 (Schlick approximation).
            return f0 + (1 - f0) * quintic(1 - cosine);
        }

        const bool Hemisphere::computeRefraction(
                math::vec3f const& normal, math::vec3f const& incidentDirection,
                float incidentRefractiveIndex,
                float excitantRefractiveIndex,
                math::vec3f& excitantDirection) {
            // See Wann Jensen Equation 2.32 for derivation.

            // Relative index of refraction.
            float eta = incidentRefractiveIndex / excitantRefractiveIndex;

            float cosine = -vec3f::dot(normal, incidentDirection);

            float outOfPlaneCoefficient = 1 - square(eta) * (1 - square(cosine));

            if(outOfPlaneCoefficient < 0) {
                return false; // total internal reflection
            } else {
                outOfPlaneCoefficient = -sqrt(outOfPlaneCoefficient);
            }

            float inPlaneCoefficient = -eta;

            excitantDirection = (-incidentDirection - normal * cosine) * inPlaneCoefficient
                + normal * outOfPlaneCoefficient;

            return true;
        }
    }
}