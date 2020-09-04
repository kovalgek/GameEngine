cmake_minimum_required(VERSION 3.18.2)

project(KovalgekGameEngine VERSION 1.0 LANGUAGES CXX)

set( CORE_FILES
	app/src/Application.h
	app/src/Application.cpp
	app/src/Renderer.h
	app/src/Renderer.cpp
)
source_group( "Source Files/Core" FILES ${CORE_FILES} )

set( DATA_PROVIDER_FILES
	app/src/MainPassData.h
	app/src/MainPassDataProvider.h
	app/src/MainPassDataProvider.cpp
	app/src/Material.h
	app/src/MaterialsDataProvider.h
	app/src/MaterialsDataProvider.cpp
	app/src/RenderItem.h
	app/src/ObjectsDataProvider.h
	app/src/ObjectsDataProvider.cpp
)
source_group( "Source Files/DataProviders" FILES ${DATA_PROVIDER_FILES} )

set( DYNAMIC_VERTEX_FILES
	app/src/DynamicVertices.h
	app/src/DynamicVerticesProvider.h
	app/src/DynamicVerticesProvider.cpp
	app/src/Waves.h
	app/src/Waves.cpp
)
source_group( "Source Files/DynamicVertex" FILES ${DYNAMIC_VERTEX_FILES} )


set( FRAME_RESOURCE_FILES
	app/src/FrameResource.h
	app/src/FrameResource.cpp
	app/src/FrameResourceController.h
	app/src/FrameResourceUpdater.h
	app/src/FrameResourceController.cpp
	app/src/FrameResourceUpdater.cpp
	app/src/MaterialConstants.h
	app/src/ObjectConstants.h
	app/src/PassConstants.h
)
source_group( "Source Files/FrameResources" FILES ${FRAME_RESOURCE_FILES} )

set( GEOMETRY_FILES
	app/src/Vertex.h
	app/src/GeometryGenerator.h
	app/src/GeometryStorage.h
	app/src/GeometryGenerator.cpp
	app/src/GeometryStorage.cpp
)
source_group( "Source Files/Geometry" FILES ${GEOMETRY_FILES} )

set( GUI_FILES
	app/src/CameraViewModel.h
	app/src/ImGuiController.h
	app/src/ImGuiController.cpp
)
source_group( "Source Files/GUI" FILES ${GUI_FILES} )


set( HELPERS_FILES
	app/src/DDSTextureLoader.h
	app/src/DDSTextureLoader.cpp
	app/src/GameTimer.h
	app/src/GameTimer.cpp
	app/src/MathHelper.h
	app/src/MathHelper.cpp
	app/src/UploadBuffer.h
	app/src/d3dUtil.h
	app/src/d3dUtil.cpp
	app/src/d3dx12.h
)
source_group( "Source Files/Helpers" FILES ${HELPERS_FILES} )

set( SHADER_HELPERS_FILES
	app/src/PipleneStateData.h
	app/src/PipleneStateData.cpp
)
source_group( "Source Files/ShaderHelpers" FILES ${SHADER_HELPERS_FILES} )

set( TEXTURES_FILES
	app/src/SrvHeapProvider.h
	app/src/SrvHeapProvider.cpp
	app/src/Texture.h
	app/src/TexturesProvider.h
	app/src/TextureStaticSamplers.h
	app/src/TexturesProvider.cpp
	app/src/TextureStaticSamplers.cpp
)
source_group( "Source Files/Textures" FILES ${TEXTURES_FILES} )

set( IMGUI_HEADERS
	imgui/imconfig.h
	imgui/imgui.h
	imgui/imgui_impl_dx12.h
	imgui/imgui_impl_win32.h
	imgui/imgui_internal.h
	imgui/imstb_rectpack.h
	imgui/imstb_textedit.h
	imgui/imstb_truetype.h
)
set( IMGUI_SOURCES
	imgui/imgui.cpp
	imgui/imgui_draw.cpp
	imgui/imgui_impl_dx12.cpp
	imgui/imgui_impl_win32.cpp
	imgui/imgui_widgets.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_HEADERS})


set( OTHER_FILES
	app/src/AppFacade.h
	app/src/AppFacadeFactory.h
	app/src/AppFacade.cpp
	app/src/AppFacadeFactory.cpp
	app/src/MainWindow.cpp	
	app/src/MyBoxDemo.h
	app/src/Resource.h
)

set( VERTEX_SHADER_FILES
	app/shaders/Default.hlsl
	app/shaders/LightingUtil.hlsl
)

set( SHADER_FILES
    ${VERTEX_SHADER_FILES}
)

source_group( "Resources\\Shaders" FILES ${SHADER_FILES} )


set_source_files_properties( ${SHADER_FILES} 
    PROPERTIES
        VS_SHADER_MODEL 5.1
        VS_SHADER_DISABLE_OPTIMIZATIONS $<$<CONFIG:Debug>:ON>
        VS_SHADER_ENABLE_DEBUG $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:ON>
)

set_source_files_properties( ${VERTEX_SHADER_FILES}
    PROPERTIES 
        VS_SHADER_TYPE Vertex
        VS_TOOL_OVERRIDE "None"
)

add_definitions(-DUNICODE -D_UNICODE)

add_executable(KovalgekGameEngine WIN32
	${CORE_FILES}
	${DATA_PROVIDER_FILES}
	${DYNAMIC_VERTEX_FILES}
	${FRAME_RESOURCE_FILES}
	${GEOMETRY_FILES}
	${GUI_FILES}
	${HELPERS_FILES}
	${SHADER_HELPERS_FILES}
	${TEXTURES_FILES}
	${OTHER_FILES}
	${IMGUI_FILES}
	${SHADER_FILES}
)

target_include_directories(imgui PUBLIC "imgui")
target_link_libraries(KovalgekGameEngine PRIVATE imgui)

target_link_libraries(KovalgekGameEngine PRIVATE "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")


set(PROJECT_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)

#shades
add_custom_command(
    TARGET KovalgekGameEngine
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app/shaders ${PROJECT_RESOURCE_DIR}/shaders
)

#models
add_custom_command(
    TARGET KovalgekGameEngine
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app/models ${PROJECT_RESOURCE_DIR}/models
)

#textures
add_custom_command(
    TARGET KovalgekGameEngine
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/app/textures ${PROJECT_RESOURCE_DIR}/textures
)
