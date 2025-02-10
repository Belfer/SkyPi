project (SkyPi)

set (BX_GAME_NAME "${PROJECT_NAME}")
set (BX_GAME_PATH "${CMAKE_CURRENT_SOURCE_DIR}/game")
set (BX_GAME_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
set (BX_GAME_ICON_PATH "${CMAKE_CURRENT_SOURCE_DIR}/game/assets/icon/bx.rc")

set (BX_GAME_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/game.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/terrain.cpp"
)

set (BX_GAME_EDITOR_SRCS
	"${CMAKE_CURRENT_SOURCE_DIR}/src/editor/game_editor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/editor/terrain_view.cpp"
)

set (BX_GAME_RUNTIME_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/runtime.cpp")

set (BX_GAME_LIBS )

add_subdirectory (extern)