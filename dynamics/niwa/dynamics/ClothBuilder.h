/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DYNAMICS_CLOTHBUILDER_H
#define NIWA_DYNAMICS_CLOTHBUILDER_H

namespace niwa {
    namespace math {
        class vec2i;
        class vec3f;
    }

    namespace dynamics {
        class Cloth;

        class ClothBuilder {
        public:
            /**
             * @param mass Mass of the resulting cloths (kg).
             *
             * @param stretchingStiffness 
             *            Hookean spring constant for stretching
             *            integrated over length (kg * m / s^2).
             *
             * @param bendingStiffness
             *            Hookean spring constant for bending
             *            integrated over length (kg * m / s^2).
             *            Typically much less than the stretching stiffness
             *            (e.g., ten times smaller).
             *
             * @param dampingCoefficient
             *            Damping coefficient integrated over length (kg * m / s).
             *
             * @param thickness
             *            Thickness of the cloth (m).
             */
            ClothBuilder(
                float mass,
                float stretchingStiffness,
                float bendingStiffness,
                float dampingCoefficient,
                float thickness);

            Cloth* buildRectangular(
                math::vec3f const& cornerPosition,
                math::vec3f const sides[2],
                math::vec2i const& dimensions,
                bool cornerConstraints[2][2]) const;

        private:
            float mass_;
            
            float stretchingStiffness_;

            float bendingStiffness_;

            float dampingCoefficient_;

            float thickness_;
        };
    }
}

#endif