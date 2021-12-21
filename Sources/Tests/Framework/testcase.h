#ifndef TESTCASE_H
#define TESTCASE_H

#include <string>

namespace UnitTests {

    class TestCase
    {
    public:
        TestCase(std::string _moduleName, std::string _functionName, void(*_function)());
        std::string GetModuleName() const;
        std::string GetFunctionName() const;
        void Exec() const;
    private:
        std::string moduleName;
        std::string functionName;
        void(*function)();
    };

}

#endif // TESTCASE_H
