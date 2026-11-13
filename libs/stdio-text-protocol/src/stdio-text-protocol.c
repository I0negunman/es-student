#include "stdio-text-protocol/stdio-text-protocol.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

char line[STDIO_TEXT_PROTOCOL_LINE_SIZE];
uint line_length = 0;

static command_t command = {0};

const command_t *stdio_text_protocol_handle(void)
{   
    int symbol = getchar_timeout_us(0);

    if (symbol == PICO_ERROR_TIMEOUT)
    {
        return NULL;
    }

    if (symbol == '\r' || symbol == '\n')
    {
        putchar('\n');
        line[line_length] = '\0';

        if (line_length > 0)
        {
            command = (command_t){0};
                for (int p = 0; p < line_length - 1; p++)
                {
                    if (line[p] == ' ')
                    {
                        line[p] = '\0';
                        if (line[p + 1] != ' ')
                        {
                            if (command.argc < STDIO_TEXT_PROTOCOL_MAX_ARGS)
                            {
                                if (command.name != 0) {
                                    command.argv[command.argc] = &line[p + 1];
                                    command.argc++;
                                }
                            }
                            else
                            {
                                command.truncated = true;
                                return &command;
                            }
                        }   
                    }
                    else {
                        if (command.name == 0)
                        {
                            command.name = &line[p];
                        }
                    }
                }
            line_length = 0;
            return &command;
        }
        return NULL;
    }

    if ((symbol == 0x08 || symbol == 0x7f) && line_length > 0){
        line[line_length] = '\0';
        line_length -= 1;
        printf("\b \b");
        return NULL;
    }

    if (line_length + 1 < STDIO_TEXT_PROTOCOL_LINE_SIZE)
    {
        line[line_length] = (char)symbol;
        line_length = line_length + 1;
        putchar(symbol);
        return NULL;
    }

    else 
    {
        line_length = 0;
        command.truncated = true;
        return &command;
    }



    // конец строки: пустую строку пропускаем, иначе переводим строку в терминале,
    // делим строку на слова и возвращаем команду

    // Backspace (0x08 или 0x7f): убираем последний символ и стираем его на экране "\b \b"

    // печатный символ: кладём в буфер и повторяем в терминал;
    // буфер полон — помечаем строку как переполненную
}
