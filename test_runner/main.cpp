/**
 * @file
 * @author Mikko Kauppila 2010.
 */

#include "niwa/testing/ITestCase.h"
#include "niwa/testing/ITestContext.h"
#include "niwa/testing/ITestSuite.h"

#include "niwa/geom/HilbertTestSuite.h"

using namespace niwa::testing;

#include <string>
#include <iostream>

#include <memory>

namespace {
    class Context : public ITestContext {
    public:
        Context() : hasFailed_(false) {
            // ignored
        }

        void onAssertFailure(std::string const& expected, std::string const& actual) {
            if(!hasFailed_) {
                hasFailed_ = true;

                std::ostringstream stream;

                stream << "expected " << expected << ", got " << actual;

                message_ = stream.str();
            }
        }

        void onAssertFailure(
                std::string const& expected, 
                std::string const& actual, 
                std::string const& message) {
            if(!hasFailed_) {
                hasFailed_ = true;

                std::ostringstream stream;

                stream << "expected " << expected 
                    << ", got " << actual 
                    << " (" << message << ")";

                message_ = stream.str();
            }
        }

        bool hasFailed() const {
            return hasFailed_;
        }

        std::string const& getMessage() const {
            return message_;
        }

    private:
        bool hasFailed_;

        // Only defined if the context has failed.
        std::string message_;
    };

    static void test(ITestSuite const& suite) {
        std::cout << "Testing suite " << suite.getType().name() << std::endl;


        for(size_t i=0; i<suite.nCases(); ++i) {
            std::auto_ptr<ITestCase> testCase(suite.newCase(i));

            std::cout << "  Testing case " << testCase->getType().name() << " ";

            Context context;

            testCase->test(context);

            if(context.hasFailed()) {
                std::cout << "FAIL: " << context.getMessage() << std::endl;
            } else {
                std::cout << "OK" << std::endl;
            }
        }
    }
}

int main(int /*argc*/, char** /*argv*/) {
    niwa::geom::HilbertTestSuite hilbert;

    test(hilbert);

    return 0;
}