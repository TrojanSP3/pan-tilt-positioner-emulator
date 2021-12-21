#include "configfile.h"

#include <fstream>
#include <sys/stat.h>

#include "config.h"
#include "utils.h"

ConfigFile::ConfigFile(const std::string filename)
{
    this->filename=filename;
}

bool ConfigFile::isFileExist()
{
    struct stat buffer;
    return (stat (this->filename.c_str(), &buffer) == 0);
}

void ConfigFile::AddLink(const ConfigPropertyLink link)
{
    config_map.push_back(link);
}

void ConfigFile::ReadConfig()
{
    ReadFile();
    FileContentToMap();
    MapToConfig();
}

void ConfigFile::OverWriteConfig()
{
    ConfigToMap();
    MapToFileContent();
    ReWriteFile();
}

void ConfigFile::SyncToConfig()
{
    ConfigToMap();
    ReadFile();
    std::vector<std::string> write_lines = CompareMapToFile();
    file_content.reserve( file_content.size() + write_lines.size() );
    file_content.insert( file_content.end(), write_lines.begin(), write_lines.end() );
    ReWriteFile();
}

void ConfigFile::ReadFile()
{
    file_content.clear();
    std::ifstream file;
    file.open(filename.c_str(), std::ios_base::in);
    if(file.is_open())
    {
        file.seekg(0,std::ios_base::beg);
        while(!file.eof())
        {
            std::string line="";
            std::getline(file, line);
            if(!line.empty())
                file_content.push_back(line);
        }
        file.close();
    }
}

void ConfigFile::FileContentToMap()
{
    for(auto it=file_content.begin(); it!=file_content.end(); it++)
    {
        std::string line = *it;
        std::pair<std::string,std::string> pair = GetPairKeyValueFromLine(line);
        if(!pair.first.empty())
            file_map[pair.first]=pair.second;
    }
}

std::pair<std::string,std::string> ConfigFile::GetPairKeyValueFromLine(const std::string line)
{
    std::pair<std::string,std::string> result;
    result.first="";
    result.second="";

    size_t pos_delim = line.find(DELIM);
    if(pos_delim !=std::string::npos && pos_delim!=line.length()-1)
    {
        std::string key=line.substr(0,pos_delim);
        Utils::Trim(key);
        std::string value=line.substr(pos_delim+1,line.length()-pos_delim-1);
        Utils::Trim(value);
        Utils::StringToControlChars(value);

        result.first=key;
        result.second=value;
    }
    return result;
}


void ConfigFile::MapToConfig()
{
    for(auto link_it=config_map.begin(); link_it!=config_map.end(); link_it++)
    {
        std::string value = file_map[link_it->name];
        if(link_it->link)
        {
            link_it->link->SetFromString(value);
        }
    }
}

void ConfigFile::ConfigToMap()
{
    for(auto link_it=config_map.begin(); link_it!=config_map.end(); link_it++)
    {
        if(link_it->link)
        {
            std::string key = link_it->name;
            std::string value = link_it->link->GetString();
            file_map[key]=value;
        }
    }
}

void ConfigFile::MapToFileContent()
{
    for(auto link_it=config_map.begin(); link_it!=config_map.end(); link_it++)
    {
        std::string name = link_it->name;
        std::string value = file_map[name];
        std::string line = GetLineFromPairKeyValue(name,value);
        if(!line.empty())
            file_content.push_back(line);
    }
}

std::string ConfigFile::GetLineFromPairKeyValue(const std::string key,const std::string value)
{
    std::string result;
    std::string tmp = value;
    Utils::ControlCharsToString(tmp);
    result=key + DELIM + tmp;
    return result;
}

void ConfigFile::ReWriteFile()
{
    std::ofstream file;
    file.open(filename.c_str(), std::ios_base::out | std::ios_base::trunc);
    if(file.is_open())
    {
        for(auto it = file_content.begin(); it!=file_content.end(); it++)
        {
            file<<*it<<"\r\n";;
        }
        file.flush();
        file.close();
    }
}

std::vector<std::string> ConfigFile::CompareMapToFile()
{
    /*
    ConfigToMap();
    ReadFile();
    std::vector<std::string> write_lines = CompareMapToFile();
    file_content.reserve( file_content.size() + write_lines.size() );
    file_content.insert( file_content.end(), write_lines.begin(), write_lines.end() );
    ReWriteFile();
     */
    /*
    std::vector<std::string> file_content;
    std::map<std::string,std::string> file_map;
    std::vector<ConfigPropertyLink> config_map;
     */
    std::vector<std::string> result;
    for(auto map_it = file_map.begin(); map_it!=file_map.end(); map_it++)
    {
        bool key_found = false;

        std::string map_key = map_it->first;
        for(auto file_it = file_content.begin(); file_it!=file_content.end(); file_it++)
        {
            std::string file_line = *file_it;
            std::string file_key = GetPairKeyValueFromLine(file_line).first;
            if(map_key==file_key)
            {
                key_found=true;
                break;
            }
        }
        if(!key_found)
        {
            std::string map_line = GetLineFromPairKeyValue(map_it->first,map_it->second);
            result.push_back(map_line);
        }
    }
    return result;
}
