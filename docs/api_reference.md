# inifile-cpp API Reference

## Overview

`inifile-cpp` is a single header-only C++ library for reading and writing INI configuration files. It supports C++11 and later. The entire API lives in the `ini` namespace.

## Core Types

### `ini::IniFile`

The main class for parsing and generating INI files. Inherits from `std::map<std::string, IniSection>`.

**Type aliases:**

| Type | Description |
|------|-------------|
| `ini::IniFile` | Case-sensitive INI file (default) |
| `ini::IniFileCaseInsensitive` | Case-insensitive section and field names |
| `ini::IniSection` | Case-sensitive section (`std::map<std::string, IniField>`) |
| `ini::IniSectionCaseInsensitive` | Case-insensitive section |

### Constructors

```cpp
IniFile();                                   // Default
IniFile(const std::string &filename);        // Load from file
IniFile(std::istream &is);                   // Decode from stream
IniFile(char fieldSep, char comment);        // Custom separators
IniFile(const std::string &filename,
        char fieldSep,
        const std::vector<std::string> &commentPrefixes);
IniFile(std::istream &is,
        char fieldSep,
        const std::vector<std::string> &commentPrefixes);
```

### File Operations

| Method | Description |
|--------|-------------|
| `void Load(const std::string &fileName)` | Load and parse an INI file from disk |
| `void Save(const std::string &fileName) const` | Write the INI file to disk |

### Stream Operations

| Method | Description |
|--------|-------------|
| `void Decode(std::istream &is)` | Parse from an input stream |
| `void Decode(const std::string &content)` | Parse from a string |
| `void Encode(std::ostream &os) const` | Write to an output stream |
| `std::string Encode() const` | Encode to a string |

### Configuration

| Method | Default | Description |
|--------|---------|-------------|
| `void SetFieldSep(char sep)` | `'='` | Set the key-value separator character |
| `void SetCommentChar(char comment)` | `'#'` | Set the comment prefix character |
| `void SetCommentPrefixes(const std::vector<std::string> &prefixes)` | `{"#", ";"}` | Set multiple comment prefix strings |
| `void SetEscapeChar(char esc)` | `'\\'` | Set the escape character |
| `void SetMultiLineValues(bool enable)` | `false` | Enable multi-line value parsing |
| `void AllowOverwriteDuplicateFields(bool allowed)` | `true` | Allow or reject duplicate fields |

### Data Access

Since `IniFile` inherits from `std::map`, all standard map operations are available:

```cpp
ini::IniFile inif;
inif.Load("config.ini");

// Access section and field
std::string val = inif["Section"]["key"].As<std::string>();

// Check existence
if (inif.find("Section") != inif.end()) { ... }

// Iterate
for (const auto &[secName, section] : inif) {
    for (const auto &[key, field] : section) {
        std::cout << key << " = " << field.As<std::string>() << "\n";
    }
}
```

---

### `ini::IniField`

Represents a single field value. Supports type-safe reading and writing.

| Method | Description |
|--------|-------------|
| `template<typename T> T As() const` | Convert and return value as type T |
| `template<typename T> IniField& operator=(const T &value)` | Assign a value of type T |

### Supported Types

The following types can be used with `As<T>()` and `operator=`:

| Type | Read (`As<T>()`) | Write (`operator=`) |
|------|:-:|:-:|
| `bool` | Yes | Yes |
| `char` | Yes | Yes |
| `unsigned char` | Yes | Yes |
| `short` | Yes | Yes |
| `unsigned short` | Yes | Yes |
| `int` | Yes | Yes |
| `unsigned int` | Yes | Yes |
| `long` | Yes | Yes |
| `unsigned long` | Yes | Yes |
| `float` | Yes | Yes |
| `double` | Yes | Yes |
| `std::string` | Yes | Yes |
| `const char*` | Yes | Yes |
| `char*` | No | Yes |
| `char[N]` | No | Yes |
| `std::string_view` | Yes (C++17) | Yes (C++17) |

Integer types support decimal, octal (prefix `0`), and hexadecimal (prefix `0x`) formats.

---

### `ini::Convert<T>`

Template functor for custom type conversions. Specialize this struct to add support for your own types.

```cpp
namespace ini {
    template<>
    struct Convert<MyType> {
        void Decode(const std::string &value, MyType &result) {
            // parse value into result
        }
        void Encode(const MyType &value, std::string &result) {
            // serialize value into result
        }
    };
}
```

---

## INI File Format

```ini
# Comment line
; Also a comment

[SectionName]
key = value
another_key = another value

[AnotherSection]
numeric = 42
boolean = true
```

### Multi-line Values

When enabled via `SetMultiLineValues(true)`, values can span multiple lines. Continuation lines must be indented:

```ini
[Section]
description = first line
    second line
    third line
```

This produces the value `"first line\nsecond line\nthird line"`.

### Escaped Comments

Comment prefixes within values can be escaped using the escape character (default `\`):

```ini
[Section]
color = red \# not a comment
```

This produces the value `"red # not a comment"`.

---

## Helper Functions

| Function | Description |
|----------|-------------|
| `void ini::Trim(std::string &str)` | Trim whitespace from both ends (in-place) |

---

## Build Integration

### CMake (subdirectory)

```cmake
add_subdirectory(path/to/inifile-cpp)
target_link_libraries(your_target PRIVATE inicpp::inicpp)
```

### CMake (install)

```sh
cd inifile-cpp && mkdir build && cd build
cmake .. && make install
```

Then in your project:

```cmake
find_package(inicpp REQUIRED)
target_link_libraries(your_target PRIVATE inicpp::inicpp)
```

### Header-only

Copy `include/inicpp.h` into your project and include it directly:

```cpp
#include "inicpp.h"
```
