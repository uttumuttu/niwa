/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_GRAPHICS_SPECTRUM_H
#define NIWA_GRAPHICS_SPECTRUM_H

namespace niwa {
    namespace graphics {
        /**
         * An RGB-spectrum for graphical computation.
         */
        class Spectrum {
        public:
            /**
             * Creates a zero spectrum.
             */
            inline Spectrum();

            inline Spectrum(float r_, float g_, float b_);

            inline explicit Spectrum(float const* values);

            inline const Spectrum operator + (Spectrum const& rhs) const;
            inline const Spectrum operator * (Spectrum const& rhs) const;
            inline const Spectrum operator * (float rhs) const;
            inline const Spectrum operator / (float rhs) const;
            
            inline Spectrum const& operator += (Spectrum const& rhs);
            inline Spectrum const& operator *= (Spectrum const& rhs);
            inline Spectrum const& operator *= (float rhs);
            inline Spectrum const& operator /= (float rhs);

            inline float average() const;

            inline float* getRaw();
            inline float const* getRaw() const;

        public:
            float r, g, b;
        };
    }
}

#include "Spectrum.inl"

#endif