cmake_minimum_required(VERSION 3.1)

add_library (glad STATIC "src/glad.c")
if (UNIX)
	target_link_libraries (glad dl)
endif()
target_include_directories (glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)