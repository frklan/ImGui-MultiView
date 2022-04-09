from conans import ConanFile

class Ftt2Project(ConanFile):
    # Note: options are copied from CMake boolean options.
    # When turned off, CMake sometimes passes them as empty strings.
    options = {
    }

    name = "multiview"
    version = "0.1"
    
    requires = (
        "spdlog/1.9.2",
        "fmt/8.0.1",
        "sdl/2.0.18",
        "stduuid/1.2.2",
        "nlohmann_json/3.10.2"
    )
    generators = "cmake", "gcc", "txt", "cmake_find_package"


    def configure(self):
        self.options["stduuid"].with_cxx20_span = True 
