from   conans       import ConanFile, CMake, tools
from   conans.tools import download, unzip
import os

class FiConan(ConanFile):
    name            = "fi"
    version         = "1.0.2"                     
    description     = "Conan package for fi."           
    url             = "https://github.com/acdemiralp/fi"
    license         = "MIT"                         
    settings        = "arch", "build_type", "compiler", "os"
    generators      = "cmake"    
    requires        = "freeimage/3.17.0_2@RWTH-VR/thirdparty"
    default_options = "freeimage:shared=True"

    def source(self):
        zip_name = "%s.zip" % self.version
        download ("%s/archive/%s" % (self.url, zip_name), zip_name, verify=False)
        unzip    (zip_name)
        os.unlink(zip_name)

    def package(self):
        include_folder = "%s-%s/include" % (self.name, self.version)
        self.copy("*.h"  , dst="include", src=include_folder)
        self.copy("*.hpp", dst="include", src=include_folder)
        self.copy("*.inl", dst="include", src=include_folder)
