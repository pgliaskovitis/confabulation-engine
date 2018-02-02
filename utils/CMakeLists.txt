cmake_minimum_required (VERSION 3.1)
project (utils)

set (CMAKE_CXX_STANDARD 11)

file (GLOB utils_SRC
	"*.h"
    "*.hpp"
)

add_library (utils SHARED ${utils_SRC})

set_target_properties (utils PROPERTIES LINKER_LANGUAGE CXX)