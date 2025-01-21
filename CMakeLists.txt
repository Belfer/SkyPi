project (SkyPi)

add_compile_definitions (GAME_PATH="${CMAKE_CURRENT_SOURCE_DIR}/game")

add_subdirectory (extern)

set (GAME_SRCS
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

# TODO: We need to make this get copied to the exe bin from the engine lib (HOW?? Idk)
#if (WIN32)
#	# We need to copy steam dll to the exe bin on windows
#	if (ONLINE_BACKEND STREQUAL "Steam" AND STEAMWORKS_FOUND)
#		# Iterate through all executables and copy the DLL to each one
#		add_custom_command(
#			TARGET SkyPi POST_BUILD
#			COMMAND ${CMAKE_COMMAND} -E copy_if_different
#				${STEAMWORKS_DLL_PATH} $<TARGET_FILE_DIR:Game>
#			COMMENT "Copying Steamworks DLL to the executable output directory"
#		)
#	endif()
#endif()