/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_ITONEMAPPER_H
#define NIWA_RAYTRACE_ITONEMAPPER_H

#include "niwa/graphics/Spectrum.h"

namespace niwa {
    namespace raytrace {
        class IToneMapper {
        public:
            virtual ~IToneMapper();

            /**
             * @param irradiation Irradiation on a point in film plane (J/m^2).
             *
             * @return Corresponding color on image plane.
             */
            virtual const graphics::Spectrum __fastcall toneMap(
                graphics::Spectrum const& irradiation) const = 0;
        };
    }
}

#endif