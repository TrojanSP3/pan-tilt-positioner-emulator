#include "testengine.h"

#include <math.h>

#include "../engine.h"
#include "../config.h"
#include "../utility"

using namespace UnitTests;

const int TestEngine::LONG_TIMEOUT=5000;

std::vector<TestCase> TestEngine::GetTestCases()
{
    const std::string LOGMODULE="TestEngine";
    std::vector<TestCase> result;
    //result.push_back(TESTCASE(Infinity_Test_Mode));
    result.push_back(TESTCASE(Start_Stop));
    result.push_back(TESTCASE(Start_Restart));
    result.push_back(TESTCASE(SetAzimuthPositionValid_positive));
    result.push_back(TESTCASE(SetAzimuthPositionValid_negative));
    result.push_back(TESTCASE(SetAzimuthPositionNonStandart_positive));
    result.push_back(TESTCASE(SetAzimuthPositionNonStandart_negative));
    result.push_back(TESTCASE(SetElevationPositionValid_positive));
    result.push_back(TESTCASE(SetElevationPositionValid_negative));
    result.push_back(TESTCASE(SetElevationPositionInvalid_positive));
    result.push_back(TESTCASE(SetElevationPositionInvalid_negative));
    result.push_back(TESTCASE(SpeedAzimuthPositive));
    result.push_back(TESTCASE(SpeedAzimuthNegative));
    result.push_back(TESTCASE(SpeedElevationPositive));
    result.push_back(TESTCASE(SpeedElevationNegative));
    result.push_back(TESTCASE(LowSpeedMoving));
    result.push_back(TESTCASE(StopWhileMoving));

    return result;
}

