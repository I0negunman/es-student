#include <stdio.h>
#include <stdint.h>
#include "hardware/regs/addressmap.h"
#include "pico/stdlib.h"

#define SRAM_SIZE_BYTES (264*1024)
#define ROM_SIZE_BYTES (16*1024)

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;

static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}

void mem_info(void)
{
    printf("area       start      end        size\n");

    row("flash", (uintptr_t)XIP_BASE, (uintptr_t)(XIP_BASE + PICO_FLASH_SIZE_BYTES));
    row("sram", (uintptr_t)SRAM_BASE, (uintptr_t)(SRAM_BASE +  SRAM_SIZE_BYTES));
    row("rom", (uintptr_t)ROM_BASE, (uintptr_t)(ROM_BASE + ROM_SIZE_BYTES));

    row("image",(uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    row("free", (uintptr_t)&__flash_binary_end, (uintptr_t)(XIP_BASE + PICO_FLASH_SIZE_BYTES));
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    row("data flash", (uintptr_t)&__etext, (uintptr_t)&__etext + (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__);
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);

    printf("total\n");
    printf("  flash image     %8u = boot2 %8u + text %8u + data %8u\n", 
        ((uintptr_t)&__flash_binary_end - (uintptr_t)&__flash_binary_start), 
        ((uintptr_t)&__boot2_end__ - (uintptr_t)&__boot2_start__), 
        ((uintptr_t)&__etext - (uintptr_t)&__boot2_end__), 
        ((uintptr_t)&__data_end__ - (uintptr_t)&__data_start__));
    printf("  flash free %8u of %8u\n",  (uintptr_t)(XIP_BASE + PICO_FLASH_SIZE_BYTES - (uintptr_t)&__flash_binary_end), (uintptr_t)PICO_FLASH_SIZE_BYTES );
    printf("  ram used %8u = data %8u + bss %8u\n",  (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__ + (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__, (uintptr_t)&__data_end__ - (uintptr_t)&__data_start__, (uintptr_t)&__bss_end__ - (uintptr_t)&__bss_start__);
    printf("  ram free %8u for heap and %8u for stack",  (uintptr_t)&__HeapLimit - (uintptr_t)&__bss_end__, (uintptr_t)&__StackTop - (uintptr_t)&__StackBottom);
    // итог: образ во флеш и из чего он сложился
    // итог: свободно во флеш-памяти из всего её объёма
    // итог: занято в ОЗУ — .data и .bss
    // итог: свободно в ОЗУ — под кучу и под стек
}
