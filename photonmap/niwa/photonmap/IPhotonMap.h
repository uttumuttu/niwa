/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_PHOTONMAP_IPHOTONMAP_H
#define NIWA_PHOTONMAP_IPHOTONMAP_H

#include "niwa/graphics/Spectrum.h"

namespace niwa {
    namespace math {
        class vec3f;
    }

    namespace photonmap {
        class Photon;
        class PhotonList;

        class IPhotonMap {
        public:
            virtual ~IPhotonMap();

            /**
             * Not thread-safe.
             */
            virtual void clear() = 0;

            /**
             * Thread-safe: can be called safely from multiple threads.
             *
             * @return Whether the photon could be added.
             */
            virtual bool __fastcall add(Photon const& photon) = 0;

            /**
             * Not thread-safe.
             */
            virtual void buildStructure() = 0;

            /**
             * Thread-safe: can be called safely from multiple threads.
             *
             * @return The sum of powers of nearby photons 
             *         (with a compatible surface normal),
             *         divided by the search surface area. 
             *         The result may be filtered.
             */
            virtual graphics::Spectrum __fastcall powerDensity(
                math::vec3f const& position,
                math::vec3f const& normal) const = 0;
        };
    }
}

#endif