std::string PrintEngineStatus(Engine* engine)
{
    const int MAX_POSITION_LEN = 5;
    const int MAX_SPEED_LEN = 5;
    const std::string WORD_AZIMUTH = "Azimuth: ";
    const std::string WORD_ELEVATION = "Elevation: ";
    const std::string WORD_POSITION = "Position: ";
    const std::string WORD_SPEED = "Speed: ";
    const int MAX_LENGTH_LINE_1 = std::max(WORD_AZIMUTH.length(),WORD_ELEVATION.length())+1;
    const int MAX_LENGTH_LINE_2 = (WORD_POSITION.length()+MAX_POSITION_LEN+1);
    const int MAX_LENGTH_LINE_3 = (WORD_SPEED.length()+MAX_SPEED_LEN+1);
    const int MAX_LENGTH = std::max(std::max(MAX_LENGTH_LINE_1,MAX_LENGTH_LINE_2),MAX_LENGTH_LINE_3);
    const int SPACES_LINE_1 = MAX_LENGTH-MAX_LENGTH_LINE_1+1+2;
    const int SPACES_LINE_2 = MAX_LENGTH-MAX_LENGTH_LINE_2+1;
    const int SPACES_LINE_3 = MAX_LENGTH-MAX_LENGTH_LINE_3+1;
    const std::string  WORD_SPACES_LINE_1 = std::string({}).append(SPACES_LINE_1,' ');
    const std::string  WORD_SPACES_LINE_2 = std::string({}).append(SPACES_LINE_2,' ');
    const std::string  WORD_SPACES_LINE_3 = std::string({}).append(SPACES_LINE_3,' ');

    int current_azimuth_position = engine->GetAzimuthPosition();
    int current_azimuth_speed = engine->GetAzimuthSpeed();
    int current_elevation_position = engine->GetElevationPosition();
    int current_elevation_speed = engine->GetElevationSpeed();

    std::string current_azimuth_position_string = std::to_string(current_azimuth_position);
    std::string current_azimuth_speed_string = std::to_string(current_azimuth_speed);
    std::string current_elevation_position_string = std::to_string(current_elevation_position);
    std::string current_elevation_speed_string = std::to_string(current_elevation_speed);

    while(current_azimuth_position_string.length()<MAX_POSITION_LEN)
        current_azimuth_position_string=" "+current_azimuth_position_string;
    while(current_elevation_position_string.length()<MAX_POSITION_LEN)
        current_elevation_position_string=" "+current_elevation_position_string;
    while(current_azimuth_speed_string.length()<MAX_SPEED_LEN)
        current_azimuth_speed_string=" "+current_azimuth_speed_string;
    while(current_elevation_speed_string.length()<MAX_SPEED_LEN)
        current_elevation_speed_string=" "+current_elevation_speed_string;

    std::string result =
            WORD_AZIMUTH+WORD_SPACES_LINE_1+WORD_ELEVATION
            +"\n"
            +WORD_POSITION+current_azimuth_position_string
            +WORD_SPACES_LINE_2+WORD_POSITION+current_elevation_position_string
            +"\n"
            +WORD_SPEED+current_azimuth_speed_string
            +WORD_SPACES_LINE_3+WORD_SPEED+current_elevation_speed_string
            ;
    return result;
}
void TestEngine::Infinity_Test_Mode()
{
    const int LONG_SLEEP = 3000;
    const int INTERVAL_SLEEP = 500;
    const int MAX_AZIMUTH=CONFIG::ENGINE::MAX_AZIMUTH.Get();
    const int MIN_AZIMUTH=0;
    const int MAX_ELEVATION=CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
    const int MIN_ELEVATION=CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
    const int SPEED_AZIMUTH=Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND.Get())/4;
    const int SPEED_ELEVATION=Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND.Get());

    const int MAX_COUNTER = 10;
    int counter = 0;
    TestFramework::LogMsg(">Running infinity test mode!");

    Engine engine;
    while(true)
    {
        TestFramework::LogMsg(PrintEngineStatus(&engine));
        SLEEPMS(LONG_SLEEP);
        engine.Start();

        TestFramework::LogMsg("\n>Stage: 1");
        engine.SetAzimuthPosition(MAX_AZIMUTH/2);
        engine.SetElevationPosition(MAX_ELEVATION);
        while((engine.GetAzimuthSpeed()==0) && (engine.GetElevationSpeed()==0))
            SLEEPMS(INTERVAL_SLEEP);

        while ((engine.GetAzimuthSpeed()) || (engine.GetElevationSpeed()))
        {
            TestFramework::LogMsg(">Moving to MAX");
            TestFramework::LogMsg(PrintEngineStatus(&engine));
            SLEEPMS(INTERVAL_SLEEP);
        }
        TestFramework::LogMsg(">Result");
        TestFramework::LogMsg(PrintEngineStatus(&engine));


        SLEEPMS(LONG_SLEEP);
        TestFramework::LogMsg("\n>Stage: 2");
        engine.SetAzimuthPosition(MIN_AZIMUTH);
        engine.SetElevationPosition(MIN_ELEVATION);
        while((engine.GetAzimuthSpeed()==0) && (engine.GetElevationSpeed()==0))
            SLEEPMS(INTERVAL_SLEEP);
        while ((engine.GetAzimuthSpeed()) || (engine.GetElevationSpeed()))
        {
            TestFramework::LogMsg(">Moving to MIN");
            TestFramework::LogMsg(PrintEngineStatus(&engine));
            SLEEPMS(INTERVAL_SLEEP);
        }
        TestFramework::LogMsg(">Result");
        TestFramework::LogMsg(PrintEngineStatus(&engine));


        SLEEPMS(LONG_SLEEP);
        TestFramework::LogMsg("\n>Stage: 3");
        engine.SetAzimuthSpeed(SPEED_AZIMUTH);
        engine.SetElevationSpeed(SPEED_ELEVATION);
        while((engine.GetAzimuthSpeed()==0) && (engine.GetElevationSpeed()==0))
            SLEEPMS(INTERVAL_SLEEP);
        counter=0;
        while ((engine.GetAzimuthSpeed()) || (engine.GetElevationSpeed()))
        {
            ++counter;
            TestFramework::LogMsg(">SPEED");
            TestFramework::LogMsg(PrintEngineStatus(&engine));
            SLEEPMS(INTERVAL_SLEEP);
            if(counter>MAX_COUNTER)
            {
                engine.SetAzimuthSpeed(0);
                engine.SetElevationSpeed(0);
            }
        }
        TestFramework::LogMsg(">Result");
        TestFramework::LogMsg(PrintEngineStatus(&engine));


        SLEEPMS(LONG_SLEEP);
        TestFramework::LogMsg("\n>Stage: 4");
        engine.SetAzimuthSpeed(-SPEED_AZIMUTH);
        engine.SetElevationSpeed(-SPEED_ELEVATION);
        while((engine.GetAzimuthSpeed()==0) && (engine.GetElevationSpeed()==0))
            SLEEPMS(INTERVAL_SLEEP);
        counter=0;
        while ((engine.GetAzimuthSpeed()) || (engine.GetElevationSpeed()))
        {
            ++counter;
            double percent = 1-double(counter)/MAX_COUNTER;
            if(counter>MAX_COUNTER)
                percent=0;
            TestFramework::LogMsg(">DECREASE: "+std::to_string(percent*100)+"%");
            TestFramework::LogMsg(PrintEngineStatus(&engine));
            int new_speed_azimuth= -percent*SPEED_AZIMUTH;
            int new_speed_elevation= -percent*SPEED_ELEVATION;
            engine.SetAzimuthSpeed(new_speed_azimuth);
            engine.SetElevationSpeed(new_speed_elevation);
            SLEEPMS(INTERVAL_SLEEP);
        }
        TestFramework::LogMsg(">Result");
        TestFramework::LogMsg(PrintEngineStatus(&engine));


        SLEEPMS(LONG_SLEEP);
        TestFramework::LogMsg("\n>Stage: 5");
        engine.SetAzimuthPosition(MIN_AZIMUTH);
        engine.SetElevationPosition(MIN_ELEVATION);
        while((engine.GetAzimuthSpeed()==0) && (engine.GetElevationSpeed()==0))
            SLEEPMS(INTERVAL_SLEEP);

        while ((engine.GetAzimuthSpeed()) || (engine.GetElevationSpeed()))
        {
            TestFramework::LogMsg(">TO START POSITION");
            TestFramework::LogMsg(PrintEngineStatus(&engine));
            SLEEPMS(INTERVAL_SLEEP);
        }
        TestFramework::LogMsg(">Result");
        TestFramework::LogMsg(PrintEngineStatus(&engine));


        SLEEPMS(LONG_SLEEP);
        TestFramework::LogMsg("\n>Stage: 6");
        engine.Stop();
        TestFramework::LogMsg(">RESET");
        TestFramework::LogMsg(PrintEngineStatus(&engine));
    }
}

