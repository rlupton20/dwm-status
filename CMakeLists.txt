cmake_minimum_required (VERSION 3.6)
project (dwm-status C)
# Set versions
set (dwm_status_VERSION_MAJOR 0)
set (dwm_status_VERSION_MINOR 1.1)

# Describe status executable
add_executable(status src/status.c)
target_link_libraries (status m)

# Install status executable
install (TARGETS status DESTINATION bin)
