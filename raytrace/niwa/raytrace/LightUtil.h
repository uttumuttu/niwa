/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_LIGHTUTIL_H
#define NIWA_RAYTRACE_LIGHTUTIL_H

namespace niwa {
    namespace raytrace {
        /**
         * Lighting utilities.
         */
        class LightUtil {
        public:
            /**
             * @return Constant radiance emitted by a diffuse emitter with
             *         given power and surface area.
             */
            static Spectrum getEmittedRadiance(Spectrum const& power, float surfaceArea);

        private: // prevent instantiation
            LightUtil();
            LightUtil(LightUtil const&);
            LightUtil& operator = (LightUtil const&);
        };
    }
}

#endif