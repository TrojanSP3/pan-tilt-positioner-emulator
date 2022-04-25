#include "testframework.h"
#include "globaltestlist.h"

#include <iostream>

using namespace UnitTests;

std::chrono::steady_clock::time_point TestFramework::timeStarted =std::chrono::steady_clock::now();

bool TestFramework::RunAllTests()
{
    bool result = true;
    try
    {
        TestFramework::LogMsg(">>Running all tests ("+std::to_string(TestListSize)+")");

        for(int i=0;i<TestListSize;++i)
        {
            TestFunctionType* ptr = TestList[i];
            TestFramework::LogMsg("\n>>Next module testing: "+std::to_string(i));
            result=TestFramework::RunTestList(ptr());
            if(!result)
                break;
        }

        if(result)
            TestFramework::LogMsg("\n>>All tests SUCCESSFULLY finished!");
        else
            TestFramework::LogMsg("\n>>Some test FAILED!");
    }
    catch(...)
    {
        TestFramework::LogMsg("\n>>Some test CRASHED!");
        result=false;
    }
    return result;
}

bool TestFramework::RunTest(TestCase test)
{
    bool result=true;
    TestFramework::LogMsg(TestFramework::FormatLogMsg(test,"STARTED"));
    try
    {
        test.Exec();
        TestFramework::LogMsg(TestFramework::FormatLogMsg(test,"PASSED"));
    }
    catch(TestException& e)
    {
        std::string what = e.what();
        TestFramework::LogMsg(TestFramework::FormatLogMsg(test,"TEST EXCEPTION: "+what));
        result=false;
    }
    catch(std::exception& e)
    {
        std::string what = e.what();
        TestFramework::LogMsg(TestFramework::FormatLogMsg(test,"GENERAL EXCEPTION: "+what));
        result=false;
    }
    catch(...)
    {
        TestFramework::LogMsg(TestFramework::FormatLogMsg(test,"UNHANDLED ERROR"));
        result=false;
    }
    return result;
}

bool TestFramework::RunTestList(std::vector<TestCase> tests)
{
    bool result=true;
    for(unsigned int i=0;i<tests.size();++i)
    {
        TestCase test = tests[i];
        result=TestFramework::RunTest(test);
        if(!result)
            break;
    }
    return result;
}

void TestFramework::LogMsg(std::string msg)
{
    std::cout<<msg<<std::endl;
    std::cout.flush();
}

std::string TestFramework::FormatLogMsg(TestCase test, std::string msg)
{
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    long long time_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - timeStarted).count();
    double time_elapsed_s = static_cast<double>(time_elapsed_ms)/1000;
    std::string timestamp=std::to_string(time_elapsed_s);
    timestamp=timestamp.substr(0,timestamp.length()-3);
    std::string module=test.GetModuleName();
    std::string function=test.GetFunctionName();
    std::string formattedmsg=
            "> "+timestamp+" | "+module+"."+function+": "+msg;
    return formattedmsg;
}
