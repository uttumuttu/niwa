/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_PACKED_VEC3I_H
#define NIWA_MATH_PACKED_VEC3I_H

#include <xmmintrin.h>

#include "vec3i.h"

#include <new>

namespace niwa {
    namespace math {
        /**
         * A collection of four three-dimensional vectors
         * (optimized for SSE). Must be aligned at 16-byte boundaries.
         */
        __declspec(align(16)) class packed_vec3i {
        public:
            static packed_vec3i __forceinline createUninitialized();

            /**
             * Creates a zero vector.
             */
            __forceinline packed_vec3i();

            /**
             * Creates a packed vector with equal components.
             *
             * @param vec The equal component.
             */
            __forceinline explicit packed_vec3i(vec3i const& vec);

            __forceinline packed_vec3i(__m128 const& x_, __m128 const& y_, __m128 const& z_);

            /**
             * Sets a component of the packed vector.
             *
             * @param i Between zero (inclusive) and four (exclusive).
             */
            __forceinline void set(int i, vec3i const& vec);

            /**
             * Gets a component of the packed vector.
             *
             * @param i Between zero (inclusive) and four (exclusive).
             */
            __forceinline vec3i get(int i) const;

        private: // prevent (trivial) heap instantiation
            void* operator new (size_t sz);
            void* operator new (size_t sz, std::nothrow_t);
            void* operator new (size_t sz, void*);

            void* operator new[] (size_t sz);
            void* operator new[] (size_t sz, std::nothrow_t);
            void* operator new[] (size_t sz, void*);

        public:
            __m128 x;
            __m128 y;
            __m128 z;
        };
    }
}

#include "packed_vec3i.inl"

#endif