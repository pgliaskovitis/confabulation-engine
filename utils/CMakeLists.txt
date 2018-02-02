cmake_minimum_required (VERSION 3.1)
project (utils)

set (CMAKE_CXX_STANDARD 11)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

file (GLOB utils_SRC
	"*.h"
    "*.hpp"
)

add_library (utils SHARED ${utils_SRC})

set_target_properties (utils PROPERTIES LINKER_LANGUAGE CXX)