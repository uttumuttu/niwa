/**
 * @file
 * @author Mikko Kauppila
 * 
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_PHOTONMAP_PHOTONHASH_H
#define NIWA_PHOTONMAP_PHOTONHASH_H

#include "IPhotonMap.h"

#define NOMINMAX
#include <windows.h>

namespace niwa {
    namespace math {
        class vec3f;
    }

    namespace photonmap {
        class PhotonHash : public IPhotonMap {
        public:
            /**
             * @param capacity How many photons can be added to the hash.
             *
             * @param searchRadius The radius used for searching photons in the hash.
             */
            PhotonHash(size_t capacity, double searchRadius);
            ~PhotonHash();

        public: // from IPhotonMap
            void clear();

            bool __fastcall add(Photon const& photon);

            void buildStructure();

            graphics::Spectrum __fastcall powerDensity(
                math::vec3f const& position,
                math::vec3f const& normal) const;

        private:
            void getGridPosition(
                math::vec3f const& position,
                int& x, int& y, int& z) const;

        private: // prevent copying
            PhotonHash(PhotonHash const&);
            PhotonHash& operator = (PhotonHash const&);

        private:
            class GridElement;

        private:
            const size_t capacity_;

            const float radius_; // search radius

            LONG volatile size_;

            Photon* photons_; // owned

            int n_; // grid size

            GridElement* grid_; // 3D grid, owned
        };
    }
}

#endif