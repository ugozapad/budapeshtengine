file(GLOB MAIN_SRC_FILES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MAIN_SRC_FILES})

add_executable(main ${MAIN_SRC_FILES})
target_include_directories(main PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(main engine sokol_rdev)

set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/"
									  OUTPUT_NAME "budapest_gl"
									  DEBUG_POSTFIX "_d")