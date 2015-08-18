/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_GEOM_HILBERT_H
#define NIWA_GEOM_HILBERT_H

namespace niwa {
    namespace geom {
        /**
         * A DIM-dimensional point on a Hilbert space curve
         * of order ORDER; the same structure is used for point original points
         * and Hilbert-encoded points.
         */
        template <size_t DIM, size_t ORDER>
        struct HilbertPoint {
        public:
            /**
             * Creates a point at origin.
             */
            HilbertPoint();

            explicit HilbertPoint(size_t elements[DIM]);

        public:

            /**
             * The method assumes that the index can be
             * represented as an size_teger, which
             * may not be the case for very high-order
             * and high-dimensional points.
             *
             * @return The one-dimensional index of the point,
             *         based on lexicographic order.
             */
            size_t index() const;

            bool const operator == (HilbertPoint<DIM,ORDER> const&) const;
            bool const operator != (HilbertPoint<DIM,ORDER> const&) const;

        public:
            size_t elements[DIM];
        };

        /**
         * Hilbert space-filling curves.
         *
         * Implementation follows the paper
         * "Calculation of Mappings Between One and n-dimensional Values
         * Using the Hilbert Space-filling Curve" by J. K. Lawder (2000).
         */
        template <size_t DIM, size_t ORDER>
        class Hilbert {
        public:
            /**
             * @param pt A DIM-dimensional point in plain coordinates.
             *
             * @return The one-dimensional Hilbert index (encoded by the resulting point).
             */
            static HilbertPoint<DIM,ORDER> encode(HilbertPoint<DIM,ORDER> const& pt);

            /**
             * @param A one-dimensional Hilbert index (encoded by the given point).
             *
             * @return The DIM-dimensional point in plain coordinates.
             */
            static HilbertPoint<DIM,ORDER> decode(HilbertPoint<DIM,ORDER> const& H);

        private:
            static size_t calc_P(int i, HilbertPoint<DIM,ORDER> const& H);

            static size_t calc_P2(size_t S);

            static size_t calc_J(size_t P);

            static size_t calc_T(size_t P);

            static size_t calc_tS_tT(size_t xJ, size_t val);

            static size_t g_mask(size_t index);

        private: // prevent instantiation
            Hilbert();

        private: // prevent copying
            Hilbert(Hilbert const&);
            Hilbert& operator = (Hilbert const&);
        };
    }
}

#include "hilbert.inl"

#endif