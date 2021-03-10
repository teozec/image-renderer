cmake_minimum_required(VERSION 3.12)

# Define a "project", providing a description and a programming language
project(image-renderer
	VERSION 1.0
	DESCRIPTION "Image renderer in C++"
	LANGUAGES CXX
)
enable_testing()

# Library
add_library(trace
	src/colors.cpp
	)

# This is needed if we keep .h files in the "include" directory
target_include_directories(trace PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	)

# image-renderer
add_executable(image-renderer
	src/image-renderer.cpp
)
target_link_libraries(image-renderer PUBLIC trace)

# color-test
add_executable(color-test
	test/colors.cpp
	)
target_link_libraries(color-test PUBLIC trace)
add_test(NAME color-test
	COMMAND color-test
	)

target_compile_features(image-renderer PUBLIC cxx_std_17)
