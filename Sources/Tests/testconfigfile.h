#ifndef TESTCONFIGFILE_H
#define TESTCONFIGFILE_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestConfigFile: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void OneLink();
        static void AddLinks();
        static void Create();
        static void Read();
        static void Update();
        static void ReadWriteControls();

    };

}
#endif // TESTCONFIGFILE_H
