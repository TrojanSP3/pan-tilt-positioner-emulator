#ifndef TCPSOCKETEXCEPTION_H
#define TCPSOCKETEXCEPTION_H

#include <exception>
#include <string>

class TcpSocketException : public std::exception
{
public:
    static std::string ErrnoToString(int _errno);
    static void ThrowErrnoException(int _errno);
public:
    TcpSocketException(std::string msg);
    const char* what() const noexcept;
private:
    std::string message;

};

#endif // TCPSOCKETEXCEPTION_H
