#ifndef GLOBALTESTLIST_H
#define GLOBALTESTLIST_H

#include <vector>

#include "testcase.h"
#include "testsuite.h"
#include "../testutils.h"
#include "../testtcpsocket.h"
#include "../testlog.h"
#include "../testproperty.h"
#include "../testconfigfile.h"
#include "../testdrawing.h"
#include "../testengine.h"
#include "../testtcpserver.h"
#include "../testprotocolmso2.h"

namespace UnitTests {

    typedef std::vector<TestCase> TestFunctionType();

    TestFunctionType* const TestList[] =
    {
        TestSuite::GetTestCases
        ,TestUtils::GetTestCases
        ,TestTcpSocket::GetTestCases   
        ,TestLog::GetTestCases
        ,TestProperty::GetTestCases
        ,TestConfigFile::GetTestCases
        ,TestDrawing::GetTestCases
        ,TestEngine::GetTestCases
        ,TestTcpServer::GetTestCases
        ,TestProtocolMSO2::GetTestCases
    };
    const int TestListSize = sizeof(TestList) / sizeof(TestList[0]);

}

#endif // GLOBALTESTLIST_H
