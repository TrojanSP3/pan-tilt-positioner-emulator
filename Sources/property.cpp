#include "property.h"

#include <algorithm>
#include <cctype>

template<>
std::string Property<std::string>::ToString() const
{
    return Get();
}

template<>
void Property<std::string>::FromString(const std::string &s)
{
    Set(s);
}

template<>
std::string Property<int>::ToString() const
{
    return std::to_string(Get());
}

template<>
void Property<int>::FromString(const std::string &s)
{
    Set(std::stoi(s));
}

template<>
std::string Property<uint64_t>::ToString() const
{
    return std::to_string(Get());
}

template<>
void Property<uint64_t>::FromString(const std::string &s)
{
    Set(std::stoull(s));
}

template<>
std::string Property<bool>::ToString() const
{
    if(Get())
    {
        return "true";
    }
    else
    {
        return "false";
    }
}

template<>
void Property<bool>::FromString(const std::string &s)
{
    std::string val = s;
    std::string comp = "false";
    std::transform(val.begin(), val.end(), val.begin(),
        [](unsigned char c){ return std::tolower(c); });
    std::transform(comp.begin(), comp.end(), comp.begin(),
        [](unsigned char c){ return std::tolower(c); });

    if(val==comp)
        Set(false);
    else
        Set(true);
}

template<>
std::string Property<char>::ToString() const
{
    return std::string(1,Get());
}

template<>
void Property<char>::FromString(const std::string &s)
{
    Set(s.at(0));
}

template<>
std::string Property<double>::ToString() const
{
    return std::to_string(Get());
}

template<>
void Property<double>::FromString(const std::string &s)
{
    Set(std::stod(s));
}

template<>
std::string Property<uint32_t>::ToString() const
{
    return std::to_string(Get());
}

template<>
void Property<uint32_t>::FromString(const std::string &s)
{
    Set(std::stoul(s));
}
