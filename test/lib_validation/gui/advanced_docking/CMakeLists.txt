# --------------------------------------------------------------------------
# Qt
# --------------------------------------------------------------------------
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# --------------------------------------------------------------------------
# Qt Advanced Docking System
# find_package name: qt5advanceddocking  target: ads::qt5advanceddocking
# --------------------------------------------------------------------------
find_package(qt5advanceddocking-custom REQUIRED)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------
add_executable(AdvancedDockingValidation
    main.cpp
)

set_target_properties(AdvancedDockingValidation PROPERTIES AUTOMOC ON)

target_compile_features(AdvancedDockingValidation PRIVATE cxx_std_17)

target_link_libraries(AdvancedDockingValidation
    PRIVATE
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        ads::qt5advanceddocking-custom
)

# --------------------------------------------------------------------------
# Windows: copy runtime DLLs and Qt platform plugins next to the exe
# --------------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET AdvancedDockingValidation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:AdvancedDockingValidation>
                $<TARGET_FILE_DIR:AdvancedDockingValidation>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_pkg "${qt_PACKAGE_FOLDER_DEBUG}")
    else()
        set(_qt_pkg "${qt_PACKAGE_FOLDER_RELEASE}")
    endif()

    add_custom_command(TARGET AdvancedDockingValidation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_pkg}/plugins/platforms"
                "$<TARGET_FILE_DIR:AdvancedDockingValidation>/platforms"
        COMMENT "Copying Qt platform plugins"
    )
endif()

