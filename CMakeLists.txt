cmake_minimum_required (VERSION 3.1)
project (confabulation_engine)

set (CMAKE_CXX_STANDARD 11)

add_subdirectory (sparse_structures)
add_subdirectory (utils)

include_directories (${CMAKE_SOURCE_DIR})
include_directories (${CMAKE_SOURCE_DIR}/google ${CMAKE_SOURCE_DIR}/google/sparsehash)
include_directories (${sparse_structures_SOURCE_DIR} ${text_processing_SOURCE_DIR} ${utils_SOURCE_DIR})

file (GLOB confabulation_engine_SRC
	"*.cpp"
)

file (GLOB text_processing_SRC
	"text_processing/*.cpp"
)

add_executable (confabulation_engine ${confabulation_engine_SRC} ${text_processing_SRC})

target_link_libraries (confabulation_engine sparse_structures utils)
