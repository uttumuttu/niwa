/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/SimpleRenderer.h"

#include "niwa/raytrace/ILight.h"
#include "niwa/raytrace/Camera.h"
#include "niwa/raytrace/IToneMapper.h"
#include "niwa/raytrace/ITraceable.h"
#include "niwa/raytrace/HitInfo.h"
#include "niwa/raytrace/ray3f.h"
#include "niwa/raytrace/Hemisphere.h"
#include "niwa/raytrace/RayTracer.h"
#include "niwa/raytrace/PhotonTracer.h"

#include "niwa/photonmap/PhotonHash.h"

#include "niwa/system/SingleThreadedParallelizer.h"
#include "niwa/system/NiwaParallelizer.h"
#include "niwa/system/OpenMpParallelizer.h"

#include "niwa/math/Constants.h"

#include <boost/shared_array.hpp>

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>

#define MAX_DEPTH 3

namespace {
    static niwa::system::IParallelizer* createMultithreadingParallelizer() {
        return niwa::system::SingleThreadedParallelizer::create();

        //return niwa::system::NiwaParallelizer::create();

        //return niwa::system::OpenMpParallelizer::create();
    }
}

using boost::shared_ptr;

using std::auto_ptr;

namespace niwa {
    using graphics::Spectrum;

    using photonmap::PhotonList;

    namespace raytrace {
        using math::constants::PI_F;

        using photonmap::Photon;

        class SimpleRenderer::RowTask : public niwa::system::IParallelizer::ICallback {
        public:
            RowTask(SimpleRenderer const& parent, boost::shared_array<Spectrum> pixelColors);

            void __fastcall invoke(int row);

        private: // prevent copying
            RowTask(RowTask const&);
            RowTask& operator = (RowTask const&);

        private:
            SimpleRenderer const& parent_;

            boost::shared_array<Spectrum> pixelColors_;
        };

        SimpleRenderer::SimpleRenderer(
            std::pair<size_t, size_t> windowSize, int photonCount, shared_ptr<photonmap::IPhotonMap> photonMap)
            : windowSize_(windowSize),
              photonCount_(photonCount),
              useOpenGl_(true),
              useMultithreading_(true) {
            parallelizer_ = shared_ptr<system::IParallelizer>(
                createMultithreadingParallelizer());

            rayTracer_ = auto_ptr<RayTracer>(new RayTracer());

            if(photonCount_ > 0 && photonMap) {
                photonMap_ = photonMap;

                photonTracer_ = auto_ptr<PhotonTracer>(new PhotonTracer());

                photonTracer_->setParallelizer(parallelizer_);

                rayTracer_->setPhotonMap(photonMap_);
            } else {
                photonCount_ = 0;
            }
        }

        bool SimpleRenderer::getUseMultithreading() const {
            return useMultithreading_;
        }

        void SimpleRenderer::setUseMultithreading(bool const useMultithreading) {
            useMultithreading_ = useMultithreading;

            if(useMultithreading_) {
                parallelizer_ = shared_ptr<system::IParallelizer>(
                    createMultithreadingParallelizer());
            } else {
                parallelizer_ = shared_ptr<system::IParallelizer>(
                    niwa::system::SingleThreadedParallelizer::create());
            }

            if(photonTracer_.get()) {
                photonTracer_->setParallelizer(parallelizer_);
            }
        }

        void SimpleRenderer::setCamera(shared_ptr<Camera> camera) {
            camera_ = camera;
        }

        void SimpleRenderer::setScene(shared_ptr<ITraceable> scene) {
            if(photonTracer_.get()) {
                photonTracer_->setScene(scene);
            }
            rayTracer_->setScene(scene);
        }

        void SimpleRenderer::setToneMapper(shared_ptr<IToneMapper> toneMapper) {
            toneMapper_ = toneMapper;
        }

        void SimpleRenderer::setUseOpenGl(bool useOpenGl) {
            useOpenGl_ = useOpenGl;
        }

        bool SimpleRenderer::getUseOpenGl() const {
            return useOpenGl_;
        }

