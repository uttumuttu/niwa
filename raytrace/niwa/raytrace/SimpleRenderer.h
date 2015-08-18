/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_SIMPLERENDERER_H
#define NIWA_RAYTRACE_SIMPLERENDERER_H

#include "IRenderer.h"

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <memory>

namespace niwa {
    namespace system {
        class IParallelizer;
    }

    namespace graphics {
        class Spectrum;
    }

    namespace photonmap {
        class IPhotonMap;
        class PhotonList;
    }

    namespace raytrace {
        class Camera;
        class ITraceable;
        class IToneMapper;
        class ILight;
        class PhotonTracer;
        class ray3f;
        class RayTracer;

        /**
         * A renderer with no adaptive subdivision.
         * Supports parallelization.
         */
        class SimpleRenderer : public IRenderer {
        public:
            /**
             * @param photonMap Null for no photon mapping.
             */
            SimpleRenderer(
                std::pair<size_t, size_t> windowSize,
                int photonCount,
                boost::shared_ptr<photonmap::IPhotonMap> photonMap);

            void setCamera(boost::shared_ptr<Camera> camera);

            void setScene(boost::shared_ptr<ITraceable> scene);

            void setLight(boost::shared_ptr<ILight> light);

            void setToneMapper(boost::shared_ptr<IToneMapper> toneMapper);

            /**
             * Sets whether to render the computed
             * color values with OpenGL pipeline.
             * This can be useful for profiling.
             */
            void setUseOpenGl(bool useOpenGl);

            /**
             * @return Whether the renderer renders the computed
             *         values with OpenGL pipeline.
             */
            bool getUseOpenGl() const;

            void setUseMultithreading(bool useMultithreading);

            bool getUseMultithreading() const;

            /**
             * @return True if and only if camera, scene and tone mapper are set.
             */
            bool render() const;

            void renderRow(
                int row, 
                boost::shared_array<graphics::Spectrum> pixelColors) const;

            class RowTask;

        private:
            /**
             * @param ray The ray along which radiance is sampled.
             *
             * @param currentRefractiveIndex The refractive index of the medium in which the ray travels, initially one.
             *
             * @param nearestPhotons Preallocated for efficiency.
             *
             * @param depth Raytracing depth, initially zero.
             */
            graphics::Spectrum sampleIncidentRadiance(
                ray3f const& ray, 
                float currentRefractiveIndex,
                photonmap::PhotonList& nearestPhotons,
                int depth) const;

        private: // prevent copying
            SimpleRenderer(SimpleRenderer const&);
            SimpleRenderer& operator = (SimpleRenderer const&);

        private:
            std::pair<size_t, size_t> windowSize_;

            int photonCount_;

            bool useOpenGl_;

            bool useMultithreading_;

            std::auto_ptr<PhotonTracer> photonTracer_;

            std::auto_ptr<RayTracer> rayTracer_;

            boost::shared_ptr<photonmap::IPhotonMap> photonMap_;

            boost::shared_ptr<system::IParallelizer> parallelizer_;

            boost::shared_ptr<Camera> camera_;

            boost::shared_ptr<IToneMapper> toneMapper_;
        };
    }
}

#endif