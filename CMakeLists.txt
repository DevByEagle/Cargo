cmake_minimum_required(VERSION 3.15)
project(Orin VERSION 1.0.0 LANGUAGES C)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include directory for modular CMake scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Load build options from a configuration file
include(CMakeOptions.txt)

# Include CMake Modules
include(PackConfigurations)

# Define the executable target and its source file
add_executable(${PROJECT_NAME} src/main.c)
