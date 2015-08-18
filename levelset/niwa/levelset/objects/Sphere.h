/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_LEVELSET_OBJECTS_SPHERE_H
#define NIWA_LEVELSET_OBJECTS_SPHERE_H

#include "niwa/levelset/ILevelSet.h"

#include "niwa/math/vec3f.h"

namespace niwa {
    namespace levelset {
        namespace objects {
            /**
             * Signed distance field for a sphere.
             */
            class Sphere : public ILevelSet {
            public:
                Sphere();

                void setPosition(math::vec3f const& position);

                void setRadius(float radius);

                float __fastcall value(math::vec3f const& position) const;

                math::vec3f __fastcall gradient(math::vec3f const& position) const;

                bool isDistanceField() const;

            private:
                math::vec3f position_;

                float radius_;
            };
        }
    }
}

#endif