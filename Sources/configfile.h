#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <string>
#include <map>
#include <vector>

#include "property.h"

class ConfigPropertyLink
{
public:
    ConfigPropertyLink(std::string _name,PropertyBase* _link): name(_name), link(_link) {}
    std::string name = "";
    PropertyBase* link=nullptr;
};

class ConfigFile
{
public:
    ConfigFile(const std::string filename);
    bool isFileExist();
    void AddLink(const ConfigPropertyLink link);
    void ReadConfig();
    void OverWriteConfig();
    void SyncToConfig();
private:
    void ReadFile();
    void ReWriteFile();
    void FileContentToMap();
    void MapToFileContent();
    void MapToConfig();
    void ConfigToMap();

    std::pair<std::string,std::string> GetPairKeyValueFromLine(const std::string line);
    std::string GetLineFromPairKeyValue(const std::string key,const std::string value);
    std::vector<std::string> CompareMapToFile();

    std::string filename="";
    std::vector<std::string> file_content;
    std::map<std::string,std::string> file_map;
    std::vector<ConfigPropertyLink> config_map;
    const char DELIM='=';
};

#endif // CONFIGFILE_H
