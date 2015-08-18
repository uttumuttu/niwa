/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/photonmap/PhotonHash.h"

#include "niwa/photonmap/Photon.h"

#include "niwa/math/packed_vec3f.h"

#include "niwa/graphics/PackedSpectrum.h"

#include "niwa/math/Constants.h"

using niwa::math::constants::PI_F;

using niwa::math::packed_vec3f;
using niwa::graphics::PackedSpectrum;

using niwa::graphics::Spectrum;

#include <algorithm>

#include <vector>

/**
 * Photon filter. Zero for no filter;
 * one for Epanechnikov filter.
 */
#define FILTER 0

namespace niwa {
    namespace photonmap {
        using math::vec3f;

        __declspec(align(16)) struct PackedPhoton {
            void set(int i, Photon const& photon) {
                position.set(i, photon.position());

                normal.set(i, photon.normal());

                power.set(i, photon.power());
            }

            packed_vec3f position;

            packed_vec3f normal;

            PackedSpectrum power;
        };

        class PhotonHash::GridElement {
        public:
            GridElement() : packedPhotons_(0), nPackedPhotons_(0) {
                // ignored
            }

            ~GridElement() {
                _mm_free(packedPhotons_);
            }

            /**
             * Not thread-safe.
             */
            void  clear() {
                photons_.clear();

                if(packedPhotons_) {
                    _mm_free(packedPhotons_);
                    packedPhotons_ = 0;
                    nPackedPhotons_ = 0;
                }
            }

            /**
             * Not thread-safe.
             */
            void addPhoton(Photon const& photon) {
                photons_.push_back(photon);
            }

            /**
             * Called after all photons have been added to the element.
             * Not thread-safe.
             */
            void buildStructure() {
                if(packedPhotons_) {
                    _mm_free(packedPhotons_);
                    packedPhotons_ = 0;
                    nPackedPhotons_ = 0;
                }

                nPackedPhotons_ = photons_.size() / 4;

                if((photons_.size() % 4) != 0) {
                    ++nPackedPhotons_;
                }

                packedPhotons_ = static_cast<PackedPhoton*>(
                    _mm_malloc(sizeof(PackedPhoton) * nPackedPhotons_, 16));

                if((photons_.size() % 4) != 0) {
                    // The packed photons were allocated
                    // manually without placement new; therefore, their constructors
                    // were not called. (This is actually good for performance.)

                    // Since some of the components of the padding
                    // packed photon may be left undefined ("dummy" photons),
                    // we must zero the last packed photon manually.

                    memset(
                        &packedPhotons_[nPackedPhotons_ - 1], 0, 
                        sizeof(PackedPhoton));
                }

                for(size_t i=0; i<nPackedPhotons_; ++i) {
                    size_t bound = std::min<size_t>(4, photons_.size() - i*4);

                    for(size_t j=0; j<bound; ++j) {
                        packedPhotons_[i].set(j, photons_[i*4+j]);
                    }
                }
            }

        private: // prevent copying
            GridElement(GridElement const&);
            GridElement& operator = (GridElement const&);

        private:
            std::vector<Photon> photons_;

            size_t nPackedPhotons_;

            PackedPhoton* packedPhotons_;

            friend class PhotonHash;
        };

        PhotonHash::PhotonHash(size_t capacity, double searchRadius) 
            : capacity_(capacity), 
              radius_(static_cast<float>(searchRadius)), size_(0) {
            photons_ = new Photon[capacity_];

            double diameter = 2 * sqrt(2.0);

            n_ = std::max(1, static_cast<int>(diameter / searchRadius));

            grid_ = new GridElement [n_ * n_ * n_];
        }

        PhotonHash::~PhotonHash() {
            delete[] grid_;
            delete[] photons_;
        }

        void PhotonHash::clear() {
            size_ = 0;

            for(int i=0; i<n_ * n_ * n_; ++i) {
                grid_[i].clear();
            }
        }

        bool PhotonHash::add(Photon const& photon) {
            size_t next = InterlockedIncrement(&size_) - 1;

            if(next < capacity_) {
                photons_[next] = photon;
                return true;
            } else {
                size_ = capacity_ - 1;
                return false;
            }
        }

