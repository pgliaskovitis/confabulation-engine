cmake_minimum_required (VERSION 3.4)
project (confabulation_engine)

set (CMAKE_CXX_STANDARD 11)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif ()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra")
	set (CMAKE_CXX_FLAGS_DEBUG "-g")
	set (CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "MSVC")
	set (CMAKE_CXX_FLAGS_RELEASE "/Ox")
endif ()

include_directories (${CMAKE_SOURCE_DIR}/google ${CMAKE_SOURCE_DIR}/google/sparsehash)
include_directories (${CMAKE_SOURCE_DIR}/utils)
include_directories (${CMAKE_SOURCE_DIR}/sparse_structures)
include_directories (${CMAKE_SOURCE_DIR}/text_processing)
include_directories (${CMAKE_SOURCE_DIR})

file (GLOB utils_SRC
	"utils/*.h"
	"utils/*.hpp"
)

file (GLOB sparse_structures_SRC
	"sparse_structures/*.hpp"
)

file (GLOB text_processing_SRC
	"text_processing/*.cpp"
)

file (GLOB confabulation_engine_SRC
	"*.cpp"
)

add_executable (confabulation_engine ${confabulation_engine_SRC} ${text_processing_SRC} ${sparse_structures_SRC} ${utils_SRC})

