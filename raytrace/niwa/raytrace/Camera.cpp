/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/raytrace/Camera.h"

namespace niwa {
    using math::vec2f;
    using math::vec3f;

    using math::packed_vec3f;

    namespace raytrace {
        struct Camera::Sse {
            Sse(vec3f const& position, 
                vec3f const& right, vec3f const& up, vec3f const& front,
                vec2f const& backplaneDimensions, float backplaneDepth) 

                : backplaneRight(right * (backplaneDimensions.x * 0.5f)),
                  backplaneUp(up * (backplaneDimensions.y * 0.5f)),
                  backplaneFront(front * backplaneDepth),
                  pinholePosition(position) {
                // ignored
            }

            void setBackplaneDimensions(
                vec3f const& right, vec3f const& up,
                vec2f const& backplaneDimensions) {
                backplaneRight = packed_vec3f(right * (backplaneDimensions.x * 0.5f));
                backplaneUp = packed_vec3f(up * (backplaneDimensions.y * 0.5f));
            }

            void setBackplaneDepth(
                vec3f const& front, float backplaneDepth) {
                backplaneFront = packed_vec3f(front * backplaneDepth);
            }

            void set(vec3f const& position, 
                vec3f const& right, vec3f const& up, vec3f const& front,
                vec2f const& backplaneDimensions, float backplaneDepth) {
                backplaneRight = packed_vec3f(right * (backplaneDimensions.x * 0.5f));
                backplaneUp = packed_vec3f(up * (backplaneDimensions.y * 0.5f));
                backplaneFront = packed_vec3f(front * backplaneDepth);

                pinholePosition = packed_vec3f(position);
            }

            packed_vec3f backplaneRight;
            packed_vec3f backplaneUp;
            packed_vec3f backplaneFront;

            packed_vec3f pinholePosition;
        };

        Camera::Camera() 
            : pinholePosition_(0,0,0), up_(0,1,0), right_(1,0,0), front_(0,0,-1),
              backplaneDimensions_(1,1), backplaneDepth_(1), shutterTime_(1) {
            sse_ = new (_mm_malloc(sizeof(Sse), 16)) Sse(
                pinholePosition_, right_, up_, front_,
                backplaneDimensions_, backplaneDepth_);
        }

        Camera::~Camera() {
            sse_->~Sse();
            _mm_free(sse_);
        }

        float Camera::getShutterTime() const {
            return shutterTime_;
        }

        void Camera::setShutterTime(float shutterTime) {
            shutterTime_ = shutterTime;
        }

        vec2f const& Camera::getBackplaneDimensions() const {
            return backplaneDimensions_;
        }

        float Camera::getBackplaneDepth() const {
            return backplaneDepth_;
        }

        void Camera::setBackplaneDimensions(vec2f const& backplaneDimensions) {
            backplaneDimensions_ = backplaneDimensions;

            sse_->setBackplaneDimensions(
                right_, up_, backplaneDimensions_);
        }

        void Camera::setBackplaneDepth(float backplaneDepth) {
            backplaneDepth_ = backplaneDepth;

            sse_->setBackplaneDepth(front_, backplaneDepth_);
        }

        void Camera::setLookat(vec3f const& pinholePosition, vec3f const& target) {
            pinholePosition_ = pinholePosition;

            front_ = target - pinholePosition;
            front_.normalize();

            up_ = vec3f(0,1,0);
            up_ -= front_ * vec3f::dot(up_, front_);
            up_.normalize();

            // Notice that up_ = (0,1,0), front_ = (0,0,-1)
            // gives the default right_ = (1,0,0).

            right_ = vec3f::cross(front_, up_);

            sse_->set(pinholePosition_, right_, up_, front_,
                backplaneDimensions_, backplaneDepth_);
        }

        const ray3f Camera::getEyeRay(float u, float v) const {
            // Position from backplane to pinhole position;
            vec3f relativePosition =
                front_ * backplaneDepth_
                - right_ * (backplaneDimensions_.x * (2*u-1) * 0.5f)
                - up_ * (backplaneDimensions_.y * (2*v-1) * 0.5f);

            vec3f direction = relativePosition;

            direction.normalize();

            return ray3f(pinholePosition_ - relativePosition, direction);
        }

        const packed_ray3f Camera::getEyeRay(__m128 u, __m128 v) const {
            u = _mm_sub_ps(_mm_add_ps(u,u), _mm_set_ps1(1.0f)); // to [-1,1]
            v = _mm_sub_ps(_mm_add_ps(v,v), _mm_set_ps1(1.0f)); // to [-1,1]

            // Position from backplane to pinhole position.
            packed_vec3f relativePosition =
                  sse_->backplaneFront
                - sse_->backplaneRight * u
                - sse_->backplaneUp * v;

            packed_vec3f direction = relativePosition;

            direction.normalizeAccurate();

            return packed_ray3f(
                sse_->pinholePosition - relativePosition,
                direction);
        }
    }
}