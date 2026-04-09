cmake_minimum_required(VERSION 3.29)
project(cross_platform_test VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Gui OpenGL)
find_package(Log4Qt REQUIRED)
find_package(VTK REQUIRED COMPONENTS
  CommonColor
  CommonCore
  FiltersSources
  InteractionStyle
  RenderingContextOpenGL2
  RenderingCore
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)
if (NOT VTK_FOUND)
  message(FATAL_ERROR "CylinderExample: Unable to find the VTK build folder.")
endif()
message(STATUS "Found Log4Qt: ${Log4Qt_VERSION} (${Log4Qt_DIR})")
message(STATUS "Found VTK:    ${VTK_VERSION}")
message(STATUS "Found Qt5:    ${Qt5_VERSION}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 服务于 vscode clangd 插件
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)

# --- 最小验证程序 ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(minimal_demo
    src/main.cpp
)

target_link_libraries(minimal_demo PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL
    Log4Qt::Log4Qt
    ${VTK_LIBRARIES}
)

vtk_module_autoinit(
    TARGETS  minimal_demo
    MODULES  ${VTK_LIBRARIES}
)

if(WIN32)
    # Windows 无 RPATH：运行时 DLL 和 Qt 平台插件需复制到 exe 同级目录
    add_custom_command(TARGET minimal_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:minimal_demo>
                $<TARGET_FILE_DIR:minimal_demo>
        COMMAND_EXPAND_LISTS
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_pkg "${qt_PACKAGE_FOLDER_DEBUG}")
    else()
        set(_qt_pkg "${qt_PACKAGE_FOLDER_RELEASE}")
    endif()
    add_custom_command(TARGET minimal_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_pkg}/plugins/platforms"
                "$<TARGET_FILE_DIR:minimal_demo>/platforms"
    )
endif()