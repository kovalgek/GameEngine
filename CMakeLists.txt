cmake_minimum_required(VERSION 3.18.2)

project(ParelhoenEngine VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(core)
add_subdirectory(app)
add_subdirectory(imgui)
add_subdirectory(entt)