# inifile-cpp 设计文档

## 概述

`inifile-cpp` 是一个轻量级、仅头文件（header-only）的 C++ INI 文件解析器，支持 C++11 及更高版本。整个库由单个头文件 `inicpp.h` 组成，无外部依赖。

## 架构设计

### 类层次结构

```
std::map<string, IniSection>
    └── IniFileBase<Comparator>
            ├── IniFile                 (大小写敏感)
            └── IniFileCaseInsensitive  (大小写不敏感)

std::map<string, IniField>
    └── IniSectionBase<Comparator>
            ├── IniSection              (大小写敏感)
            └── IniSectionCaseInsensitive

IniField
    └── 内部存储 std::string，通过 Convert<T> 进行类型转换
```

### 核心组件

```
+------------------+
|   IniFileBase    |  继承 std::map<string, IniSection>
|                  |  负责 INI 文件的编解码
+--------+---------+
         |
         | 包含
         v
+------------------+
|  IniSectionBase  |  继承 std::map<string, IniField>
|                  |  表示一个 [Section]
+--------+---------+
         |
         | 包含
         v
+------------------+
|    IniField      |  封装字段值（std::string）
|                  |  通过 Convert<T> 提供类型转换
+--------+---------+
         |
         | 使用
         v
+------------------+
|   Convert<T>     |  模板仿函数
|                  |  Decode(): string -> T
|                  |  Encode(): T -> string
+------------------+
```

### 数据流

```
           Load()/Decode()                    Save()/Encode()
文件/流 ─────────────────> IniFileBase ────────────────────> 文件/流
                                │
                    ┌───────────┼───────────┐
                    v           v           v
               [Section1]  [Section2]  [Section3]
                    │           │           │
                ┌───┼───┐   ┌──┼──┐     ┌──┼──┐
                v   v   v   v  v  v     v  v  v
              key1 key2 .. key1 ..    key1 key2 ..
              (IniField)   (IniField)  (IniField)
```

## 解析流程

### Decode() 状态机

```
START ──┬── 空行/注释行 ──> 跳过
        │
        ├── '[' 开头 ──> 解析 Section 名称 ──> 设置 currentSection
        │
        └── 其他 ──┬── currentSection == null ──> 抛异常
                   │
                   ├── multiLine && 有缩进 && 有前序字段 ──> 追加多行值
                   │
                   ├── 找到 fieldSep ──> 解析 key=value
                   │       │
                   │       └── 检查重复字段 ──> overwrite 或抛异常
                   │
                   └── 无 fieldSep ──> 抛异常
```

### 注释处理

- 支持多种注释前缀（默认 `#` 和 `;`）
- 转义字符（默认 `\`）可转义注释前缀
- 注释前缀匹配采用前缀搜索，支持多字符前缀（如 `REM`、`//`）

### 类型转换机制

`Convert<T>` 模板特化提供双向转换：

| 方法 | 功能 |
|------|------|
| `Decode(const string&, T&)` | 字符串 -> 目标类型 |
| `Encode(const T&, string&)` | 目标类型 -> 字符串 |

整数类型支持十进制、八进制（`0` 前缀）、十六进制（`0x` 前缀）三种格式。

用户可通过特化 `ini::Convert<T>` 扩展自定义类型支持，如 `std::vector<T>`。

## 设计决策

### 继承 std::map

`IniFileBase` 和 `IniSectionBase` 直接继承 `std::map`，允许用户使用所有标准容器操作（`[]`、`find`、`begin/end`、`size` 等）。

### 模板化大小写策略

通过 `Comparator` 模板参数（`std::less<string>` 或 `StringInsensitiveLess`）实现大小写敏感/不敏感两种模式，无运行时开销。

### 值类型统一存储

所有值以 `std::string` 内部存储，读取时通过 `Convert<T>` 按需转换。这简化了存储模型，代价是每次读取需要一次转换。

### 头文件实现

整个库在 `inicpp.h` 中实现，方便集成。使用 `inline` 修饰非模板函数避免 ODR 违规。

## 文件格式规范

```
# 注释行
; 也是注释

[Section]        ; Section 名称在方括号内
key = value      ; key-value 对，用 = 分隔
key2=value2      ; = 两侧空格可选

[Section2]
multi = line1    ; 多行值需启用 SetMultiLineValues(true)
    line2        ; 续行需要缩进（空格或 Tab）
    line3

escaped = hello \# world  ; 转义注释字符
```

## 构建与测试

```sh
mkdir build && cd build
cmake .. -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DINICPP_CXX_STANDARD=17
cmake --build .
ctest --output-on-failure
```

### CMake 选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| `INICPP_CXX_STANDARD` | `11` | C++ 标准版本 |
| `BUILD_TESTS` | `OFF` | 构建单元测试 |
| `BUILD_EXAMPLES` | `OFF` | 构建示例程序 |
| `GENERATE_COVERAGE` | `OFF` | 生成代码覆盖率 |
