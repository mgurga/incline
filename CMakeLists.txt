cmake_minimum_required(VERSION 3.11)

project(incline LANGUAGES CXX)
set(TARGET_NAME incline)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_STANDARD 11)

include(UrhoCommon)

define_source_files()
setup_main_executable()
