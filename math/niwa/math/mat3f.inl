/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_MATH_MAT3F_INL
#define NIWA_MATH_MAT3F_INL

#include "vec3f.h"

namespace niwa {
    namespace math {
        mat3f::Row::Row(float* row) : row_(row) {
            // ignored
        }

        float& mat3f::Row::operator [] (int column) {
            return row_[column];
        }

        mat3f::ConstRow::ConstRow(float const* row) : row_(row) {
            // ignored
        }

        float mat3f::ConstRow::operator [] (int column) const {
            return row_[column];
        }

        mat3f::mat3f(Initialization initialization) {
            if(initialization == Uninitialized) {
                return;
            }
            for(int i=0; i<9; ++i) {
                raw_[i] = 0;
            }
            if(initialization == Identity) {
                raw_[0] = raw_[3] = raw_[9] = 1;
            }
        }

        mat3f::Row mat3f::operator[] (int row) {
            return Row(&raw_[3*row]);
        }

        mat3f::ConstRow mat3f::operator [] (int row) const {
            return ConstRow(&raw_[3*row]);
        }

        float const* mat3f::getRaw() const {
            return raw_;
        }

        float* mat3f::getRaw() {
            return raw_;
        }

        /**
         * @param rhs A column vector.
         *
         * @return A column vector.
         */
        static inline vec3f operator * (mat3f const& lhs, vec3f const& rhs) {
            return vec3f(
                lhs[0][0] * rhs[0] + lhs[0][1] * rhs[1] + lhs[0][2] * rhs[2],
                lhs[1][0] * rhs[0] + lhs[1][1] * rhs[1] + lhs[1][2] * rhs[2],
                lhs[2][0] * rhs[0] + lhs[2][1] * rhs[1] + lhs[2][2] * rhs[2]);
        }

        /**
        * @param rhs A row vector.
        *
        * @return A row vector.
        */
        static inline vec3f operator * (vec3f const& lhs, mat3f const& rhs) {
            return vec3f(
                lhs[0] * rhs[0][0] + lhs[1] * rhs[1][0] + lhs[2] * rhs[2][0],
                lhs[0] * rhs[0][1] + lhs[1] * rhs[1][1] + lhs[2] * rhs[2][1],
                lhs[0] * rhs[0][2] + lhs[1] * rhs[1][2] + lhs[2] * rhs[2][2]);
        }

        static inline mat3f operator * (mat3f const& lhs, mat3f const& rhs) {
            mat3f out(mat3f::Zero);

            for(int i=0; i<3; ++i) {
                for(int j=0; j<3; ++j) {
                    for(int k=0; k<3; ++k) {
                        out[i][j] += lhs[i][k] * rhs[k][j];
                    }
                }
            }
            return out;
        }

        static inline mat3f operator * (mat3f const& lhs, float rhs) {
            mat3f out(mat3f::Uninitialized);

            for(int i=0; i<3; ++i) {
                for(int j=0; j<3; ++j) {
                    out[i][j] = lhs[i][j] * rhs;
                }
            }
            return out;
        }

        static inline mat3f& operator += (mat3f& lhs, mat3f const& rhs) {
            for(int i=0; i<3; ++i) {
                for(int j=0; j<3; ++j) {
                    lhs[i][j] += rhs[i][j];
                }
            }
            return lhs;
        }

        bool mat3f::isAntiSymmetric() const {
            for(int i=0; i<3; ++i) {
                for(int j=i; j<3; ++j) {
                    if((*this)[i][j] != -(*this)[j][i]) {
                        return false;
                    }
                }
            }
            return true;
        }

        static inline mat3f trn(mat3f const& mat) {
            mat3f out(mat3f::Uninitialized);

            for(int i=0; i<3; ++i) {
                for(int j=0; j<3; ++j) {
                    out[i][j] = mat[j][i];
                }
            }
            return out;
        }

        static inline float det(mat3f const& m) {
            return m[0][0] * (m[2][2] * m[1][1] - m[2][1] * m[1][2])
                 - m[1][0] * (m[2][2] * m[0][1] - m[2][1] * m[0][2])
                 + m[2][0] * (m[1][2] * m[0][1] - m[1][1] * m[0][2]);
        }

        static inline mat3f inv(mat3f const& m) {
            mat3f out(mat3f::Uninitialized);

            out[0][0] =   m[2][2] * m[1][1] - m[2][1] * m[1][2];
            out[0][1] = -(m[2][2] * m[0][1] - m[2][1] * m[0][2]);
            out[0][2] =   m[1][2] * m[0][1] - m[1][1] * m[0][2];

            out[1][0] = -(m[2][2] * m[1][0] - m[2][0] * m[1][2]);
            out[1][1] =   m[2][2] * m[0][0] - m[2][0] * m[0][2];
            out[1][2] = -(m[1][2] * m[0][0] - m[1][0] * m[0][2]);

            out[2][0] =   m[2][1] * m[1][0] - m[2][0] * m[1][1];
            out[2][1] = -(m[2][1] * m[0][0] - m[2][0] * m[0][1]);
            out[2][2] =   m[1][1] * m[0][0] - m[1][0] * m[0][1];

            return out;
        }
    }
}

#endif