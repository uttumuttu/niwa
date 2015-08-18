/**
 * @file
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_PHOTONMAP_PHOTON_INL
#define NIWA_PHOTONMAP_PHOTON_INL

namespace niwa {
    namespace photonmap {
        Photon::Photon() {
            // ignored
        }

        Photon::Photon(
                math::vec3f const& position,
                math::vec3f const& normal,
                graphics::Spectrum const& power) 
            : position_(position),
              normal_(normal),
              power_(power) {
            // ignored
        }


        math::vec3f const& Photon::position() const {
            return position_;
        }

        math::vec3f const& Photon::normal() const {
            return normal_;
        }

        graphics::Spectrum const& Photon::power() const {
            return power_;
        }
    }
}

#endif