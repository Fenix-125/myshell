cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME myshell)
project(${PROJECT_NAME} CXX)

set(CMAKE_CXX_STANDARD 17)

# Options
option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors." ON)
option(ENABLE_SANITIZERS "Use sanitizers to detect errors." OFF)
option(ENABLE_CONAN "Use Conan as a package manager." OFF)

## PVS Studio
#include(cmake/PVS-Studio.cmake)
#SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include fixed CMake configuration
include(cmake/config.cmake)

# Include headers
include_directories(inc)
#file(GLOB_RECURSE SRC "inc/*.h" "src/*.cpp")

### Add mycat
add_subdirectory(programs/mycat)
add_subdirectory(programs/mycp)

# Project source compilation
add_executable(${PROJECT_NAME} src/main.cpp src/shell.cpp src/options_parser.cpp src/glob_posix.cpp)

install(EXPORT mycat DESTINATION ${PROJECT_SOURCE_DIR}/bin)
install(EXPORT mycp DESTINATION ${PROJECT_SOURCE_DIR}/bin)
#add_library(operations SHARED src/operations.cpp inc/operations/operations.hpp)
#add_executable(add src/main.cpp)
#target_link_libraries(add operations)

# Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${readline_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} Boost::program_options)
target_link_libraries(${PROJECT_NAME} readline)

# Sanitizers configuration
if (ENABLE_SANITIZERS)
    set(SANITIZE_UNDEFINED ON)
    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
    set(SANITIZE_MEMORY ON)
    #    set(SANITIZE_ADDRESS ON)
    #    set(SANITIZE_THREAD ON)
    find_package(Sanitizers)

    # CMake targets to which apply sanitizers
    add_sanitizers(operations ${PROJECT_NAME})
endif ()

#####################################
# create own target for library
#pvs_studio_add_target(TARGET ${PROJECT_NAME}.analyze ALL
#        ANALYZE ${PROJECT_NAME}
#        OUTPUT FORMAT errorfile
#        MODE GA:1,2,3 OP:1,2,3 GA:1,2,3 OP:1,2,3 64:1,2,3 CS:1,2,3 MISRA:1,2,3
#        LOG target_${PROJECT_NAME}.err)
##################################%###
