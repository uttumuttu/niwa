/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_MATH_PRIMES_H
#define NIWA_MATH_PRIMES_H

namespace niwa {
    namespace math {
        /**
         * Utilities for dealing with prime numbers.
         */
        class Primes {
        public:
            /**
             * Tests primality test with a naive \f$O(\sqrt{n}) = O(2^{b/2})\f$
             * algorithm, where \f$n\f$ is the magnitude
             * and \f$b\f$ is the number of bits \f$(n = 2^b)\f$.
             *
             * @return Whether the given value is a prime (false for zero and one).
             *
             * @param value A natural number (non-negative integer).
             */
            static bool isPrime(int value);

            /**
             * Computes the first primes with a simple
             * sieve algorithm with space complexity
             * \f$O(c)\f$ and (crude) approximate time complexity 
             * \f$O(c^2 / \log^3{c})\f$, where \f$c\f$ is the number
             * of primes to be computed.
             *
             * @param count The number of primes to be computed.
             *
             * @return An array of the first primes.
             */
            static int* getFirstPrimes(int count);

        private: // prevent instantiation
            Primes();

        private: // prevent copying
            Primes(Primes const&);
            Primes& operator = (Primes const&);
        };
    }
}

#endif
