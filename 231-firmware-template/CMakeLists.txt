cmake_minimum_required(VERSION 3.15)

set(PICO_BOARD pico)
set(PICO_PLATFORM rp2040)

include(pico_sdk_import.cmake)

project(231_firmware_template)
pico_sdk_init()

add_compile_options(-Wall)

# Подключаем внешние библиотеки
add_subdirectory(../libs ${CMAKE_BINARY_DIR}/libs)


# ==========================================
# PLATFORM: взаимодействие с железом
# ==========================================


add_library(platform STATIC
    platform/platform.c
)

target_include_directories(platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(platform PRIVATE
    pico_stdlib
    pico_unique_id
)


# ==========================================
# LOGIC: логика прибора
# ==========================================

add_library(logic STATIC
    logic/api/api.c
    logic/api/api-callbacks.c
    logic/led-task/led-task.c
    logic/button-task/button-task.c
    logic/pi-task/pi-task.c
)

target_include_directories(logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/logic
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(logic
    PUBLIC
        stdio-text-protocol
    PRIVATE
        platform
        device
        profiling
        logging
        systime
)


# ==========================================
# EXECUTABLE: главная программа
# ==========================================

add_executable(${PROJECT_NAME}
    main.c
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    logic
    profiling
    logging
    pico_stdlib
)

target_compile_definitions(device PUBLIC
    DEVICE_BOARD="${PICO_BOARD}"
)

target_compile_definitions(profiling PUBLIC
    PROFILING_ENABLED=1
)

pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

pico_add_extra_outputs(${PROJECT_NAME})
