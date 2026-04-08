# CMake 编码规范

> 基于 [Modern CMake](https://cliutils.gitlab.io/modern-cmake/chapters/intro/dodonot.html) 与 [Effective Modern CMake](https://gist.github.com/mbinna/c61dbb39bca0e4fb7d1f73b0d66a4fd1) 整理。

---

## 1. 版本要求

```cmake
cmake_minimum_required(VERSION 3.29)
```

- 所有 CMakeLists.txt 和 `.cmake` 文件必须使用 3.29。
- 禁止使用低于 3.29 的版本号。
- 最低版本不得早于所支持的最旧编译器版本。

---

## 2. 项目声明

```cmake
project(MyProject
    VERSION 1.0.0
    LANGUAGES CXX
)
```

- `project()` 必须声明 `VERSION` 和 `LANGUAGES`。
- 只允许在顶层 `CMakeLists.txt` 中调用 `project()`。

---

## 3. 目标优先原则

**将目标视为对象：**

| 操作     | CMake 命令                                                                                                                                                                              |
| -------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 构造     | `add_library()` / `add_executable()`                                                                                                                                                |
| 设置属性 | `target_compile_definitions()` / `target_compile_options()` / `target_compile_features()` / `target_include_directories()` / `target_link_libraries()` / `target_sources()` |
| 读取属性 | `get_target_property()`                                                                                                                                                               |

- 所有构建属性必须挂载到具体目标上。
- 禁止使用目录级命令污染全局状态（见第 4 节）。
- 为相关联的功能创建 `IMPORTED INTERFACE` 目标，而非散落的变量。

---

## 4. 禁止使用的命令

以下命令作用于目录树，会产生隐式依赖，**严禁使用**：

| 禁止命令                      | 替代方案                         |
| ----------------------------- | -------------------------------- |
| `include_directories()`     | `target_include_directories()` |
| `link_directories()`        | `target_link_libraries()`      |
| `link_libraries()`          | `target_link_libraries()`      |
| `add_compile_options()`     | `target_compile_options()`     |
| `add_compile_definitions()` | `target_compile_definitions()` |
| `add_definitions()`         | `target_compile_definitions()` |

**禁止直接修改编译器标志变量：**

```cmake
# 错误
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# 正确
target_compile_features(MyTarget PRIVATE cxx_std_17)
```

---

## 5. 源文件管理

### 5.1 禁止使用 GLOB

```cmake
# 错误：新增或删除文件不会触发重新生成
file(GLOB SOURCES "src/*.cpp")

# 正确：逐一显式列举
add_library(MyLibrary
    src/foo.cpp
    src/bar.cpp
)
```

---

## 6. 可见性关键字

所有 `target_*` 命令必须显式指定可见性，**禁止省略**：

| 关键字        | 含义                                           |
| ------------- | ---------------------------------------------- |
| `PRIVATE`   | 仅本目标使用，不传播给依赖者                   |
| `PUBLIC`    | 本目标使用，同时传播给依赖者                   |
| `INTERFACE` | 本目标不使用，只传播给依赖者（适用于头文件库） |

```cmake
target_include_directories(MyLibrary
    PUBLIC  include          # 消费者可见
    PRIVATE src/internal     # 仅内部使用
)

target_link_libraries(MyApp
    PRIVATE MyLibrary        # MyApp 依赖，但不传播
)
```

---

## 7. 编译定义

```cmake
# 正确
target_compile_definitions(MyTarget
    PRIVATE
        MY_INTERNAL_FLAG
    PUBLIC
        MY_PUBLIC_DEFINE=1
)
```

- 使用 `target_compile_definitions()`，禁止 `add_definitions()`。
- 按配置区分时使用生成器表达式：

```cmake
target_compile_definitions(MyTarget PRIVATE
    $<IF:$<CONFIG:Debug>,MY_DEBUG,MY_RELEASE>
)
```

---

## 8. 编译选项

```cmake
target_compile_options(MyTarget PRIVATE /W4)
```

- 使用 `target_compile_options()`，禁止全局 `add_compile_options()`。
- 警告选项（`-Wall`、`/W4` 等）只能放 `PRIVATE`，**禁止放 PUBLIC/INTERFACE**。
- **禁止使用 `-Werror` / `/WX`**（参见第 15 节）。
- 影响 ABI 的选项（如 `/MD` vs `/MT`、`-fPIC`）不得放入 `target_compile_options()`，应全局或通过 toolchain 设置。

---

## 9. 头文件路径

优先使用 target_sources  中FILE_SET 的形式。

---

## 10. 依赖链接

```cmake
target_link_libraries(MyApp
    PRIVATE
        MyLibrary
        OpenSSL::SSL        # 使用 imported target，而非变量
)
```

- 始终指定 PUBLIC/PRIVATE/INTERFACE。
- 使用 `find_package` 导出的 imported targets（命名空间形式 `Foo::Bar`），**禁止使用旧式变量** `${Foo_LIBRARIES}`。
- **禁止链接构建产物路径**（如 `/path/to/libfoo.a`）。

---

## 11. 语言标准

```cmake
# 正确：声明所需特性，由 CMake 推导标准
target_compile_features(MyTarget PRIVATE cxx_std_17)

# 错误：硬编码标准变量
set(CMAKE_CXX_STANDARD 17)
```

- 让 CMake 根据 `target_compile_features()` 自动决定编译器标志。

---

## 12. 外部依赖查找

```cmake
find_package(OpenSSL 3.0 REQUIRED)

target_link_libraries(MyTarget PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

- 优先使用支持导出目标的 find 模块（CMake 3.4+ 内置模块均支持）。
- 找不到依赖时必须提供清晰的 `FATAL_ERROR` 信息。
- 对于没有 CMake 支持的第三方库，编写自定义 Find 模块并正确导出目标。

---

## 13. ALIAS 目标

```cmake
add_library(Foo::Bar ALIAS Bar)
```

- 所有对外暴露的库必须提供命名空间别名。
- 确保 `add_subdirectory()` 和 `find_package()` 两种使用方式下，消费者代码完全一致。

---

## 14. 函数与宏

```cmake
# 优先使用 function（有独立作用域）
function(my_setup_target target)
    target_compile_options(${target} PRIVATE /W3)
endfunction()

# macro 仅用于极小片段或需要修改调用方变量的情形
macro(my_macro out_var)
    set(${out_var} "value" PARENT_SCOPE)
endmacro()
```

- 优先使用 `function()`，避免变量泄漏到父作用域。
- `macro()` 内部**禁止**使用目录级命令（`include_directories` 等）。
- 复杂参数使用 `cmake_parse_arguments()` 解析。

---

## 15. 命名规范

| 类型           | 风格                  | 示例                  |
| -------------- | --------------------- | --------------------- |
| 函数 / 宏名    | 小写下划线            | `my_add_library()`  |
| 项目内部变量   | 大写 + 项目前缀       | `MYPROJECT_SOURCES` |
| CMake 内置变量 | 大写                  | `CMAKE_BUILD_TYPE`  |
| 目标名         | PascalCase 或项目约定 | `MyLibrary`         |

---

## 16. 迭代语法

```cmake
# 正确：现代语法
foreach(item IN ITEMS foo bar baz)
    message(STATUS "item: ${item}")
endforeach()

foreach(item IN LISTS my_list)
    message(STATUS "item: ${item}")
endforeach()

foreach(item IN LISTS my_list ITEMS extra1 extra2)
    message(STATUS "item: ${item}")
endforeach()
```

---

## 17. 安装与导出

```cmake
install(TARGETS MyLibrary
    EXPORT MyLibraryTargets
    FILE_SET HEADERS
)

install(EXPORT MyLibraryTargets
    NAMESPACE MyLibrary::
    DESTINATION lib/cmake/MyLibrary
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/MyLibraryConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/MyLibraryConfig.cmake
    INSTALL_DESTINATION lib/cmake/MyLibrary
)
```

- 发布库必须提供 `Config.cmake` 文件，支持下游 `find_package()` 使用。
- 使用 `BUILD_INTERFACE` / `INSTALL_INTERFACE` 区分构建目录与安装目录的路径。

---

## 18. 警告管理

- **禁止使用 `-Werror` / `/WX`**，原因：
  - 新编译器版本可能引入新警告，导致构建立即失败。
  - 升级依赖库时触发废弃警告，阻塞所有工作。
  - 强制每位开发者使用相同编译器版本。
- 正确流程：先将警告归零，再逐步提高警告级别，逐条修复。
- 警告选项只放 `PRIVATE`，不传播给依赖者。

---

## 19. 静态分析（可选）

在目标上启用，不影响不使用该目标的构建：

```cmake
set_target_properties(MyTarget PROPERTIES
    CXX_CLANG_TIDY     "clang-tidy;-checks=modernize-*"
    CXX_CPPLINT        "cpplint"
    CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use"
)
```

- 每个头文件必须有对应的 `.cpp` 文件在顶部 `#include` 它，以确保分析工具可以检查头文件。

---

## 20. 打包

```cmake
# CPackConfig.cmake
include(${CMAKE_CURRENT_BINARY_DIR}/CPackConfig_generated.cmake)
# ... 自定义配置
```

- 使用 CMake 内置的 CPack 进行打包。
- 打包配置写入单独的 `CPackConfig.cmake`，不写入 `CMakeLists.txt`。

---

## 21. 代码质量

- `CMakeLists.txt` 和 `.cmake` 文件与应用代码同等对待：保持整洁、可读、可维护。
- CI 特定配置（覆盖率、内存检查等）放入 CTest 脚本（`CTestScript.cmake`），**不写入** `CMakeLists.txt`。
- 所有构建错误必须立即修复，禁止带错误提交。
- 函数优于宏，目标优于变量，显式优于隐式。
