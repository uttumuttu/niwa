/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#ifndef NIWA_GEOM_HILBERTTESTSUITE_H
#define NIWA_GEOM_HILBERTTESTSUITE_H

#include "niwa/testing/ITestSuite.h"

namespace niwa {
    namespace geom {
        class HilbertTestSuite : public niwa::testing::ITestSuite {
        public:
            std::type_info const& getType() const;

            size_t nCases() const;

            niwa::testing::ITestCase* newCase(size_t index) const;
        };
    }
}

#endif
