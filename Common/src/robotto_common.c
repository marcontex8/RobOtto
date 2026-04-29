/*
 * Copyright (c) 2026 Marco Contessotto
 * All rights reserved.
 */
#include <string.h>

#include "FreeRTOS.h"
#include "SEGGER_SYSVIEW.h"
#include "task.h"

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // nothing
}

#ifdef DEBUG

#define LINE_SIZE 128
void SYSVIEW_PrintLines(const char *buf)
{
    static char line[LINE_SIZE];

    const char *line_start = buf;
    const char *p = buf;

    while (*p != '\0')
    {
        if (*p == '\n' || *p == '\r')
        {
            size_t len = p - line_start;

            if (len > 0 && len < LINE_SIZE)
            {
                memcpy(line, line_start, len);
                line[len] = '\0';
                SEGGER_SYSVIEW_PrintfHost("New Data Line: %s", line);
            }

            /* Skip \r\n */
            if (*p == '\r' && *(p + 1) == '\n')
            {
                p++;
            }

            p++;
            line_start = p;
        }
        else
        {
            p++;
        }
    }

    /* Handle last line if no newline at end */
    size_t len = p - line_start;
    if (len > 0 && len < LINE_SIZE)
    {
        memcpy(line, line_start, len);
        line[len] = '\0';
        SEGGER_SYSVIEW_PrintfHost("New Data Line: %s", line);
    }
}

#endif
