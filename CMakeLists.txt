cmake_minimum_required (VERSION 3.4)
project (confabulation_engine)

set (CMAKE_CXX_STANDARD 14)
if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Release)
endif ()

if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra")
	set (CMAKE_CXX_FLAGS_DEBUG "-g")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2")
	set (CMAKE_CXX_FLAGS_RELEASE "-O3")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set (CMAKE_CXX_FLAGS "-Wall -Wextra")
	set (CMAKE_CXX_FLAGS_DEBUG "-g")
	set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2")
	set (CMAKE_CXX_FLAGS_RELEASE "-O3")
endif ()

include_directories (${CMAKE_SOURCE_DIR}/google ${CMAKE_SOURCE_DIR}/google/sparsehash)
include_directories (${CMAKE_SOURCE_DIR}/klib)
include_directories (${CMAKE_SOURCE_DIR}/flat_hash_map)
include_directories (${CMAKE_SOURCE_DIR}/sparse_structures)
include_directories (${CMAKE_SOURCE_DIR}/text_processing)
include_directories (${CMAKE_SOURCE_DIR}/utils)
include_directories (${CMAKE_SOURCE_DIR}/core)
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

file (GLOB core_SRC
	"core/*.cpp"
	"core/*.hpp"
)

add_executable (confabulation_engine
	${sparse_structures_SRC}
	${text_processing_SRC}
	${utils_SRC}
	${core_SRC})

TARGET_LINK_LIBRARIES(confabulation_engine pthread)
