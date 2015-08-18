/**
 * @file
 * @author Mikko Kauppila
 *
 * Copyright (C) Mikko Kauppila 2010.
 */

#include "HilbertTestSuite.h"

#include "niwa/testing/ITestCase.h"
#include "niwa/testing/ITestContext.h"

using niwa::testing::ITestCase;
using niwa::testing::ITestContext;

#include "Hilbert.h"

#include <cassert>
#include <set>

namespace niwa {
    namespace geom {
        /**
         * Tests 2-dimensional Hilbert curve of order 1.
         *
         * The ground truth is from J K Lawder's paper
         * on Hilbert curve implementations.
         */
        class Hilbert21 : public ITestCase {
        public:
            std::type_info const& getType() const {
                return typeid(Hilbert21);
            }

            void test(ITestContext& context) const {
                int expected[4] = {0, 3, 1, 2};
                int actual[4];

                for(size_t y=0; y<2; ++y) {
                    for(size_t x=0; x<2; ++x) {
                        HilbertPoint<2,1> point;

                        point.elements[0] = x;
                        point.elements[1] = y;

                        HilbertPoint<2,1> encoded = Hilbert<2,1>::encode(point);

                        actual[y*2+x] = encoded.index();
                    }
                }

                for(size_t i=0; i<4; ++i) {
                    context.assertEquals(expected[i], actual[i]);
                }
            }
        };

        /**
         * Tests 2-dimensional Hilbert curve of order 2.
         *
         * The ground truth is from J K Lawder's paper
         * on Hilbert curve implementations.
         */
        class Hilbert22 : public ITestCase {
        public:
            std::type_info const& getType() const {
                return typeid(Hilbert22);
            }

            void test(ITestContext& context) const {
                int expected[16] = {
                    0,1,14,15,
                    3,2,13,12,
                    4,7,8,11,
                    5,6,9,10
                };

                int actual[16];

                for(size_t y=0; y<4; ++y) {
                    for(size_t x=0; x<4; ++x) {
                        HilbertPoint<2,2> point;

                        point.elements[0] = x;
                        point.elements[1] = y;

                        HilbertPoint<2,2> encoded = Hilbert<2,2>::encode(point);

                        actual[y*4+x] = encoded.index();
                    }
                }

                for(size_t i=0; i<16; ++i) {
                    context.assertEquals(expected[i], actual[i]);
                }
            }
        };

        /**
        * Tests injectivity of 3-dimensional Hilbert curve.
        */
        class Hilbert3 : public ITestCase {
        public:
            std::type_info const& getType() const {
                return typeid(Hilbert3);
            }

            void test(ITestContext& context) const {
                testOrder<1>(context);
                testOrder<2>(context);
                testOrder<3>(context);
                testOrder<4>(context);
                testOrder<5>(context);
            }

            template <size_t ORDER>
            void testOrder(ITestContext& context) const {
                size_t orderPow2 = 1 << ORDER;
                size_t orderPow6 = 1 << (3 * ORDER);

                std::set<size_t> indices;

                for(size_t z=0; z<orderPow2; ++z) {
                    for(size_t y=0; y<orderPow2; ++y) {
                        for(size_t x=0; x<orderPow2; ++x) {
                            HilbertPoint<3,ORDER> point;

                            point.elements[0] = x;
                            point.elements[1] = y;
                            point.elements[2] = z;

                            HilbertPoint<3,ORDER> encoded = Hilbert<3,ORDER>::encode(point);

                            for(size_t i=0; i<3; ++i) {
                                context.assertEquals<size_t>(
                                    orderPow2-1,
                                    std::max(orderPow2-1, encoded.elements[i]),
                                    "encoded elements not in bounds");
                            }

                            size_t index = encoded.index();

                            context.assertEquals<size_t>(
                                orderPow6-1,
                                std::max(orderPow6-1, index),
                                "Hilbert index not in bounds");

                            indices.insert(encoded.index());
                        }
                    }
                }

                context.assertEquals<size_t>(orderPow6, indices.size(),
                    "Hilbert code not injective");
            }
        };

        std::type_info const& HilbertTestSuite::getType() const {
            return typeid(HilbertTestSuite);
        }

        size_t HilbertTestSuite::nCases() const {
            return 3;
        }

        ITestCase* HilbertTestSuite::newCase(size_t index) const {
            switch(index) {
            case 0:
                return new Hilbert21();
            case 1:
                return new Hilbert22();
            case 2:
                return new Hilbert3();
            default:
                assert(false);
                return 0;
            }
        }
    }
}