/*
 * test_edge_cases.cpp
 *
 * Tests for edge cases, error handling, and unusual inputs.
 */

#include "inicpp.h"
#include <catch2/catch.hpp>
#include <sstream>

/***************************************************
 *          Parsing Edge Cases
 ***************************************************/

TEST_CASE("parse field with equals sign in value", "EdgeCase")
{
    std::istringstream ss("[Foo]\nurl=http://example.com?a=1&b=2");
    ini::IniFile inif(ss);

    REQUIRE(inif["Foo"]["url"].as<std::string>() == "http://example.com?a=1&b=2");
}

TEST_CASE("parse field with only equals sign as value", "EdgeCase")
{
    std::istringstream ss("[Foo]\nsep==");
    ini::IniFile inif(ss);

    REQUIRE(inif["Foo"]["sep"].as<std::string>() == "=");
}

TEST_CASE("parse field with multiple equals signs", "EdgeCase")
{
    std::istringstream ss("[Foo]\nexpr=a=b=c=d");
    ini::IniFile inif(ss);

    REQUIRE(inif["Foo"]["expr"].as<std::string>() == "a=b=c=d");
}

TEST_CASE("parse section with trailing content after bracket", "EdgeCase")
{
    std::istringstream ss("[Foo] some trailing text\nbar=bla");
    ini::IniFile inif(ss);

    // section name should be "Foo"
    REQUIRE(inif.find("Foo") != inif.end());
}

TEST_CASE("parse empty value field", "EdgeCase")
{
    std::istringstream ss("[Sec]\nempty=\nnotempty=val");
    ini::IniFile inif(ss);

    REQUIRE(inif["Sec"]["empty"].as<std::string>() == "");
    REQUIRE(inif["Sec"]["notempty"].as<std::string>() == "val");
}

TEST_CASE("parse value with leading and trailing spaces", "EdgeCase")
{
    std::istringstream ss("[Sec]\nkey=  spaced value  ");
    ini::IniFile inif(ss);

    REQUIRE(inif["Sec"]["key"].as<std::string>() == "spaced value");
}

TEST_CASE("parse key with leading and trailing spaces", "EdgeCase")
{
    std::istringstream ss("[Sec]\n  spaced key  =value");
    ini::IniFile inif(ss);

    REQUIRE(inif["Sec"].find("spaced key") != inif["Sec"].end());
    REQUIRE(inif["Sec"]["spaced key"].as<std::string>() == "value");
}

TEST_CASE("parse section name with leading and trailing spaces", "EdgeCase")
{
    std::istringstream ss("  [MySection]  \nk=v");
    ini::IniFile inif(ss);

    REQUIRE(inif.find("MySection") != inif.end());
}

TEST_CASE("parse windows-style line endings (CRLF)", "EdgeCase")
{
    std::istringstream ss("[Foo]\r\nbar=hello\r\nbaz=world\r\n");
    ini::IniFile inif(ss);

    REQUIRE(inif.size() == 1);
    REQUIRE(inif["Foo"]["bar"].as<std::string>() == "hello");
    REQUIRE(inif["Foo"]["baz"].as<std::string>() == "world");
}

TEST_CASE("parse with blank lines between sections", "EdgeCase")
{
    std::istringstream ss("[A]\nx=1\n\n\n[B]\ny=2\n\n");
    ini::IniFile inif(ss);

    REQUIRE(inif.size() == 2);
    REQUIRE(inif["A"]["x"].as<int>() == 1);
    REQUIRE(inif["B"]["y"].as<int>() == 2);
}

TEST_CASE("parse with mixed comment styles", "EdgeCase")
{
    std::istringstream ss("[Foo]\n# hash comment\n; semicolon comment\nbar=val");
    ini::IniFile inif;
    inif.setCommentPrefixes({"#", ";"});
    inif.decode(ss);

    REQUIRE(inif["Foo"].size() == 1);
    REQUIRE(inif["Foo"]["bar"].as<std::string>() == "val");
}

TEST_CASE("numeric string as section name", "EdgeCase")
{
    std::istringstream ss("[123]\nk=v");
    ini::IniFile inif(ss);

    REQUIRE(inif.find("123") != inif.end());
}

TEST_CASE("very long field value", "EdgeCase")
{
    std::string longVal(10000, 'x');
    std::string content = "[Sec]\nlong=" + longVal;
    std::istringstream ss(content);
    ini::IniFile inif(ss);

    REQUIRE(inif["Sec"]["long"].as<std::string>() == longVal);
}

TEST_CASE("many sections", "EdgeCase")
{
    std::ostringstream builder;
    for (int i = 0; i < 100; ++i)
    {
        builder << "[Section" << i << "]\nkey" << i << "=val" << i << "\n";
    }

    ini::IniFile inif;
    inif.decode(builder.str());
    REQUIRE(inif.size() == 100);
    REQUIRE(inif["Section0"]["key0"].as<std::string>() == "val0");
    REQUIRE(inif["Section99"]["key99"].as<std::string>() == "val99");
}

/***************************************************
 *          Error Handling
 ***************************************************/

TEST_CASE("fail to parse field before any section", "EdgeCase")
{
    ini::IniFile inif;
    REQUIRE_THROWS(inif.decode("orphan=value"));
}

