/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#include "QuaternionJulia.h"

#include "niwa/demolib/IGraphics.h"

#include "niwa/graphics/MarchingCubes.h"

#include "niwa/levelset/objects/Grid.h"
#include "niwa/levelset/objects/GridBuilder.h"

#include "niwa/system/NiwaParallelizer.h"
#include "niwa/system/SingleThreadedParallelizer.h"

#define WINDOW_SIZE 1.15f
#define THRESHOLD 4.0f
#define N_JULIA_ITERATIONS 10
#define SSE

#ifdef SSE
#include <xmmintrin.h>
#endif

#define NOMINMAX
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>

using namespace niwa::geom;
using namespace niwa::graphics;
using namespace niwa::levelset::objects;
using namespace niwa::math;
using namespace niwa::system;

QuaternionJulia::QuaternionJulia() : timeSeconds_(0) {
    marchingCubes_ = std::auto_ptr<MarchingCubes>(new MarchingCubes());

    // All dimensions (except for z) MUST be
    // divisable by four; otherwise the SSE code will crash
    // (since it assumes 16-byte alignment).
    GridBuilder builder(
        aabb(vec3f(-1,-1,-1), vec3f(1,1,1)),
        vec3i(100,100,100));

    grid_ = std::auto_ptr<Grid>(builder.buildEmpty());

    parallelizer_ = boost::shared_ptr<IParallelizer>(NiwaParallelizer::create());
}

/**
 * This compilation unit uses only packed SSE operations,
 * so we'll use operator overloading for shorthand.
 */
namespace {
    static __forceinline __m128 __fastcall operator + (
            __m128 const& lhs, __m128 const& rhs) {
        return _mm_add_ps(lhs, rhs);
    }

    static __forceinline __m128 __fastcall operator - (
            __m128 const& lhs, __m128 const& rhs) {
        return _mm_sub_ps(lhs, rhs);
    }

    static __forceinline __m128 __fastcall operator * (
            __m128 const& lhs, __m128 const& rhs) {
        return _mm_mul_ps(lhs, rhs);
    }
}

namespace {
    class SliceTask : public IParallelizer::ICallback {
    public:
        explicit SliceTask(Grid& grid, double timeSeconds) 
                : grid_(grid), timeSeconds_(timeSeconds) {
            // ignored
        }

        void __fastcall invoke(int z);

    private: // prevent copying
        SliceTask(SliceTask const&);
        SliceTask& operator = (SliceTask const&);

    private:
        Grid& grid_;

        double timeSeconds_;
    };

    void SliceTask::invoke(int z) {
        const vec3i dim = grid_.getDimensions();

        const double t = timeSeconds_ * 10;

#ifdef SSE
        __m128 cr = _mm_set_ps1(-0.2f);
        __m128 ca = _mm_set_ps1(0.8f);
        //__m128 cb = _mm_set_ps1(0.0f);
        //__m128 cc = _mm_set_ps1(0.0f);

        __m128 cInitial = _mm_set_ps1(static_cast<float>(sin(t*0.1f)) * .75f);

        __m128 threshold = _mm_set_ps1(THRESHOLD);

        __m128 xFactor = _mm_set_ps1(1.0f / dim.x * 2 * WINDOW_SIZE);

        __m128 xOffset = _mm_set_ps(
            (3.5f / dim.x * 2 - 1) * WINDOW_SIZE,
            (2.5f / dim.x * 2 - 1) * WINDOW_SIZE,
            (1.5f / dim.x * 2 - 1) * WINDOW_SIZE,
            (0.5f / dim.x * 2 - 1) * WINDOW_SIZE);

        __m128* out = reinterpret_cast<__m128*>(&grid_.elementAt(0,0,z));

        __m128 bInitial = _mm_set_ps1(((z+.5f) / dim.z * 2 - 1) * WINDOW_SIZE);

        for(int y=0; y<dim.y; ++y) {
            __m128 aInitial = _mm_set_ps1(((y+.5f) / dim.y * 2 - 1) * WINDOW_SIZE);

            for(int x=0; x<dim.x; x += 4) {
                __m128 r = _mm_set_ps1(static_cast<float>(x)) * xFactor + xOffset;

                __m128 a = aInitial;
                __m128 b = bInitial;
                __m128 c = cInitial;

                for(int i=0; i<N_JULIA_ITERATIONS; ++i) {
                    __m128 dr = r+r;

                    r = (r*r-a*a) - (b*b+c*c) + cr;
                    a = dr*a + ca;
                    b = dr*b; // + cb;
                    c = dr*c; // + cc;
                }

                (*out++) = r*r + a*a + b*b + c*c - threshold;
            }
        }
#else
        const float cr = -0.2f;
        const float ca = 0.8f;
        //const float cb = 0.0f;
        //const float cc = 0.0f;

        const float tt = static_cast<float>(sin(t * .1f)) * .75f;

        for(int y=0; y<dim.y; ++y) {
            for(int x=0; x<dim.x; ++x) {
                float r = ((x+.5f) / dim.x * 2 - 1) * WINDOW_SIZE;
                float a = ((y+.5f) / dim.y * 2 - 1) * WINDOW_SIZE;
                float b = ((z+.5f) / dim.z * 2 - 1) * WINDOW_SIZE;
                float c = tt;

                for(int i = 0; i<N_JULIA_ITERATIONS; ++i) {
                    float dr = r+r;

                    r = (r*r - a*a) - (b*b + c*c) + cr;
                    a = dr*a + ca;
                    b = dr*b;// + cb;
                    c = dr*c;// + cc;
                }

                grid_.gridElementAt(x,y,z) = r*r + a*a + b*b + c*c - THRESHOLD;
            }
        }
#endif
    }
}

void QuaternionJulia::render(niwa::demolib::IGraphics const& g) const {
    const double t = timeSeconds_ * 10;

    SliceTask task(*grid_, timeSeconds_);

    parallelizer_->loop(task, 0, grid_->getDimensions().z);

    glViewport(0, 0, g.getWidth(), g.getHeight());
        
    glClearColor(.2f,0.0f,.4f,1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();

    gluPerspective(45.0, g.getAspectRatio(), 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glTranslated(0,0,-3);

    glRotated(t*2, 1,0,0);
    glRotated(t*3, 0,1,0);
    glRotated(t*5, 0,0,1);

    marchingCubes_->render(*grid_);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

void QuaternionJulia::update(double secondsElapsed) {
    timeSeconds_ += secondsElapsed;
}

void QuaternionJulia::onNormalKeys(unsigned char key, int /*modifiers*/) {
    if(key == ' ') {
        marchingCubes_->setUseOpenGl(
            !marchingCubes_->getUseOpenGl());
    }
}
