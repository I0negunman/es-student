add_library(profiling STATIC
    src/profiling.c
)

target_include_directories(profiling PUBLIC
    include
)

target_link_libraries(profiling PRIVATE
    pico_stdlib
)

target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1
)
