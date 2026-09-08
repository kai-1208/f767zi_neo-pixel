add_library(UserLibs STATIC
    neo-pixel/neo-pixel.c
)

target_include_directories(UserLibs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/neo-pixel
)

target_link_libraries(UserLibs PUBLIC stm32cubemx)