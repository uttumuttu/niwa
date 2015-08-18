/**
* @file
* @author Mikko Kauppila
*
* Copyright (C) Mikko Kauppila 2009.
*/

#ifndef QUATERNIONJULIA_H
#define QUATERNIONJULIA_H

#include "niwa/demolib/AbstractEffect.h"

#include <boost/shared_ptr.hpp>
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

class QuaternionJulia : public niwa::demolib::AbstractEffect {
public:
    QuaternionJulia();

    void render(niwa::demolib::IGraphics const&) const;

    void update(double secondsElapsed);

    void onNormalKeys(unsigned char key, int modifiers);

private: // prevent copying
    QuaternionJulia(QuaternionJulia const&);
    QuaternionJulia& operator = (QuaternionJulia const&);

private:
    std::auto_ptr<niwa::levelset::objects::Grid> grid_;

    std::auto_ptr<niwa::graphics::MarchingCubes> marchingCubes_;

    double timeSeconds_;

    boost::shared_ptr<niwa::system::IParallelizer> parallelizer_;
};

#endif