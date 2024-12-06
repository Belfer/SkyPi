cmake_minimum_required (VERSION 3.22)
message("CMake Version: ${CMAKE_VERSION}")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions (WINDOW_GLFW_BACKEND)
add_compile_definitions (GRAPHICS_OPENGL_BACKEND)
add_compile_definitions (DEBUG_BUILD)

add_compile_definitions (GAME_PATH="${CMAKE_CURRENT_SOURCE_DIR}/game")

add_subdirectory (extern)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

set (ENGINE_SRCS
	"src/engine/application.cpp"
	"src/engine/log.cpp"
	"src/engine/math.cpp"

	"src/engine/window.cpp"
	"src/engine/glfw/window_glfw.cpp"

	"src/engine/graphics.cpp"
	"src/engine/opengl/graphics_opengl.cpp"
)

set (ENGINE_LIBS fmt stb glm glad glfw wren imgui)

add_library (Engine STATIC ${ENGINE_SRCS})
target_link_libraries (Engine ${ENGINE_LIBS})
target_include_directories (Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory (game)