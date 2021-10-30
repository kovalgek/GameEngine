project(NetworkProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add common library
set( HEADER_FILES
	include/network/UDPSocket.h
	include/network/TCPSocket.h
	include/network/SocketAddress.h
	include/network/Shared.h
	include/network/SocketUtil.h
	include/network/StringUtils.h
	include/network/MemoryBitStream.h
	include/network/RoboMath.h
	#include/network/NetworkManager.h

	include/network/GameObject.h
	include/network/GameObjectRegistry.h
	include/network/Mouse.h
	include/network/RoboCat.h
	include/network/Yarn.h
	include/network/World.h
	include/network/Timing.h
)
set( SRC_FILES
	src/UDPSocket.cpp
	src/TCPSocket.cpp
	src/SocketAddress.cpp
	src/SocketUtil.cpp
	src/StringUtils.cpp
	src/MemoryBitStream.cpp
	src/RoboMath.cpp
	#src/NetworkManager.cpp

	src/GameObject.cpp
	src/GameObjectRegistry.cpp
	src/Mouse.cpp
	src/RoboCat.cpp
	src/World.cpp
	src/Yarn.cpp
	src/Timing.cpp
)

add_library(Network STATIC ${HEADER_FILES} ${SRC_FILES})

target_include_directories(Network PUBLIC include/network)
target_include_directories(Network PRIVATE src)

#target_link_libraries(Game Rendering)
