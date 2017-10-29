#ifndef _LOGGING_H
#define _LOGGING_H

#include <stdio.h>

/* Simple logging macros */
#define INFO(str, ...) fprintf(stderr, "[INFO] " str "\n", ##__VA_ARGS__)
#define ERROR(str, ...)                                                        \
    fprintf(stderr, "[ERROR] (%s, line %d) " str "\n", __FILE__, __LINE__,     \
            ##__VA_ARGS__)
/* DEBUG logging is defined only when DEBUG_MODE is enabled */
#ifdef DEBUG_MODE
#define DEBUG(str, ...)                                                        \
    fprintf(stderr, "[DEBUG] (%s, line %d) " str "\n", __FILE__, __LINE__,     \
            ##__VA_ARGS__)
#else
#define DEBUG(...)
#endif

#endif
