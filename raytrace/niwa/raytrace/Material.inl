/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_MATERIAL_INL
#define NIWA_RAYTRACE_MATERIAL_INL

namespace niwa {
    namespace raytrace {
        Material::Material()
            : type_(MATERIAL_BLACK), refractiveIndex_(1.0f) {
            // ignored
        }

        const Material::Type Material::getType() const {
            return type_;
        }

        graphics::Spectrum const& Material::getEmittedRadiance() const {
            return spectrum_;
        }

        graphics::Spectrum const& Material::getReflectance() const {
            return spectrum_;
        }

        float Material::getRefractiveIndex() const {
            return refractiveIndex_;
        }

        void Material::setRefractiveIndex(float value) {
            refractiveIndex_ = value;
        }

        Material Material::createEmitting(graphics::Spectrum const& emittedRadiance) {
            return Material(MATERIAL_EMITTING, emittedRadiance);
        }

        Material Material::createDiffuse(graphics::Spectrum const& reflectance) {
            return Material(MATERIAL_DIFFUSE, reflectance);
        }

        Material Material::createSpecular(graphics::Spectrum const& reflectance) {
            return Material(MATERIAL_SPECULAR, reflectance);
        }

        Material Material::createDielectric(float refractiveIndex) {
            return Material(MATERIAL_DIELECTRIC, refractiveIndex);
        }

        Material Material::createBlack() {
            return Material();
        }

        Material::Material(Type type, graphics::Spectrum const& spectrum) 
            : type_(type), spectrum_(spectrum) {
            // ignored
        }

        Material::Material(Type type, float refractiveIndex) 
            : type_(type), refractiveIndex_(refractiveIndex) {
            // ignored
        }
    }
}

#endif