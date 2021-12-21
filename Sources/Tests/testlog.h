#ifndef TESTLOG_H
#define TESTLOG_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestLog: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void FileExistingAndRecreation();
        static void WriteDebug();
        static void WriteInfo();
        static void WriteError();
        static void WriteCrit();
        static void WriteEmptyArgument();
        static void FileOverflow();
    private:
        static const int LONG_TIMEOUT;
        static void DeleteLogFile();
        static bool IsLogFileExist();
        static bool CheckDataInLogFile(std::string keyword);
        static size_t LogFileSize();
    };

}

#endif // TESTLOG_H
