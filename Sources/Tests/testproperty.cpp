#include "testproperty.h"
#include "../property.h"

using namespace UnitTests;

std::vector<TestCase> TestProperty::GetTestCases()
{
    const std::string LOGMODULE="TestProperty";
    std::vector<TestCase> result;
    result.push_back(TESTCASE(base_create_get_set_string));
    result.push_back(TESTCASE(base_class_get_set_string));
    result.push_back(TESTCASE(readonly_create_get_set_string));
    result.push_back(TESTCASE(atomic_create_get_set_string));

    result.push_back(TESTCASE(base_int_get_set_string));
    result.push_back(TESTCASE(base_bool_get_set_string));
    result.push_back(TESTCASE(base_string_get_set_string));
    result.push_back(TESTCASE(base_uint64_t_get_set_string));
    result.push_back(TESTCASE(base_char_get_set_string));
    result.push_back(TESTCASE(base_double_get_set_string));
    result.push_back(TESTCASE(base_uint32_get_set_string));

    return result;
}

void TestProperty::base_create_get_set_string()
{
    const int KEY_1_INT = 42;
    const std::string KEY_1_STR = std::to_string(KEY_1_INT);
    const int KEY_2_INT = 8080;
    const std::string KEY_2_STR = std::to_string(KEY_2_INT);

    Property<int> value(KEY_1_INT);
    ASSERT(value.Get()==KEY_1_INT);

    value.Set(KEY_2_INT);
    ASSERT(value.Get()==KEY_2_INT);

    std::string str = value.GetString();
    ASSERT(str==KEY_2_STR);

    value.SetFromString(KEY_1_STR);
    ASSERT(value.Get()==KEY_1_INT);

    str = value.GetString();
    ASSERT(str==KEY_1_STR);
}

void TestProperty::base_class_get_set_string()
{
    const int KEY_1_INT = 16;
    const std::string KEY_1_STR = std::to_string(KEY_1_INT);
    const int KEY_2_INT = 7654;
    const std::string KEY_2_STR = std::to_string(KEY_2_INT);

    Property<int> value(KEY_1_INT);
    PropertyBase *ptr=&value;
    ASSERT(ptr->GetString()==KEY_1_STR);
    ptr->SetFromString(KEY_2_STR);
    ASSERT(value.Get()==KEY_2_INT);
}


void TestProperty::readonly_create_get_set_string()
{
    const int KEY_1_INT = 42;
    const std::string KEY_1_STR = std::to_string(KEY_1_INT);
    const int KEY_2_INT = 8080;
    const std::string KEY_2_STR = std::to_string(KEY_2_INT);

    PropertyReadOnly<int> value(KEY_1_INT);
    ASSERT(value.Get()==KEY_1_INT);

    ((Property<int>)value).Set(KEY_2_INT);
    ASSERT(value.Get()==KEY_1_INT);

    std::string str = value.GetString();
    ASSERT(str==KEY_1_STR);

    ((Property<int>)value).SetFromString(KEY_2_STR);
    ASSERT(value.Get()==KEY_1_INT);

    str = value.GetString();
    ASSERT(str==KEY_1_STR);
}

void TestProperty::atomic_create_get_set_string()
{
    const int KEY_1_INT = 42;
    const std::string KEY_1_STR = std::to_string(KEY_1_INT);
    const int KEY_2_INT = 8080;
    const std::string KEY_2_STR = std::to_string(KEY_2_INT);

    PropertyAtomic<int> value(KEY_1_INT);
    ASSERT(value.Get()==KEY_1_INT);

    value.Set(KEY_2_INT);
    ASSERT(value.Get()==KEY_2_INT);

    std::string str = value.GetString();
    ASSERT(str==KEY_2_STR);

    value.SetFromString(KEY_1_STR);
    ASSERT(value.Get()==KEY_1_INT);

    str = value.GetString();
    ASSERT(str==KEY_1_STR);
}

void TestProperty::base_int_get_set_string()
{
    const int DEFAULT = 0;
    const int VALUE = 42;
    const std::string STR = "42";

    Property<int> prop_from_val(DEFAULT);
    Property<int> prop_from_str(DEFAULT);

    prop_from_val.Set(VALUE);
    prop_from_str.SetFromString(STR);

    ASSERT(prop_from_val.Get()==VALUE);
    ASSERT(prop_from_val.GetString()==STR);
    ASSERT(prop_from_str.Get()==VALUE);
    ASSERT(prop_from_str.GetString()==STR);
}

