/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/RayTracer.h"

#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ITraceable.h"
#include "niwa/raytrace/Hemisphere.h"
#include "niwa/raytrace/ILight.h"

#include "niwa/photonmap/IPhotonMap.h"
#include "niwa/photonmap/Photon.h"

#include "niwa/math/Constants.h"

#define MAX_DEPTH 3

using boost::shared_ptr;

namespace niwa {
    namespace raytrace {
        using math::constants::PI_F;

        using math::vec3f;

        using graphics::Spectrum;
        using photonmap::PhotonList;
        using photonmap::IPhotonMap;
        using photonmap::Photon;

        RayTracer::RayTracer() {
            // ignored
        }

        void RayTracer::setScene(shared_ptr<ITraceable> scene) {
            scene_ = scene;
        }

        void RayTracer::setLight(shared_ptr<ILight> light) {
            light_ = light;
        }

        void RayTracer::setPhotonMap(shared_ptr<IPhotonMap> photonMap) {
            photonMap_ = photonMap;
        }

        const Spectrum RayTracer::sampleIncidentRadiance(
                ray3f const& ray) const {
            return sampleIncidentRadiance(ray, 1, 0);
        }

        const Spectrum RayTracer::sampleIncidentRadiance(
                ray3f const& ray, 
                float currentRefractiveIndex, 
                int depth) const {
            if(depth > MAX_DEPTH) {
                return Spectrum(0,0,0);
            }

            HitInfo hitInfo = HitInfo::createUninitialized();

            if(scene_->raytrace(ray, hitInfo)) {
                Material const& material = hitInfo.material();

                if(material.getType() == Material::MATERIAL_EMITTING) {
                    return material.getEmittedRadiance();
                } else if(material.getType() == Material::MATERIAL_DIFFUSE) {
                    return sampleDirectRadiance(hitInfo)
                         + estimateIndirectRadiance(hitInfo);
                } else if(material.getType() == Material::MATERIAL_SPECULAR) {
                    ray3f reflectedRay(
                        hitInfo.position(),
                        Hemisphere::mirrorReflection(
                            hitInfo.normal(),
                            ray.getDirection()));

                    Spectrum radiance( 
                        sampleIncidentRadiance(
                            reflectedRay,
                            currentRefractiveIndex, depth+1) );

                    return radiance * material.getReflectance();
                } else if(material.getType() == Material::MATERIAL_DIELECTRIC) {
                    vec3f refractedDirection;

                    bool isTotalInternalReflection = 
                        Hemisphere::computeRefraction(
                            hitInfo.normal(), ray.getDirection(),
                            currentRefractiveIndex, material.getRefractiveIndex(),
                            refractedDirection) == false;

                    ray3f reflectedRay(
                        hitInfo.position(),
                        Hemisphere::mirrorReflection(
                            hitInfo.normal(),
                            ray.getDirection()));

                    Spectrum reflectedRadiance( 
                        sampleIncidentRadiance(
                            reflectedRay,
                            currentRefractiveIndex, depth+1) );

                    if(isTotalInternalReflection) {
                        return reflectedRadiance;
                    } else {
                        ray3f refractedRay(
                            hitInfo.position(),
                            refractedDirection);

                        Spectrum refractedRadiance(
                            sampleIncidentRadiance(
                                refractedRay,
                                material.getRefractiveIndex(), depth+1));

                        float fresnelCoefficient = Hemisphere::fresnelCoefficient(
                            hitInfo.normal(), ray.getDirection(),
                            currentRefractiveIndex,
                            material.getRefractiveIndex());

                        return reflectedRadiance * fresnelCoefficient
                            + refractedRadiance * (1 - fresnelCoefficient);
                    }
                } else {
                    return Spectrum(0,0,0);
                }
            } else {
                return Spectrum(0,0,0);
            }
        }

        Spectrum RayTracer::sampleDirectRadiance(HitInfo const& hitInfo) const {
            Spectrum directIrradiance(
                light_->sampleIrradiance(
                    hitInfo.position(), hitInfo.normal(),
                    *scene_));

            Spectrum directRadiosity( 
                directIrradiance * hitInfo.material().getReflectance() );

            // radiance = radiosity / pi, AGI Equation 2.12.

            return directRadiosity / PI_F;
        }

        Spectrum RayTracer::estimateIndirectRadiance(
                HitInfo const& hitInfo) const {
            if(!photonMap_) {
                return Spectrum(0,0,0);
            }

            // See [Jensen] Figure 7.4 for derivation.

            Spectrum powerDensity = photonMap_->powerDensity(
                hitInfo.position(), hitInfo.normal());

            // AGI, Equation 2.23.
            Spectrum brdf( hitInfo.material().getReflectance() / PI_F );

            Spectrum radiance = powerDensity * brdf;

            return radiance;
        }
    }
}