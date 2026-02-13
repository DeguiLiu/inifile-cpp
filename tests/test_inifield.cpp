/*
 * test_inifield.cpp
 *
 * Tests for ini::IniField class.
 */

#include "inicpp.h"
#include <catch2/catch.hpp>

TEST_CASE("IniField default constructor creates empty field", "IniField")
{
    ini::IniField field;
    REQUIRE(field.as<std::string>() == "");
}

TEST_CASE("IniField string constructor", "IniField")
{
    ini::IniField field("hello");
    REQUIRE(field.as<std::string>() == "hello");
}

TEST_CASE("IniField copy constructor", "IniField")
{
    ini::IniField original("world");
    ini::IniField copy(original);
    REQUIRE(copy.as<std::string>() == "world");
}

TEST_CASE("IniField assignment from various types", "IniField")
{
    ini::IniField field;

    field = 42;
    REQUIRE(field.as<int>() == 42);

    field = true;
    REQUIRE(field.as<bool>() == true);

    field = 3.14;
    REQUIRE(field.as<double>() == Approx(3.14).margin(1e-3));

    field = std::string("test");
    REQUIRE(field.as<std::string>() == "test");

    field = "literal";
    REQUIRE(field.as<std::string>() == "literal");
}

TEST_CASE("IniField copy assignment", "IniField")
{
    ini::IniField a("source");
    ini::IniField b;
    b = a;
    REQUIRE(b.as<std::string>() == "source");
}

TEST_CASE("IniField as<T> type conversion", "IniField")
{
    ini::IniField intField("123");
    REQUIRE(intField.as<int>() == 123);
    REQUIRE(intField.as<long>() == 123L);
    REQUIRE(intField.as<unsigned int>() == 123u);
    REQUIRE(intField.as<double>() == Approx(123.0));
    REQUIRE(intField.as<float>() == Approx(123.0f));

    ini::IniField boolField("true");
    REQUIRE(boolField.as<bool>() == true);

    ini::IniField charField("x");
    REQUIRE(charField.as<char>() == 'x');
}

TEST_CASE("IniField as<T> throws on invalid conversion", "IniField")
{
    ini::IniField field("not_a_number");
    REQUIRE_THROWS(field.as<int>());
    REQUIRE_THROWS(field.as<double>());
    REQUIRE_THROWS(field.as<bool>());
}

TEST_CASE("IniField overwrite preserves latest value", "IniField")
{
    ini::IniField field;
    field = 10;
    REQUIRE(field.as<int>() == 10);

    field = 20;
    REQUIRE(field.as<int>() == 20);

    field = "now a string";
    REQUIRE(field.as<std::string>() == "now a string");
}
