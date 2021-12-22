#include "testconfigfile.h"

#include <sys/stat.h>
#include <fstream>

#include "../configfile.h"

using namespace UnitTests;

std::vector<TestCase> TestConfigFile::GetTestCases()
{
    const std::string LOGMODULE="TestConfigFile";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(OneLink));
    result.push_back(TESTCASE(AddLinks));
    result.push_back(TESTCASE(Create));
    result.push_back(TESTCASE(Read));
    result.push_back(TESTCASE(Update));
    result.push_back(TESTCASE(ReadWriteControls));
    return result;
}

inline std::string FILENAME()
{
    return "./testconfig.cfg";
}

struct CFG
{
    CFG():
        int_1(0), int_2(0), bool_1(false), string_1("")
    {
        int_1_name="";
        int_2_name="";
        bool_1_name="";
        string_1_name="";
    }
    std::string int_1_name;
    Property<int> int_1;
    std::string int_2_name;
    Property<int> int_2;
    std::string bool_1_name;
    Property<bool> bool_1;
    std::string string_1_name;
    Property<std::string> string_1;
};

inline CFG InitCfg()
{
    CFG result;
    result.int_1_name="Value_1";
    result.int_1.Set(0);
    result.int_2_name="Value_2";
    result.int_2.Set(0);
    result.bool_1_name="Logic";
    result.bool_1.Set(false);
    result.string_1_name="Text";
    result.string_1.Set("");
    return result;
}

bool IsFileExist()
{
    struct stat buffer;
    return (stat (FILENAME().c_str(), &buffer) == 0);
}

void DeleteFile()
{
    if(IsFileExist())
    {
        std::remove(FILENAME().c_str());
    }
}

bool CheckDataInFile(std::string keyword)
{
    if(!IsFileExist())
    {
        return false;
    }
    std::ifstream file(FILENAME());
    std::string str;
    bool found=false;
    while(!file.eof())
    {
        file>>str;
        size_t pos = str.find(keyword,0);
        found=(pos!=std::string::npos);
        if(found)
            break;
    }
    file.close();
    return found;
}

void TestConfigFile::OneLink()
{
    const int KEY_SINGLE = 16;
    const std::string KEY_SINGLE_STR = std::to_string(KEY_SINGLE);
    Property<int> a_property(-1);
    Property<int>* a_pointer = &a_property;
    TestFramework::LogMsg("Property: "+std::to_string(uint64_t(&a_property)));
    TestFramework::LogMsg("Pointer:  "+std::to_string(uint64_t(a_pointer)));
    ConfigPropertyLink a_link_1("reference",&a_property);
    ConfigPropertyLink a_link_2("pointer",a_pointer);
    ConfigPropertyLink a_link_2b("pointer",a_pointer);
    ConfigPropertyLink a_link_3("manual",nullptr);
    a_link_3.link=a_pointer;

    TestFramework::LogMsg("Link_ref: "+std::to_string(uint64_t(a_link_1.link)));
    TestFramework::LogMsg("Link_ptr: "+std::to_string(uint64_t(a_link_2.link)));
    TestFramework::LogMsg("Link_pt2: "+std::to_string(uint64_t(a_link_2b.link)));
    TestFramework::LogMsg("Link_man: "+std::to_string(uint64_t(a_link_3.link)));
    ASSERT(a_link_2.link==a_link_2b.link);
    ASSERT(a_pointer==a_link_1.link);
    ASSERT(a_pointer==a_link_2.link);
    ASSERT(a_pointer==a_link_3.link);

    a_link_1.link->SetFromString(KEY_SINGLE_STR);
    ASSERT(a_property.Get()==KEY_SINGLE);

}

void TestConfigFile::AddLinks()
{
    const int KEY_INT = 37;
    const bool KEY_BOOL = true;
    const std::string KEY_TEXT = "BOLOOCAP";

    CFG cfg = InitCfg();

    ConfigPropertyLink link_int("test int",&(cfg.int_1));
    ConfigPropertyLink link_bool("test bool",&cfg.bool_1);
    ConfigPropertyLink link_text("test str",&cfg.string_1);

    link_int.link->SetFromString(std::to_string(KEY_INT));
    link_bool.link->SetFromString(std::to_string(KEY_BOOL));
    link_text.link->SetFromString(KEY_TEXT);

    ASSERT(cfg.int_1.Get()==KEY_INT);
    ASSERT(cfg.bool_1.Get()==KEY_BOOL);
    ASSERT(cfg.string_1.Get()==KEY_TEXT);
}

