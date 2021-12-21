#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>

class Program
{
public:
    static int main(int argc, char **argv);
private:
    Program()=delete;

    static int StartUnitTests();
    static void PrintHelp(const std::string program);
    static int StartMSO2();
};

#endif // PROGRAM_H
