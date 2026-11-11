#pragma once

#include "pico/unique_id.h"

#define DEVICE_NAME "es-led-module"
#define FIRMWARE_VERSION "1.0.0"

#define DEVICE_PROJECT "231-firmware-template"
#define DEVICE_REPO "https://github.com/I0negunman/es-student"

#ifndef DEVICE_BOARD
#define DEVICE_BOARD "unknown"
#endif

void info(void);

typedef struct
{
    uint32_t version;
    char name[13];
    uint8_t revision; 
} info_t;

typedef struct{
    char name[13];
    char board_id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char sdk_version[16];
} device_info_t;