/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2009.
 */

#ifndef NIWA_SYSTEM_PROCESSORINFO_H
#define NIWA_SYSTEM_PROCESSORINFO_H

namespace niwa {
    namespace system {
        /**
         * Information about processor capabilities,
         * including OS support for the available processor features.
         *
         * @todo Has not yet been tested on an AMD processor.
         */
        class ProcessorInfo {
        public:
            static ProcessorInfo create();

            /**
             * @return Whether both the processor and the OS support 
             *         the MMX instruction set.
             */
            bool supportsMmx() const;

            /**
             * @return Whether both the processor and the OS support
             *         the SSE instruction set.
             */
            bool supportsSse() const;

            /**
             * @return Whether both the processor and the OS support
             *         the SSE2 instruction set.
             */
            bool supportsSse2() const;

            /**
             * @return The number of logical processors.
             */
            int getLogicalProcessorCount() const;

        private:
            ProcessorInfo(
                bool supportsMmx,
                bool supportsSse,
                bool supportsSse2,
                int logicalProcessorCount);

        private:
            bool supportsMmx_;
            bool supportsSse_;
            bool supportsSse2_;

            int logicalProcessorCount_;
        };
    }
}

#endif