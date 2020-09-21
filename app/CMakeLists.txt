
project(AppKitProject VERSION 1.0 LANGUAGES CXX)

set( SRC
	src/MainWindow.cpp
	src/MyBoxDemo.h
	src/Resource.h
)

add_executable(AppKit WIN32 ${SRC})

target_link_libraries(AppKit CoreKit)
target_link_libraries(AppKit ImGui)


add_definitions(-DUNICODE -D_UNICODE)
