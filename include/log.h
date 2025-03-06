#ifndef _OBJ_TO_BSP_LOGGER_H_
#define _OBJ_TO_BSP_LOGGER_H_

#include <stdio.h>

#define LOG_DEBUG_INFO(msg, ...) (printf ("[DEBUG]\t" msg, __VA_ARGS__))
#define LOG_ERROR(msg, ...) (fprintf (stderr, "[ERROR]\t" msg, __VA_ARGS__))
#define LOG_ERROR_MSG(msg) (fprintf (stderr, "[ERROR]\t" msg))
#define LOG_NOTE(msg) (printf ("[NOTE]\t" msg))

#endif /* _OBJ_TO_BSP_LOGGER_H_ */
