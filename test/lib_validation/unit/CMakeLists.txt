# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------
find_package(GTest REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)

# Log4Qt depends on Qt5::Core
find_package(Log4Qt REQUIRED)

# QuaZip: Conan package name is QuaZip-Qt5 for Qt5 builds
find_package(QuaZip-Qt5 REQUIRED)

find_package(qBreakpad REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem)
find_package(FFTW3 REQUIRED)
find_package(GSL REQUIRED)
find_package(matio REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(OpenCV REQUIRED)
find_package(InstrumentorTimer REQUIRED)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------
# test_main.cpp provides main() with QCoreApplication for Qt-dependent tests.
# Link against gtest::gtest (not gtest_main) since we supply our own main.
add_executable(LibValidationTests
    test_main.cpp
    test_boost.cpp
    test_eigen.cpp
    test_fftw.cpp
    test_gsl.cpp
    test_instrumentortimer.cpp
    test_log4qt.cpp
    test_matio.cpp
    test_opencv.cpp
    test_qbreakpad.cpp
    test_quazip.cpp
)

set_target_properties(LibValidationTests PROPERTIES AUTOMOC ON)

target_compile_features(LibValidationTests PRIVATE cxx_std_17)

target_link_libraries(LibValidationTests
    PRIVATE
        gtest::gtest
        Qt5::Core
        Log4Qt::Log4Qt
        QuaZip::QuaZip
        qBreakpad::qBreakpad
        Boost::filesystem
        fftw::fftw
        GSL::gsl
        GSL::gslcblas
        matio::matio
        Eigen3::Eigen
        opencv::opencv
        InstrumentorTimer::InstrumentorTimer
)

include(GoogleTest)
gtest_discover_tests(LibValidationTests DISCOVERY_MODE PRE_TEST)

# --------------------------------------------------------------------------
# Windows: copy runtime DLLs and Qt platform plugins next to the exe
# --------------------------------------------------------------------------
if(WIN32)
    add_custom_command(TARGET LibValidationTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_RUNTIME_DLLS:LibValidationTests>
                $<TARGET_FILE_DIR:LibValidationTests>
        COMMAND_EXPAND_LISTS
        COMMENT "Copying runtime DLLs"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(_qt_pkg "${qt_PACKAGE_FOLDER_DEBUG}")
    else()
        set(_qt_pkg "${qt_PACKAGE_FOLDER_RELEASE}")
    endif()

    add_custom_command(TARGET LibValidationTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${_qt_pkg}/plugins/platforms"
                "$<TARGET_FILE_DIR:LibValidationTests>/platforms"
        COMMENT "Copying Qt platform plugins"
    )
endif()

