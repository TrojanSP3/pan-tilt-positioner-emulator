#include <vector>
#include <iostream>

#include "config.h"
#include "log.h"

#include "Tests/Framework/testframework.h"

#include "Tests/testtcpsocket.h"
#include "Tests/testtcpserver.h"
#include "Tests/testprotocolmso2.h"
#include "Tests/testdrawing.h"

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    LOG.Start(CONFIG::LOG::TEST_FILENAME.Get());

    /*std::vector<UnitTests::TestCase> TestList = UnitTests::TestProtocolMSO2::GetTestCases();
    //for(int i=0; i<14;++i)
        //TestList.erase(TestList.begin());
    bool test_result = UnitTests::TestFramework::RunTestList(TestList);//*/

    bool test_result = UnitTests::TestFramework::RunAllTests();
    std::cout<<"Finished"<<std::endl;
    if(test_result)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}