TEST_CASE("fail to parse unclosed section bracket", "EdgeCase")
{
    ini::IniFile inif;
    REQUIRE_THROWS(inif.decode("[Unclosed\nk=v"));
}

TEST_CASE("fail to parse empty section name", "EdgeCase")
{
    ini::IniFile inif;
    REQUIRE_THROWS(inif.decode("[]\nk=v"));
}

TEST_CASE("fail to parse line without separator in section", "EdgeCase")
{
    ini::IniFile inif;
    REQUIRE_THROWS(inif.decode("[Sec]\nno_separator_here"));
}

TEST_CASE("duplicate field throws when disallowed", "EdgeCase")
{
    ini::IniFile inif;
    inif.allowOverwriteDuplicateFields(false);
    REQUIRE_THROWS(inif.decode("[Sec]\nk=v1\nk=v2"));
}

TEST_CASE("duplicate field overwrites when allowed", "EdgeCase")
{
    ini::IniFile inif;
    inif.allowOverwriteDuplicateFields(true);
    inif.decode("[Sec]\nk=v1\nk=v2");
    REQUIRE(inif["Sec"]["k"].as<std::string>() == "v2");
}

/***************************************************
 *          Custom Separator
 ***************************************************/

TEST_CASE("custom field separator colon", "EdgeCase")
{
    std::istringstream ss("[Sec]\nkey:value");
    ini::IniFile inif;
    inif.setFieldSep(':');
    inif.decode(ss);

    REQUIRE(inif["Sec"]["key"].as<std::string>() == "value");
}

TEST_CASE("custom field separator in constructor", "EdgeCase")
{
    std::istringstream ss("[Sec]\nkey:value");
    ini::IniFile inif(':', '#');
    inif.decode(ss);

    REQUIRE(inif["Sec"]["key"].as<std::string>() == "value");
}

TEST_CASE("encode with custom field separator", "EdgeCase")
{
    ini::IniFile inif(':', '#');
    inif["S"]["k"] = "v";

    std::string result = inif.encode();
    REQUIRE(result.find("k:v") != std::string::npos);
}

/***************************************************
 *          Escape Character
 ***************************************************/

TEST_CASE("custom escape character", "EdgeCase")
{
    ini::IniFile inif;
    inif.setEscapeChar('!');
    inif.setCommentPrefixes({"#"});
    inif.decode("[Sec]\nval=hello !# world");

    REQUIRE(inif["Sec"]["val"].as<std::string>() == "hello # world");
}

/***************************************************
 *          Multi-line Values
 ***************************************************/

TEST_CASE("multi-line value with multiple continuation lines", "EdgeCase")
{
    std::istringstream ss("[Sec]\ntext=line1\n\tline2\n\tline3\n\tline4");
    ini::IniFile inif;
    inif.setMultiLineValues(true);
    inif.decode(ss);

    REQUIRE(inif["Sec"]["text"].as<std::string>() == "line1\nline2\nline3\nline4");
}

TEST_CASE("multi-line value encode produces continuation with tab", "EdgeCase")
{
    ini::IniFile inif;
    inif.setMultiLineValues(true);
    inif["Sec"]["ml"] = "first\nsecond\nthird";

    std::string result = inif.encode();
    REQUIRE(result.find("ml=first\n\tsecond\n\tthird") != std::string::npos);
}

TEST_CASE("multi-line roundtrip", "EdgeCase")
{
    ini::IniFile original;
    original.setMultiLineValues(true);
    original["Config"]["desc"] = "line1\nline2\nline3";

    std::string encoded = original.encode();

    ini::IniFile decoded;
    decoded.setMultiLineValues(true);
    decoded.decode(encoded);

    REQUIRE(decoded["Config"]["desc"].as<std::string>() == "line1\nline2\nline3");
}

/***************************************************
 *          trim() Function Tests
 ***************************************************/

TEST_CASE("trim removes tabs and newlines", "Trim")
{
    std::string s = "\t\n  hello  \n\t";
    ini::trim(s);
    REQUIRE(s == "hello");
}

TEST_CASE("trim preserves internal whitespace", "Trim")
{
    std::string s = "  hello   world  ";
    ini::trim(s);
    REQUIRE(s == "hello   world");
}

TEST_CASE("trim on already trimmed string is no-op", "Trim")
{
    std::string s = "clean";
    ini::trim(s);
    REQUIRE(s == "clean");
}

TEST_CASE("trim whitespace-only string becomes empty", "Trim")
{
    std::string s = "   \t\n\r  ";
    ini::trim(s);
    REQUIRE(s.empty());
}

/***************************************************
 *          decode(string) overload
 ***************************************************/

TEST_CASE("decode from string directly", "EdgeCase")
{
    ini::IniFile inif;
    inif.decode("[Direct]\nk=v");
    REQUIRE(inif["Direct"]["k"].as<std::string>() == "v");
}

TEST_CASE("decode clears previous content", "EdgeCase")
{
    ini::IniFile inif;
    inif.decode("[First]\na=1");
    REQUIRE(inif.size() == 1);

    inif.decode("[Second]\nb=2\n[Third]\nc=3");
    REQUIRE(inif.size() == 2);
    REQUIRE(inif.find("First") == inif.end());
}
