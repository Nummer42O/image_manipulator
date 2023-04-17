cmake_minimum_required(VERSION 3.10)

project(test)

# Generate symbols for IDE indexer
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use the package PkgConfig to detect GTK+ headers/library files
find_package(OpenCV 4 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0 glibmm-2.4)

file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(test ${SOURCES} ${HEADERS})
target_link_libraries(test PRIVATE ${GTKMM_LIBRARIES} ${OpenCV_LIBS})
target_include_directories(test
    PRIVATE ${GTKMM_INCLUDE_DIRS}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_directories(test PRIVATE ${GTKMM_LIBRARY_DIRS})