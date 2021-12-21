#ifndef TESTEXCEPTION_H
#define TESTEXCEPTION_H

#include <exception>
#include <string>

namespace UnitTests {

    class TestException: public std::exception
    {
    public:
        TestException(std::string msg, std::string _file, int _line);
        const char* what();
    private:
        std::string message;
    };

}

#endif // TESTEXCEPTION_H
