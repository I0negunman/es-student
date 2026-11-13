#include "device/device.h"
#include <stddef.h>
#include <stdio.h>
#include "pico/unique_id.h"
#include "pico/version.h"
#include "hardware/regs/addressmap.h"
#include "hardware/structs/sysinfo.h"
#include "string.h"
#include "pico/unique_id.h"

void device_get_info(device_info_t *info) {
    volatile uint32_t *chip_id = (uint32_t *)(SYSINFO_BASE + SYSINFO_CHIP_ID_OFFSET);
    uint32_t id = *chip_id;

    info->manufacturer = (id & SYSINFO_CHIP_ID_MANUFACTURER_BITS)
                        >> SYSINFO_CHIP_ID_MANUFACTURER_LSB;

    info->part = (id & SYSINFO_CHIP_ID_PART_BITS)
                >> SYSINFO_CHIP_ID_PART_LSB;

    info->revision = (id & SYSINFO_CHIP_ID_REVISION_BITS)
                    >> SYSINFO_CHIP_ID_REVISION_LSB;
    pico_get_unique_board_id_string(info->board_id, sizeof(info->board_id));
    strcpy(info->sdk_version, PICO_SDK_VERSION_STRING);
}
