/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_HEMISPHERE_H
#define NIWA_RAYTRACE_HEMISPHERE_H

namespace niwa {
    namespace math {
        class vec3f;
    }

    namespace raytrace {
        class Hemisphere {
        public:
            /**
             * Creates a cosine-weighted direction
             * above a hemisphere.
             *
             * @param normal Unit surface normal.
             * @param u A uniformly distributed surface parameter in [0, 1).
             * @param v A uniformly distributed surface parameter in [0, 1).
             */
            static const math::vec3f cosineWeightedDirection(
                math::vec3f const& normal, float u, float v);

            /**
             * @param normal Unit surface normal.
             *
             * @param incidentDirection Incident direction (inside the surface).
             *
             * @return Outgoing direction (outside the surface).
             */
            static const math::vec3f mirrorReflection(
                    math::vec3f const& normal, math::vec3f const& incidentDirection);

            static const float fresnelCoefficient(
                math::vec3f const& normal, math::vec3f const& incidentDirection,
                float incidentRefractiveIndex,
                float excitantRefractiveIndex);

            /**
             * @param normal Unit surface normal (outwards).
             *
             * @param incidentDirection
             *         Incident direction (inside the surface).
             *
             * @param incidentRefractiveIndex Refractive index in the incident medium.
             *
             * @param excitantRefractiveIndex Refractive index in the excitant medium.
             *
             * @param excitantDirection 
             *         Contains the excitant direction (inside the surface)
             *         if there was no total internal reflection.
             *
             * @return True if there was no total internal reflection;
             *         false if there was total internal reflection.
             */
            static const bool computeRefraction(
                    math::vec3f const& normal, math::vec3f const& incidentDirection,
                    float incidentRefractiveIndex,
                    float excitantRefractiveIndex,
                    math::vec3f& excitantDirection);

        private: // prevent instantiation
            Hemisphere();
            Hemisphere(Hemisphere const&);
            Hemisphere& operator = (Hemisphere const&);
        };
    }
}

#endif