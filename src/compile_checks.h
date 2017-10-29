#ifndef _COMPILE_CHECKS_H
#define _COMPILE_CHECKS_H

/* Compile time check macro */
#define IS_VALID_INTERVAL(i)                                                   \
    static_assert(0 < (i) && (i) < MAX_INTERVAL,                               \
                  #i " is not a valid interval; failed 0 < " #i " && " #i      \
                     " < MAX_INTERVAL")

/* Compile time assertions */
IS_VALID_INTERVAL(BATTERY_UPDATE_INTERVAL);
IS_VALID_INTERVAL(TEMPERATURE_UPDATE_INTERVAL);
static_assert(MAX_DATE_LENGTH > sizeof(DATE_ERR), "MAX_DATE_LENGTH too small");

#endif
