/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_RAYTRACER_H
#define NIWA_RAYTRACE_RAYTRACER_H

#include "niwa/graphics/Spectrum.h"

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace photonmap {
        class IPhotonMap;
    }

    namespace raytrace {
        class HitInfo;
        class ITraceable;
        class ILight;
        class ray3f;

        class RayTracer {
        public:
            RayTracer();

            void setScene(boost::shared_ptr<ITraceable> scene);

            void setLight(boost::shared_ptr<ILight> light);

            void setPhotonMap(boost::shared_ptr<photonmap::IPhotonMap> photonMap);

            /**
             * @param ray The ray along which radiance is sampled.
             */
            const graphics::Spectrum sampleIncidentRadiance(
                ray3f const& ray) const;

        private:
            /**
             * @param ray The ray along which radiance is sampled.
             *
             * @param currentRefractiveIndex Initially one (air).
             *
             * @param depth Initially zero.
             */
            const graphics::Spectrum sampleIncidentRadiance(
                ray3f const& ray, 
                float currentRefractiveIndex,
                int depth) const;

            graphics::Spectrum sampleDirectRadiance(
                HitInfo const& hitInfo) const;

            graphics::Spectrum estimateIndirectRadiance(
                HitInfo const& hitInfo) const;

        private:
            boost::shared_ptr<ITraceable> scene_;

            boost::shared_ptr<ILight> light_;

            boost::shared_ptr<photonmap::IPhotonMap> photonMap_;
        };
    }
}

#endif