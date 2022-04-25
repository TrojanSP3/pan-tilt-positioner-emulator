#ifndef TESTCLASS_H
#define TESTCLASS_H
#include "testcase.h"

#include <vector>

namespace UnitTests {

    class TestClass {
        TestClass(){}
    public:
        static std::vector<TestCase> GetTestCases();
    };

}

#endif // TESTCLASS_H
