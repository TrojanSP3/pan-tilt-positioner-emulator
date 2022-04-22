#include "testexception.h"

using namespace UnitTests;

TestException::TestException(std::string msg, std::string _file="", int _line=0)
{
    message = "TestException: "+msg;
    if(_file!="")
    {
        message+="\n at "+_file;
        if(_line)
        {
            message+="\n line: "+std::to_string(_line);
        }
    }
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
