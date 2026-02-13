# inifile-cpp

![License](https://img.shields.io/badge/license-MIT-blue.svg)
[![CMake](https://github.com/DeguiLiu/inifile-cpp/actions/workflows/cmake.yml/badge.svg)](https://github.com/DeguiLiu/inifile-cpp/actions/workflows/cmake.yml)

[English](README.md)

`inifile-cpp` 是一个简单易用的 C++ 仅头文件（header-only）INI 文件编解码器。

## 安装

通过 CMake 构建系统安装头文件：

```sh
cd <path-to-repo>
mkdir build
cd build
cmake ..
make install
```

或者直接将头文件复制到你的项目中使用。

## 使用方法

更多使用示例请参考 `examples/` 目录。

### 从流中加载

`inifile-cpp` 支持从任何 `std::istream` 加载数据，只需一次函数调用或使用重载构造函数：

```cpp
#include <inicpp.h>

int main()
{
    // 创建 istream 对象 "is" ...

    // 使用函数
    ini::IniFile myIni;
    myIni.decode(is);

    // 或使用构造函数
    ini::IniFile myIni2(is);
}
```

### 从文件加载

使用 `load()` 函数直接从文件加载 INI 数据：

```cpp
#include <inicpp.h>

int main()
{
    ini::IniFile myIni;
    myIni.load("config.ini");
}
```

### 多行值

启用 `setMultiLineValues(true)` 后可以解析多行值。续行需要缩进，最终值中各行以 `\n` 分隔：

```cpp
#include <inicpp.h>

int main()
{
    ini::IniFile myIni;
    myIni.setMultiLineValues(true);
    myIni.load("config.ini");
}
```

### 重复字段处理

默认情况下重复字段会覆盖之前的值。使用 `allowOverwriteDuplicateFields(false)` 可以禁止重复字段，遇到重复时抛出异常：

```cpp
#include <inicpp.h>

int main()
{
    ini::IniFile myIni;
    myIni.allowOverwriteDuplicateFields(false);
    // 如果 INI 文件中有重复字段则抛出异常
    myIni.load("config.ini");
}
```

### 读取值

使用下标运算符 `[]` 访问 Section 和字段，通过 `as<T>()` 进行类型转换：

```cpp
bool myBool = myIni["Foo"]["myBool"].as<bool>();
char myChar = myIni["Foo"]["myChar"].as<char>();
int myInt = myIni["Foo"]["myInt"].as<int>();
unsigned int myUInt = myIni["Foo"]["myUInt"].as<unsigned int>();
long myLong = myIni["Foo"]["myLong"].as<long>();
float myFloat = myIni["Foo"]["myFloat"].as<float>();
double myDouble = myIni["Foo"]["myDouble"].as<double>();
std::string myStr = myIni["Foo"]["myStr"].as<std::string>();
```

原生支持的类型：

* `bool`
* `char` / `unsigned char`
* `short` / `unsigned short`
* `int` / `unsigned int`
* `long` / `unsigned long`
* `float`
* `double`
* `std::string`
* `const char *`
* `std::string_view`（C++17）

### 写入值

使用赋值运算符写入值，然后编码输出：

```cpp
#include <inicpp.h>

int main()
{
    ini::IniFile myIni;

    myIni["Foo"]["myInt"] = 1;
    myIni["Foo"]["myStr"] = "Hello world";
    myIni["Foo"]["myBool"] = true;
    myIni["Bar"]["myDouble"] = 1.2;

    myIni.save("output.ini");
}
```

### 自定义类型转换

通过特化 `ini::Convert<T>` 模板可以添加自定义类型支持。例如支持 `std::vector`：

```cpp
namespace ini
{
    template<typename T>
    struct Convert<std::vector<T>>
    {
        void decode(const std::string &value, std::vector<T> &result)
        {
            result.clear();
            T decoded;
            size_t startPos = 0;
            size_t endPos = 0;

            while (endPos != std::string::npos)
            {
                if (endPos != 0)
                    startPos = endPos + 1;
                endPos = value.find(',', startPos);
                size_t cnt = (endPos == std::string::npos)
                    ? value.size() - startPos
                    : endPos - startPos;
                Convert<T> conv;
                conv.decode(value.substr(startPos, cnt), decoded);
                result.push_back(decoded);
            }
        }

        void encode(const std::vector<T> &value, std::string &result)
        {
            std::stringstream ss;
            for (size_t i = 0; i < value.size(); ++i)
            {
                std::string encoded;
                Convert<T> conv;
                conv.encode(value[i], encoded);
                ss << encoded;
                if (i != value.size() - 1)
                    ss << ',';
            }
            result = ss.str();
        }
    };
}
```

### 大小写不敏感模式

使用 `ini::IniFileCaseInsensitive` 可以忽略 Section 和字段名称的大小写：

```cpp
ini::IniFileCaseInsensitive inif;
inif.load("config.ini");

// 以下访问等价
auto val1 = inif["SECTION"]["KEY"].as<std::string>();
auto val2 = inif["section"]["key"].as<std::string>();
```

## 文档

* [API 参考](docs/api_reference.md)
* [设计文档](docs/design_zh.md)

## 构建与测试

```sh
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DINICPP_CXX_STANDARD=17
cmake --build .
ctest --output-on-failure
```

## 贡献

欢迎提交 Pull Request 来贡献新功能或修复 Bug。请确保所有 CI 检查通过。

## 许可证

`inifile-cpp` 使用 [MIT 许可证](LICENSE.txt)。
