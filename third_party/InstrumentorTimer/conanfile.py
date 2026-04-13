from conan import ConanFile
from conan.tools.files import copy
import os


class InstrumentorTimerConan(ConanFile):
    name = "instrumentortimer"
    version = "1.0.0"
    description = "Basic instrumentation profiler for C++ (header-only). Thread-safe version by GavinSun0921."
    license = "MIT"
    url = "https://github.com/GavinSun0921/InstrumentorTimer"
    homepage = "https://github.com/GavinSun0921/InstrumentorTimer"
    topics = ("profiling", "timer", "instrumentation", "header-only")

    # header-only: 不需要 settings / compiler / build_type
    package_type = "header-library"
    no_copy_source = True

    # 把头文件随 recipe 一起导出（本地 vendored 模式）
    exports_sources = "*.h"

    def package(self):
        copy(self, "*.h",
             src=self.source_folder,
             dst=os.path.join(self.package_folder, "include", "InstrumentorTimer"))

    def package_info(self):
        # header-only: 没有 lib / bin
        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []

        # CMake INTERFACE target 名称
        self.cpp_info.set_property("cmake_target_name", "InstrumentorTimer::InstrumentorTimer")
        self.cpp_info.set_property("cmake_file_name", "InstrumentorTimer")

    def package_id(self):
        # header-only 的包 ID 不依赖编译器/平台
        self.info.clear()