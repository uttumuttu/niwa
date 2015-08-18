/**
 * @file
 * @author Mikko Kauppila 
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#include "HilbertPhotonHash.h"

#include "niwa/photonmap/Photon.h"

#include "niwa/geom/Hilbert.h"

#include "niwa/math/Constants.h"

using niwa::math::constants::PI_F;

#include <algorithm>

namespace {
    /**
     * Consider what happens when ORDER_BITS * 3 > 32;
     * then we cannot simply compare indices. Therefore,
     * the maximum order is 10.
     */
    static const size_t ORDER_BITS = 10;

    static const size_t ORDER_POW2 = 1U << ORDER_BITS;

    /**
     * Converts Euclidean coordinates to grid positions.
     */
    static __forceinline void fGetGridPosition(
            niwa::math::vec3f const& vec, 
            niwa::geom::HilbertPoint<3,ORDER_BITS>& point) {
        niwa::math::vec3f const temp = 
            (vec + niwa::math::vec3f(1,1,1)) * (.5f * ORDER_POW2);

        for(size_t i=0; i<3; ++i) {
            int elt = static_cast<int>(temp[i]);

            if(elt < 0) {
                point.elements[i] = 0;
            } else if(elt >= ORDER_POW2) {
                point.elements[i] = ORDER_POW2;
            } else {
                point.elements[i] = elt;
            }
        }
    }

    static __forceinline size_t fGetHilbertIndex(niwa::math::vec3f const& vec) {
        niwa::geom::HilbertPoint<3, ORDER_BITS> point;

        fGetGridPosition(vec, point);

        point = niwa::geom::Hilbert<3, ORDER_BITS>::encode(point);

        return point.index();
    }

    /**
     * @return The distance between the two unsigned indices.
     */
    static __forceinline size_t fIndexDistance(size_t lhs, size_t rhs) {
        // Because the values are unsigned, 
        // we cannot use the absolute value function.

        if(lhs < rhs) {
            return rhs - lhs;
        } else {
            return lhs - rhs;
        }
    }
}

namespace niwa {
    namespace photonmap {
        struct HilbertPhotonHash::IndexPhotonPair {
            IndexPhotonPair() : hilbertIndex(0) {
                // ignored
            }

            IndexPhotonPair(size_t aHilbertIndex, Photon const& aPhoton)
                : hilbertIndex(aHilbertIndex), photon(aPhoton) {
                // ignored
            }

            size_t hilbertIndex;
            Photon photon;
        };

        HilbertPhotonHash::HilbertPhotonHash(size_t capacity, size_t nNeighbors) 
            : capacity_(capacity), nNeighbors_(nNeighbors), pairs_(0), size_(0) {
            pairs_ = new IndexPhotonPair[capacity_];
        }

        HilbertPhotonHash::~HilbertPhotonHash() {
            delete[] pairs_;
        }

        void HilbertPhotonHash::clear() {
            size_ = 0;
        }

        bool __fastcall HilbertPhotonHash::add(Photon const& photon) {
            size_t const hilbertIndex = fGetHilbertIndex(photon.position());

            size_t next = InterlockedIncrement(&size_) - 1;

            if(next < capacity_) {
                pairs_[next] = IndexPhotonPair(hilbertIndex, photon);
                return true;
            } else {
                size_ = capacity_ - 1;
                return false;
            }
        }

        void HilbertPhotonHash::buildStructure() {
            size_t const nPairs = static_cast<size_t>(size_);

            struct SortingPair {
                SortingPair(size_t aHilbertIndex, size_t aPairIndex)
                    : hilbertIndex(aHilbertIndex), pairIndex(aPairIndex) {
                    // ignored
                }

                bool const operator < (SortingPair const& rhs) const {
                    return hilbertIndex < rhs.hilbertIndex;
                }

                size_t hilbertIndex;
                size_t pairIndex;
            };

            std::vector<SortingPair> sortingPairs;

            for(size_t i=0; i<nPairs; ++i) {
                sortingPairs.push_back(SortingPair(pairs_[i].hilbertIndex, i));
            }

            std::sort(sortingPairs.begin(), sortingPairs.end());

            std::vector<IndexPhotonPair> sortedPairs;

            for(size_t i=0; i<nPairs; ++i) {
                sortedPairs.push_back(pairs_[sortingPairs[i].pairIndex]);
            }

            for(size_t i=0; i<nPairs; ++i) {
                pairs_[i] = sortedPairs[i];
            }
        }

        graphics::Spectrum __fastcall HilbertPhotonHash::powerDensity(
                math::vec3f const& position,
                math::vec3f const& normal) const {
            if(size_ == 0) {
                return niwa::graphics::Spectrum(0,0,0);
            }

            size_t const hilbertIndex = fGetHilbertIndex(position);

            // First phase: find the Hilbert-nearest photon.

            size_t start = 0;
            size_t end = size_;
            size_t mid = (start + end) / 2;

            size_t val = pairs_[mid].hilbertIndex;

            // Notice that the start position has to be compared
            // with the mid-position; comparing with the end position
            // could cause an infinite loop when the difference between
            // start and end positions stalls at one.
            while(start != mid && val != hilbertIndex) {
                if(val < hilbertIndex) {
                    start = mid;
                } else {
                    end = mid;
                }

                mid = (start + end) / 2;
                val = pairs_[mid].hilbertIndex;
            }

            // Phase two: extend the Hilbert-nearest photon to a range
            // of Hilbert-nearest photons. From now on, start and end refer
            // to the endpoints of the Hilbert-nearest photons.

            start = mid;
            end = mid+1; // exclusive

            size_t nNeighbors = std::min<size_t>(nNeighbors_, size_);

            if(start == 0) {
                end = nNeighbors;
            } else if(end == static_cast<size_t>(size_)) {
                start = end - nNeighbors;
            } else {
                size_t startDistance = fIndexDistance(
                    hilbertIndex, pairs_[start].hilbertIndex);

                size_t endDistance = fIndexDistance(
                    hilbertIndex, pairs_[end-1].hilbertIndex);

                for(size_t i=0; i<nNeighbors; ++i) {
                    if(startDistance < endDistance) {
                        if(--start == 0) {
                            end = nNeighbors;
                            break;
                        } else {
                            startDistance = fIndexDistance(
                                hilbertIndex, pairs_[start].hilbertIndex);
                        }
                    } else {
                        if(++end == static_cast<size_t>(size_)) {
                            start = end - nNeighbors;
                            break;
                        } else {
                            endDistance = fIndexDistance(
                                hilbertIndex, pairs_[end-1].hilbertIndex);
                        }
                    }
                }
            }

            niwa::graphics::Spectrum powerScore(0,0,0);

            for(size_t i=start; i<end; ++i) {
                Photon const& photon = pairs_[i].photon;

                if(math::vec3f::dot(normal, photon.normal()) >= 0) {
                    powerScore += photon.power();
                }
            }

            return powerScore * 10.0f;
        }
    }
}