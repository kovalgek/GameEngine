project(CoreKitProject VERSION 1.0 LANGUAGES CXX)

# ------------------------------------------------
#add sources to the main target
set(SOURCE_FILES_DIR "Source Files")

#core
set( CORE_FILES
	src/Application.h
	src/Application.cpp
	src/Renderer.h
	src/Renderer.cpp
)
source_group( ${SOURCE_FILES_DIR}/Core FILES ${CORE_FILES} )

#data provides
set( DATA_PROVIDER_FILES
	src/MainPassData.h
	src/MainPassDataProvider.h
	src/MainPassDataProvider.cpp
	src/Material.h
	src/MaterialsDataProvider.h
	src/MaterialsDataProvider.cpp
	src/RenderItem.h
	src/ObjectsDataProvider.h
	src/ObjectsDataProvider.cpp
	src/MainPassModelsListener.h
	src/MainPassModels.h
)
source_group( ${SOURCE_FILES_DIR}/DataProviders FILES ${DATA_PROVIDER_FILES} )

#dynamic vertex
set( DYNAMIC_VERTEX_FILES
	src/DynamicVertices.h
	src/DynamicVerticesProvider.h
	src/DynamicVerticesProvider.cpp
	src/Waves.h
	src/Waves.cpp
)
source_group( ${SOURCE_FILES_DIR}/DynamicVertex FILES ${DYNAMIC_VERTEX_FILES} )

#frame resources
set( FRAME_RESOURCE_FILES
	src/FrameResource.h
	src/FrameResource.cpp
	src/FrameResourceController.h
	src/FrameResourceUpdater.h
	src/FrameResourceController.cpp
	src/FrameResourceUpdater.cpp
	src/MaterialConstants.h
	src/ObjectConstants.h
	src/PassConstants.h
)
source_group( ${SOURCE_FILES_DIR}/FrameResources FILES ${FRAME_RESOURCE_FILES} )

#geometry
set( GEOMETRY_FILES
	src/Vertex.h
	src/GeometryGenerator.h
	src/GeometryStorage.h
	src/GeometryGenerator.cpp
	src/GeometryStorage.cpp
)
source_group( ${SOURCE_FILES_DIR}/Geometry FILES ${GEOMETRY_FILES} )

#GUI
set( GUI_FILES
	src/ViewModels.h
	src/ViewController.h
	src/ViewController.cpp
)
source_group( ${SOURCE_FILES_DIR}//GUI FILES ${GUI_FILES} )

#helper
set( HELPERS_FILES
	src/DDSTextureLoader.h
	src/DDSTextureLoader.cpp
	src/GameTimer.cpp
	src/MathHelper.h
	src/MathHelper.cpp
	src/UploadBuffer.h
	src/d3dUtil.h
	src/d3dUtil.cpp
	src/d3dx12.h
)
source_group( ${SOURCE_FILES_DIR}/Helpers FILES ${HELPERS_FILES} )

#shader helpers
set( SHADER_HELPERS_FILES
	src/PipleneStateData.h
	src/PipleneStateData.cpp
)
source_group( ${SOURCE_FILES_DIR}/ShaderHelpers FILES ${SHADER_HELPERS_FILES} )

#textures
set( TEXTURES_FILES
	src/SrvHeapProvider.h
	src/SrvHeapProvider.cpp
	src/Texture.h
	src/TexturesProvider.h
	src/TextureStaticSamplers.h
	src/TexturesProvider.cpp
	src/TextureStaticSamplers.cpp
)
source_group( ${SOURCE_FILES_DIR}/Textures FILES ${TEXTURES_FILES} )

#other
set( OTHER_FILES
	src/AppFacade.cpp
	src/AppContextFactory.h
	src/AppContextFactory.cpp
	src/AppContext.h
	src/AppContext.cpp
)

set( HEADER_FILES
	include/core/GameTimer.h
	include/core/AppFacade.h
)

# ------------------------------------------------
#main target
add_library(CoreKit
	${HEADER_FILES}
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
)


target_include_directories(CoreKit PUBLIC include/core)
target_include_directories(CoreKit PRIVATE src)

add_definitions(-DUNICODE -D_UNICODE)


# ------------------------------------------------
# add directX libraries
target_link_libraries(CoreKit "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
target_link_libraries(CoreKit ImGui)


# ------------------------------------------------
# copy resources
set(PROJECT_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)

#shades
add_custom_command(
    TARGET CoreKit
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/core/shaders ${PROJECT_RESOURCE_DIR}/shaders
)

#models
add_custom_command(
    TARGET CoreKit
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/core/models ${PROJECT_RESOURCE_DIR}/models
)

#textures
add_custom_command(
    TARGET CoreKit
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/core/textures ${PROJECT_RESOURCE_DIR}/textures
)
