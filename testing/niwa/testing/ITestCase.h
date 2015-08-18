/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_TESTING_ITESTCASE_H
#define NIWA_TESTING_ITESTCASE_H

#include <typeinfo>

namespace niwa {
    namespace testing {
        class ITestContext;

        class ITestCase {
        public:
            virtual ~ITestCase();

            virtual std::type_info const& getType() const = 0;

            virtual void test(ITestContext& context) const = 0;
        };
    }
}

#endif