file(GLOB SOUND_SRC_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOUND_SRC_FILES})

add_library(sound SHARED ${SOUND_SRC_FILES})
target_precompile_headers(sound PRIVATE "pch.h")
target_include_directories(sound  PRIVATE "${CMAKE_SOURCE_DIR}/src"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty"
								  PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/stb"
								  PUBLIC "${CMAKE_SOURCE_DIR}/thirdparty/SDL2-2.26.5/include")
								  
target_link_directories(sound PRIVATE "${CMAKE_SOURCE_DIR}/thirdparty/openal")
target_link_libraries(sound engine ogg vorbis vorbisfile)