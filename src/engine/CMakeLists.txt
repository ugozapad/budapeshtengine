file(GLOB ENGINE_SRC_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SRC_FILES})

add_library(engine STATIC ${ENGINE_SRC_FILES})
target_precompile_headers(engine PRIVATE "pch.h")
target_compile_definitions(engine PRIVATE -DENABLE_PHYSFS -DENGINE_EXPORTS)
target_include_directories(engine PRIVATE "${CMAKE_SOURCE_DIR}/src"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/SDL2-2.26.5/include"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/glm"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/stb")

target_link_libraries(engine SDL2 physfs)