/*
 * robotto_common.c
 *
 *  Created on: Dec 14, 2025
 *      Author: marco
 */
#include <string.h>

#include "SEGGER_SYSVIEW.h"

void SYSVIEW_PrintLines(const char *buf, char *line, size_t line_size)
{
    const char *line_start = buf;
    const char *p = buf;

    while (*p != '\0')
    {
        if (*p == '\n' || *p == '\r')
        {
            size_t len = p - line_start;

            if (len > 0 && len < line_size)
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
    if (len > 0 && len < line_size)
    {
        memcpy(line, line_start, len);
        line[len] = '\0';
        SEGGER_SYSVIEW_PrintfHost("New Data Line: %s", line);
    }
}
