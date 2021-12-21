#include "utils.h"

#include <iostream>
#include <thread>
#include <math.h>
#include <algorithm>
#include <ctime>

#ifdef _WIN32
	#include <windows.h>
	//#include <processthreadsapi.h>
	#include <sstream>
#else
    #include <unistd.h>
	#include <sys/prctl.h>
#endif

#include "config.h"

void Utils::Sleep(const int time_ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds((time_ms)));
}

void Utils::ClearScreen()
{
    std::cout.flush();
    system("clear");
    std::cout.flush();
}

void Utils::SetThreadName(const std::string name)
{
    const int MAX_NAME_LEN=16;//prctl docs
    if(name.length()>MAX_NAME_LEN-1)
        name.substr(0,MAX_NAME_LEN-1);
    std::string tmp = name+'\0';

#ifdef _WIN32
	//HRESULT hr = SetThreadDescription(GetCurrentThread(), L"simulation_thread");
#else
	prctl(PR_SET_NAME,tmp.c_str(),0,0,0);
#endif
	    
}

double Utils::RadToDegrees(const double rad)
{
    return rad/M_PI*180;
}
double Utils::DegreesToRad(const double deg)
{
    return deg*M_PI/180;
}
double Utils::MsoRadToDegrees(const int msorad)
{
    return RadToDegrees(MsoRadToRad(msorad));
}
double Utils::MsoRadToRad(const int msorad)
{
    return double(msorad)/10000;
}
int Utils::RadToMsoRad(const double rad)
{
    return int(rad*10000);
}
int Utils::DegreesToMsoRad(const double deg)
{
    return RadToMsoRad(DegreesToRad(deg));
}

std::string Utils::GetCurrentTime()
{
    const std::time_t t = std::time(nullptr);
    const std::tm tm = *std::localtime(&t);

    std::string datetime_string = std::asctime(&tm);
    //012345678901234567890123
    //Www Mmm dd hh:mm:ss yyyy
    std::string hours = datetime_string.substr(11,2);
    std::string minutes = datetime_string.substr(14,2);
    std::string seconds = datetime_string.substr(17,2);
    std::string milliseconds = std::to_string(t%1000);
    while(milliseconds.length()<3)
        milliseconds="0"+milliseconds;

    std::string result = hours + ":" + minutes + ":" + seconds + "." + milliseconds;
    return result;

}

void Utils::Ltrim(std::string &s)
{//https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
}

void Utils::Rtrim(std::string &s)
{//https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

void Utils::Trim(std::string &s)
{
    Ltrim(s);
    Rtrim(s);
}

void Utils::ControlCharsToString(std::string &s)
{
    uint32_t i=0;
    while (i<s.length())
    {
        bool erase =false;
        std::string replace;

        char c = s.at(i);
        switch(c)
        {
            case '\r': erase=true; replace="\\r"; break;
            case '\n': erase=true; replace="\\n"; break;
        };

        if(erase)
        {
            s.erase(i,1);
            s.insert(i,replace);
        }

        ++i;
    }
}

void Utils::StringToControlChars(std::string &s)
{
    if(s.empty())
        return;
    uint32_t i=0;
    while (i<s.length()-1)
    {
        bool erase =false;
        std::string replace;

        char c1 = s.at(i);
        if(c1=='\\')
        {
            char c2 = s.at(i+1);
            switch(c2)
            {
                case 'r': erase=true; replace="\r"; break;
                case 'n': erase=true; replace="\n"; break;
            };
            if(erase)
            {
                s.erase(i+1,1);
                s.erase(i,1);
                s.insert(i,replace);
            }
        }
        ++i;
    }
}

int Utils::Mso_Azimuth(int val)
{
    while(val<0)
        val+=CONFIG::ENGINE::MAX_AZIMUTH.Get();
    val=val%CONFIG::ENGINE::MAX_AZIMUTH.Get();
    return val;
}
int Utils::Mso_Elevation(int val)
{
    if(val<CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get())
        val=CONFIG::ENGINE::LIMIT_MIN_ELEVATION.Get();
    if(val>CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get())
        val=CONFIG::ENGINE::LIMIT_MAX_ELEVATION.Get();
    return val;
}
int Utils::Mso_VelToEps(int val)
{
    val=int(float(val)/CONFIG::ENGINE::VEL_TO_EPS_COEFFICIENT.Get());
    return val;
}
int Utils::Mso_EpsToVel(int val)
{
    val=int(float(val)*CONFIG::ENGINE::VEL_TO_EPS_COEFFICIENT.Get());
    return val;
}
