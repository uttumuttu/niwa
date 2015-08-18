/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_TESTING_ITESTSUITE_H
#define NIWA_TESTING_ITESTSUITE_H

#include <typeinfo>

namespace niwa {
    namespace testing {
        class ITestCase;

        /**
         * A collection of related test cases.
         */
        class ITestSuite {
        public:
            virtual ~ITestSuite();

            virtual std::type_info const& getType() const = 0;

            virtual size_t nCases() const = 0;

            /**
             * Creates a new test case, passing ownership.
             */
            virtual ITestCase* newCase(size_t index) const = 0;
        };
    }
}

#endif