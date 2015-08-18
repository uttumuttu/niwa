/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/math/Primes.h"

#include <cmath>

namespace niwa {
    namespace math {
        bool Primes::isPrime(int value) {
            if(value < 2) {
                return false;
            }

            int bound = static_cast<int>(sqrt(static_cast<double>(value)));

            for(int i=2; i<=bound; ++i) {
                if((value % i) == 0) {
                    return false;
                }
            }
            return true;
        }

        /**
         * Analysis of complexity:
         *
         *  \f$c\f$ = count,
         *  \f$n\f$ = the \f$c\f$'th prime \f$\sim c / \log{c}\f$,
         *  \f$\pi\f$ = prime counting function.
         *
         * Clearly, the space complexity is \f$O(c)\f$.
         *
         * Time cost \f$T\f$ is:
         * 
         * \f{eqnarray*}{
         *  T &=& \sum_{k=2}^n [\pi(k)-1] \\
         *  &=& (n-1) + \sum_{k=2}^n \pi(k) \\
         * &\leq& (n-1) + (n-1) \pi(n) \\
         * &\leq& (n-1) (1 + \pi(n)) \\
         *  &\sim& (n-1) (1 + n / \log {n}) \\
         *  &=& O(n^2 / \log{n}) \\
         *  &\sim& O([c^2 / \log^2{c}] / [\log{(c / \log{c})}]) \\
         *  &=& O(c^2 / [(\log^2{c}) (\log{c} - \log{\log{c}})]) \\
         *  &=& O(c^2 / [\log^3{c} - \log^2{c} \log{\log{c}}]) \\
         *  &=& O(c^2 / \log^3{c}). 
         * \f}
         */
        int* Primes::getFirstPrimes(int count) {
            int* primes = new int[count];

            int nFound = 0;
            int current = 2;

            while(nFound < count) {
                bool isPrime = true;

                for(int i=0; i<nFound; ++i) {
                    if( (current % primes[i]) == 0 ) {
                        isPrime = false;
                        break;
                    }
                }

                if(isPrime) {
                    primes[nFound++] = current;
                }

                ++current;
            }

            return primes;
        }
    }
}
