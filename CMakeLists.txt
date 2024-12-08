cmake_minimum_required(VERSION 3.15)
project(Cargo VERSION 1.0.0)

# Export compilation database in JSON format.
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_executable(${PROJECT_NAME} src/main.c)
