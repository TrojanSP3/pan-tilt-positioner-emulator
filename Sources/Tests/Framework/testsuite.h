#ifndef TESTSUITE
#define TESTSUITE

#include "testframework.h"

namespace UnitTests {

    class TestSuite: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void TemplateTestCase();
        static void CreateButDontThrowException();
        static void ThrowException();
        static void TimeoutPass();
        static void TimeoutFail();
        static void AssertPass();
        static void AssertFail();
    };

}
#endif // TESTSUITE
