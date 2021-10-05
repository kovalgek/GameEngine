project(ImGuiProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add imgui library
set( IMGUI_HEADERS
	include/imgui/imconfig.h
	include/imgui/imgui.h
	include/imgui/imgui_impl_dx12.h
	include/imgui/imgui_impl_win32.h
	include/imgui/imgui_internal.h
	include/imgui/imstb_rectpack.h
	include/imgui/imstb_textedit.h
	include/imgui/imstb_truetype.h
)
set( IMGUI_SOURCES
	src/imgui.cpp
	src/imgui_draw.cpp
	src/imgui_impl_dx12.cpp
	src/imgui_impl_win32.cpp
	src/imgui_widgets.cpp
)
add_library(ImGui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})

target_include_directories(ImGui PUBLIC include/imgui)