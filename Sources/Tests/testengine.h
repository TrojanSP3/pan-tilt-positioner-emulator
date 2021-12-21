#ifndef TESTENGINE_H
#define TESTENGINE_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestEngine: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void Infinity_Test_Mode();
        static void Start_Stop();
        static void Start_Restart();
        static void SetAzimuthPositionValid_positive();
        static void SetAzimuthPositionValid_negative();
        static void SetAzimuthPositionNonStandart_positive();
        static void SetAzimuthPositionNonStandart_negative();
        static void SetElevationPositionValid_positive();
        static void SetElevationPositionValid_negative();
        static void SetElevationPositionInvalid_positive();
        static void SetElevationPositionInvalid_negative();
        static void SpeedAzimuthPositive();
        static void SpeedAzimuthNegative();
        static void SpeedElevationPositive();
        static void SpeedElevationNegative();
        static void StopWhileMoving();
        static void LowSpeedMoving();
    private:
        static const int LONG_TIMEOUT;

    };

}
#endif // TESTENGINE_H
