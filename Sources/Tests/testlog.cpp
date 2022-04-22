#include "testlog.h"

#include <sys/stat.h>
#include <fstream>

#include "../log.h"

using namespace UnitTests;

const int TestLog::LONG_TIMEOUT=5000;

std::vector<TestCase> TestLog::GetTestCases()
{
    const std::string LOGMODULE="TestLog";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(FileExistingAndRecreation));
    result.push_back(TESTCASE(WriteDebug));
    result.push_back(TESTCASE(WriteInfo));
    result.push_back(TESTCASE(WriteError));
    result.push_back(TESTCASE(WriteCrit));
    result.push_back(TESTCASE(WriteEmptyArgument));
    result.push_back(TESTCASE(FileOverflow));
    return result;
}

inline std::string FILENAME()
{
    return "./testlog.log";
}

void TestLog::DeleteLogFile()
{
    if(IsLogFileExist())
    {
        std::remove(FILENAME().c_str());
    }
}
bool TestLog::IsLogFileExist()
{
    struct stat buffer;
    return (stat (FILENAME().c_str(), &buffer) == 0);
}

bool TestLog::CheckDataInLogFile(std::string keyword)
{
    if(!IsLogFileExist())
    {
        return false;
    }
    std::ifstream file(FILENAME());
    std::string str;
    bool found=false;
    while(!file.eof())
    {
        file>>str;
        size_t pos = str.find(keyword,0);
        found=(pos!=std::string::npos);
        if(found)
            break;
    }
    file.close();
    return found;
}

size_t TestLog::LogFileSize()
{
    if(!IsLogFileExist())
    {
        return 0;
    }
    std::ifstream file(FILENAME());
    file.seekg(0,std::ios_base::end);
    long stream_status = file.tellg();
    size_t size = 0;
    if(stream_status!=-1)
        size = static_cast<size_t>(stream_status);
    else
        size = 0;
    return size;
}

void TestLog::FileExistingAndRecreation()
{
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteDebug(FILENAME(),FILENAME(),FILENAME());
    TIMEOUT(IsLogFileExist(),LONG_TIMEOUT);
    logger.Stop();

}

void TestLog::WriteDebug()
{
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteDebug(FILENAME(),FILENAME(),FILENAME());
    TIMEOUT(CheckDataInLogFile(FILENAME()),LONG_TIMEOUT);
    logger.Stop();
}
void TestLog::WriteInfo()
{
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteInfo(FILENAME(),FILENAME(),FILENAME());
    TIMEOUT(CheckDataInLogFile(FILENAME()),LONG_TIMEOUT);
    logger.Stop();
}
void TestLog::WriteError()
{
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteError(FILENAME(),FILENAME(),FILENAME());
    TIMEOUT(CheckDataInLogFile(FILENAME()),LONG_TIMEOUT);
    logger.Stop();
}
void TestLog::WriteCrit()
{
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteCrit(FILENAME(),FILENAME(),FILENAME());
    TIMEOUT(CheckDataInLogFile(FILENAME()),LONG_TIMEOUT);
    logger.Stop();
}
void TestLog::WriteEmptyArgument()
{
    const std::string KEYWORD ="UNKNOWN";
    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME());
    logger.WriteDebug("","","");
    TIMEOUT(CheckDataInLogFile(KEYWORD),LONG_TIMEOUT);
    logger.Stop();
}

void TestLog::FileOverflow()
{
    const std::string KEYWORD ="LogFile";
    const uint64_t MAX_SIZE=1024;
    const uint64_t SIZE = static_cast<uint64_t>(1.1*MAX_SIZE);
    const uint64_t MAX_WORD_SIZE_T = 100;

    const uint64_t MAX_WORD_SIZE = std::min(MAX_WORD_SIZE_T,SIZE);
    const uint64_t WORDS_NUM = SIZE/MAX_WORD_SIZE;

    std::string word = "";
    for(uint32_t i=0;i<MAX_WORD_SIZE;++i)
    {
        word.append("O");
    }

    TestLog::DeleteLogFile();
    Log logger;
    logger.Start(FILENAME(),MAX_SIZE);
    for(uint64_t i=0;i<WORDS_NUM;++i)
    {
        if((i+1)%1000==0)
        {
            std::string msg=std::to_string(i+1)+"/"+std::to_string(WORDS_NUM);
            TestFramework::LogMsg(msg);
        }
        logger.WriteDebug("X","Y",word);
    }
    TestFramework::LogMsg(std::to_string(WORDS_NUM)+"/"+std::to_string(WORDS_NUM));
    /*if(LogFileSize()<MAX_SIZE)
    {
        TestFramework::LogMsg("Wait...");
        SLEEPMS(LONG_TIMEOUT);
    }*/
    TIMEOUT(LogFileSize()<MAX_SIZE,LONG_TIMEOUT);
    TestFramework::LogMsg("Checking...");
    TIMEOUT(CheckDataInLogFile(KEYWORD),LONG_TIMEOUT);
    logger.Stop();
}

