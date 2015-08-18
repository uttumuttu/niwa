/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_PHOTONMAP_HILBERTPHOTONHASH_H
#define NIWA_PHOTONMAP_HILBERTPHOTONHASH_H

#include "IPhotonMap.h"

#define NOMINMAX
#include <windows.h>

#include <vector>

namespace niwa {
    namespace math {
        class vec3f;
    }

    namespace photonmap {
        /**
         * An approximate photon hash based
         * on the Hilbert space-filling curve.
         */
        class HilbertPhotonHash : public IPhotonMap {
        public:
            HilbertPhotonHash(size_t capacity, size_t nNeighbors_);
            ~HilbertPhotonHash();

        public: // from IPhotonMap
            void clear();

            bool __fastcall add(Photon const& photon);

            void buildStructure();

            graphics::Spectrum __fastcall powerDensity(
                math::vec3f const& position,
                math::vec3f const& normal) const;

        private: // prevent copying
            HilbertPhotonHash(HilbertPhotonHash const&);
            HilbertPhotonHash& operator = (HilbertPhotonHash const&);

        private:
            struct IndexPhotonPair;

        private:
            size_t const capacity_;

            size_t const nNeighbors_;

        public: // Thread-safe store for photons before structure-building.
            LONG volatile size_;

            IndexPhotonPair* pairs_; // owned
        };
    }
}

#endif