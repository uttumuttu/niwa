/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_COMPOSITETRACEABLE_H
#define NIWA_RAYTRACE_OBJECTS_COMPOSITETRACEABLE_H

#include "niwa/raytrace/AbstractTraceable.h"

#include <boost/shared_ptr.hpp>

#include <vector>

namespace niwa {
    namespace raytrace {
        class HitInfo;

        namespace objects {
            /**
             * Brute force object composition,
             * not optimized in any way (e.g., with KD-trees).
             */
            class CompositeTraceable : public AbstractTraceable {
            public:
                CompositeTraceable();
                ~CompositeTraceable();

                void setObjects(
                    std::vector<boost::shared_ptr<ITraceable>> const& objects);

                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const;

                __m128 __fastcall raytraceShadow(
                    packed_ray3f const& ray, __m128 cutoffDistance, ILight const* light) const;
      
            private:
                std::vector<boost::shared_ptr<ITraceable>> objects_;
            };
        }
    }
}

#endif