        void PhotonHash::getGridPosition(
                vec3f const& position,
                int& x, int& y, int& z) const {
            x = static_cast<int>((position.x + 1) * .5f * n_);
            y = static_cast<int>((position.y + 1) * .5f * n_);
            z = static_cast<int>((position.z + 1) * .5f * n_);

            x = std::max(0, std::min(n_-1, x));
            y = std::max(0, std::min(n_-1, y));
            z = std::max(0, std::min(n_-1, z));
        }

        void PhotonHash::buildStructure() {
            int x,y,z;

            for(int i=0; i<size_; ++i) {
                getGridPosition(
                    photons_[i].position(), x,y,z);

                grid_[(z*n_ + y)*n_ + x].addPhoton(photons_[i]);
            }

            for(int i=0; i<n_ * n_ * n_; ++i) {
                grid_[i].buildStructure();
            }
        }

        Spectrum PhotonHash::powerDensity(
                math::vec3f const& unpackedPosition,
                math::vec3f const& unpackedNormal) const {
            const float unpackedRadius = radius_;

            vec3f const radiusVector(unpackedRadius, unpackedRadius, unpackedRadius);

            int xMin, yMin, zMin;
            int xMax, yMax, zMax;

            getGridPosition(unpackedPosition - radiusVector, xMin, yMin, zMin);
            getGridPosition(unpackedPosition + radiusVector, xMax, yMax, zMax);

            // The surface normal is deliberately modified
            // to have negative z direction. This will later
            // lead to savings when comparing dot products
            // with the normal against zeros.
            const vec3f modifiedUnpackedNormal(
                unpackedNormal.x, unpackedNormal.y, -unpackedNormal.z);

            const float unpackedRadiusSquared = unpackedRadius * unpackedRadius;
            const packed_vec3f position(unpackedPosition);
            const __m128 radiusSquared = _mm_set_ps1(unpackedRadiusSquared);
            const packed_vec3f modifiedNormal(modifiedUnpackedNormal);
            PackedSpectrum powerScore;

#if FILTER
            const __m128 invRadiusSquared = _mm_set_ps1(1 / unpackedRadiusSquared);

            const __m128 one = _mm_set_ps1(1.0f);
#endif

            for(int z=zMin; z<=zMax; ++z) {
                for(int y=yMin; y<=yMax; ++y) {
                    int gridPosition = (z*n_ + y)*n_ + xMin;

                    for(int x=xMin; x<=xMax; ++x, ++gridPosition) {
                        GridElement const& element = grid_[ gridPosition ];

                        const size_t m = element.nPackedPhotons_;

                        PackedPhoton const*const photons = element.packedPhotons_;

                        for(size_t i=0; i<m; ++i) {
                            PackedPhoton const& photon = photons[i];

                            const packed_vec3f delta(photon.position - position);

                            const __m128 squareLength = delta.squareLength();

                            const __m128 cond1 = _mm_cmple_ps(squareLength, radiusSquared);

                            const __m128 cond2 = _mm_cmpgt_ps(
                                _mm_add_ps(
                                    _mm_mul_ps(
                                        photon.normal.x, modifiedNormal.x),
                                    _mm_mul_ps(
                                        photon.normal.y, modifiedNormal.y)),
                                _mm_mul_ps(
                                    photon.normal.z, modifiedNormal.z));

#if FILTER
                            const __m128 weight = _mm_sub_ps(
                                one, _mm_mul_ps(squareLength, invRadiusSquared));

                            powerScore += 
                                photon.power * _mm_and_ps(weight, _mm_and_ps(cond1, cond2));
#else
                            powerScore += photon.power & _mm_and_ps(cond1, cond2);
#endif
                        }
                    } // x-loop
                } // y-loop
            } // z-loop

            Spectrum unpackedPowerScore;

            for(int i=0; i<4; ++i) {
                unpackedPowerScore += powerScore.get(i);
            }

#if FILTER
            return unpackedPowerScore / ((0.5f * PI_F) * unpackedRadiusSquared);
#else
            return unpackedPowerScore / (PI_F * unpackedRadiusSquared);
#endif
        }
    }
}