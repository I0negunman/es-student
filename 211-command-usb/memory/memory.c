#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "hardware/regs/addressmap.h"
#include "hardware/regs/sysinfo.h"
#include "pico/stdlib.h"
#include "command.h"
#include "device.h"
#include "led.h"


#define VECTOR_TABLE 0x10000100
#define SRAM_SIZE_BYTES (264*1024)
#define ROM_SIZE_BYTES (16*1024)

int main(void);
uint32_t data_variable = 100;
uint32_t bss_variable;

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

void fw_info(void) {
   uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u);
   uint16_t *fw_info_code  = (uint16_t *)((uintptr_t)fw_info & ~1u);
   data_variable++;
   bss_variable++;
   uint32_t stack_variable = 1946;
   uint32_t *heap_variable = malloc(sizeof(uint32_t));

   if (heap_variable != NULL)
   {
        *heap_variable = 1951;
   }
   printf("object address value\n");
   printf("main 0x%08x 0x%04x\n", main, *main_code);
   printf("fw_info 0x%08x 0x%04x\n", fw_info, *fw_info_code);
   printf("commands 0x%08x\n", &commands);
   for (int i = 0; i < command_count; i++) {
      printf("- %s 0x%08x\n", commands[i].name, commands[i].handler);
   }
   printf("DEVICE_PROJECT 0x%08x %s\n", &DEVICE_PROJECT, DEVICE_PROJECT);
   printf("DEVICE_BOARD 0x%08x %s\n", &DEVICE_BOARD, DEVICE_BOARD);
   printf("data_variable 0x%08x %d\n", &data_variable, data_variable);
   printf("bss_variable 0x%08x %d\n", &bss_variable, bss_variable);
   printf("stack_variable 0x%08x %d\n", &stack_variable, stack_variable);
   printf("heap_variable 0x%08x %d\n", heap_variable, *heap_variable);
   free(heap_variable);
}


void boot_info(void) {
    const uint32_t *vectors = (const uint32_t *)VECTOR_TABLE;
    uint32_t stack_top = vectors[0];
    uint32_t reset_handler = vectors[1];
    printf("vector table 0x%08x\n", vectors);
    printf("  stack top 0x%08x\n", stack_top);
    printf("  reset 0x%08x\n", reset_handler);
    printf("  reset (even) 0x%08x\n", reset_handler & ~1u);

    volatile uint32_t *gpio_in = (uint32_t *)(SIO_BASE + SIO_GPIO_IN_OFFSET);
    volatile uint32_t level = (*gpio_in >> led_pin()) & 1u;
    printf("gpio in    0x%08x\n", SIO_BASE + SIO_GPIO_IN_OFFSET);
    printf("  led bit  %d\n", level);
    printf("  gpio_get %d\n", gpio_get(led_pin()));
}
