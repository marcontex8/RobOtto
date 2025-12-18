/*
 * buffer_parser.c
 *
 *  Created on: Dec 14, 2025
 *      Author: marco
 */
#include <string.h>

#include "buffer_parser.h"



const char* findLineInBuffer(const char* buffer, const char* target)
{
    const char* line_start = buffer;
    const char* p = buffer;
    size_t target_len = strlen(target);

    while (*p != '\0')
    {
        /* Line ending detected */
        if (*p == '\n' || *p == '\r')
        {
            size_t line_len = (size_t)(p - line_start);

            /* Compare length first, then content */
            if (target_len <= line_len &&
                strncmp(line_start, target, target_len) == 0)
            {
                return line_start;
            }

            /* Skip CRLF or LFCR */
            if ((*p == '\r' && *(p + 1) == '\n') ||
                (*p == '\n' && *(p + 1) == '\r'))
            {
                p++;
            }

            p++;
            line_start = p;
            continue;
        }

        p++;
    }

    /* Check last line (if buffer does not end with newline) */
    if (p != line_start)
    {
        size_t line_len = (size_t)(p - line_start);

        if (target_len <= line_len &&
            strncmp(line_start, target, target_len) == 0)
        {
            return line_start;
        }
    }

    return NULL;  /* Not found */
}
