project(EnTTProject VERSION 1.0 LANGUAGES CXX)

add_library(entt INTERFACE)
target_include_directories(entt INTERFACE include/entt)