void TestProperty::base_bool_get_set_string()
{
    std::string tmp ="";
    const std::string KEY_1_FALSE = "false";
    const std::string KEY_2_FALSE = "FaLsE";
    const std::string KEY_3_TRUE = "true";
    const std::string KEY_4_TRUE = "TrUe";
    const std::string KEY_5_TRUE = "anything";

    Property<bool> value(true);
    ASSERT(value.Get());
    value.Set(false);
    ASSERT(!value.Get());
    value.Set(true);
    ASSERT(value.Get());

    value.SetFromString(KEY_1_FALSE);
    ASSERT(!value.Get());

    value.Set(true);
    value.SetFromString(KEY_2_FALSE);
    ASSERT(!value.Get());

    value.Set(false);
    value.SetFromString(KEY_3_TRUE);
    ASSERT(value.Get());

    value.Set(false);
    value.SetFromString(KEY_4_TRUE);
    ASSERT(value.Get());

    value.Set(false);
    value.SetFromString(KEY_5_TRUE);
    ASSERT(value.Get());
}

void TestProperty::base_string_get_set_string()
{
    std::string tmp ="";
    const std::string KEY_1 = "Value";
    const std::string KEY_2 = "Changed";

    Property<std::string> value(KEY_1);
    ASSERT(value.Get()==KEY_1);
    tmp = value.GetString();
    ASSERT(tmp==KEY_1);

    value.Set(KEY_2);
    ASSERT(value.Get()==KEY_2);
    tmp = value.GetString();
    ASSERT(tmp==KEY_2);

    value.SetFromString(KEY_1);
    ASSERT(value.Get()==KEY_1);
    tmp = value.GetString();
    ASSERT(tmp==KEY_1);
}

void TestProperty::base_uint64_t_get_set_string()
{
    const uint64_t DEFAULT = 0;
    const uint64_t VALUE =  0xFFEEDDCCBBAA9988;
    const std::string STR = "18441921395520346504";

    Property<uint64_t> prop_from_val(DEFAULT);
    Property<uint64_t> prop_from_str(DEFAULT);

    prop_from_val.Set(VALUE);
    prop_from_str.SetFromString(STR);

    ASSERT(prop_from_val.Get()==VALUE);
    ASSERT(prop_from_val.GetString()==STR);
    ASSERT(prop_from_str.Get()==VALUE);
    ASSERT(prop_from_str.GetString()==STR);
}

void TestProperty::base_char_get_set_string()
{
    const char DEFAULT = 0;
    const char VALUE =  'a';
    const std::string STR = "a";

    Property<char> prop_from_val(DEFAULT);
    Property<char> prop_from_str(DEFAULT);

    prop_from_val.Set(VALUE);
    prop_from_str.SetFromString(STR);

    ASSERT(prop_from_val.Get()==VALUE);
    ASSERT(prop_from_val.GetString()==STR);
    ASSERT(prop_from_str.Get()==VALUE);
    ASSERT(prop_from_str.GetString()==STR);
}

void TestProperty::base_double_get_set_string()
{
    const double DEFAULT = 0;
    const double VALUE =  0.756550;
    const std::string STR = "0.756550";

    Property<double> prop_from_val(DEFAULT);
    Property<double> prop_from_str(DEFAULT);

    prop_from_val.Set(VALUE);
    prop_from_str.SetFromString(STR);

    ASSERT(prop_from_val.Get()==VALUE);
    ASSERT(prop_from_val.GetString()==STR);
    ASSERT(prop_from_str.Get()==VALUE);
    ASSERT(prop_from_str.GetString()==STR);
}

void TestProperty::base_uint32_get_set_string()
{
    const uint32_t DEFAULT = 0;
    const uint32_t VALUE =  0xFFEEDDCC;
    const std::string STR = "4293844428";

    Property<uint32_t> prop_from_val(DEFAULT);
    Property<uint32_t> prop_from_str(DEFAULT);

    prop_from_val.Set(VALUE);
    prop_from_str.SetFromString(STR);

    ASSERT(prop_from_val.Get()==VALUE);
    ASSERT(prop_from_val.GetString()==STR);
    ASSERT(prop_from_str.Get()==VALUE);
    ASSERT(prop_from_str.GetString()==STR);
}
