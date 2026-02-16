/*
 * test_inifield.cpp
 *
 * Tests for ini::IniField class.
 */

#include "inicpp.h"

#include <catch2/catch.hpp>

TEST_CASE("IniField default constructor creates empty field", "IniField") {
  ini::IniField field;
  REQUIRE(field.As<std::string>() == "");
}

TEST_CASE("IniField string constructor", "IniField") {
  ini::IniField field("hello");
  REQUIRE(field.As<std::string>() == "hello");
}

TEST_CASE("IniField copy constructor", "IniField") {
  ini::IniField original("world");
  ini::IniField copy(original);
  REQUIRE(copy.As<std::string>() == "world");
}

TEST_CASE("IniField assignment from various types", "IniField") {
  ini::IniField field;

  field = 42;
  REQUIRE(field.As<int>() == 42);

  field = true;
  REQUIRE(field.As<bool>() == true);

  field = 3.14;
  REQUIRE(field.As<double>() == Approx(3.14).margin(1e-3));

  field = std::string("test");
  REQUIRE(field.As<std::string>() == "test");

  field = "literal";
  REQUIRE(field.As<std::string>() == "literal");
}

TEST_CASE("IniField copy assignment", "IniField") {
  ini::IniField a("source");
  ini::IniField b;
  b = a;
  REQUIRE(b.As<std::string>() == "source");
}

TEST_CASE("IniField as<T> type conversion", "IniField") {
  ini::IniField intField("123");
  REQUIRE(intField.As<int>() == 123);
  REQUIRE(intField.As<long>() == 123L);
  REQUIRE(intField.As<unsigned int>() == 123u);
  REQUIRE(intField.As<double>() == Approx(123.0));
  REQUIRE(intField.As<float>() == Approx(123.0f));

  ini::IniField boolField("true");
  REQUIRE(boolField.As<bool>() == true);

  ini::IniField charField("x");
  REQUIRE(charField.As<char>() == 'x');
}

#if defined(__cpp_exceptions)
TEST_CASE("IniField as<T> throws on invalid conversion", "IniField") {
  ini::IniField field("not_a_number");
  REQUIRE_THROWS(field.As<int>());
  REQUIRE_THROWS(field.As<double>());
  REQUIRE_THROWS(field.As<bool>());
}
#endif

TEST_CASE("IniField overwrite preserves latest value", "IniField") {
  ini::IniField field;
  field = 10;
  REQUIRE(field.As<int>() == 10);

  field = 20;
  REQUIRE(field.As<int>() == 20);

  field = "now a string";
  REQUIRE(field.As<std::string>() == "now a string");
}
