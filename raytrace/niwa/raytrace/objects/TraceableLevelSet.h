/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_TRACEABLELEVELSET_H
#define NIWA_RAYTRACE_OBJECTS_TRACEABLELEVELSET_H

#include "niwa/raytrace/AbstractTraceable.h"
#include "niwa/raytrace/Material.h"

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace levelset {
        class ILevelSet;
    }

    namespace raytrace {
        namespace objects {
            class TraceableLevelSet : public AbstractTraceable {
            public:
                /**
                 * @param levelSet A signed distance field.
                 *
                 * @param material Material of the surface (assumed uniform).
                 *                 It's currently not safe to use dielectric
                 *                 materials since tracing inside the surface
                 *                 is not yet robust.
                 */
                TraceableLevelSet(
                    boost::shared_ptr<levelset::ILevelSet> levelSet,
                    Material const& material);

                ~TraceableLevelSet();

            public: // from ITraceable
                bool __fastcall raytrace(ray3f const& ray, HitInfo& hitInfo) const;

            private:
                boost::shared_ptr<levelset::ILevelSet> levelSet_;

                Material const material_;
            };
        }
    }
}

#endif