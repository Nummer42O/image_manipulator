cmake_minimum_required(VERSION 3.10)
project(image_manipulator)

# generate symbols for IDE indexer
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# use the package PkgConfig to detect GTK+ headers/library files
find_package(OpenCV 4 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0 glibmm-2.4)

# main program
file(GLOB SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/image_proc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

add_executable(main ${SOURCES})
target_link_libraries(main PRIVATE ${GTKMM_LIBRARIES} ${OpenCV_LIBS})
target_include_directories(main
    PRIVATE ${GTKMM_INCLUDE_DIRS}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_directories(main PRIVATE ${GTKMM_LIBRARY_DIRS})

# channel bmp generator
add_executable(bmp_generator ${CMAKE_CURRENT_SOURCE_DIR}/src/bmp_generator.cpp)
target_link_libraries(bmp_generator PRIVATE ${OpenCV_LIBS})
target_include_directories(bmp_generator PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)