cmake_minimum_required (VERSION 3.4)
project (utils)

set (CMAKE_CXX_STANDARD 11)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif ()

file (GLOB utils_SRC
	"*.h"
    "*.hpp"
)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra")
	set (CMAKE_CXX_FLAGS_DEBUG "-g")
	set (CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
	set (CMAKE_CXX_FLAGS_RELEASE "/Ox")
endif ()

add_library (utils SHARED ${utils_SRC})
set_target_properties (utils PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties (utils PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

