cmake_minimum_required(VERSION 3.20)
project(Azer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(Azer/vendor/spdlog)

add_subdirectory(Azer)
add_subdirectory(SandBox)