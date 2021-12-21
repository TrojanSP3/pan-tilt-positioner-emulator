#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
public:
    static void Sleep(const int time_ms);
    static void ClearScreen();
    static void SetThreadName(const std::string name);

    static double RadToDegrees(const double rad);
    static double DegreesToRad(const double deg);
    static double MsoRadToDegrees(const int msorad);
    static double MsoRadToRad(const int msorad);
    static int RadToMsoRad(const double rad);
    static int DegreesToMsoRad(const double deg);

    static std::string GetCurrentTime();

    static void Ltrim(std::string &s);
    static void Rtrim(std::string &s);
    static void Trim(std::string &s);
    static void ControlCharsToString(std::string &s);
    static void StringToControlChars(std::string &s);

    static int Mso_Azimuth(int val);
    static int Mso_Elevation(int val);
    static int Mso_VelToEps(int val);
    static int Mso_EpsToVel(int val);

    static bool StringCompareCaseInsensitive(const std::string& str1, const std::string& str2 );
};

#endif // UTILS_H
