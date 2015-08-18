/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Engine.h"

#include "niwa/demolib/IGraphics.h"
#include "niwa/demolib/IEffect.h"

#include "niwa/system/Timer.h"

#include "niwa/logging/Logger.h"

#include <gl/glew.h>
#include <glut.h>

#include <sstream>
#include <cmath>
#include <algorithm>

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

#include "niwa/demolib/LuaRef.h"

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

namespace {
    class Graphics : public niwa::demolib::IGraphics {
    public:
        Graphics(size_t width, size_t height)
            : width_(width), height_(height) {
            // ignored
        }

    public: // from niwa::demolib::IGraphics
        size_t getWidth() const {
            return width_;
        }

        size_t getHeight() const {
            return height_;
        }

        std::pair<size_t, size_t> getDimensions() const {
            return std::pair<size_t, size_t>(width_, height_);
        }

        double getAspectRatio() const {
            return height_ == 0 ? 1 : width_ / static_cast<double>(height_);
        }

    public: // new functions
        void setDimensions(std::pair<size_t, size_t> dimensions) {
            width_ = dimensions.first;
            height_ = dimensions.second;
        }

    private:
        size_t width_;
        size_t height_;
    };
}

namespace {
    static const char* gTitle = "Niwa";

    static niwa::demoengine::Engine* gEngine = NULL;

    static Graphics gGraphics(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

    static niwa::system::Timer* gTimer = NULL;

    static double gPreviousSeconds = -1;

    static double gPreviousFps = 0;
}

namespace niwa {
    using demolib::IEffect;
    using demolib::LuaRef;

    namespace demoengine {
        static logging::Logger log(typeid(Engine));

        std::vector<Span> Engine::getActiveSpans(double timeSeconds) {
            std::vector<Span> activeSpans;

            for(size_t i=0; i<spans_.size(); ++i) {
                if(spans_[i].isActive(timeSeconds)) {
                    activeSpans.push_back(spans_[i]);
                }
            }

            std::sort(
                activeSpans.begin(), 
                activeSpans.end(), Span::ascendingLayer);

            return activeSpans;
        }

        void Engine::setupSpans(double timeSeconds) {
            std::vector<Span*> setupSpans;

            for(size_t i=0; i<spans_.size(); ++i) {
                Span* span = &spans_[i];

                if(span->hasStarted(timeSeconds) && !span->hasBeenSetup()) {
                    setupSpans.push_back(span);
                }
            }

            std::sort(
                setupSpans.begin(), 
                setupSpans.end(), Span::ascendingStartTime);

            for(size_t i=0; i<setupSpans.size(); ++i) {
                Span* span = setupSpans[i];

                // Even if the setup fails, we
                // mark the span setup.
                span->setHasBeenSetup();

                boost::shared_ptr<LuaRef> setup(
                    span->getSetupCallback());

                if(setup->isValid()) {
                    setup->pushValue();

                    if(lua_pcall(L_, 0, LUA_MULTRET, 0) != 0) {
                        log.warn() << "setup callback failed, reason: "
                            << lua_tostring(L_, -1);

                        lua_pop(L_, 1); // error message
                    }
                }
            }
        }

        void Engine::render() {
            double timeSeconds = 0;

            if(!gTimer || !gTimer->measureTime(timeSeconds)) {
                return;
            }

            if(gEngine) {
                gEngine->setupSpans(timeSeconds);
            }

            std::vector<Span> spans = 
                gEngine->getActiveSpans(timeSeconds);

            if(gPreviousSeconds != -1) {
                double lapSeconds = timeSeconds - gPreviousSeconds;
                gPreviousSeconds = timeSeconds;

                if(spans.empty()) {
                    glClearColor(0,0,0,1);
                    glClear(GL_COLOR_BUFFER_BIT);
                }

                for(size_t i=0; i<spans.size(); ++i) {
                    spans[i].getEffect()->update(lapSeconds);
                }

                if(lapSeconds > 0) {
                    double k = exp(-lapSeconds * 2);

                    double currentFps = (1-k) / lapSeconds + k * gPreviousFps;

                    double visualFps = floor(currentFps*100) / 100;

                    std::ostringstream title;

                    if(title << gTitle 
                        && title << ' ' 
                        && title << visualFps 
                        && title << " fps"
                        && title.flush()) {
                        glutSetWindowTitle(title.str().c_str());
                    }

                    gPreviousFps = currentFps;
                }
            } else {
                gPreviousSeconds = timeSeconds;
            }

            for(size_t i=0; i<spans.size(); ++i) {
                spans[i].getEffect()->render(gGraphics);
            }

            glutSwapBuffers();
        }

        void Engine::processNormalKeys(unsigned char key, int /*x*/, int /*y*/) {
            if (key == 27) {
                exit(0);
            }

            double timeSeconds;

            if(gTimer && gTimer->measureTime(timeSeconds)) {
                std::vector<Span> spans = 
                    gEngine->getActiveSpans(timeSeconds);

                for(size_t i=0; i<spans.size(); ++i) {
                    spans[i].getEffect()->onNormalKeys(key, glutGetModifiers());
                }
            }
        }

        void Engine::processSpecialKeys(int key, int x, int y) {
            double timeSeconds;

            if(gTimer && gTimer->measureTime(timeSeconds)) {
                std::vector<Span> spans = 
                    gEngine->getActiveSpans(timeSeconds);

                for(size_t i=0; i<spans.size(); ++i) {
                    spans[i].getEffect()->onSpecialKeys(key, glutGetModifiers());
                }
            }
        }
    }
}

namespace {
    static void resizeWindow(int w, int h) {
        gGraphics.setDimensions(std::pair<size_t, size_t>(w, h));
    }
}

namespace niwa {
    namespace demoengine {
        Engine::Engine(lua_State* L) : L_(L) {
            // ignored
        }

        void Engine::addSpan(Span const& span) {
            spans_.push_back(span);
        }
        
        bool Engine::initOpenGl(int argc, char** argv) {
            glutInit(&argc, argv);

            glutInitDisplayMode(
                GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);

            glutInitWindowPosition(100,100);
            glutInitWindowSize(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
            glutCreateWindow(gTitle);

            int stencilBits = glutGet(GLUT_WINDOW_STENCIL_SIZE);

            log.info() << "number of stencil bits: " << stencilBits;

            glutDisplayFunc(Engine::render);
            glutIdleFunc(Engine::render);
            glutReshapeFunc(resizeWindow);

            // Resizing the window before rendering seems to add
            // robustness to the first frame.
            resizeWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

            glutKeyboardFunc(Engine::processNormalKeys);
            glutSpecialFunc(Engine::processSpecialKeys);

            if(GLEW_OK != glewInit()) {
                log.error() << "Cannot initialize GLEW.";
                return false;
            } else if(!glewIsSupported("GL_VERSION_2_0")) {
                log.error() << "OpenGL 2.0 not supported.";
                return false;
            } else {
                return true;
            }
        }

        void Engine::start() {
            gEngine = this;

            niwa::system::Timer timer;

            gTimer = &timer;

            if(!timer.start()) {
                // ignored
            }

            log.debug() << "entering GLUT main loop";
            glutMainLoop();
            log.debug() << "exiting GLUT main loop";
        }
    }
}
