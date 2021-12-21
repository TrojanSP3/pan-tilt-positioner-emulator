#ifndef TESTPROPERTY_H
#define TESTPROPERTY_H

#include "Framework/testframework.h"

namespace UnitTests
{

    class TestProperty: TestClass
    {
    public:
        static std::vector<TestCase> GetTestCases();
    private:
        static void base_create_get_set_string();
        static void base_class_get_set_string();
        static void readonly_create_get_set_string();
        static void atomic_create_get_set_string();

        static void base_int_get_set_string();
        static void base_bool_get_set_string();
        static void base_string_get_set_string();
        static void base_uint64_t_get_set_string();
        static void base_char_get_set_string();
        static void base_double_get_set_string();
        static void base_uint32_get_set_string();
    };

}
#endif // TESTPROPERTY_H
