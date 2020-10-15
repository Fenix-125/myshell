cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME mycat)

project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." OFF)
option(ENABLE_SANITIZERS "Use sanitizers to detect errors." OFF)
option(ENABLE_CONAN "Use Conan as a package manager." OFF)

# Include fixed CMake configuration
include(../../cmake/config.cmake)

## include Headers
include_directories(inc)
include_directories(inc/exceptions)
include_directories(inc/file_interface)
#file(GLOB_RECURSE SRC "inc/*.h" "src/*.cpp")

# Project source compilation
add_executable(${PROJECT_NAME} src/main.cpp src/file_interface/file_interface.cpp src/options_parser_mycat.cpp)

# Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options)
message(${CMAKE_INSTALL_PREFIX})
install (TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/bin)
export(EXPORT ${PROJECT_NAME} FILE "${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}Targets.cmake")

# Sanitizers configuration
if (ENABLE_SANITIZERS)
    set(SANITIZE_UNDEFINED ON)
    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
    set(SANITIZE_MEMORY ON)
    #    set(SANITIZE_ADDRESS ON)
    #    set(SANITIZE_THREAD ON)
    find_package(Sanitizers)

    # CMake targets to which apply sanitizers
    add_sanitizers(${PROJECT_NAME})
endif ()
