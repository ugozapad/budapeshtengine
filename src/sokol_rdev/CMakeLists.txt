file(GLOB SOKOL_RDEV_SRC_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RENDER_SRC_FILES})

add_library(sokol_rdev STATIC ${RENDER_SRC_FILES})
target_include_directories(sokol_rdev PRIVATE "${CMAKE_SOURCE_DIR}/src"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/sokol"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/stb"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/SDL2-2.26.5/include")

target_link_libraries(sokol_rdev microui)