cmake_minimum_required(VERSION 3.22)
project(windows_camera_feed VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gdignore" "")

set(GODOT_CPP_DIR "C:/libs/godot-cpp" CACHE PATH "Path to godot-cpp")

if(NOT EXISTS "${GODOT_CPP_DIR}/CMakeLists.txt")
    message(FATAL_ERROR
        "godot-cpp not found.\n"
        "GODOT_CPP_DIR = ${GODOT_CPP_DIR}\n"
        "Pass -DGODOT_CPP_DIR=/path/to/godot-cpp if needed.")
endif()

add_subdirectory(${GODOT_CPP_DIR} godot-cpp)

find_package(OpenCV REQUIRED)

set(SOURCES
    src/register_types.cpp
    src/windows_camera_feed.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        godot-cpp
        ${OpenCV_LIBS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/Release"
    LIBRARY_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/bin/Debug"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin/Release"
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()