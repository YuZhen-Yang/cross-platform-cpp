import os
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
from conan.tools.files import copy, replace_in_file


class Log4QtConan(ConanFile):
    name = "log4qt"
    version = "1.5.1"
    description = "Logging for the Qt cross-platform application framework"
    license = "Apache-2.0"
    url = "https://github.com/MEONMedical/Log4Qt"
    homepage = "https://github.com/MEONMedical/Log4Qt"
    topics = ("logging", "qt", "log4j")
    package_type = "library"

    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared":       [True, False],
        "fPIC":         [True, False],
        "with_qml":     [True, False],
        "with_telnet":  [True, False],
        "with_db":      [True, False],
    }
    default_options = {
        "shared":       True,
        "fPIC":         True,
        "with_qml":     False,
        "with_telnet":  False,
        "with_db":      False,
        "qt/*:shared":  True,
    }

    # Source lives alongside this recipe in the same directory.
    exports_sources = (
        "CMakeLists.txt",
        "LICENSE",
        "Readme.md",
        "src/*",
        "cmake/*",
        "include/*",
    )

    # -------------------------------------------------------------------------
    # Conan API callbacks
    # -------------------------------------------------------------------------

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        # Core + Concurrent are always needed.
        # The specific Qt modules pulled in depend on the chosen options,
        # but declaring the base package is sufficient; CMake will find the rest.
        self.requires("qt/[>=5.12 <7]")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.cache_variables["BUILD_SHARED_LIBS"]         = self.options.shared
        tc.cache_variables["BUILD_WITH_QML_LOGGING"]    = self.options.with_qml
        tc.cache_variables["BUILD_WITH_TELNET_LOGGING"] = self.options.with_telnet
        tc.cache_variables["BUILD_WITH_DB_LOGGING"]     = self.options.with_db
        tc.cache_variables["LOG4QT_ENABLE_TESTS"]       = False
        tc.cache_variables["LOG4QT_ENABLE_EXAMPLES"]    = False
        tc.cache_variables["BUILD_WITH_DOCS"]           = False

        # CMakeDeps does not populate Qt5Core_VERSION / Qt6Core_VERSION, but
        # Log4Qt's CMakeLists.txt uses that variable for its minimum-version
        # guard.  Read the actual version from the resolved dependency and
        # inject it so the check passes.
        qt_version = str(self.dependencies["qt"].ref.version)
        qt_major   = qt_version.split(".")[0]
        tc.cache_variables[f"Qt{qt_major}Core_VERSION"] = qt_version

        tc.generate()

    def build(self):
        # Remove IDE-only custom target that references .github files not present in exported sources
        replace_in_file(self, os.path.join(self.source_folder, "CMakeLists.txt"),
            'add_custom_target(githubworkflows SOURCES\n'
            '                  ".github/workflows/cmake-ubuntu.yml"\n'
            '                  ".github/workflows/cmake-windows.yml"\n'
            '                  ".github/workflows/cmake-macos.yml")',
            '# githubworkflows target removed (not needed for Conan build)')
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()
        copy(self, "LICENSE",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "licenses"))

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name",   "Log4Qt")
        self.cpp_info.set_property("cmake_target_name", "Log4Qt::Log4Qt")

        suffix = "_d" if self.settings.build_type == "Debug" and self.settings.compiler == "msvc" else ""
        self.cpp_info.libs = ["log4qt" + suffix]

        # Propagate Qt transitive dependencies (component names from ConanCenter qt recipe).
        self.cpp_info.requires = ["qt::qtCore", "qt::qtConcurrent"]
        if self.options.with_telnet:
            self.cpp_info.requires.append("qt::qtNetwork")
        if self.options.with_qml:
            self.cpp_info.requires.append("qt::qtQml")
        if self.options.with_db:
            self.cpp_info.requires.append("qt::qtSql")
