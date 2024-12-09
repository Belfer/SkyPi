cmake_minimum_required (VERSION 3.16)
message("CMake Version: ${CMAKE_VERSION}")

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

option (BUILD_EDITOR "Build as editor binaries" ON)
option (BUILD_TESTS "Build as editor binaries" ON)

set (WINDOW_BACKEND "GLFW" CACHE STRING "Choose the window backend: GLFW")
set_property (CACHE WINDOW_BACKEND PROPERTY STRINGS "GLFW")
message ("Window backend: ${WINDOW_BACKEND}")

set (GRAPHICS_BACKEND "OpenGL" CACHE STRING "Choose the graphics backend: OpenGL, OpenGLES")
set_property (CACHE GRAPHICS_BACKEND PROPERTY STRINGS "OpenGL" "OpenGLES")
message ("Graphics backend: ${GRAPHICS_BACKEND}")

set (AUDIO_BACKEND "PortAudio" CACHE STRING "Choose the audio backend: PortAudio")
set_property (CACHE AUDIO_BACKEND PROPERTY STRINGS "PortAudio")
message ("Audio backend: ${AUDIO_BACKEND}")

message ("CMake build type: ${CMAKE_BUILD_TYPE}")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_compile_definitions (DEBUG_BUILD)
endif()

if (BUILD_EDITOR)
	add_compile_definitions (EDITOR_BUILD)
endif()

add_compile_definitions (GRAPHICS_OPENGLES_BACKEND)

add_compile_definitions (GAME_PATH="${CMAKE_CURRENT_SOURCE_DIR}/game")

add_subdirectory (extern)

set (ENGINE_SRCS
	"src/engine/application.cpp"
	"src/engine/graphics.cpp"
	"src/engine/log.cpp"
	"src/engine/math.cpp"
	"src/engine/profiler.cpp"
	"src/engine/uuid.cpp"
	"src/engine/window.cpp"

	"src/framework/camera.cpp"
)

set (ENGINE_LIBS fmt stb glm wren)

if (WINDOW_BACKEND STREQUAL "GLFW")
	add_compile_definitions (WINDOW_GLFW_BACKEND)
    set (ENGINE_SRCS ${ENGINE_SRCS} "src/engine/glfw/window_glfw.cpp")
    set (ENGINE_LIBS ${ENGINE_LIBS} glfw)

else ()
    message(FATAL_ERROR "Unknown window backend: ${WINDOW_BACKEND}")
endif ()

if (GRAPHICS_BACKEND STREQUAL "OpenGL")
	add_compile_definitions (GRAPHICS_OPENGL_BACKEND)
    set (ENGINE_SRCS ${ENGINE_SRCS} "src/engine/opengl/graphics_opengl.cpp")
    set (ENGINE_LIBS ${ENGINE_LIBS} glad)

elseif (GRAPHICS_BACKEND STREQUAL "OpenGLES")
	add_compile_definitions (GRAPHICS_OPENGLES_BACKEND)
    set (ENGINE_SRCS ${ENGINE_SRCS} "src/engine/opengl/graphics_opengles.cpp")
    set (ENGINE_LIBS ${ENGINE_LIBS} glad)

else ()
    message(FATAL_ERROR "Unknown graphics backend: ${GRAPHICS_BACKEND}")
endif ()

if (BUILD_EDITOR)
	set (ENGINE_SRCS ${ENGINE_SRCS}
		"src/editor/editor.cpp"
		"src/editor/inspector.cpp"
	)
	set (ENGINE_LIBS ${ENGINE_LIBS} imgui implot ImGuizmo)
endif()

add_library (Engine STATIC ${ENGINE_SRCS})
target_link_libraries (Engine ${ENGINE_LIBS})
target_include_directories (Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (BUILD_TESTS)
	enable_testing()
	#add_subdirectory(tests)
endif ()

add_subdirectory (game)