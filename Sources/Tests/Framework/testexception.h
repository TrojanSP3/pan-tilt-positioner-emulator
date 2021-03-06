#ifndef TESTEXCEPTION_H
#define TESTEXCEPTION_H
#include "../../crossplatform.h"

#include <exception>
#include <string>

namespace UnitTests {

    class TestException: public std::exception
    {
    public:
        TestException(std::string msg, std::string _file, int _line);
        virtual ~TestException();
        virtual const char* what() const NOEXCEPT;
    private:
        std::string message;
    };

}

#endif // TESTEXCEPTION_H
