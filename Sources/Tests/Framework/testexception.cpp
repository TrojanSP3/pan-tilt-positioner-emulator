#include "testexception.h"

using namespace UnitTests;

TestException::TestException(std::string msg, std::string _file, int _line)
{
    message = "TestException: "+msg
            +"\n at "+_file
            +"\n line: "+std::to_string(_line);
            //+"\n";
}
TestException::~TestException()
{
    message = "Don't use it after destruction!";
}
const char* TestException::what() const noexcept
{
    return message.c_str();
}
