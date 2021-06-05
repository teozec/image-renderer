# Copyright (C) 2021 Luca Nigro and Matteo Zeccoli Marazzini
#
# This file is part of image-renderer.
#
# image-renderer is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# image-renderer is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with image-renderer.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.12)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENTIONS OFF)

# Define a "project", providing a description and a programming language
project(image-renderer
	VERSION 1.0
	DESCRIPTION "Image renderer in C++"
	LANGUAGES CXX
)
enable_testing()

# Library
add_library(trace
	src/color.cpp
	src/hdr-image.cpp
	)

find_package(GD REQUIRED)
target_link_libraries(trace PUBLIC ${GD_LIBRARIES})

# This is needed if we keep .h files in the "include" directory
target_include_directories(trace PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
	${GD_INCLUDE_DIR}
	)

# image-renderer
add_executable(image-renderer
	src/image-renderer.cpp
)
target_link_libraries(image-renderer PUBLIC trace)

# color-test
add_executable(color-test
	test/color.cpp
	)
target_link_libraries(color-test PUBLIC trace)
add_test(NAME color-test
	COMMAND color-test
	)

# hdr-image-test
add_executable(hdr-image-test
	test/hdr-image.cpp
	)
target_link_libraries(hdr-image-test PUBLIC trace)
add_test(NAME hdr-image-test
	COMMAND hdr-image-test
	)

# geometry-test
add_executable(geometry-test
	test/geometry.cpp
	)
target_link_libraries(geometry-test PUBLIC trace)
add_test(NAME geometry-test
	COMMAND geometry-test
	)

# camera-test
add_executable(camera-test
	test/camera.cpp
	)

target_link_libraries(camera-test PUBLIC trace)
add_test(NAME camera-test
	COMMAND camera-test
	)

# shape-test
add_executable(shape-test
	test/shape.cpp
	)

target_link_libraries(shape-test PUBLIC trace)
add_test(NAME shape-test
	COMMAND shape-test
	)

# material-test
add_executable(material-test
	test/material.cpp
	)

target_link_libraries(material-test PUBLIC trace)
add_test(NAME material-test
	COMMAND material-test
	)

# random-test
add_executable(random-test
	test/random.cpp
	)

target_link_libraries(random-test PUBLIC trace)
add_test(NAME random-test
	COMMAND random-test
	)

# renderer-test
add_executable(renderer-test
	test/renderer.cpp
	)

target_link_libraries(renderer-test PUBLIC trace)
add_test(NAME renderer-test
	COMMAND renderer-test
	)

target_compile_features(image-renderer PUBLIC cxx_std_17)
