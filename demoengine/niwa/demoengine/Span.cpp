/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "Span.h"

#include "niwa/demolib/LuaRef.h"

namespace niwa {
    namespace demoengine {
        using demolib::IEffect;
        using demolib::LuaRef;

        Span::Span(boost::shared_ptr<LuaRef> effect,
            double startTimeSeconds, double endTimeSeconds, 
            int layer, boost::shared_ptr<LuaRef> setupCallback)
            : effect_(effect), startTimeSeconds_(startTimeSeconds),
              endTimeSeconds_(endTimeSeconds), layer_(layer),
              hasBeenSetup_(false), setupCallback_(setupCallback) {
            // ignored
        }

        bool Span::hasStarted(double timeSeconds) const {
            return startTimeSeconds_ <= timeSeconds;
        }

        bool Span::isActive(double timeSeconds) const {
            return startTimeSeconds_ <= timeSeconds 
                && timeSeconds < endTimeSeconds_;
        }

        IEffect* Span::getEffect() {
            return reinterpret_cast<IEffect*>(effect_->getObject());
        }

        bool Span::hasBeenSetup() const {
            return hasBeenSetup_;
        }

        void Span::setHasBeenSetup() {
            hasBeenSetup_ = true;
        }

        boost::shared_ptr<LuaRef> Span::getSetupCallback() const {
            return setupCallback_;
        }

        bool Span::ascendingLayer(Span const& lhs, Span const& rhs) {
            return lhs.layer_ < rhs.layer_;
        }

        bool Span::ascendingStartTime(Span const* lhs, Span const* rhs) {
            if(lhs->startTimeSeconds_ < rhs->startTimeSeconds_) {
                return true;
            } else if(lhs->startTimeSeconds_ > rhs->startTimeSeconds_) {
                return false;
            } else {
                return lhs->layer_ < rhs->layer_;
            }
        }
    }
}