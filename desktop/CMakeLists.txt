project(desktop) # Your project name

set(CMAKE_CXX_STANDARD 17) # This is equal to QMAKE_CXX_FLAGS += -std=c++0x

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# This will find the Qt5 files.
if (APPLE)
	set (CMAKE_PREFIX_PATH "/usr/local/opt/qt/lib/cmake/")
endif (APPLE)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED) 

set(SOURCES desktop.cpp window.cpp)
set(HEADERS window.h)

# This will create you executable
add_executable(desktop ${SOURCES} ${HEADERS})
# This will link necessary Qt5 libraries to your project
target_link_libraries(desktop Qt5::Widgets Qt5::PrintSupport)
