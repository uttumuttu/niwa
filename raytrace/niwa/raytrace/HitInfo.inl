/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_RAYTRACE_HITINFO_INL
#define NIWA_RAYTRACE_HITINFO_INL

namespace niwa {
    namespace raytrace {
        HitInfo::HitInfo()
            : 
#ifdef NIWA_RAYTRACE_HITINFO_VALIDITY_CHECK
            isValid_(false),
#endif
            distance_(0), position_(0,0,0), normal_(0,0,0) {
            // ignored
        }

        HitInfo HitInfo::createInitialized() {
            return HitInfo();
        }

#pragma warning(push)
#pragma warning(disable:4700)
        HitInfo HitInfo::createUninitialized() {
            // TODO: the uninitialized version crashes with debugging; find a workaround.
            return createInitialized();
            //unsigned char space[sizeof(HitInfo)];
            //return *reinterpret_cast<HitInfo*>(space);
        }
#pragma warning(pop)

#ifdef NIWA_RAYTRACE_HITINFO_VALIDITY_CHECK
        bool HitInfo::isValid() const {
            return isValid_;
        }
#endif

        void HitInfo::setValues(
                float distance, 
                math::vec3f const& position,
                math::vec3f const& normal,
                Material const& material) {
#ifdef NIWA_RAYTRACE_HITINFO_VALIDITY_CHECK
            isValid_ = true;
#endif

            distance_ = distance;
            position_ = position;
            normal_ = normal;

            material_ = material;
        }

        float HitInfo::distance() const {
            return distance_;
        }

        math::vec3f    const& HitInfo::position() const {
            return position_;
        }

        math::vec3f const& HitInfo::normal() const {
            return normal_;
        }

        Material const& HitInfo::material() const {
            return material_;
        }
    }
}

#endif