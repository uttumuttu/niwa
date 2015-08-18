/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#include "niwa/random/Halton.h"

#include "niwa/random/RandomSet.h"
#include "niwa/random/EvenlySpacedSequence.h"

#include "niwa/math/Primes.h"

#define NOMINMAX
#include <windows.h>

namespace {
    /**
     * Computes the ith value of a Halton sequence.
     *
     * @param index Zero-based index value.
     * @param base The Halton base.
     * @param invBase Inverse of base (for efficiency).
     */
    static double __forceinline getHaltonAt(
        unsigned int index, const int base, const double invBase) {
        if(base == 2) {
            double f = 0.5;

            double value = 0.0;

            while(index) {
                value += f * static_cast<double>(index & 1);

                index >>= 1;
                f *= invBase;
            }
            return value;
        } else {
            double f = invBase;

            double value = 0.0;

            while(index) {
                value += f * static_cast<double>(index % base);

                index /= base;
                f *= invBase;
            }
            return value;
        }
    }

    /**
     * Computes the next value of a Halton sequence.
     *
     * @param value Current value, in range [0,1).
     * @param invBase Inverse of base (for efficiency).
     */
    static double getNextHalton(double value, const double invBase) {
        const double residual = (1.0 - 0.0000000001) - value;

        if(invBase < residual) {
            value += invBase;
        } else {
            double h = invBase;
            double hh;

            do{
                hh = h;
                h *= invBase;
            } while ( h >= residual );

            value += hh + h - 1.0;
        }

        return value;
    }
}

namespace niwa {
    namespace random {
        Halton::Halton(int base)  
            : value_(0.0), 
              base_(base),
              invBase_(1.0 / base) {
            // ignored
        }

        void Halton::setSeed(long seed) {
            value_ = static_cast<double>(getHaltonAt(seed, base_, invBase_));
        }

        float Halton::nextf() {
            return static_cast<float>(nextd());
        }

        double Halton::nextd() {
            // Make a backup of the value
            // before computing the new value:
            // in the end, we'll return the old value.
            const double oldValue = value_;

            value_ = getNextHalton(value_, invBase_);

            return oldValue;
        }

        RandomSet* Halton::createHaltonSet(int dimension, bool isThreadSafe) {
            IRandom** components = new IRandom* [dimension];

            int* primes = math::Primes::getFirstPrimes(dimension);

            for(int i=0; i<dimension; ++i) {
                components[i] = new Halton(primes[i]);
            }

            delete[] primes;

            return new RandomSet(dimension, components, isThreadSafe);
        }

        RandomSet* Halton::createHaltonHammersleySet(int dimension, int length, bool isThreadSafe) {
            IRandom** components = new IRandom* [dimension];

            if(dimension > 0) {
                components[0] = new EvenlySpacedSequence(length == 0 ? 1 : length, 0);
            }

            int* primes = math::Primes::getFirstPrimes(dimension-1);

            for(int i=0; i<dimension-1; ++i) {
                components[i+1] = new Halton(primes[i]);
            }

            delete[] primes;

            return new RandomSet(dimension, components, isThreadSafe);
        }
    }
}