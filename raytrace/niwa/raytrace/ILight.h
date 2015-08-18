/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_ILIGHT_H
#define NIWA_RAYTRACE_ILIGHT_H

#include "ITraceable.h"

#include "ray3f.h"

#include "niwa/graphics/Spectrum.h"

namespace niwa {
    namespace math {
        class vec2f;
        class vec3f;
    }

    namespace raytrace {
        /**
         * Light source.
         */
        class ILight : public ITraceable {
        public:
            virtual const graphics::Spectrum __fastcall getPower() const = 0;

            virtual const graphics::Spectrum __fastcall sampleIrradiance(
                math::vec3f const& position,
                math::vec3f const& normal,
                ITraceable const& scene) const = 0;

            /**
             * Samples an initial photon for photon tracing.
             * In expectation, the photon carries the whole
             * power of the light (the power can be split
             * easily in photon tracing).
             *
             * @param power Contains the amount of power carried
             *              by this photon. Expected value equal
             *              to getPower(), but may vary due to sampling.
             *
             * @param positionParameter A hint for photon position, in [0,1) x [0,1).
             *
             * @param directionParameter A hint for photon direction, in [0,1) x [0,1).
             */
            virtual const ray3f __fastcall samplePhoton(graphics::Spectrum& power,
                    math::vec2f const& positionParameter,
                    math::vec2f const& directionParameter) const = 0;
        };
    }
}

#endif