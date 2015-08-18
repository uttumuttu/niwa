/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_OBJECTS_MESH_H
#define NIWA_RAYTRACE_OBJECTS_MESH_H

#include "niwa/raytrace/AbstractTraceable.h"

namespace niwa {
    namespace geom {
        class aabb;
    }

    namespace autodesk {
        class Model;
    }

    namespace raytrace {
        namespace objects {
            class KdTree;

            class Mesh : public AbstractTraceable {
            public:
                /**
                 * Creates an empty mesh.
                 */
                Mesh();

                /**
                 * Constructs a traceable mesh to desired bounds.
                 */
                Mesh(autodesk::Model const& model, geom::aabb const& modelBounds);

                ~Mesh();

            public:
                bool __fastcall raytrace(
                    ray3f const& ray, HitInfo& hitInfo) const;

                bool __fastcall raytraceShadow(
                    ray3f const& ray, float cutoffDistance, ILight const* light) const;

            private: // prevent copying
                Mesh(Mesh const&);
                Mesh& operator = (Mesh const&);

            private:
                void computeAccelerationStructures();

            private:
                int nVertices_;
                int nFaces_;

                float* vertices_;
                int* faces_;

                KdTree* tree_;
            };
        }
    }
}

#endif