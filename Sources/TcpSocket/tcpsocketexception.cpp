#include "tcpsocketexception.h"

#include <cstring>

std::string TcpSocketException::ErrnoToString(int _errno)
{
    std::string result(strerror(_errno));
    if(result.empty())
        result="Unknown error: "+std::to_string(_errno);
    result=std::to_string(_errno)+": "+result;
    return result;
}

void TcpSocketException::ThrowErrnoException(int _errno)
{
    std::string errorString = TcpSocketException::ErrnoToString(_errno);
    throw TcpSocketException(errorString);
}

TcpSocketException::TcpSocketException(std::string msg)
{
    message="TcpSocketException: "+msg;
}

const char* TcpSocketException::what() const noexcept
{
    return message.c_str();
}
