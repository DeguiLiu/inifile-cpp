/*
 * test_convert.cpp
 *
 * Tests for ini::Convert<T> template specializations.
 */

#include "inicpp.h"

#include <cstring>

#include <catch2/catch.hpp>
#include <limits>

/***************************************************
 *          Convert<bool> Tests
 ***************************************************/

TEST_CASE("convert bool decode true variants", "Convert") {
  ini::Convert<bool> conv;
  bool result = false;

  conv.Decode("true", result);
  REQUIRE(result == true);

  conv.Decode("TRUE", result);
  REQUIRE(result == true);

  conv.Decode("True", result);
  REQUIRE(result == true);

  conv.Decode("tRuE", result);
  REQUIRE(result == true);
}

TEST_CASE("convert bool decode false variants", "Convert") {
  ini::Convert<bool> conv;
  bool result = true;

  conv.Decode("false", result);
  REQUIRE(result == false);

  conv.Decode("FALSE", result);
  REQUIRE(result == false);

  conv.Decode("False", result);
  REQUIRE(result == false);
}

#if defined(__cpp_exceptions)
TEST_CASE("convert bool decode invalid throws", "Convert") {
  ini::Convert<bool> conv;
  bool result;

  REQUIRE_THROWS_AS(conv.Decode("yes", result), std::invalid_argument);
  REQUIRE_THROWS_AS(conv.Decode("no", result), std::invalid_argument);
  REQUIRE_THROWS_AS(conv.Decode("1", result), std::invalid_argument);
  REQUIRE_THROWS_AS(conv.Decode("0", result), std::invalid_argument);
  REQUIRE_THROWS_AS(conv.Decode("", result), std::invalid_argument);
}
#endif

TEST_CASE("convert bool encode", "Convert") {
  ini::Convert<bool> conv;
  std::string result;

  conv.Encode(true, result);
  REQUIRE(result == "true");

  conv.Encode(false, result);
  REQUIRE(result == "false");
}

/***************************************************
 *          Convert<char> Tests
 ***************************************************/

TEST_CASE("convert char decode", "Convert") {
  ini::Convert<char> conv;
  char result;

  conv.Decode("a", result);
  REQUIRE(result == 'a');

  conv.Decode("Z", result);
  REQUIRE(result == 'Z');

  // multi-char string: takes first char
  conv.Decode("hello", result);
  REQUIRE(result == 'h');
}

TEST_CASE("convert char encode", "Convert") {
  ini::Convert<char> conv;
  std::string result;

  conv.Encode('x', result);
  REQUIRE(result == "x");
}

TEST_CASE("convert unsigned char decode and encode", "Convert") {
  ini::Convert<unsigned char> conv;
  unsigned char result;

  conv.Decode("A", result);
  REQUIRE(result == 'A');

  std::string encoded;
  conv.Encode('B', encoded);
  REQUIRE(encoded == "B");
}

/***************************************************
 *          Convert<int> Tests
 ***************************************************/

TEST_CASE("convert int decode decimal", "Convert") {
  ini::Convert<int> conv;
  int result;

  conv.Decode("42", result);
  REQUIRE(result == 42);

  conv.Decode("-100", result);
  REQUIRE(result == -100);

  conv.Decode("0", result);
  REQUIRE(result == 0);
}

TEST_CASE("convert int decode hex", "Convert") {
  ini::Convert<int> conv;
  int result;

  conv.Decode("0xFF", result);
  REQUIRE(result == 255);

  conv.Decode("0x1A", result);
  REQUIRE(result == 26);
}

TEST_CASE("convert int decode octal prefix", "Convert") {
  ini::Convert<int> conv;
  int result;

  // strToLong tries decimal first, so "010" parses as decimal 10
  conv.Decode("010", result);
  REQUIRE(result == 10);

  // "077" parses as decimal 77
  conv.Decode("077", result);
  REQUIRE(result == 77);
}

#if defined(__cpp_exceptions)
TEST_CASE("convert int decode invalid throws", "Convert") {
  ini::Convert<int> conv;
  int result;

  // "abc" is valid hex (0xabc=2748), so it does NOT throw
  // "12.5" fails decimal/octal/hex due to the dot
  REQUIRE_THROWS_AS(conv.Decode("12.5", result), std::invalid_argument);
  REQUIRE_THROWS_AS(conv.Decode("xyz", result), std::invalid_argument);
}
#endif

TEST_CASE("convert int encode", "Convert") {
  ini::Convert<int> conv;
  std::string result;

  conv.Encode(42, result);
  REQUIRE(result == "42");

  conv.Encode(-7, result);
  REQUIRE(result == "-7");
}

/***************************************************
 *          Convert<unsigned int> Tests
 ***************************************************/

TEST_CASE("convert unsigned int decode and encode", "Convert") {
  ini::Convert<unsigned int> conv;
  unsigned int result;

  conv.Decode("123", result);
  REQUIRE(result == 123);

  std::string encoded;
  conv.Encode(456u, encoded);
  REQUIRE(encoded == "456");
}

