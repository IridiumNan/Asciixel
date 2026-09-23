cmake_minimum_required(VERSION 3.16)
project(Asciixel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(asciixel_core STATIC
	src/core/image_sampler.cpp
)

target_include_directories(asciixel_core
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(CTest)

if(BUILD_TESTING)
	add_executable(test_image_sampler
		tests/test_image_sampler.cpp
	)
	target_link_libraries(test_image_sampler PRIVATE asciixel_core)

	add_test(NAME image_sampler COMMAND test_image_sampler)
endif()

