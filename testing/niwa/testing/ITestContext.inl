/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_TESTING_ITESTCONTEXT_INL
#define NIWA_TESTING_ITESTCONTEXT_INL

#include <sstream>

namespace niwa {
    namespace testing {
        template <typename T>
        void ITestContext::assertEquals(T const& expected, T const& actual) {
            if(expected != actual) {
                std::ostringstream expectedString;
                std::ostringstream actualString;

                expectedString << expected;
                actualString << actual;

                onAssertFailure(expectedString.str(), actualString.str());
            }
        }

        template <typename T>
        void ITestContext::assertEquals(
                T const& expected, T const& actual, std::string const& message) {
            if(expected != actual) {
                std::ostringstream expectedString;
                std::ostringstream actualString;

                expectedString << expected;
                actualString << actual;

                onAssertFailure(expectedString.str(), actualString.str(), message);
            }
        }
    }
}

#endif