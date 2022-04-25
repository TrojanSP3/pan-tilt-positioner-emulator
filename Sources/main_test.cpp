#include "crossplatform.h"

#include <vector>
#include <iostream>

#include "config.h"
#include "log.h"

#include "Tests/Framework/testframework.h"
#include "Tests/Framework/globaltestlist.h"

int run(int argc, char ** argv)
{
    (void)argc;
	PlatformInitialization();
	std::string directory = Utils::GetDirectoryFromARGV0(argv[0]);
	std::cout<<"File path: "<<directory<<std::endl;
	std::cout<<"Changing working directory..."<<std::endl;
	Utils::ChangeWorkingDirectory(directory);

	LOG.Start(CONFIG::LOG::TEST_FILENAME.Get());

    /*
	std::vector<UnitTests::TestCase> TestList = UnitTests::TestEngine::GetTestCases();
    //for(int i=0; i<15;++i)
        //TestList.erase(TestList.begin());
    bool test_result = UnitTests::TestFramework::RunTestList(TestList);
    //*/

    bool test_result = UnitTests::TestFramework::RunAllTests();
    std::cout<<std::endl<<"Program finished"<<std::endl;
    if(test_result)
        return EXIT_SUCCESS;
    else
        return EXIT_FAILURE;
}


int main(int argc, char *argv[])
{
	int result;
	try
	{
		result = run(argc, static_cast<char**>(argv));
	}
	catch(std::exception& e)
	{
		std::cerr<<"UNHANDLED EXCEPTION: "<<e.what()<<std::endl;;
		result = EXIT_FAILURE;
	}
	catch(...)
	{
		std::cerr<<"UNHANDLED ERROR"<<std::endl;;
		result = EXIT_FAILURE;
	}
	std::cout.flush();
    std::cerr.flush();
	std::cin.get();
	return result;
}
