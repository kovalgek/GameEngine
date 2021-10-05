
project(WindowsAppProject VERSION 1.0 LANGUAGES CXX)

set( SRC
	src/MainWindow.cpp
	src/Resource.h
)

add_executable(WindowsApp WIN32 ${SRC})


target_link_libraries(WindowsApp ImGui)
target_link_libraries(WindowsApp Common)
target_link_libraries(WindowsApp AppContext)

add_definitions(-DUNICODE -D_UNICODE)