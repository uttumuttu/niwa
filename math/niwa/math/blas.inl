/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_MATH_BLAS_INL
#define NIWA_MATH_BLAS_INL

namespace niwa {
    namespace math {
        namespace blas {
            float Vector::operator[] (int index) const {
                return data_[index];
            }

            float& Vector::operator[] (int index) {
                return data_[index];
            }
        }
    }
}

#endif