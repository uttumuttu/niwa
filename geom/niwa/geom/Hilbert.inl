/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_GEOM_HILBERT_INL
#define NIWA_GEOM_HILBERT_INL

#include <cassert>
#include <iostream>

namespace niwa {
    namespace geom {
        template <size_t DIM, size_t ORDER>
        size_t HilbertPoint<DIM,ORDER>::index() const {
            size_t result = elements[DIM-1];

            size_t factor = 1U << ORDER;

            for(int i=DIM-2; i>=0; --i) {
                result *= factor;
                result += elements[i];
            }
            return result;
        }

        template <size_t DIM, size_t ORDER>
        bool const HilbertPoint<DIM,ORDER>::operator == (HilbertPoint<DIM,ORDER> const& rhs) const {
            for(size_t i=0; i<DIM; ++i) {
                if(elements[i] != rhs.elements[i]) {
                    return false;
                }
            }
            return true;
        }

        template <size_t DIM, size_t ORDER>
        bool const HilbertPoint<DIM,ORDER>::operator != (HilbertPoint<DIM,ORDER> const& rhs) const {
            return !(*this == rhs);
        }

        template <size_t DIM, size_t ORDER>
        HilbertPoint<DIM,ORDER> Hilbert<DIM, ORDER>::encode(HilbertPoint<DIM,ORDER> const& pt) {
            HilbertPoint<DIM,ORDER> h; // result

            size_t mask = 1U << (ORDER - 1);

            size_t A = 0;

            for(size_t j=0; j<DIM; ++j) {
                if(pt.elements[j] & mask) {
                    A |= g_mask(j);
                }
            }

            size_t P = calc_P2(A);

            // Add in DIM bits to h.

            // Must be signed (for condition-testing).
            int i = ORDER * DIM - DIM;

            {
                size_t element = i / ORDER;
                if((i % ORDER) > 
                        static_cast<int>(ORDER) - static_cast<int>(DIM)) {
                    h.elements[element  ] |= P << (i % ORDER);
                    h.elements[element+1] |= P >> (ORDER - i % ORDER);

                    assert(h.elements[element  ] < (1U << ORDER));
                    assert(h.elements[element+1] < (1U << ORDER));
                } else {
                    assert(i - element * ORDER >= 0);

                    std::cout << "A: " << A << std::endl;
                    std::cout << "P: " << P << ", i: " << i << ", elt: " << element << std::endl;

                    h.elements[element  ] |= P << (i - element * ORDER);

                    assert(h.elements[element  ] < (1U << ORDER));
                }
            }

            size_t W = 0;
            size_t J = calc_J(P);
            size_t xJ = J - 1U;
            size_t T = calc_T(P);
            size_t tT = T;

            for (i -= DIM, mask >>= 1U; i >= 0; i -= DIM, mask >>= 1U) {
                A = 0;

                for(size_t j = 0; j < DIM; ++j) {
                    if(pt.elements[j] & mask) {
                        A |= g_mask(j);
                    }
                }

                W ^= tT;
                size_t tS = A ^ W;
                size_t S = calc_tS_tT(xJ, tS);
                P = calc_P2(S);

                // Add in DIM bits to hcode.
                {
                    size_t element = i / ORDER;
                    if(i % ORDER > static_cast<int>(ORDER) - static_cast<int>(DIM)) {
                        h.elements[element  ] |= P << (i % ORDER);
                        h.elements[element+1] |= P >> (ORDER - i % ORDER);

                        assert(h.elements[element  ] < (1U << ORDER));
                        assert(h.elements[element+1] < (1U << ORDER));
                    } else {
                        h.elements[element  ] |= P << (i - element * ORDER);

                        assert(h.elements[element  ] < (1U << ORDER));
                    }
                }

                if(i > 0) {
                    T   = calc_T(P);
                    tT  = calc_tS_tT(xJ, T);
                    J   = calc_J(P);
                    xJ += J - 1U;
                }
            }

            return h;
        }

