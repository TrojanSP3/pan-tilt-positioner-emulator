#ifndef TESTUTILS_H
#define TESTUTILS_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestUtils: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    protected:
        static void Sleep();
        static void AnglesConvert();
        static void DateTimeString();
        static void StringTrim();
        static void ControlChars();
    };

}
#endif // TESTUTILS_H
