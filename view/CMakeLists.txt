project(ViewKitProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add imgui library
set( HEADER_FILES
	include/ViewKit/ViewController.h
	include/ViewKit/ViewModels.h
)
set( SRC_FILES
	src/ViewController.cpp
)

add_library(ViewKit
	${HEADER_FILES}
	${SRC_FILES}
)

target_include_directories(ViewKit PUBLIC include/ViewKit)
target_include_directories(ViewKit PRIVATE src)

target_link_libraries(ViewKit CoreKit)
