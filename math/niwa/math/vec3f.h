/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_VEC3F_H
#define NIWA_MATH_VEC3F_H

#include "niwa/math/templates/templates.h"

namespace niwa {
    namespace math {
        /**
         * A mutable three-dimensional single-precision vector.
         */
        class vec3f 
#ifdef NIWA_MATH_USE_TEMPLATES
            : public templates::base<3,float,vec3f> 
#endif
        {
#ifdef NIWA_MATH_USE_TEMPLATES
        public:
            __forceinline float eval(int index) const {
                return (*this)[index];
            }
#endif

        public:
            static vec3f __forceinline createUninitialized();

            /**
             * Creates a vector of zeros.
             */
            __forceinline vec3f();

            __forceinline vec3f(float x_, float y_, float z_);

            __forceinline vec3f(float* values);

#ifndef NIWA_MATH_USE_TEMPLATES
            __forceinline const vec3f __fastcall operator - () const;

            __forceinline const vec3f __fastcall operator + (vec3f const& rhs) const;
            __forceinline const vec3f __fastcall operator - (vec3f const& rhs) const;
            __forceinline const vec3f __fastcall operator * (float rhs) const;
            __forceinline const vec3f __fastcall operator / (float rhs) const;

            __forceinline vec3f& __fastcall operator += (vec3f const& rhs);
            __forceinline vec3f& __fastcall operator -= (vec3f const& rhs);
#else
            template <typename D>
            __forceinline vec3f(templates::base<3,float,D> const& copy);

            template <typename D>
            __forceinline vec3f& __fastcall operator = (templates::base<3,float,D> const& copy);

            template <typename D>
            __forceinline vec3f& __fastcall operator += (templates::base<3,float,D> const& rhs);

            template <typename D>
            __forceinline vec3f& __fastcall operator -= (templates::base<3,float,D> const& rhs);
#endif
            __forceinline vec3f& __fastcall operator *= (float rhs);
            __forceinline vec3f& __fastcall operator /= (float rhs);

            __forceinline bool __fastcall operator == (vec3f const& rhs) const;

            __forceinline float __fastcall length() const;

            __forceinline float __fastcall squareLength() const;

            __forceinline void __fastcall normalize();

            __forceinline const float __fastcall operator [] (int index) const;

            __forceinline float& __fastcall operator [] (int index);

            __forceinline float* __fastcall getRaw();
            
            __forceinline float const* __fastcall getRaw() const;

        public:
            static __forceinline float __fastcall dot(vec3f const& lhs, vec3f const& rhs);

            static __forceinline const vec3f __fastcall cross(vec3f const& lhs, vec3f const& rhs);

        public:
            float x, y, z;
        };
    }
}

#include "vec3f.inl"

#endif
