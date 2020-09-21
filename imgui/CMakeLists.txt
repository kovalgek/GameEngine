project(ImGuiProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
# add imgui library
set( IMGUI_HEADERS
	src/imconfig.h
	src/imgui.h
	src/imgui_impl_dx12.h
	src/imgui_impl_win32.h
	src/imgui_internal.h
	src/imstb_rectpack.h
	src/imstb_textedit.h
	src/imstb_truetype.h
)
set( IMGUI_SOURCES
	src/imgui.cpp
	src/imgui_draw.cpp
	src/imgui_impl_dx12.cpp
	src/imgui_impl_win32.cpp
	src/imgui_widgets.cpp
)
add_library(ImGui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})
target_include_directories(ImGui PUBLIC "src")