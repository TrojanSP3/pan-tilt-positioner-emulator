#ifndef TESTCLASS_H
#define TESTCLASS_H

#include <vector>

#include "testcase.h"

namespace UnitTests {

    class TestClass {
        TestClass()=delete;
    public:
        static std::vector<TestCase> GetTestCases();
    };

}

#endif // TESTCLASS_H
