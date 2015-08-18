/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_TEMPLATES_H
#define NIWA_MATH_TEMPLATES_H

// It seems that the expression templates are
// slowing many applications down, so we have
// turned them off for now.

// #define NIWA_MATH_USE_TEMPLATES

#ifdef NIWA_MATH_USE_TEMPLATES
namespace niwa {
    namespace math {
        /**
         * Expression templates based on the paper
         * "Improving Large Vector Operations with C++
         * Expression Templates and ATLAS" by
         * L. Plagne and F. Hulsemann.
         */
        namespace templates {
            template <int N, typename T, typename D>
            struct base {
                __forceinline D const& __fastcall derived() const {
                    return static_cast<D const&>(*this);
                }
            };

            template <typename T>
            struct add {
                __forceinline static T __fastcall apply(T lhs, T rhs) {
                    return lhs + rhs;
                }
            };

            template <typename T>
            struct sub {
                __forceinline static T __fastcall apply(T lhs, T rhs) {
                    return lhs - rhs;
                }
            };

            template <int N, typename T, typename D>
            struct neg : public base<N, T, neg<N,T,D>> {
                D const& arg;

                __forceinline neg(D const& a) : arg(a) {
                    // ignored
                }

                __forceinline T __fastcall eval(int index) const {
                    return -arg.eval(index);
                }
            };

            template <int N, typename T, typename L, typename R, typename OP>
            struct exp : public base<N, T, exp<N,T,L,R,OP>> {
                L const& lhs;
                R const& rhs;

                __forceinline exp(L const& l, R const& r) : lhs(l), rhs(r) {
                    // ignored
                }

                __forceinline T __fastcall eval(int index) const {
                    return OP::apply(lhs.eval(index), rhs.eval(index));
                }
            };

            template <int N, typename T, typename L>
            struct rightmul : public base<N, T, rightmul<N,T,L>> {
                L const& lhs;
                T rhs;

                __forceinline rightmul(L const& l, T r) : lhs(l), rhs(r) {
                    // ignored
                }

                __forceinline T __fastcall eval(int index) const {
                    return lhs.eval(index) * rhs;
                }
            };

            template <int N, typename T, typename D>
            static __forceinline const neg<N,T,D> __fastcall operator - (
                    base<N,T,D> const& arg) {
                return neg<N,T,D>(arg.derived());
            }

            template <int N, typename T, typename L, typename R>
            static __forceinline const exp<N,T,L,R,add<T>> __fastcall operator + (
                    base<N,T,L> const& lhs,
                    base<N,T,R> const& rhs) {
                return exp<N,T,L,R,add<T>>(lhs.derived(), rhs.derived());
            }

            template <int N, typename T, typename L, typename R>
            static __forceinline const exp<N,T,L,R,sub<T>> __fastcall operator - (
                    base<N,T,L> const& lhs,
                    base<N,T,R> const& rhs) {
                return exp<N,T,L,R,sub<T>>(lhs.derived(), rhs.derived());
            }

            template <int N, typename T, typename L>
            static __forceinline const rightmul<N,T,L> __fastcall operator * (
                    base<N,T,L> const& lhs,
                    float rhs) {
                return rightmul<N,T,L>(lhs.derived(), rhs);
            }

            template <int N, typename T, typename L>
            static __forceinline const rightmul<N,T,L> __fastcall operator / (
                    base<N,T,L> const& lhs,
                    float rhs) {
                return lhs * ((rhs == 0) ? 1 : 1 / rhs);
            }
        }
    }
}
#endif

#endif