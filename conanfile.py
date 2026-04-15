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
        # Qt options
        "qt/*:shared": True,
        "qt/*:widgets": True,
        "qt/*:gui": True,

        #VTK options
        "vtk/*:qt/*:shared": True,   # ← 确保 VTK 依赖的 Qt 也是 shared

        #GTest options
        "gtest/*:shared":False,
        "gtest/*:build_gmock":True,
        "gtest/*:no_main":False,

        # Boost options
        "boost/*:shared": False,
        "boost/*:without_locale": False,      
        
        "qt-advanced-docking-system-custom/*:shared":True,
        "fftw/*:shared":True,
        "gsl/*:shared":False,
        "matio/*:shared":True,
        "quazip/*:shared":True,

        "opencv/*:shared":True,
        
         
    }

    def requirements(self):
        self.requires("qt/[~5.15]")
        self.requires("vtk/[~9.3]")
        self.requires("log4qt/[~1.5]")
        self.requires("gtest/[~1.17]")
        self.requires("boost/[~1.90]")
        self.requires("instrumentortimer/1.0.0")
        self.requires("qbreakpad/0.4.0")
        self.requires("qt-advanced-docking-system-custom/4.5.0")
        self.requires("fftw/3.3.10")
        self.requires("gsl/2.7.1")
        self.requires("matio/1.5.27")
        self.requires("eigen/3.4.0")#opencv/4.13 demand eigen/3.4.0
        self.requires("quazip/1.4")
        self.requires("opencv/4.13.0")
        self.requires("opengl/system")
        
    def layout(self):
        build_type = str(self.settings.build_type)
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
