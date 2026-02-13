/*
 * test_file_io.cpp
 *
 * Tests for file load/save operations.
 */

#include "inicpp.h"
#include <catch2/catch.hpp>
#include <cstdio>
#include <fstream>
#include <string>

static const char *kTempFile = "test_inifile_tmp.ini";

static void writeTempFile(const std::string &content)
{
    std::ofstream ofs(kTempFile);
    ofs << content;
}

static void removeTempFile()
{
    std::remove(kTempFile);
}

TEST_CASE("load ini file from disk", "FileIO")
{
    writeTempFile("[Server]\nhost=localhost\nport=9090\n");

    ini::IniFile inif;
    inif.load(kTempFile);

    REQUIRE(inif.size() == 1);
    REQUIRE(inif["Server"]["host"].as<std::string>() == "localhost");
    REQUIRE(inif["Server"]["port"].as<int>() == 9090);

    removeTempFile();
}

TEST_CASE("save ini file to disk", "FileIO")
{
    ini::IniFile inif;
    inif["App"]["name"] = "test_app";
    inif["App"]["version"] = 2;
    inif.save(kTempFile);

    // reload and verify
    ini::IniFile loaded;
    loaded.load(kTempFile);
    REQUIRE(loaded["App"]["name"].as<std::string>() == "test_app");
    REQUIRE(loaded["App"]["version"].as<int>() == 2);

    removeTempFile();
}

TEST_CASE("save and load roundtrip with multiple sections", "FileIO")
{
    ini::IniFile inif;
    inif["DB"]["host"] = "db.example.com";
    inif["DB"]["port"] = 3306;
    inif["DB"]["user"] = "admin";
    inif["Cache"]["enabled"] = true;
    inif["Cache"]["ttl"] = 300;
    inif.save(kTempFile);

    ini::IniFile loaded;
    loaded.load(kTempFile);

    REQUIRE(loaded["DB"]["host"].as<std::string>() == "db.example.com");
    REQUIRE(loaded["DB"]["port"].as<int>() == 3306);
    REQUIRE(loaded["DB"]["user"].as<std::string>() == "admin");
    REQUIRE(loaded["Cache"]["enabled"].as<bool>() == true);
    REQUIRE(loaded["Cache"]["ttl"].as<int>() == 300);

    removeTempFile();
}

TEST_CASE("load from file constructor", "FileIO")
{
    writeTempFile("[Test]\nval=42\n");

    ini::IniFile inif(kTempFile);
    REQUIRE(inif["Test"]["val"].as<int>() == 42);

    removeTempFile();
}

TEST_CASE("load empty file", "FileIO")
{
    writeTempFile("");

    ini::IniFile inif;
    inif.load(kTempFile);
    REQUIRE(inif.size() == 0);

    removeTempFile();
}

TEST_CASE("load file with only comments", "FileIO")
{
    writeTempFile("# this is a comment\n; another comment\n# more comments\n");

    ini::IniFile inif;
    inif.load(kTempFile);
    REQUIRE(inif.size() == 0);

    removeTempFile();
}

TEST_CASE("load file with BOM-like leading whitespace", "FileIO")
{
    writeTempFile("  [Section]\n  key=value\n");

    ini::IniFile inif;
    inif.load(kTempFile);
    REQUIRE(inif.find("Section") != inif.end());
    REQUIRE(inif["Section"]["key"].as<std::string>() == "value");

    removeTempFile();
}

TEST_CASE("save overwrites existing file", "FileIO")
{
    // first write
    {
        ini::IniFile inif;
        inif["Old"]["data"] = "old_value";
        inif.save(kTempFile);
    }

    // second write (overwrite)
    {
        ini::IniFile inif;
        inif["New"]["data"] = "new_value";
        inif.save(kTempFile);
    }

    // verify only new content exists
    ini::IniFile loaded;
    loaded.load(kTempFile);
    REQUIRE(loaded.find("Old") == loaded.end());
    REQUIRE(loaded["New"]["data"].as<std::string>() == "new_value");

    removeTempFile();
}

TEST_CASE("load multiple times clears previous content", "FileIO")
{
    writeTempFile("[A]\nx=1\n");
    ini::IniFile inif;
    inif.load(kTempFile);
    REQUIRE(inif.size() == 1);

    writeTempFile("[B]\ny=2\n[C]\nz=3\n");
    inif.load(kTempFile);
    REQUIRE(inif.size() == 2);
    REQUIRE(inif.find("A") == inif.end());
    REQUIRE(inif["B"]["y"].as<int>() == 2);

    removeTempFile();
}
