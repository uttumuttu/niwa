/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_DEMOLIB_ARGUMENT_INL
#define NIWA_DEMOLIB_ARGUMENT_INL

namespace niwa {
    namespace demolib {
        template <typename T, size_t N>
        Argument::Array<T,N>::Array() {
            for(size_t i=0; i<N; ++i) {
                values_[i] = T();
            }
        }

        template <typename T, size_t N>
        Argument::Array<T,N>::Array(T const* values) {
            for(size_t i=0; i<N; ++i) {
                values_[i] = values[i];
            }
        }

        template <typename T, size_t N>
        Argument::Array<T,N>::operator T * () {
            return values_;
        }

        template <typename T, size_t N>
        Argument::Array<T,N>::operator T const* () const {
            return values_;
        }

        template <typename T> T Argument::asNumber() const {
            return asNumber(T());
        }

        template <typename T> T Argument::asNumber(T defaultValue) const {
            return type_ == NUMBER ? static_cast<T>(numbers_[0]) : defaultValue;
        }

        template <typename T> std::vector<T> Argument::asVector() const {
            if(type_ == VECTOR) {
                std::vector<T> result;

                for(size_t i=0; i<numbers_.size(); ++i) {
                    result.push_back(static_cast<T>(numbers_[i]));
                }
                return result;
            } else {
                return std::vector<T>();
            }
        }

        template <typename T, size_t N> 
        Argument::Array<T,N> Argument::asVector() const {
            return asVector(Array<T,N>());
        }

        template <typename T, size_t N> 
        Argument::Array<T,N> Argument::asVector(
            Argument::Array<T,N> const& defaultValue) const {
            Array<T,N> result;

            if(type_ == VECTOR) {
                size_t bound = std::min(N, numbers_.size());

                for(size_t i=0; i<bound; ++i) {
                    result[i] = static_cast<T>(numbers_[i]);
                }
                return result;
            } else {
                return defaultValue;
            }
        }

        template <typename T> std::vector<T> Argument::asMatrix() const {
            if(type_ == MATRIX) {
                std::vector<T> result;

                for(size_t i=0; i<numbers_.size(); ++i) {
                    result.push_back(static_cast<T>(numbers_[i]));
                }
                return result;
            } else {
                return std::vector<T>();
            }
        }
    }
}

#endif