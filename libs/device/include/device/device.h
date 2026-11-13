#pragma once

#include "pico/stdlib.h"

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
    char board_id[17];
    uint32_t manufacturer;
    uint32_t part;
    uint32_t revision;
    char sdk_version[16];
} device_info_t;

void device_get_info(device_info_t *info);
