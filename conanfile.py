from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps
import os

class CrossPlatformProjectConan(ConanFile):
    name = "cross_platform_test"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
    }

    def requirements(self):
        pass

    def layout(self):
        build_type = str(self.settings.build_type)
        self.folders.source = "."
        self.folders.build = os.path.join("conan", build_type)
        self.folders.generators = os.path.join("conan", build_type)

    def generate(self):
        tc = CMakeToolchain(self, generator="Ninja")
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()
        # 删除 Conan 自动生成的 CMakeUserPresets.json（我们用自己的 CMakePresets.json）
        user_presets = os.path.join(self.source_folder, "CMakeUserPresets.json")
        if os.path.isfile(user_presets):
            os.remove(user_presets)
