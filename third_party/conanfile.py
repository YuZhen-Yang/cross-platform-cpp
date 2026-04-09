from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps
import os

class LibEnv(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    def layout(self):
        build_type = str(self.settings.build_type)
        self.folders.generators = os.path.join("conan", build_type)
    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def requirements(self):
        self.requires("qt/[~5.15]")