#include "testdrawing.h"

#include "../drawing.h"
#include "../config.h"
#include "../utils.h"

using namespace UnitTests;

std::vector<TestCase> TestDrawing::GetTestCases()
{
    const std::string LOGMODULE="TestDrawing";
    std::vector<TestCase> result;
    //result.push_back(TESTCASE(VisualCheckSpecificAngle));
    //result.push_back(TESTCASE(VisualCheckSomeParts));
    //result.push_back(TESTCASE(InfinitySpin));
    result.push_back(TESTCASE(AllCharactersPresentOnPicture));

    return result;
}


void TestDrawing::InfinitySpin()
{

    Drawing drawing(CONFIG::DRAWING::HEIGHT.Get());
    const int INCREASE = 1000;
    int azimuth = -INCREASE;
    int elevation = -INCREASE;
    while(true)
    {
        azimuth+=INCREASE;
        azimuth=Utils::Mso_Azimuth(azimuth);
        elevation+=INCREASE;
        if(elevation>CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get())
            elevation=0;
        elevation=Utils::Mso_Elevation(elevation);;

        drawing.SetAzimuth(azimuth);
        drawing.SetElevation(elevation);

        ASCIIPicture picture = drawing.PaintAll();
        TestFramework::LogMsg(picture);
        SLEEPMS(300);
    }
    TestFramework::LogMsg("Test finished");
    while(true)
        SLEEPMS(1000);
}

void TestDrawing::VisualCheckSomeParts()
{
    Drawing drawing(CONFIG::DRAWING::HEIGHT.Get());
    const int PARTS =16;
    for(int mso=0;mso<=CONFIG::ENGINE::MAX_AZIMUTH.Get();mso+=CONFIG::ENGINE::MAX_AZIMUTH.Get()/PARTS)
    {
        int azimuth = Utils::Mso_Azimuth(mso);
        int elevation = Utils::Mso_Elevation(mso);

        drawing.SetAzimuth(azimuth);
        drawing.SetElevation(elevation);

        ASCIIPicture picture = drawing.PaintAll();
        TestFramework::LogMsg(picture);
        SLEEPMS(300);
    }
    TestFramework::LogMsg("Test finished");
    while(true)
        SLEEPMS(1000);
}

void TestDrawing::VisualCheckSpecificAngle()
{
    Drawing drawing(CONFIG::DRAWING::HEIGHT.Get());
    const int MSO_ANGLE =0;//CONFIG::ENGINE::MAX_AZIMUTH*0;

    int azimuth = Utils::Mso_Azimuth(MSO_ANGLE);
    int elevation = Utils::Mso_Elevation(MSO_ANGLE);

    azimuth = Utils::Mso_Azimuth(3306);
    elevation = Utils::Mso_Elevation(15709);

    drawing.SetAzimuth(azimuth);
    drawing.SetElevation(elevation);

    ASCIIPicture picture_1 = drawing.PaintAll();
    ASCIIPicture picture_2 = drawing.PaintAzimuth();
    ASCIIPicture picture_3 = drawing.PaintElevation();

    TestFramework::LogMsg("===IMAGE===");
    TestFramework::LogMsg(picture_2);
    TestFramework::LogMsg("===IMAGE===");
    TestFramework::LogMsg(picture_3);
    TestFramework::LogMsg("===IMAGE===");
    TestFramework::LogMsg(picture_1);
    TestFramework::LogMsg("Test finished");
    while(true)
        SLEEPMS(1000);
}

bool TestDrawing::IsCharPresent(std::string line, char chr)
{
    std::size_t pos = line.find(chr,0);
    if(pos==std::string::npos)
        return false;
    else
        return true;
}
bool TestDrawing::CheckPresenceOfAllCharacters(ASCIIPicture picture)
{

    bool empty_found=IsCharPresent(picture,CONFIG::DRAWING::CHAR_EMPTY.Get());
    bool border_found=IsCharPresent(picture,CONFIG::DRAWING::CHAR_BORDER.Get());
    bool circle_found=IsCharPresent(picture,CONFIG::DRAWING::CHAR_CIRCLE.Get());
    bool center_found=IsCharPresent(picture,CONFIG::DRAWING::CHAR_CENTER.Get());
    bool pointer_found=IsCharPresent(picture,CONFIG::DRAWING::CHAR_POINTER.Get());
    bool result = empty_found && border_found && circle_found && center_found && pointer_found;
    return result;
}

void TestDrawing::AllCharactersPresentOnPicture()
{
    Drawing drawing(CONFIG::DRAWING::HEIGHT.Get());
    const int PARTS =360;
    const int REPORTS = 20;
    int reportedStatuses=0;

    for(int mso=0;mso<=CONFIG::ENGINE::MAX_AZIMUTH.Get();mso+=CONFIG::ENGINE::MAX_AZIMUTH.Get()/PARTS)
    {
        double report_progress = double(reportedStatuses)/REPORTS;
        double progress = double(mso)/CONFIG::ENGINE::MAX_AZIMUTH.Get();
        if(progress>report_progress)
        {
            int progress_int= progress*100;
            std::string msg = "Test progress: "+std::to_string(progress_int)+"%";
            TestFramework::LogMsg(msg);
            ++reportedStatuses;
        }

        int azimuth = Utils::Mso_Azimuth(mso);
        int elevation = Utils::Mso_Elevation(mso);

        drawing.SetAzimuth(azimuth);
        drawing.SetElevation(elevation);

        ASCIIPicture picture_all = drawing.PaintAll();
        ASCIIPicture picture_az = drawing.PaintAzimuth();
        ASCIIPicture picture_el = drawing.PaintElevation();

        bool check_all= CheckPresenceOfAllCharacters(picture_all);
        bool check_az = CheckPresenceOfAllCharacters(picture_az);
        bool check_el = CheckPresenceOfAllCharacters(picture_el);
        bool check_result = check_all&&check_az&&check_el;
        if(!check_result)
        {
            std::string message =
                    "Test failed at "
                    " azimuth= "+std::to_string(azimuth)
                    +" elevation= "+std::to_string(elevation);
            EXCEPTION(message);
        }
    }
}
