# --------------------------------------------------------------------------
# VTK
# --------------------------------------------------------------------------
find_package(VTK REQUIRED COMPONENTS
    CommonCore
    CommonDataModel
    FiltersSources
    GUISupportQt
    RenderingContextOpenGL2
    RenderingCore
    RenderingOpenGL2
    RenderingQt
)

if(NOT TARGET VTK::GUISupportQt)
    message(FATAL_ERROR "VtkQtValidation: VTK not built with Qt support.")
endif()

if(NOT DEFINED VTK_QT_VERSION)
    set(VTK_QT_VERSION 5)
endif()

# --------------------------------------------------------------------------
# Qt
# --------------------------------------------------------------------------
find_package(Qt${VTK_QT_VERSION} REQUIRED COMPONENTS Core Gui Widgets)

# --------------------------------------------------------------------------
# Log4Qt
# --------------------------------------------------------------------------
find_package(Log4Qt REQUIRED)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
add_executable(VtkQtValidation
    main.cpp
)

set_target_properties(VtkQtValidation PROPERTIES AUTOMOC ON)

target_compile_features(VtkQtValidation PRIVATE cxx_std_17)

target_link_libraries(VtkQtValidation
    PRIVATE
        Qt${VTK_QT_VERSION}::Core
        Qt${VTK_QT_VERSION}::Gui
        Qt${VTK_QT_VERSION}::Widgets
        ${VTK_LIBRARIES}
        Log4Qt::Log4Qt
)

vtk_module_autoinit(
    TARGETS VtkQtValidation
    MODULES ${VTK_LIBRARIES}
)

# --------------------------------------------------------------------------
# Windows: copy runtime DLLs and Qt platform plugins next to the exe
# --------------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET VtkQtValidation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:VtkQtValidation>
                $<TARGET_FILE_DIR:VtkQtValidation>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_pkg "${qt_PACKAGE_FOLDER_DEBUG}")
    else()
        set(_qt_pkg "${qt_PACKAGE_FOLDER_RELEASE}")
    endif()

    add_custom_command(TARGET VtkQtValidation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_pkg}/plugins/platforms"
                "$<TARGET_FILE_DIR:VtkQtValidation>/platforms"
        COMMENT "Copying Qt platform plugins"
    )
endif()

