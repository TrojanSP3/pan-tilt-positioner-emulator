#ifndef TESTDRAWING_H
#define TESTDRAWING_H

#include "Framework/testframework.h"
#include "../drawing.h"

namespace UnitTests
{

    class TestDrawing: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void VisualCheckSpecificAngle();
        static void VisualCheckSomeParts();
        static void InfinitySpin();

        static void AllCharactersPresentOnPicture();
    private:
        static bool IsCharPresent(std::string line, char chr);
        static bool CheckPresenceOfAllCharacters(ASCIIPicture picture);
    };

}
#endif // TESTDRAWING_H
