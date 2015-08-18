/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef MORPHING_H
#define MORPHING_H

#include "niwa/demolib/AbstractEffect.h"
#include "niwa/demolib/Arguments.h"

#include <memory>

namespace niwa {
    namespace levelset {
        namespace objects {
            class Grid;
        }
    }
    namespace graphics {
        class MarchingCubes;
    }
    namespace system {
        class IParallelizer;
    }
}

/**
 * Morphing effect takes three mandatory construction-time parameters:
 *
 * - source_model (string) -- points to a 3DS model to be morphed
 *
 * - destination_model (string) -- points to a 3DS model to be morphyed
 *
 * - resolution (int) -- How many voxels are used along each dimension.
 *                       Must be positive.
 *
 * It also takes one optional parameter (changable at any time):
 *
 * - vertex_normals (bool) -- Whether to use vertex normals (default value true).
 *                            Cannot be time-dependent.
 */
class Morphing : public niwa::demolib::AbstractEffect {
public:
    Morphing(niwa::demolib::CheckableArguments const& args);
    ~Morphing();

    void setArguments(niwa::demolib::CheckableArguments const& args);

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

    void onNormalKeys(unsigned char key, int modifiers);

private: // prevent copying
    Morphing(Morphing const&);
    Morphing& operator = (Morphing const&);

private:
    std::auto_ptr<niwa::levelset::objects::Grid> sourceGrid_;
    std::auto_ptr<niwa::levelset::objects::Grid> destinationGrid_;

    boost::shared_ptr<niwa::levelset::objects::Grid> workingGrid_;

    std::auto_ptr<niwa::graphics::MarchingCubes> marchingCubes_;

    double timeSeconds_;

    boost::shared_ptr<niwa::system::IParallelizer> parallelizer_;

    niwa::demolib::Arguments args_;
};

#endif