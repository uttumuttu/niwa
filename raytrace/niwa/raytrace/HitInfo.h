/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_HITINFO_H
#define NIWA_RAYTRACE_HITINFO_H

#include "niwa/raytrace/Material.h"
#include "niwa/math/vec3f.h"

//#define NIWA_RAYTRACE_HITINFO_VALIDITY_CHECK

namespace niwa {
    namespace raytrace {
        /**
         * Mutable information about a ray hit.
         *
         * Mutability is justified since allocation
         * -- especially dynamic allocation -- 
         * is prohibitively expensive.
         */
        class HitInfo {
        public:
            /**
             * Creates hit information that is not yet valid,
             * but is otherwise properly initialized.
             */
            static inline HitInfo createInitialized();

            /**
             * Creates an uninitialized hit info (for efficiency).
             */
            static inline HitInfo createUninitialized();

            /**
             * Sets the hit information; also makes
             * the hit automatically valid.
             *
             * We use a single setter to ensure
             * that the information stays coherent,
             * that is, the caller can't forget
             * to set some fields.
             */
            inline void setValues(
                float distance,
                math::vec3f const& position,
                math::vec3f const& normal,
                Material const& material);

            //inline bool isValid() const;

            /**
             * @return Distance from the originating position to the hit.
             */
            inline float distance() const;

            /**
             * @return Position of the hit.
             */
            inline math::vec3f const& position() const;

            /**
             * @return Outward unit surface normal at the hit.
             */
            inline math::vec3f const& normal() const;

            /**
             * @return Surface material.
             */
            inline Material const& material() const;

        private:
            inline HitInfo();

        private:
#ifdef NIWA_RAYTRACE_HITINFO_VALIDITY_CHECK
            bool isValid_;
#endif

            float distance_;

            math::vec3f position_;

            math::vec3f normal_;

            Material material_;
        };
    }
}

#include "HitInfo.inl"

#endif