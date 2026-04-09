import os
from conan import ConanFile
from conan.tools.build import can_run
from conan.tools.cmake import cmake_layout, CMake, CMakeToolchain, CMakeDeps


class TestPackageConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    generators = "VirtualRunEnv"

    def requirements(self):
        self.requires(self.tested_reference_str)
        # Qt must be a direct dependency so CMakeDeps generates full include paths for Qt5::Core.
        # When qt is only a transitive dep, CMakeDeps leaves INTERFACE_INCLUDE_DIRECTORIES empty.
        self.requires("qt/[>=5.12 <7]")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if can_run(self):
            bin_path = os.path.join(self.cpp.build.bindir, "Log4QtTest")
            self.run(bin_path, env="conanrun")
