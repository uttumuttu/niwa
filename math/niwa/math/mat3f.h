/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_MAT3F_H
#define NIWA_MATH_MAT3F_H

namespace niwa {
    namespace math {
        /**
         * Single-precision 3x3 matrix.
         */
        class mat3f {
        public:
            enum Initialization {
                Zero,
                Identity,
                Uninitialized
            };
        private:
            class Row {
            public:
                inline explicit Row(float* row_);

                inline float& operator [] (int column);

            private:
                float* row_;
            };

            class ConstRow {
            public:
                inline explicit ConstRow(float const* row_);

                inline float operator [] (int column) const;

            private:
                float const* row_;
            };
        public:
            inline explicit mat3f(Initialization initialization);

            inline Row operator[] (int row);

            inline ConstRow operator [] (int row) const;

            /**
             * @return Pointer to raw data (row-major).
             */
            inline float const* getRaw() const;

            /**
             * @return Pointer to raw data (row-major).
             */
            inline float* getRaw();

            /**
             * @return Whether the matrix is antisymmetric (in exact arithmetic).
             */
            inline bool isAntiSymmetric() const;

        private:
            float raw_[3*3];
        };
    }
}

#include "mat3f.inl"

#endif