/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_EXPONENTIALTONER_H
#define NIWA_RAYTRACE_EXPONENTIALTONER_H

#include "IToneMapper.h"

namespace niwa {
    namespace raytrace {
        class ExponentialToner : public IToneMapper {
        public:
            ExponentialToner();

            /**
             * Sets the constant of exponentiation,
             * which is assumed constant for all wavelengths.
             *
             * @param strength Pixels per unit of irradiation (J/m^2).
             */
            void setStrength(float strength);

            const graphics::Spectrum __fastcall toneMap(
                graphics::Spectrum const& irradiation) const;

        private:
            float strength_;
        };
    }
}

#endif