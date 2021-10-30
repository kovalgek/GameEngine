project(RenderingProject VERSION 1.0 LANGUAGES CXX)

set( HEADERS 
	include/rendering/Renderer.h
	include/rendering/RendererFactory.h
	include/rendering/GPUService.h
	include/rendering/GPUServiceFactory.h
	include/rendering/MainPassDataProvider.h
	include/rendering/MaterialsDataProvider.h
	include/rendering/MaterialsDataProviderConfigurator.h
	include/rendering/Scene.h
	include/rendering/DynamicVerticesProvider.h
	include/rendering/FrameResource.h
	include/rendering/RingBuffer.h
	include/rendering/ConstantBufferUpdating.h
	include/rendering/MainPassConstantBufferUpdater.h
	include/rendering/MaterialConstantBufferUpdater.h
	include/rendering/ObjectConstantBufferUpdater.h
	include/rendering/GeometryGenerator.h	
	include/rendering/GeometryStorage.h	
	include/rendering/GeometryStorageConfigurator.h	
	include/rendering/OBJFileLoader.h
	include/rendering/ViewController.h
	include/rendering/d3dUtil.h
	include/rendering/SrvHeapProvider.h
	include/rendering/TexturesProvider.h
)

# ------------------------------------------------
#add sources to the main target
set(SOURCE_FILES_DIR "Source Files")

#core
set( CORE_FILES
	include/rendering/Renderer.h
	src/Renderer.cpp
	include/rendering/RendererFactory.h
	src/RendererFactory.cpp	
	include/rendering/GPUService.h
	src/GPUService.cpp
	include/rendering/GPUServiceFactory.h
	src/GPUServiceFactory.cpp
)
source_group( ${SOURCE_FILES_DIR}/Core FILES ${CORE_FILES} )

#data provides
set( DATA_PROVIDER_FILES
	include/rendering/MainPassData.h
	include/rendering/MainPassDataProvider.h
	src/MainPassDataProvider.cpp
	src/Material.h
	include/rendering/MaterialsDataProvider.h
	src/MaterialsDataProvider.cpp
	src/MaterialsDataProviderConfigurator.cpp
	include/rendering/MaterialsDataProviderConfigurator.h
	src/MainPassModelsListener.h
	include/rendering/MainPassModels.h
	include/rendering/Scene.h
	src/Scene.cpp
	include/rendering/Components.h
	src/GameObject.cpp
	include/rendering/GameObject.h
	src/MirrorController.cpp
	include/rendering/MirrorController.h
)
source_group( ${SOURCE_FILES_DIR}/DataProviders FILES ${DATA_PROVIDER_FILES} )

#dynamic vertex
set( DYNAMIC_VERTEX_FILES
	src/DynamicVertices.h
	include/rendering/DynamicVerticesProvider.h
	src/DynamicVerticesProvider.cpp
	src/Waves.h
	src/Waves.cpp
)
source_group( ${SOURCE_FILES_DIR}/DynamicVertex FILES ${DYNAMIC_VERTEX_FILES} )

#frame resources
set( FRAME_RESOURCE_FILES
	include/rendering/FrameResource.h
	src/FrameResource.cpp
	include/rendering/FrameResourceUpdater.h
	src/FrameResourceUpdater.cpp
	include/rendering/MaterialConstants.h
	include/rendering/ObjectConstants.h
	include/rendering/PassConstants.h
	include/rendering/RingBuffer.h
	include/rendering/ConstantBufferUpdating.h
	include/rendering/MainPassConstantBufferUpdater.h
	src/MainPassConstantBufferUpdater.cpp
	include/rendering/MaterialConstantBufferUpdater.h
	src/MaterialConstantBufferUpdater.cpp
	include/rendering/ObjectConstantBufferUpdater.h
	src/ObjectConstantBufferUpdater.cpp
)
source_group( ${SOURCE_FILES_DIR}/FrameResources FILES ${FRAME_RESOURCE_FILES} )

#geometry/generator
set( GEOMETRY_GENERATOR_FILES
	include/rendering/GeometryGenerator.h	
	src/GeometryGenerator.cpp
	src/MeshData.h
	src/MeshVertex.h
)
source_group( ${SOURCE_FILES_DIR}/Geometry/Generator FILES ${GEOMETRY_GENERATOR_FILES} )

