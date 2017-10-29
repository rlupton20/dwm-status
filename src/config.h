#ifndef _CONFIG_H
#define _CONFIG_H

/* Segments to show - set to 0 to disable */
#define SHOW_TEMPERATURE_SENSORS 1
#define SHOW_BATTERY_INFO 1

/* Time and date settings */
#define DATE_FMT "%H:%M:%S %D"

/* Battery status settings */
#define BATTERY_UPDATE_INTERVAL 5
#define BATTERY_PATH "/sys/class/power_supply/" BATTERY_ID "/"
#define CAPACITY_FILE "capacity"
#define POWER_USAGE_FILE "power_now"
#define POWER_REMAINING_FILE "energy_now"
#define BATTERY_ID "BAT0"

/* Temperature sensor settings parameters */
#define TEMPERATURE_UPDATE_INTERVAL 1
#define TEMPERATURE_SENSOR_PATH "/sys/devices/platform/coretemp.0/hwmon/hwmon2/"

typedef struct sensor_t {
    char *name;
    char *file;
} sensor_t;

/* Names and files to use to source temperature information */
const sensor_t temperature_sensors[] = {{"Core 1", "temp2_input"},
                                        {"Core 2", "temp3_input"}};

#endif
