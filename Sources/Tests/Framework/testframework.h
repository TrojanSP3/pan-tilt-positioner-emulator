#ifndef TESTFRAMEWORK_H
#define TESTFRAMEWORK_H
#include "testcase.h"
#include "testclass.h"
#include "testexception.h"
#include "testutilities.h"

#include <chrono>
#include <thread>

namespace UnitTests {

    class TestFramework
    {
        TestFramework(){}
    public:
		static bool RunAllTests();
        static bool RunTestList(std::vector<TestCase> tests);
        static bool RunTest(TestCase test);
        static void LogMsg(std::string msg);
    private:
        static std::string FormatLogMsg(TestCase test, std::string msg);
        static std::chrono::steady_clock::time_point timeStarted;
    };

    #define TESTCASE(function) \
    TestCase(LOGMODULE,#function,function)

    #define SLEEPMS(time_ms) \
    {std::this_thread::sleep_for(std::chrono::milliseconds((time_ms)));}

    #define EXCEPTION(message) \
    {TestException _test_exception_macro((message), (__FILE__), (__LINE__)); throw _test_exception_macro;}

    #define TIMEOUT(condition, time_ms) \
    {int _timeout=0; while(_timeout<100) { \
    if((condition)) break; SLEEPMS((time_ms)/100); ++_timeout; } \
    if(_timeout==100) { \
    std::string msg="Timeout. Line: "+std::to_string(__LINE__)+" "+#condition; EXCEPTION(msg); }}

    #define ASSERT(condition) \
    {if(!(condition)) EXCEPTION("Assert: "#condition" Line: "+std::to_string(__LINE__));}

}

#endif // TESTFRAMEWORK_H
