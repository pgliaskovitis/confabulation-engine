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
endif ()

add_subdirectory (sparse_structures)
add_subdirectory (utils)

include_directories (${CMAKE_SOURCE_DIR})
include_directories (${CMAKE_SOURCE_DIR}/google ${CMAKE_SOURCE_DIR}/google/sparsehash)
include_directories (${sparse_structures_SOURCE_DIR} ${utils_SOURCE_DIR} text_processing)

file (GLOB confabulation_engine_SRC
	"*.cpp"
)

file (GLOB text_processing_SRC
	"text_processing/*.cpp"
)

add_executable (confabulation_engine ${confabulation_engine_SRC} ${text_processing_SRC})
target_link_libraries (confabulation_engine sparse_structures utils)