        template <size_t DIM, size_t ORDER>
        HilbertPoint<DIM,ORDER> Hilbert<DIM, ORDER>::decode(HilbertPoint<DIM,ORDER> const& H) {
            HilbertPoint<DIM,ORDER> pt; // result

            size_t mask = 1U << (ORDER - 1U);

            // Must be signed (for condition-testing).
            int i = ORDER * DIM - DIM;

            size_t P = calc_P(i, H);
            size_t J = calc_J(P);
            size_t xJ = J - 1U;

            size_t tS = P ^ (P / 2U);
            size_t S = tS;
            size_t A = tS;

            size_t T = calc_T(P);
            size_t tT = T;

            for(int j=DIM-1; P > 0; P >>= 1U, --j) {
                if(P & 1) {
                    pt.elements[j] |= mask;
                }
            }

            size_t W = 0;

            for(i -= DIM, mask >>= 1U; i >= 0; i -= DIM, mask >>= 1U) {
                P = calc_P(i, H);
                S = P ^ (P / 2U);
                tS = calc_tS_tT(xJ, S);
                W ^= tT;
                A = W ^ tS;

                // Distribute bits to coordinates.
                for(int j=DIM-1; A > 0; A >>= 1U, --j) {
                    if(A & 1U) {
                        pt.elements[j] |= mask;
                    }
                }

                if(i > 0) {
                    T = calc_T(P);
                    tT = calc_tS_tT(xJ, T);
                    J = calc_J(P);
                    xJ += J - 1U;
                }
            }

            return pt;
        }

        template <size_t DIM, size_t ORDER>
        HilbertPoint<DIM,ORDER>::HilbertPoint() {
            for(size_t i=0; i<DIM; ++i) {
                elements[i] = 0;
            }
        }

        template <size_t DIM, size_t ORDER>
        HilbertPoint<DIM,ORDER>::HilbertPoint(size_t aElements[DIM]) {
            for(size_t i=0; i<DIM; ++i) {
                elements[i] = aElements[i];
            }
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::calc_P(int i, HilbertPoint<DIM,ORDER> const& H) {
            int const element = i / ORDER;

            size_t P = H.elements[element];

            if (i % ORDER > static_cast<int>(ORDER) - static_cast<int>(DIM)) {
                size_t temp1 = H.elements[element + 1];
                P >>= i % ORDER;
                temp1 <<= ORDER - i % ORDER;
                P |= temp1;
            } else {
                P >>= i % ORDER; // P is a DIM bit point
            }

            // the & masks out spurious highbit values
            if(DIM < ORDER) {
                P &= (1U << DIM) - 1U;
            }

            return P;
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::calc_P2(size_t S) {
            size_t P = S & g_mask(0);

            for(size_t i=1; i < DIM; ++i) {
                size_t m = g_mask(i);

                if( S & m ^ (P >> 1) & m) {
                    P |= m;
                }
            }

            return P;
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::calc_J(size_t P) {
            // @todo: This function is inefficiently designed
            //        for single-return point.

            size_t J = DIM;
            size_t i;

            for(i=1; i < DIM; ++i) {
                if((P >> i & 1U) == (P & 1U)) {
                    continue;
                } else {
                    break;
                }
            }
            if(i != DIM) {
                J -= i;
            }
            return J;
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::calc_T(size_t P) {
            if(P < 3) {
                return 0;
            }
            if(P % 2) {
                return (P-1) ^ ((P-1) / 2U);
            }
            return (P-2) ^ ((P-2) / 2U);
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::calc_tS_tT(size_t xJ, size_t val) {
            size_t retval = val;

            if((xJ % DIM) != 0) {
                size_t temp1 = val >> (xJ % DIM);
                size_t temp2 = val << (DIM - (xJ % DIM));

                retval = temp1 | temp2;
                retval &= (1U << DIM) - 1U;
            }

            return retval;
        }

        template <size_t DIM, size_t ORDER>
        size_t Hilbert<DIM, ORDER>::g_mask(size_t index) {
            assert(index >= 0 && index < DIM);

            return 1U << (DIM-1U-index);
        }
    }
}

#endif