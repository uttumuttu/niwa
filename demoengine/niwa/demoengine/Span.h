/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_DEMOENGINE_SPAN_H
#define NIWA_DEMOENGINE_SPAN_H

#include <boost/shared_ptr.hpp>

namespace niwa {
    namespace demolib {
        class IEffect;
        class LuaRef;
    }

    namespace demoengine {
        class Span {
        public:
            /**
             * @param setupCallback The setup callback for the span.
             *                      Will be invoked exactly once before
             *                      any functions of the effect associated
             *                      with the span are invoked.
             */
            Span(
                boost::shared_ptr<demolib::LuaRef> effect,
                double startTimeSeconds, double endTimeSeconds,
                int layer, boost::shared_ptr<demolib::LuaRef> setupCallback);

            bool hasStarted(double timeSeconds) const;

            bool isActive(double timeSeconds) const;

            demolib::IEffect* getEffect();

            /**
             * @return The setup callback for the span.
             */
            boost::shared_ptr<demolib::LuaRef> getSetupCallback() const;

            /**
             * @return Whether the span has been setup.
             */
            bool hasBeenSetup() const;

            /**
             * Invoked after the span has been setup.
             * Before calling this method, hasBeenSetup
             * will return false; after calling this method,
             * hasBeenSetup will return true.
             */
            void setHasBeenSetup();

        public: // sorting criteria
            /**
             * Partial order based on ascending layer.
             */
            static bool ascendingLayer(Span const& lhs, Span const& rhs);

            /**
             * Partial order based on ascending start time.
             * If the spans start at equal time, the layer is used as a
             * secondary criteria.
             */
            static bool ascendingStartTime(Span const* lhs, Span const* rhs);

        private:
            boost::shared_ptr<demolib::LuaRef> effect_;

            double startTimeSeconds_;

            double endTimeSeconds_;

            int layer_;

            bool hasBeenSetup_;

            boost::shared_ptr<demolib::LuaRef> setupCallback_;
        };
    }
}

#endif