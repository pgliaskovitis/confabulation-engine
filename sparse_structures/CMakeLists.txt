cmake_minimum_required (VERSION 3.4)
project (sparse_structures)

set (CMAKE_CXX_STANDARD 11)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif ()

file (GLOB sparse_structures_SRC
    "*.hpp"
)

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra")
	set (CMAKE_CXX_FLAGS_DEBUG "-g")
	set (CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

add_library (sparse_structures SHARED ${sparse_structures_SRC})
set_target_properties (sparse_structures PROPERTIES LINKER_LANGUAGE CXX)

