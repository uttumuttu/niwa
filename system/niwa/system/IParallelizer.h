/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_IPARALLELIZER_H
#define NIWA_SYSTEM_IPARALLELIZER_H

namespace niwa {
    namespace system {
        /**
         * Automatic parallelization of for loops.
         * 
         * Notice that individual IParallelizer
         * objects are generally not thread-safe: their
         * methods must be invoked from a single
         * thread only.
         */
        class IParallelizer {
        public:
            /**
             * Callback to a for loop.
             */
            class ICallback {
            public:
                virtual ~ICallback();

                /**
                 * Invokes the for loop at the given index.
                 */
                virtual void __fastcall invoke(int index) = 0;
            };

            virtual ~IParallelizer();

            /**
             * Automatically parallelized for loop.
             * Generally not thread-safe: must be invoked from
             * a single thread only.
             *
             * @param callback Callback to the for loop.
             * @param start Start index of the loop (inclusive).
             * @param end End index of the loop (exclusive).
             */
            virtual void loop(ICallback& callback, int start, int end) = 0;

            /**
             * Automatically parallelized for loop.
             * Generally not thread-safe: must be invoked from
             * a single thread only.
             *
             * @param callback Callback to the for loop.
             * @param start Start index of the loop (inclusive).
             * @param end End index of the loop (exclusive).
             * @param stride Granularity for parallelization (default 1).
             */
            virtual void loop(ICallback& callback, int start, int end, int stride) = 0;
        };
    }
};

#endif