/***************************************************
 *          Convert<short> Tests
 ***************************************************/

TEST_CASE("convert short decode and encode", "Convert") {
  ini::Convert<short> conv;
  short result;

  conv.Decode("32000", result);
  REQUIRE(result == 32000);

  conv.Decode("-32000", result);
  REQUIRE(result == -32000);

  std::string encoded;
  conv.Encode(static_cast<short>(100), encoded);
  REQUIRE(encoded == "100");
}

TEST_CASE("convert unsigned short decode and encode", "Convert") {
  ini::Convert<unsigned short> conv;
  unsigned short result;

  conv.Decode("65000", result);
  REQUIRE(result == 65000);

  std::string encoded;
  conv.Encode(static_cast<unsigned short>(200), encoded);
  REQUIRE(encoded == "200");
}

/***************************************************
 *          Convert<long> Tests
 ***************************************************/

TEST_CASE("convert long decode and encode", "Convert") {
  ini::Convert<long> conv;
  long result;

  conv.Decode("1000000", result);
  REQUIRE(result == 1000000L);

  conv.Decode("-999999", result);
  REQUIRE(result == -999999L);

  std::string encoded;
  conv.Encode(12345L, encoded);
  REQUIRE(encoded == "12345");
}

#if defined(__cpp_exceptions)
TEST_CASE("convert long decode invalid throws", "Convert") {
  ini::Convert<long> conv;
  long result;

  REQUIRE_THROWS_AS(conv.Decode("not_a_number", result), std::invalid_argument);
}
#endif

TEST_CASE("convert unsigned long decode and encode", "Convert") {
  ini::Convert<unsigned long> conv;
  unsigned long result;

  conv.Decode("4294967295", result);
  REQUIRE(result == 4294967295UL);

  std::string encoded;
  conv.Encode(9999UL, encoded);
  REQUIRE(encoded == "9999");
}

#if defined(__cpp_exceptions)
TEST_CASE("convert unsigned long decode invalid throws", "Convert") {
  ini::Convert<unsigned long> conv;
  unsigned long result;

  REQUIRE_THROWS_AS(conv.Decode("xyz", result), std::invalid_argument);
}
#endif

/***************************************************
 *          Convert<double> Tests
 ***************************************************/

TEST_CASE("convert double decode", "Convert") {
  ini::Convert<double> conv;
  double result;

  conv.Decode("3.14159", result);
  REQUIRE(result == Approx(3.14159).margin(1e-5));

  conv.Decode("-0.001", result);
  REQUIRE(result == Approx(-0.001).margin(1e-6));

  conv.Decode("1e10", result);
  REQUIRE(result == Approx(1e10));

  conv.Decode("0", result);
  REQUIRE(result == Approx(0.0));
}

#if defined(__cpp_exceptions)
TEST_CASE("convert double decode invalid throws", "Convert") {
  ini::Convert<double> conv;
  double result;

  REQUIRE_THROWS(conv.Decode("not_a_double", result));
}
#endif

TEST_CASE("convert double encode", "Convert") {
  ini::Convert<double> conv;
  std::string result;

  conv.Encode(1.5, result);
  REQUIRE(result == "1.5");
}

/***************************************************
 *          Convert<float> Tests
 ***************************************************/

TEST_CASE("convert float decode and encode", "Convert") {
  ini::Convert<float> conv;
  float result;

  conv.Decode("2.5", result);
  REQUIRE(result == Approx(2.5f).margin(1e-3f));

  std::string encoded;
  conv.Encode(3.0f, encoded);
  REQUIRE(encoded == "3");
}

/***************************************************
 *          Convert<std::string> Tests
 ***************************************************/

TEST_CASE("convert string decode and encode", "Convert") {
  ini::Convert<std::string> conv;
  std::string result;

  conv.Decode("hello world", result);
  REQUIRE(result == "hello world");

  conv.Decode("", result);
  REQUIRE(result == "");

  std::string encoded;
  conv.Encode("test string", encoded);
  REQUIRE(encoded == "test string");
}

/***************************************************
 *          Convert<const char*> Tests
 ***************************************************/

TEST_CASE("convert const char* decode and encode", "Convert") {
  ini::Convert<const char*> conv;

  std::string encoded;
  conv.Encode("literal", encoded);
  REQUIRE(encoded == "literal");
}

#ifdef __cpp_lib_string_view
TEST_CASE("convert string_view decode and encode", "Convert") {
  ini::Convert<std::string_view> conv;
  std::string_view result;
  std::string backing = "test_value";

  conv.Decode(backing, result);
  REQUIRE(result == "test_value");

  std::string encoded;
  conv.Encode(std::string_view("view_val"), encoded);
  REQUIRE(encoded == "view_val");
}
#endif
