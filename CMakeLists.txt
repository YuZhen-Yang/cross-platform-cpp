cmake_minimum_required(VERSION 3.29)

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
    cmake_policy(SET CMP0071 NEW)
endif()


project(MinimalQtVTKApp
VERSION 1.0.0
LANGUAGES CXX
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --------------------------------------------------------------------------
# VTK
# --------------------------------------------------------------------------
find_package(VTK REQUIRED COMPONENTS
CommonCore
CommonDataModel
FiltersSources
GUISupportQt
InteractionStyle
RenderingContextOpenGL2
RenderingCore
RenderingFreeType
RenderingGL2PSOpenGL2
RenderingOpenGL2
RenderingQt
)

if(NOT TARGET VTK::GUISupportQt)
message(FATAL_ERROR "MinimalQtVTKApp: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
set(VTK_QT_VERSION 5)
endif()

# --------------------------------------------------------------------------
# Qt
# --------------------------------------------------------------------------
set(MINIMALQTVTK_QT_COMPONENTS Core Gui Widgets)
if(VTK_QT_VERSION VERSION_GREATER_EQUAL 6)
list(APPEND MINIMALQTVTK_QT_COMPONENTS OpenGLWidgets)
endif()

find_package(Qt${VTK_QT_VERSION} REQUIRED COMPONENTS ${MINIMALQTVTK_QT_COMPONENTS})

set(MINIMALQTVTK_QT_TARGETS "")
foreach(_comp IN LISTS MINIMALQTVTK_QT_COMPONENTS)
list(APPEND MINIMALQTVTK_QT_TARGETS "Qt${VTK_QT_VERSION}::${_comp}")
endforeach()

message(STATUS "VTK_VERSION: ${VTK_VERSION}, Qt Version: ${Qt${VTK_QT_VERSION}Widgets_VERSION}")

# --------------------------------------------------------------------------
# Log4Qt
# --------------------------------------------------------------------------
find_package(Log4Qt REQUIRED)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# Prevent "command line is too long" on Windows.
set(CMAKE_NINJA_FORCE_RESPONSE_FILE ON CACHE BOOL "Force Ninja to use response files.")

add_executable(MinimalQtVTKApp MACOSX_BUNDLE
src/main.cpp
)

target_compile_features(MinimalQtVTKApp PRIVATE cxx_std_17)

target_link_libraries(MinimalQtVTKApp
PRIVATE
${MINIMALQTVTK_QT_TARGETS}
${VTK_LIBRARIES}
Log4Qt::Log4Qt
)

vtk_module_autoinit(
  TARGETS MinimalQtVTKApp
  MODULES ${VTK_LIBRARIES}
  )
  
  # --------------------------------------------------------------------------
  # clangd: keep compile_commands.json at source root
  # --------------------------------------------------------------------------
  add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    DEPENDS MinimalQtVTKApp
    COMMENT "Copying compile_commands.json to source root"
)

# --------------------------------------------------------------------------
# Windows: copy runtime DLLs and Qt platform plugins next to the exe
# --------------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET MinimalQtVTKApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:MinimalQtVTKApp>
                $<TARGET_FILE_DIR:MinimalQtVTKApp>
        COMMAND_EXPAND_LISTS
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_pkg "${qt_PACKAGE_FOLDER_DEBUG}")
    else()
        set(_qt_pkg "${qt_PACKAGE_FOLDER_RELEASE}")
    endif()
    add_custom_command(TARGET MinimalQtVTKApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_pkg}/plugins/platforms"
                "$<TARGET_FILE_DIR:MinimalQtVTKApp>/platforms"
    )
endif()