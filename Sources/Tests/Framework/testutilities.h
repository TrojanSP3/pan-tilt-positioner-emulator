#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H
#include "testexception.h"

namespace UnitTests {
    class TestUtilities
    {
        TestUtilities(){}
    public:
        static uint16_t TCP_GetFreePort();
    };
}
#endif // TESTUTILITIES_H
