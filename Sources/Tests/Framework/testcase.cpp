#include "testcase.h"

using namespace UnitTests;

TestCase::TestCase(std::string _moduleName, std::string _functionName, void(*_function)())
{
    moduleName=_moduleName;
    functionName=_functionName;
    function=_function;
}

std::string TestCase::GetModuleName() const
{
    return moduleName;
}

std::string TestCase::GetFunctionName() const
{
    return functionName;
}

void TestCase::Exec() const
{
    function();
}
