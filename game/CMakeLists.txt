project (SkyPi)

set (GAME_SRCS
	"src/runtime.cpp"
	"src/game.cpp"
	"src/terrain.cpp"
)

if (BUILD_EDITOR)
	set (GAME_SRCS ${GAME_SRCS}
		"src/editor/game_editor.cpp"
		"src/editor/terrain_view.cpp"
	)
endif()

set (GAME_LIBS Engine)

add_library (Game STATIC ${GAME_SRCS})
target_link_libraries (Game ${GAME_LIBS})
target_include_directories (Game PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#if(WIN32)
#	add_executable (bx-editor "assets/icon.rc" ${GAME_SRCS})
#elseif(APPLE)
#elseif(UNIX)
#else()
	add_executable (SkyPi "src/runtime")
#endif()

target_link_libraries (SkyPi Game)