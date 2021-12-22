#include "testsuite.h"

using namespace UnitTests;

std::vector<TestCase> TestSuite::GetTestCases()
{
    const std::string LOGMODULE="TestSuite";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(TemplateTestCase));
    result.push_back(TESTCASE(CreateButDontThrowException));
    result.push_back(TESTCASE(ThrowException));
    result.push_back(TESTCASE(TimeoutPass));
    result.push_back(TESTCASE(TimeoutFail));
    result.push_back(TESTCASE(AssertPass));
    result.push_back(TESTCASE(AssertFail));
    return result;
}

void TestSuite::TemplateTestCase()
{
    //empty
}

void TestSuite::CreateButDontThrowException()
{
    TestException test1("Exception should not be thrown",(__FILE__), (__LINE__));
    TestException test2("","",-1);
}

void TestSuite::ThrowException()
{
    try
    {
        EXCEPTION("This exception should be catched");
    }
    catch(TestException)
    {
        return;
    }
    //warning: code never be executed
    throw TestException("Exception was not catched",(__FILE__), (__LINE__));
}


void TestSuite::TimeoutPass()
{
    bool a=true;
    bool b=true;
    TIMEOUT(a==b,0);
}

void TestSuite::TimeoutFail()
{
    try
    {
        bool a=true;
        bool b=false;
        TIMEOUT(a==b,100);
    }
    catch(TestException)
    {
        return;
    }
    EXCEPTION("Exception was not thrown")
}

void TestSuite::AssertPass()
{
    bool a=true;
    bool b=true;
    ASSERT(a==b);
}

void TestSuite::AssertFail()
{
    try
    {
        bool a=true;
        bool b=false;
        ASSERT(a==b);
    }
    catch(TestException)
    {
        return;
    }
}
