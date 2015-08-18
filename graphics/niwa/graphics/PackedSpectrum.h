/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_PACKEDSPECTRUM_H
#define NIWA_GRAPHICS_PACKEDSPECTRUM_H

#include <xmmintrin.h>

#include "Spectrum.h"

namespace niwa {
    namespace graphics {
        /**
        * A collection of four spectra
        * (optimized for SSE). Must be aligned at 16-byte boundaries.
        */
        __declspec(align(16)) class PackedSpectrum {
        public:
            /**
             * Creates a zero spectrum.
             */
            __forceinline PackedSpectrum();

            __forceinline PackedSpectrum(
                __m128 const& r_, __m128 const& g_, __m128 const& b_);

            __forceinline void set(int i, Spectrum const& spectrum);

            __forceinline Spectrum get(int i) const;

            __forceinline PackedSpectrum& operator += (PackedSpectrum const&);

            __forceinline PackedSpectrum const operator * (__m128 const&) const;

            __forceinline PackedSpectrum const operator & (__m128 const&) const;

        private: // prevent (trivial) heap instantiation
            void* operator new (size_t sz);
            void* operator new (size_t sz, std::nothrow_t);
            void* operator new (size_t sz, void*);

            void* operator new[] (size_t sz);
            void* operator new[] (size_t sz, std::nothrow_t);
            void* operator new[] (size_t sz, void*);

        public:
            __m128 r;
            __m128 g;
            __m128 b;
        };
    }
}

#include "PackedSpectrum.inl"

#endif