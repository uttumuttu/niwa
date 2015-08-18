/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_PHOTONMAP_PHOTON_H
#define NIWA_PHOTONMAP_PHOTON_H

#include "niwa/graphics/Spectrum.h"
#include "niwa/math/vec3f.h"

namespace niwa {
    namespace photonmap {
        /**
         * Photon hit.
         */
        class Photon {
        public:
            inline Photon();

            inline Photon(
                math::vec3f const& position,
                math::vec3f const& normal,
                graphics::Spectrum const& power);

            inline math::vec3f const& position() const;

            /**
             * Unit normal of the surface where the photon hit.
             */
            inline math::vec3f const& normal() const;

            inline graphics::Spectrum const& power() const;
        private:
            math::vec3f position_;

            math::vec3f normal_;

            graphics::Spectrum power_;
        };
    }
}

#include "Photon.inl"

#endif