#geometry/storage
set( GEOMETRY_STORAGE_FILES
	include/rendering/Vertex.h
	include/rendering/GeometryStorage.h
	src/GeometryStorage.cpp
	include/rendering/GeometryStorageConfigurator.h
	src/GeometryStorageConfigurator.cpp	
	src/OBJFileLoader.cpp
	include/rendering/OBJFileLoader.h
	src/WaveFrontReader.h
	include/rendering/MeshGeometry.h
)
source_group( ${SOURCE_FILES_DIR}/Geometry/Storage FILES ${GEOMETRY_STORAGE_FILES} )

#GUI
set( GUI_FILES
	include/rendering/ViewController.h
	src/ViewController.cpp
	src/CameraView.h
	src/CameraView.cpp
	src/CameraViewModel.h
	src/CameraViewModel.cpp
	src/View.h
	src/ViewModel.h
	src/LightView.h
	src/LightView.cpp
	src/LightViewModel.h
	src/LightViewModel.cpp
	src/FogViewModel.h
	src/FogViewModel.cpp
	src/FogView.h
	src/FogView.cpp
	src/PrimitiveView.h
	src/PrimitiveView.cpp
	src/PrimitiveViewModel.h
	src/PrimitiveViewModel.cpp
)
source_group( ${SOURCE_FILES_DIR}//GUI FILES ${GUI_FILES} )

#helper
set( HELPERS_FILES
	src/DDSTextureLoader.h
	src/DDSTextureLoader.cpp
	include/rendering/MathHelper.h
	src/MathHelper.cpp
	src/UploadBuffer.h
	include/rendering/d3dUtil.h
	src/d3dUtil.cpp
	src/d3dx12.h
)
source_group( ${SOURCE_FILES_DIR}/Helpers FILES ${HELPERS_FILES} )

#shader helpers
set( SHADER_HELPERS_FILES
	src/PSOProvider.cpp
	src/PSOProvider.h
	src/PSOProviderConfigurator.cpp
	src/PSOProviderConfigurator.h
)
source_group( ${SOURCE_FILES_DIR}/ShaderHelpers FILES ${SHADER_HELPERS_FILES} )

#textures
set( TEXTURES_FILES
	include/rendering/SrvHeapProvider.h
	src/SrvHeapProvider.cpp
	src/Texture.h
	include/rendering/TexturesProvider.h
	src/TextureStaticSamplers.h
	src/TexturesProvider.cpp
	src/TextureStaticSamplers.cpp
)
source_group( ${SOURCE_FILES_DIR}/Textures FILES ${TEXTURES_FILES} )
                                                        
# ------------------------------------------------
#main target
add_library(Rendering
	${CORE_FILES}
	${DATA_PROVIDER_FILES}
	${DYNAMIC_VERTEX_FILES}
	${FRAME_RESOURCE_FILES}
	${GEOMETRY_STORAGE_FILES}
	${GEOMETRY_GENERATOR_FILES}
	${GUI_FILES}
	${HELPERS_FILES}
	${SHADER_HELPERS_FILES}
	${TEXTURES_FILES}
	${OTHER_FILES}
)

target_include_directories(Rendering PUBLIC include/rendering)

add_definitions(-DUNICODE -D_UNICODE)

# ------------------------------------------------
# add directX libraries
target_link_libraries(Rendering "d3d12.lib" "dxgi.lib" "d3dcompiler.lib")
target_link_libraries(Rendering ImGui)
target_link_libraries(Rendering entt)
target_link_libraries(Rendering Common)

# ------------------------------------------------
# copy resources
set(PROJECT_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)

#shades
add_custom_command(
    TARGET Rendering
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/rendering/shaders ${PROJECT_RESOURCE_DIR}/shaders
)

#models
add_custom_command(
    TARGET Rendering
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/rendering/models ${PROJECT_RESOURCE_DIR}/models
)

#textures
add_custom_command(
    TARGET Rendering
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/rendering/textures ${PROJECT_RESOURCE_DIR}/textures
)
