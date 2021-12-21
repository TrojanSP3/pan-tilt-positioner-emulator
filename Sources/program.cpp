#define VERSION (__DATE__ " " __TIME__)

#include <vector>
#include <iostream>

#include "program.h"

#include "Tests/Framework/testframework.h"

const std::string KEYWORD_TEST = "test";
const std::string KEYWORD_PROTOCOL_MSO2 = "mso2";

int Program::main(int argc, char **argv)
{
    std::string program_path = argv[0];
    switch(argc)
    {
        case 2: {
            std::string parameter = argv[1];

            break;
        }
        case 3:{

            break;
        }
        default:
        {

        }
    }
}

int Program::StartUnitTests()
{
    bool test_result = UnitTests::TestFramework::RunAllTests();
    if(test_result)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}

void Program::PrintHelp(const std::string program)
{
    std::string tmp;
    std::vector<std::string> lines;

    tmp=std::string("Version: ")+VERSION;
    lines.push_back(tmp);

    tmp="Usage:";
    lines.push_back(tmp);

    tmp=program+" "+KEYWORD_TEST+" - For starting unit-tests";
    lines.push_back(tmp);

    tmp=program+" [PROTOCOL] [CONFIGFILE] - For normal execution";
    lines.push_back(tmp);

    tmp="Protocols:";
    lines.push_back(tmp);

    tmp="MSO2 - "+KEYWORD_PROTOCOL_MSO2;
    lines.push_back(tmp);

    for(auto it=lines.begin(); it!=lines.end(); it++)
        std::cout<<*it<<std::endl;

}

int Program::StartMSO2()
{

}