void TestConfigFile::Create()
{
    DeleteFile();
    CFG cfg = InitCfg();

    ConfigFile file(FILENAME());
    file.AddLink(ConfigPropertyLink(cfg.bool_1_name,&cfg.bool_1));
    file.AddLink(ConfigPropertyLink(cfg.int_1_name,&cfg.int_1));
    file.AddLink(ConfigPropertyLink(cfg.int_2_name,&cfg.int_2));
    file.AddLink(ConfigPropertyLink(cfg.string_1_name,&cfg.string_1));
    file.OverWriteConfig();

    ASSERT(CheckDataInFile(cfg.int_1_name));
    DeleteFile();
}

void TestConfigFile::Read()
{
    DeleteFile();

    CFG cfg_write = InitCfg();
    ConfigFile file_write(FILENAME());
    file_write.AddLink(ConfigPropertyLink(cfg_write.bool_1_name,&cfg_write.bool_1));
    file_write.AddLink(ConfigPropertyLink(cfg_write.int_1_name,&cfg_write.int_1));
    file_write.AddLink(ConfigPropertyLink(cfg_write.int_2_name,&cfg_write.int_2));
    file_write.AddLink(ConfigPropertyLink(cfg_write.string_1_name,&cfg_write.string_1));
    file_write.OverWriteConfig();

    CFG cfg_read;
    ConfigFile file_read(FILENAME());
    file_read.AddLink(ConfigPropertyLink(cfg_write.bool_1_name,&cfg_read.bool_1));
    file_read.AddLink(ConfigPropertyLink(cfg_write.int_1_name,&cfg_read.int_1));
    file_read.AddLink(ConfigPropertyLink(cfg_write.int_2_name,&cfg_read.int_2));
    file_read.AddLink(ConfigPropertyLink(cfg_write.string_1_name,&cfg_read.string_1));
    file_read.ReadConfig();

    ASSERT(cfg_read.bool_1.Get()==cfg_write.bool_1.Get());
    ASSERT(cfg_read.int_1.Get()==cfg_write.int_1.Get());
    ASSERT(cfg_read.int_2.Get()==cfg_write.int_2.Get());
    ASSERT(cfg_read.string_1.Get()==cfg_write.string_1.Get());

    DeleteFile();
}

void TestConfigFile::Update()
{
    DeleteFile();

    CFG cfg_write = InitCfg();
    ConfigFile file_write(FILENAME());
    file_write.AddLink(ConfigPropertyLink(cfg_write.bool_1_name,&cfg_write.bool_1));
    file_write.AddLink(ConfigPropertyLink(cfg_write.int_1_name,&cfg_write.int_1));
    file_write.AddLink(ConfigPropertyLink(cfg_write.int_2_name,&cfg_write.int_2));
    file_write.OverWriteConfig();

    file_write.AddLink(ConfigPropertyLink(cfg_write.string_1_name,&cfg_write.string_1));
    file_write.SyncToConfig();

    ASSERT(CheckDataInFile(cfg_write.int_1_name));
    ASSERT(CheckDataInFile(cfg_write.string_1_name));
    DeleteFile();
}

void TestConfigFile::ReadWriteControls()
{
    const std::string KEY = "Controls";
    const std::string VALUE1 = "\\r\\n";
    const std::string VALUE2 = "\r\n";

    PropertyReadOnly<std::string> write_container(VALUE2);
    Property<std::string> read_container("");

    DeleteFile();

    ConfigFile file_write(FILENAME());
    file_write.AddLink(ConfigPropertyLink(KEY,&write_container));
    file_write.OverWriteConfig();
    ASSERT(CheckDataInFile(VALUE1));

    ConfigFile file_read(FILENAME());
    file_read.AddLink(ConfigPropertyLink(KEY,&read_container));
    file_read.ReadConfig();

    ASSERT(read_container.Get()==VALUE2);
}
