/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_CLOTHSPRING_H
#define NIWA_DYNAMICS_CLOTHSPRING_H

#include "ClothParticle.h"

#include <xmmintrin.h>

#include <vector>

namespace niwa {
    namespace dynamics {
        class ClothSpring {
        public:
            /**
             * @param integralStiffness 
             *            Hookean stiffness integrated
             *            over length (kg * m / s^2).
             *
             * @param integralDampingCoefficient
             *            Damping coefficient integrated
             *            over length (kg * m / s).
             */
            ClothSpring(
                std::vector<ClothParticle> const& particles,
                int firstIndex, int secondIndex,
                float integralStiffness,
                float integralDampingCoefficient);

            void __fastcall accumulateForces(
                std::vector<ClothParticle> const& particles,
                math::vec3f* forces) const;

            void __fastcall accumulateJacobians(
                std::vector<ClothParticle> const& particles,
                math::vec3f const* src, math::vec3f* dst) const;

            /**
             * Compute impulses for particle velocities
             * which help limit excess strain.
             */
            void __fastcall limitStrain(
                std::vector<ClothParticle>& particles,
                float timeSeconds) const;

        public:
            bool operator < (ClothSpring const&) const;

        private:
            /**
             * Particle indices in the particle array.
             */
            int indices_[2];

            float restLength_;

            /**
            * Hookean stiffness (kg / s^2).
            */
            float stiffness_;

            /**
            * Damping coefficient (kg / s).
            */
            float dampingCoefficient_;

            friend class ClothPackedSpring;
        };

        /**
         * A collection of four springs
         * (for SSE optimizations).
         */
        __declspec(align(16)) class ClothPackedSpring {
        public:
            /**
             * Creates an uninitialized
             * packed spring; use the initialization method
             * to initialize the values.
             */
            ClothPackedSpring();

            /**
             * The four successive springs from the 
             * iterator are used to build the packed spring.
             */
            void setup(std::vector<ClothSpring>::const_iterator it);

            void __fastcall accumulateForces(
                std::vector<ClothParticle> const& particles,
                math::vec3f* forces) const;

            void __fastcall accumulateJacobians(
                std::vector<ClothParticle> const& particles,
                math::vec3f const* src, math::vec3f* dst) const;

            void __fastcall limitStrain(
                std::vector<ClothParticle>& particles,
                float timeSeconds) const;

        public:
            void* operator new[] (size_t sz);
            void operator delete[] (void* p);

        private:
            void* operator new   (size_t);
            void* operator new   (size_t, std::nothrow_t);
            void* operator new   (size_t, void*);

            void* operator new[] (size_t, std::nothrow_t);
            void* operator new[] (size_t, void*);

        private:
            int indices_[2*4];

            __m128 restLength_;

            __m128 stiffness_;

            __m128 dampingCoefficient_;
        };
    }
}

#endif