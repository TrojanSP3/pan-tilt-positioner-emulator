#ifndef TESTUTILITIES_H
#define TESTUTILITIES_H

#include <stdint.h>

#include "testexception.h"

namespace UnitTests {
    class TestUtilities
    {
    public:
        TestUtilities() = delete ;
        static uint16_t TCP_GetFreePort();
    };
}
#endif // TESTUTILITIES_H
