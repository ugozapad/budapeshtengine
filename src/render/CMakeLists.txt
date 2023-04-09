file(GLOB RENDER_SRC_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDER_SRC_FILES})

add_library(render STATIC ${RENDER_SRC_FILES})
target_include_directories(render PRIVATE "${CMAKE_SOURCE_DIR}/src"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/sokol"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/stb"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/SDL2-2.26.5/include")