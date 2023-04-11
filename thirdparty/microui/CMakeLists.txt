set(MICROUI_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/microui.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/microui.h"
)

add_library(microui STATIC ${MICROUI_SRC})
target_include_directories(microui PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")