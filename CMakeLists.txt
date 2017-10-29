project("dwm-status")

add_executable(status src/status.c)
target_link_libraries(status m)
