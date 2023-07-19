set(VORBISFILE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_library(vorbisfile STATIC
	"${VORBISFILE_SOURCE_DIR}/../vorbis/vorbisfile.c"
	"${VORBISFILE_SOURCE_DIR}/../vorbis/vorbisfile.def"
	"${VORBISFILE_SOURCE_DIR}/../vorbis/vorbisfile.h"
)

target_include_directories(vorbisfile PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")