        void SimpleRenderer::setLight(shared_ptr<ILight> light) {
            if(photonTracer_.get()) {
                photonTracer_->setLight(light);
            }
            rayTracer_->setLight(light);
        }

        SimpleRenderer::RowTask::RowTask(
            SimpleRenderer const& parent, boost::shared_array<Spectrum> pixelColors)
            : parent_(parent), pixelColors_(pixelColors) {
            // ignored
        }

        void SimpleRenderer::RowTask::invoke(int row) {
            parent_.renderRow(row, pixelColors_);
        }

        void SimpleRenderer::renderRow(int y, boost::shared_array<Spectrum> pixelColors) const {
            size_t windowWidth = windowSize_.first;
            size_t windowHeight = windowSize_.second;

            size_t nPacks = windowWidth / 4;

            if((windowWidth&3) != 0) {
                ++nPacks;
            }

            __m128 xOffsets = _mm_setr_ps(0.5f, 1.5f, 2.5f, 3.5f);

            __m128 v = _mm_set_ps1(1 - (y+.5f) / windowHeight);

            for(size_t pack = 0; pack < nPacks; ++pack) {
                // The one minus stems from the fact
                // that pinhole camera backplane produces
                // the inverse image; we wish to render
                // the non-inverted image instead.

                __m128 u = _mm_sub_ps(
                    _mm_set_ps1(1.0f),
                    _mm_div_ps(
                        _mm_add_ps(_mm_set_ps1(static_cast<float>(pack * 4)), xOffsets),
                        _mm_set_ps1(static_cast<float>(windowWidth))));

                packed_ray3f eyeRay( camera_->getEyeRay(u,v) );

                size_t xStart = pack * 4;
                size_t xEnd = std::min(xStart + 4, windowWidth);

                for(size_t x=xStart; x<xEnd; ++x) {
                    Spectrum radiance = rayTracer_->sampleIncidentRadiance(
                        eyeRay.get(x-xStart));

                    // Our method of computing irradiation has several simplifications:

                    // 1) We are not integrating over the backplane area corresponding
                    //    to the pixel, but use a single estimate (i.e., no antialising).

                    // 2) We are using a pinhole camera, which means the integral
                    //    over solid angles is replaced by a dirac delta function,
                    //    which equals radiance times hemispherical solid angle (2*PI).

                    // 3) We ignore the cosine factor in the measure equation
                    //    ("natural vignette"), assuming that the camera has some implicit 
                    //    mechanism for compensating the cosine falloff.

                    Spectrum irradiation = 
                        radiance
                            * (camera_->getShutterTime() * 2 * PI_F);

                    Spectrum tone = toneMapper_->toneMap(irradiation);

                    pixelColors[y * windowWidth + x] = tone;
                }
            }
        }

        bool SimpleRenderer::render() const {
            glClear(GL_COLOR_BUFFER_BIT);

            if(photonCount_ > 0) {
                photonMap_->clear();

                   photonTracer_->tracePhotons(*photonMap_, photonCount_);

                photonMap_->buildStructure();
            }

            glDisable(GL_DEPTH_TEST);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();

            size_t windowWidth = windowSize_.first;
            size_t windowHeight = windowSize_.second;

            glTranslatef(-1, -1, 0);
            glScalef(2.0f / (windowWidth-1), 2.0f / (windowHeight-1), 1);

            // row-major
            boost::shared_array<Spectrum> pixelColors(new Spectrum[windowWidth * windowHeight]);

            RowTask rowTask(*this, pixelColors);

            parallelizer_->loop(rowTask, 0, windowHeight, 4);

            if(useOpenGl_) {
                for(size_t y=0; y<windowHeight-1; ++y) {
                    glBegin(GL_TRIANGLE_STRIP);

                    for(size_t x=0; x<windowWidth; ++x) {
                        Spectrum const& c1 = pixelColors[y * windowWidth + x];
                        Spectrum const& c2 = pixelColors[(y+1) * windowWidth + x];

                        glColor3f(c1.r, c1.g, c1.b);
                        glVertex2i(x,y);

                        glColor3f(c2.r, c2.g, c2.b);
                        glVertex2i(x,y+1);
                    }

                    glEnd();
                }
            }

            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            return true;
        }
    }
}