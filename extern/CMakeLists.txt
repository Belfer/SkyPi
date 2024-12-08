# FMT lib
add_subdirectory (fmt)

# STB lib
add_library (stb INTERFACE)
set_property(TARGET stb APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/stb)

# GLM lib
add_subdirectory (glm)

# Portaudio lib
add_subdirectory (portaudio)

# GLAD lib
add_subdirectory (glad)

# GLFW lib
add_subdirectory (glfw)

# Wren lib
add_library (wren STATIC
	"wren/src/optional/wren_opt_meta.c"
	"wren/src/optional/wren_opt_random.c"
	"wren/src/vm/wren_compiler.c"
	"wren/src/vm/wren_core.c"
	"wren/src/vm/wren_debug.c"
	"wren/src/vm/wren_primitive.c"
	"wren/src/vm/wren_utils.c"
	"wren/src/vm/wren_value.c"
	"wren/src/vm/wren_vm.c"
)
target_include_directories (wren PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/wren/src/include ${CMAKE_CURRENT_SOURCE_DIR}/wren/src/optional ${CMAKE_CURRENT_SOURCE_DIR}/wren/src/vm)

if (BUILD_EDITOR)
	# ImGui lib
	set(IMGUI_SRCS
		"imgui/imgui.cpp"
		"imgui/imgui_demo.cpp"
		"imgui/imgui_draw.cpp"
		"imgui/imgui_tables.cpp"
		"imgui/imgui_widgets.cpp"
		"imgui/misc/cpp/imgui_stdlib.cpp"
	)

	if (WINDOW_BACKEND STREQUAL "GLFW")
		list(APPEND IMGUI_SRCS "imgui/backends/imgui_impl_glfw.cpp")
		list(APPEND IMGUI_LIBS glfw)
	endif()

	if (GRAPHICS_BACKEND STREQUAL "OpenGL" OR GRAPHICS_BACKEND STREQUAL "OpenGLES")
		list(APPEND IMGUI_SRCS "imgui/backends/imgui_impl_opengl3.cpp")

		if (GRAPHICS_BACKEND STREQUAL "OpenGLES")
			find_package(OpenGL REQUIRED)
			list(APPEND IMGUI_LIBS ${OPENGL_LIBRARIES})
		endif()
	endif()

	add_library(imgui STATIC ${IMGUI_SRCS})
	target_link_libraries(imgui PRIVATE ${IMGUI_LIBS})
	target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

	if (GRAPHICS_BACKEND STREQUAL "OpenGLES")
		target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_ES3)
	endif()


	# ImPlot lib
	add_library (implot STATIC "implot/implot.cpp" "implot/implot_demo.cpp" "implot/implot_items.cpp")
	target_link_libraries (implot imgui)
	target_include_directories (implot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/implot)

	# ImGuizmo lib
	add_library (ImGuizmo STATIC "ImGuizmo/GraphEditor.cpp" "ImGuizmo/ImCurveEdit.cpp" "ImGuizmo/ImGradient.cpp" "ImGuizmo/ImGuizmo.cpp" "ImGuizmo/ImSequencer.cpp")
	target_link_libraries (ImGuizmo imgui)
	target_include_directories (ImGuizmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ImGuizmo)
endif()