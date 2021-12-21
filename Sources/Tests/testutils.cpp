#include "testutils.h"

#include <chrono>
#include <cmath>

#include "../utils.h"

using namespace UnitTests;

std::vector<TestCase> TestUtils::GetTestCases()
{
    const std::string LOGMODULE="TestUtils";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(Sleep));
    result.push_back(TESTCASE(AnglesConvert));
    result.push_back(TESTCASE(DateTimeString));
    result.push_back(TESTCASE(StringTrim));
    result.push_back(TESTCASE(ControlChars));
    return result;
}

void TestUtils::Sleep()
{
    const int SLEEP_TIME_MS = 250;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    Utils::Sleep(SLEEP_TIME_MS);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    int time_elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    ASSERT(time_elapsed_ms>=SLEEP_TIME_MS);
}

void TestUtils::AnglesConvert()
{
    const int PRECISION_RAD = 5;
    const int PRECISION_RAD_VALUE = std::pow(10,PRECISION_RAD);
    const int PRECISION_DEG = 2;
    const int PRECISION_DEG_VALUE = std::pow(10,PRECISION_DEG);

    const double ANGLE_DEG = 45;
    const double ANGLE_RAD = 0.785398163397448;
    const int ANGLE_MSO = ANGLE_RAD*10000+1;

    double rounded_deg, rounded_rad, rounded_mso;

    rounded_deg=Utils::RadToDegrees(ANGLE_RAD);
    ASSERT(abs(rounded_deg-ANGLE_DEG)<=(1/PRECISION_DEG_VALUE));

    rounded_rad=Utils::DegreesToRad(ANGLE_DEG);
    ASSERT(abs(rounded_rad-ANGLE_RAD)<=(1/PRECISION_RAD_VALUE));

    rounded_deg=Utils::MsoRadToDegrees(ANGLE_MSO);
    ASSERT(abs(rounded_deg-ANGLE_DEG)<=(1/PRECISION_DEG_VALUE));

    rounded_rad=Utils::MsoRadToRad(ANGLE_MSO);
    ASSERT(abs(rounded_rad-ANGLE_RAD)<=(1/PRECISION_RAD_VALUE));

    rounded_mso=Utils::RadToMsoRad(ANGLE_RAD);
    ASSERT(abs(rounded_mso-ANGLE_MSO)<=1);

    rounded_mso=Utils::DegreesToMsoRad(ANGLE_DEG);
    ASSERT(abs(rounded_mso-ANGLE_MSO)<=1);
}

void TestUtils::DateTimeString()
{
    const std::string FORMAT="hh:mm:ss.zzz";
    const int NUMBERS_COUNT=9;
    std::string msg = Utils::GetCurrentTime();
    TestFramework::LogMsg(msg);
    ASSERT(!msg.empty());
    ASSERT(msg.length()==FORMAT.length());

    int num_cnt=0;
    for(uint32_t i=0;i<msg.length();++i)
    {
        char ch = msg[i];
        if( ch<='9' && ch>='0')
            ++num_cnt;
    }
    ASSERT(num_cnt==NUMBERS_COUNT);
}

void TestUtils::StringTrim()
{
    const std::string TRIM_PART = " \r\n\0\t ";
    const std::string LINE = "ABCD1234!";

    const std::string LEFT = TRIM_PART+LINE;
    const std::string RIGHT = LINE+TRIM_PART;
    const std::string BOTH = TRIM_PART+LINE+TRIM_PART;

    std::string line;

    line=LEFT;
    Utils::Rtrim(line);
    ASSERT(line==LEFT);
    Utils::Ltrim(line);
    ASSERT(line==LINE);

    line=RIGHT;
    Utils::Ltrim(line);
    ASSERT(line==RIGHT);
    Utils::Rtrim(line);
    ASSERT(line==LINE);

    line=BOTH;
    Utils::Trim(line);
    ASSERT(line==LINE);
}

void TestUtils::ControlChars()
{
    const std::string STRING =  "\\\\ N \n R \r";
    const std::string ESCAPED = "\\\\ N \\n R \\r";

    std::string str = STRING;
    Utils::ControlCharsToString(str);
    ASSERT(str == ESCAPED);

    str = ESCAPED;
    Utils::StringToControlChars(str);
    ASSERT(str == STRING);
}

