project(CommonProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add common library
set( HEADER_FILES
	include/common/GameTimer.h
)
set( SRC_FILES
	src/GameTimer.cpp
)

add_library(Common STATIC ${SRC_FILES})

target_include_directories(Common PUBLIC include/common)
target_include_directories(Common PRIVATE src)
