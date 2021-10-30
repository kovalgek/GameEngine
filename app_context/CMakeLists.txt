project(AppContextProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add common library
set( HEADER_FILES
	include/core/AppFacade.h
)
set( SRC_FILES
	src/AppFacade.cpp
	src/AppContextFactory.h
	src/AppContextFactory.cpp
	src/AppContext.h
	src/AppContext.cpp
)

add_library(AppContext STATIC ${SRC_FILES})

target_include_directories(AppContext PUBLIC include/app_context)
target_include_directories(AppContext PRIVATE src)

target_link_libraries(AppContext Rendering)
#target_link_libraries(AppContext Game)
