# desktop submodule
project(desktop)
set(CMAKE_CXX_STANDARD 17)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# This will find the Qt5 files.
if (APPLE)
	execute_process(
		COMMAND brew --prefix qt5 
		RESULT_VARIABLE BREW_QT5
		OUTPUT_VARIABLE BREW_QT5_PREFIX
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	if (BREW_QT5 EQUAL 0 AND EXISTS "${BREW_QT5_PREFIX}")
		message(STATUS "Found QT5 installed by Homebrew at ${BREW_QT5_PREFIX}")
		set(CMAKE_PREFIX_PATH "${BREW_QT5_PREFIX}")
	endif()
endif (APPLE)
find_package(Qt5Widgets REQUIRED)

# Set sources and headers for this submodule
set(SOURCES desktop.cpp window.cpp)
set(HEADERS window.h)

# Create the executable
add_executable(desktop ${SOURCES} ${HEADERS})

# Link necessary Qt5 libraries to the project
target_link_libraries(desktop trace Qt5::Widgets)
