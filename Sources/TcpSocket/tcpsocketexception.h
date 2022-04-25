#ifndef TCPSOCKETEXCEPTION_H
#define TCPSOCKETEXCEPTION_H
#include "../crossplatform.h"

#include <exception>
#include <string>

class TcpSocketException : public std::exception
{
public:
    static std::string ErrorToString(int code);
	static int GetLastErrorCode();
    static TcpSocketException CreateErrorException(int code);
public:
    TcpSocketException(std::string msg);
    const char* what() const NOEXCEPT;
private:
    std::string message;
};

#endif // TCPSOCKETEXCEPTION_H