void TestEngine::Start_Stop()
{
    Engine engine;
    engine.Start();
    engine.Stop();
}

void TestEngine::Start_Restart()
{
    Engine engine;
    engine.Start();
    engine.Stop();
    TestFramework::LogMsg("Restarting engine");
    engine.Start();
    engine.Stop();
}

void TestEngine::SetAzimuthPositionValid_positive()
{
    const int AZIMUTH_VALID = CONFIG::ENGINE::MAX_AZIMUTH.Get()/8;

    Engine engine;
    engine.Start();

    engine.SetAzimuthPosition(AZIMUTH_VALID);
    TIMEOUT(AZIMUTH_VALID==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.Stop();
}

void TestEngine::SetAzimuthPositionValid_negative()
{
    const int START_POSITION = CONFIG::ENGINE::MAX_AZIMUTH.Get()/4;
    const int AZIMUTH_VALID = CONFIG::ENGINE::MAX_AZIMUTH.Get()/8;

    Engine engine;
    engine.Start();

    engine.SetAzimuthPosition(START_POSITION);
    TIMEOUT(START_POSITION==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.SetAzimuthPosition(AZIMUTH_VALID);
    TIMEOUT(AZIMUTH_VALID==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.Stop();
}

void TestEngine::SetAzimuthPositionNonStandart_positive()
{
    const int MAX_AZIMUTH = CONFIG::ENGINE::MAX_AZIMUTH.Get();
    const int START_AZIMUTH_POSITION= MAX_AZIMUTH*7/8;
    const int AZIMUTH_VALID = MAX_AZIMUTH/4;
    const int AZIMUTH_NONSTANDART = MAX_AZIMUTH+AZIMUTH_VALID;

    Engine engine;
    engine.Start();

    engine.SetAzimuthPosition(START_AZIMUTH_POSITION);
    TIMEOUT(START_AZIMUTH_POSITION==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.SetAzimuthPosition(AZIMUTH_NONSTANDART);
    TIMEOUT(AZIMUTH_VALID==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.Stop();
}


void TestEngine::SetAzimuthPositionNonStandart_negative()
{
    const int START_AZIMUTH_POSITION= 0;
    const int MAX_AZIMUTH = CONFIG::ENGINE::MAX_AZIMUTH.Get();
    const int AZIMUTH_NONSTANDART = -MAX_AZIMUTH/8;
    const int AZIMUTH_VALID = MAX_AZIMUTH+AZIMUTH_NONSTANDART;

    Engine engine;
    engine.Start();

    ASSERT(engine.GetAzimuthPosition()==START_AZIMUTH_POSITION);

    engine.SetAzimuthPosition(AZIMUTH_NONSTANDART);
    TIMEOUT(AZIMUTH_VALID==engine.GetAzimuthPosition(), LONG_TIMEOUT);

    engine.Stop();
}


void TestEngine::SetElevationPositionValid_positive()
{
    const int START_POSITION= CONFIG::ENGINE::ZERO_ELEVATION.Get();
    const int MAX_POSITION = CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
    const int ELEVATION_VALID = START_POSITION+(MAX_POSITION-START_POSITION)/2;

    Engine engine;
    engine.Start();

    ASSERT(engine.GetElevationPosition()==START_POSITION);

    engine.SetElevationPosition(ELEVATION_VALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);

    engine.Stop();
}

void TestEngine::SetElevationPositionValid_negative()
{
    const int START_POSITION= CONFIG::ENGINE::ZERO_ELEVATION.Get();
    const int MIN_POSITION = CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
    const int ELEVATION_VALID = START_POSITION-(START_POSITION-MIN_POSITION)/2;

    Engine engine;
    engine.Start();

    ASSERT(engine.GetElevationPosition()==START_POSITION);

    engine.SetElevationPosition(ELEVATION_VALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);

    engine.Stop();
}

void TestEngine::SetElevationPositionInvalid_positive()
{
    const int START_POSITION= CONFIG::ENGINE::ZERO_ELEVATION.Get();
    const int MAX_POSITION = CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
    const int ELEVATION_INVALID = MAX_POSITION*2+100;
    const int ELEVATION_VALID = MAX_POSITION;

    Engine engine;
    engine.Start();

    ASSERT(engine.GetElevationPosition()==START_POSITION);

    engine.SetElevationPosition(ELEVATION_INVALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);
    SLEEPMS(100);
    engine.SetElevationPosition(ELEVATION_INVALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);

    engine.Stop();

}

void TestEngine::SetElevationPositionInvalid_negative()
{
    const int START_POSITION= CONFIG::ENGINE::ZERO_ELEVATION.Get();
    const int MIN_POSITION = CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
    const int ELEVATION_INVALID = MIN_POSITION/2-100;
    const int ELEVATION_VALID = MIN_POSITION;

    Engine engine;
    engine.Start();

    ASSERT(engine.GetElevationPosition()==START_POSITION);

    engine.SetElevationPosition(ELEVATION_INVALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);
    SLEEPMS(100);
    engine.SetElevationPosition(ELEVATION_INVALID);
    TIMEOUT(engine.GetElevationPosition()==ELEVATION_VALID, LONG_TIMEOUT);

    engine.Stop();

}

void TestEngine::SpeedAzimuthPositive()
{
    const int SPEED = Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND.Get()/4);
    ASSERT(SPEED!=0);

    Engine engine;
    engine.Start();


    int current_position = engine.GetAzimuthPosition();
    engine.SetAzimuthSpeed(SPEED);
    SLEEPMS(100);
    ASSERT(engine.GetAzimuthSpeed()==SPEED);
    int new_position = engine.GetAzimuthPosition();
    ASSERT(current_position<new_position);//current_position==0 new_position>0

    engine.Stop();

}

void TestEngine::SpeedAzimuthNegative()
{
    const int SPEED = -Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_AZIMUTH_SPEED_PER_SECOND.Get()/4);
    ASSERT(SPEED!=0);

    Engine engine;
    engine.Start();


    int current_position = engine.GetAzimuthPosition();
    engine.SetAzimuthSpeed(SPEED);
    SLEEPMS(100);
    ASSERT(engine.GetAzimuthSpeed()==SPEED);
    int new_position = engine.GetAzimuthPosition();
    ASSERT(current_position<new_position);//current_position==0 new_position~MAX_AZIMUTH

    engine.Stop();

}

void TestEngine::SpeedElevationPositive()
{
    const int SPEED = Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND.Get());
    ASSERT(SPEED!=0);
    const int MAX_POSITION=CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();

    Engine engine;
    engine.Start();


    int current_position = engine.GetElevationPosition();
    engine.SetElevationSpeed(SPEED);
    SLEEPMS(100);
    ASSERT(engine.GetElevationSpeed()==SPEED);
    int new_position = engine.GetElevationPosition();
    ASSERT(current_position<new_position);

    TIMEOUT(engine.GetElevationPosition()==MAX_POSITION,LONG_TIMEOUT);
    ASSERT(engine.GetElevationSpeed()==0);

    engine.Stop();

}

void TestEngine::SpeedElevationNegative()
{
    const int SPEED = -Utils::Mso_EpsToVel(CONFIG::ENGINE::MAX_ELEVATION_SPEED_PER_SECOND.Get());
    ASSERT(SPEED!=0);
    const int MIN_POSITION=CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();

    Engine engine;
    engine.Start();


    int current_position = engine.GetElevationPosition();
    engine.SetElevationSpeed(SPEED);
    SLEEPMS(100);
    ASSERT(engine.GetElevationSpeed()==SPEED);
    int new_position = engine.GetElevationPosition();
    ASSERT(current_position>new_position);

    TIMEOUT(engine.GetElevationPosition()==MIN_POSITION,LONG_TIMEOUT);
    ASSERT(engine.GetElevationSpeed()==0);

    engine.Stop();

}

void TestEngine::StopWhileMoving()
{
    const int SPEED = 30;

    Engine engine;
    engine.Start();


    int current_azimuth = engine.GetAzimuthPosition();
    int current_elevation = engine.GetElevationPosition();
    engine.SetAzimuthSpeed(SPEED);
    engine.SetElevationSpeed(SPEED);
    TIMEOUT(engine.GetAzimuthSpeed()==SPEED,LONG_TIMEOUT);
    TIMEOUT(engine.GetElevationSpeed()==SPEED,LONG_TIMEOUT);
    SLEEPMS(1000);

    engine.SetAzimuthSpeed(0);
    engine.SetElevationSpeed(0);
    TIMEOUT(engine.GetAzimuthSpeed()==0,LONG_TIMEOUT);
    TIMEOUT(engine.GetElevationSpeed()==0,LONG_TIMEOUT);
    SLEEPMS(1000);

    int new_azimuth = engine.GetAzimuthPosition();
    int new_elevation = engine.GetElevationPosition();
    ASSERT(current_azimuth<new_azimuth);
    ASSERT(current_elevation<new_elevation);

    engine.Stop();


    ASSERT(engine.GetAzimuthPosition()==new_azimuth);
    ASSERT(engine.GetElevationPosition()==new_elevation);
}

void TestEngine::LowSpeedMoving()
{
    const int SPEED = 10;

    Engine engine;
    engine.Start();


    int current_azimuth = engine.GetAzimuthPosition();
    int current_elevation = engine.GetElevationPosition();
    engine.SetAzimuthSpeed(SPEED);
    engine.SetElevationSpeed(SPEED);
    TIMEOUT(engine.GetAzimuthSpeed()==SPEED,LONG_TIMEOUT);
    TIMEOUT(engine.GetElevationSpeed()==SPEED,LONG_TIMEOUT);
    SLEEPMS(1000);
    TIMEOUT(engine.GetAzimuthPosition()!=current_azimuth,LONG_TIMEOUT);
    TIMEOUT(engine.GetElevationPosition()!=current_elevation,LONG_TIMEOUT);
}
