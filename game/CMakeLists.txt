project (SkyPi)

add_executable (SkyPi "src/main.cpp")
target_link_libraries (SkyPi Engine)
target_include_directories (SkyPi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)