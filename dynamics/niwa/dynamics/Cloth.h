/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_CLOTH_H
#define NIWA_DYNAMICS_CLOTH_H

#include "ClothParticle.h"
#include "ClothSpring.h"

#include "niwa/math/vec3f.h"

#include <vector>

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace system {
        class IParallelizer;
    }

    namespace math {
        namespace blas {
            class Vector;
        }
    }

    namespace dynamics {
        class ClothCollider;

        class Cloth {
        public:
            ~Cloth();

        public:
            /**
            * Triangle structure for rendering.
            */
            struct Triangle {
                explicit Triangle(int indices[3]);

                int indices[3];
            };

        public:
            void simulate(float timeSeconds);

            std::vector<ClothParticle> const& getParticles() const;

            std::vector<Triangle> const& getTriangles() const;

            void setCollider(boost::shared_ptr<ClothCollider> collider);

            void setParallelizer(boost::shared_ptr<system::IParallelizer> parallelizer);

            /**
             * Sets whether the cloth dynamics should use
             * implicit integration for damping forces
             * (the implicit method is more stable,
             * but a lot slower); default value false.
             */
            void setUseImplicitMethod(bool useImplicitMethod);

            /**
             * Sets whether the cloth dynamics should use
             * strain limiting for springs
             * (strain limiting is more stable,
             * but slightly slower); default value false.
             */
            void setUseStrainLimiting(bool useStrainLimiting);

        private:
            void accumulateForces(math::vec3f* forces) const;

            /**
             * Operator used for implicit integration.
             */
            static void implicitOperator(
                math::blas::Vector const& src, math::blas::Vector& dst,
                void* arg);

            /**
            * Operator used for implicit integration.
            */
            void implicitOperator(
                math::blas::Vector const& src, math::blas::Vector& dst,
                float timeSeconds) const;

        private:
            Cloth(
                std::vector<ClothParticle> const& particles,
                std::vector<ClothSpring> const& springs,
                std::vector<Triangle> const& triangles,
                float thickness);

        private: // prevent copying
            Cloth(Cloth const&);
            Cloth& operator = (Cloth const&);

        private:
            class AccumulationTask;

        private:
            std::vector<ClothParticle> particles_;

            std::vector<ClothSpring> springs_;

            ClothPackedSpring* packedSprings_; // owned

            std::vector<Triangle> triangles_;

            float thickness_;

            boost::shared_ptr<ClothCollider> collider_;

            boost::shared_ptr<system::IParallelizer> parallelizer_;

            bool useImplicitMethod_;

            bool useStrainLimiting_;

            friend class ClothBuilder;
        };
    }
}

#endif
