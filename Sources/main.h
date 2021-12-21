#ifndef MAIN_H
#define MAIN_H

#include <string>
#include "protocol_base.h"

class Main
{
public:
    static int main(int argc, char **argv);
private:
    Main()=delete;
    static void ChangeWorkingDir(char *argv_0);
    static void PrintHelp(const std::string program);
    static bool Start_Config(const std::string config_path);
    static bool Start_Protocol(Protocol_Base& protocol);
    static bool Start_Loop(Protocol_Base& protocol);
    static int StartMSO2(std::string config_path);

};

#endif // MAIN_H
