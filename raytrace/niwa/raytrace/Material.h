/**
 * @file 
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_MATERIAL_H
#define NIWA_RAYTRACE_MATERIAL_H

#include "niwa/graphics/Spectrum.h"

namespace niwa {
    namespace raytrace {
        /**
         * Surface material.
         */
        class Material {
        public:
            enum Type {
                MATERIAL_BLACK,
                MATERIAL_EMITTING,
                MATERIAL_DIFFUSE,
                MATERIAL_SPECULAR,
                MATERIAL_DIELECTRIC
            };

        public:
            /**
             * Creates a black (MATERIAL_BLACK) material.
             */
            inline Material();

            inline const Type getType() const;

            /**
             * Only used for MATERIAL_EMITTING.
             */
            inline graphics::Spectrum const& getEmittedRadiance() const;

            /**
             * Only used for MATERIAL_DIFFUSE and MATERIAL_SPECULAR.
             */
            inline graphics::Spectrum const& getReflectance() const;

            /**
             * Only used for MATERIAL_DIELECTRIC.
             */
            inline float getRefractiveIndex() const;

            /**
             * Only used for MATERIAL_DIELECTRIC.
             */
            inline void setRefractiveIndex(float value);

            /**
             * Creates a diffusely emitting material.
             *
             * @param emittedRadiance The constant radiance this material emits.
             */
            inline static Material createEmitting(graphics::Spectrum const& emittedRadiance);

            inline static Material createDiffuse(graphics::Spectrum const& reflectance);

            inline static Material createSpecular(graphics::Spectrum const& reflectance);

            inline static Material createDielectric(float refractiveIndex);

            inline static Material createBlack();
        private:
            inline Material(Type type, graphics::Spectrum const& spectrum);

            inline Material(Type type, float refractiveIndex);

        private:
            Type type_;

            /**
             * Radiance for emitting materials,
             * reflectance for diffuse and specular materials.
             */
            graphics::Spectrum spectrum_;

            /**
             * Refractive index for dielectric materials.
             */
            float refractiveIndex_;
        };
    }
}

#include "Material.inl"

#endif