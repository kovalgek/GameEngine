project(GameProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add common library
set( HEADER_FILES
	include/game/Scene.h
)
set( SRC_FILES
	src/Scene.cpp
)

add_library(Game STATIC ${HEADER_FILES} ${SRC_FILES})

target_include_directories(Game PUBLIC include/game)
target_include_directories(Game PRIVATE src)

target_link_libraries(Game Rendering)
target_link_libraries(Game Network)