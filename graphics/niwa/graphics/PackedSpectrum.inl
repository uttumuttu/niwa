/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef NIWA_GRAPHICS_PACKEDSPECTRUM_INL
#define NIWA_GRAPHICS_PACKEDSPECTRUM_INL

namespace niwa {
    namespace graphics {
        PackedSpectrum::PackedSpectrum()
            : r(_mm_setzero_ps()), g(_mm_setzero_ps()), b(_mm_setzero_ps()) {
            // ignored
        }

        PackedSpectrum::PackedSpectrum(
            __m128 const& r_, __m128 const& g_, __m128 const& b_) 
            : r(r_), g(g_), b(b_) {
            // ignored
        }

        void PackedSpectrum::set(int i, Spectrum const& spectrum) {
            r.m128_f32[i] = spectrum.r;
            g.m128_f32[i] = spectrum.g;
            b.m128_f32[i] = spectrum.b;
        }

        Spectrum PackedSpectrum::get(int i) const {
            return Spectrum(r.m128_f32[i], g.m128_f32[i], b.m128_f32[i]);
        }    

        PackedSpectrum& PackedSpectrum::operator += (PackedSpectrum const& rhs) {
            r = _mm_add_ps(r, rhs.r);
            g = _mm_add_ps(g, rhs.g);
            b = _mm_add_ps(b, rhs.b);
            return *this;
        } 

        PackedSpectrum const PackedSpectrum::operator * (__m128 const& rhs) const {
            return PackedSpectrum(
                _mm_mul_ps(r, rhs),
                _mm_mul_ps(g, rhs),
                _mm_mul_ps(b, rhs));
        }

        PackedSpectrum const PackedSpectrum::operator & (__m128 const& rhs) const {
            return PackedSpectrum(
                _mm_and_ps(r, rhs),
                _mm_and_ps(g, rhs),
                _mm_and_ps(b, rhs));
        }
    }
}

#endif