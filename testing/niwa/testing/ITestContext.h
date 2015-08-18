/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_TESTING_ITESTCONTEXT_H
#define NIWA_TESTING_ITESTCONTEXT_H

#include <string>

namespace niwa {
    namespace testing {
        /**
         * A test context provides assertion capabilities.
         *
         * Assertions may never throw exceptions; thus, the only
         * way for a test context to "fail immediately" is to
         * ignore extra assertion failures until the test case (or suite)
         * is done.
         */
        class ITestContext {
        public:
            virtual ~ITestContext();

        public: // templatized context
            template <typename T>
            void assertEquals(T const& expected, T const& actual);

            template <typename T>
            void assertEquals(T const& expected, T const& actual, std::string const& message);

        private: // non-templatized context
            virtual void onAssertFailure(
                std::string const& expected, 
                std::string const& actual) = 0;

            virtual void onAssertFailure(
                std::string const& expected,
                std::string const& actual,
                std::string const& message) = 0;
        };
    }
}

#include "ITestContext.inl"

#endif