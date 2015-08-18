/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_CAMERA_H
#define NIWA_RAYTRACE_CAMERA_H

#include "niwa/math/vec2f.h"
#include "niwa/math/vec3f.h"

#include "packed_ray3f.h"

namespace niwa {
    namespace raytrace {
        /**
         * A pinhole camera.
         */
        class Camera {
        public:
            Camera();
            ~Camera();

            float getShutterTime() const;

            void setShutterTime(float shutterTime);

            math::vec2f const& getBackplaneDimensions() const;

            void setBackplaneDimensions(math::vec2f const& backplaneDimensions);

            float getBackplaneDepth() const;

            void setBackplaneDepth(float backplaneDepth);

            math::vec3f const& getPinholePosition() const;

            void setLookat(math::vec3f const& pinholePosition, math::vec3f const& target);

            /**
            * Computes eye ray given the parameterized
            * backplane position.
            *
            * @param u Backplane position parameter between 0 and 1 (inclusive).
            * @param v Backplane position parameter between 0 and 1 (inclusive).
            */
            const ray3f __fastcall getEyeRay(float u, float v) const;

            /**
             * Computes four eye rays given the parameterized
             * backplane position.
             *
             * @param u Backplane position parameter between 0 and 1 (inclusive).
             * @param v Backplane position parameter between 0 and 1 (inclusive).
             */
            const packed_ray3f __fastcall getEyeRay(__m128 u, __m128 v) const;

        private:
            struct Sse;

        private:
            math::vec3f pinholePosition_;

            math::vec3f up_;
            math::vec3f right_;
            math::vec3f front_;

            math::vec2f backplaneDimensions_;
            float backplaneDepth_;

            float shutterTime_;

            Sse* sse_;
        };
    }
}

#endif