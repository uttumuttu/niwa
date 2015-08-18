/** 
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_SPECTRUM_INL
#define NIWA_GRAPHICS_SPECTRUM_INL

namespace niwa {
    namespace graphics {
        Spectrum::Spectrum() : r(0), g(0), b(0) {
            // ignored
        }

        Spectrum::Spectrum(float r_, float g_, float b_) 
            : r(r_), g(g_), b(b_) {
            // ignored
        }

        Spectrum::Spectrum(float const* values) 
            : r(values[0]), g(values[1]), b(values[2]) {
            // ignored
        }

        const Spectrum Spectrum::operator + (Spectrum const& rhs) const {
            return Spectrum(r + rhs.r, g + rhs.g, b + rhs.b);
        }

        const Spectrum Spectrum::operator * (Spectrum const& rhs) const {
            return Spectrum(r * rhs.r, g * rhs.g, b * rhs.b);
        }

        const Spectrum Spectrum::operator * (float rhs) const {
            return Spectrum(r * rhs, g * rhs, b * rhs);
        }

        const Spectrum Spectrum::operator / (float rhs) const {
            return *this * ((rhs == 0) ? 1 : 1/rhs);
        }
        
        Spectrum const& Spectrum::operator += (Spectrum const& rhs) {
            r += rhs.r;
            g += rhs.g;
            b += rhs.b;
            return *this;
        }

        Spectrum const& Spectrum::operator *= (Spectrum const& rhs) {
            r *= rhs.r;
            g *= rhs.g;
            b *= rhs.b;
            return *this;
        }

        Spectrum const& Spectrum::operator *= (float rhs) {
            r *= rhs;
            g *= rhs;
            b *= rhs;
            return *this;
        }

        Spectrum const& Spectrum::operator /= (float rhs) {
            return *this *= ((rhs == 0) ? 1 : 1/rhs);
        }

        float Spectrum::average() const {
            return (r + g + b) / 3;
        }

        float* Spectrum::getRaw() {
            return &r;
        }

        float const* Spectrum::getRaw() const {
            return &r;
        }
    }
}

#endif