/* dwm-status */
/* Generates status information at suitable intervals
 * more efficiently than a bash script. Written in C
 * to be in keeping with dwm's minimalist and simple
 * philosophy.
 */

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

/* Turn on debug logging by defining DEBUG_MODE */
/* #define DEBUG_MODE */

/* Internal boundaries and settings */
#define MAX_INTERVAL 60
#define MAX_DATE_LENGTH 64
#define DATE_ERR "Error getting date"

const uint8_t num_sensors = sizeof temperature_sensors / sizeof(sensor_t);

#include "compile_checks.h"
#include "logging.h"

/* Program structures */
typedef struct hours_and_minutes_t {
    uint8_t hours;
    uint8_t minutes;
} hours_and_minutes_t;

typedef struct battery_status_t {
    uint8_t percent;
    hours_and_minutes_t time_left;
} battery_status_t;

/* Function declarations */
void log_source_information();
uint8_t get_battery_status(battery_status_t *status);
uint8_t get_date_time(char *buffer, uint8_t max_len);
uint8_t read_file_as_int(char *filename, int64_t *result);
hours_and_minutes_t hours_to_hours_and_minutes(float hours);
uint8_t decimal_to_minutes(float hours);
uint8_t get_temperatures(int16_t *temperatures);

int main(int argc, char *argv[argc]) {
    battery_status_t battery = {0};
    int16_t temperatures[num_sensors];
    char date_time[MAX_DATE_LENGTH] = {0};

    DEBUG("Disabling buffering on stdout");
    setbuf(stdout, NULL);
    DEBUG("Buffering on stdout disabled");
    log_source_information();

    for (uint32_t i = 0; i < MAX_INTERVAL + 1; i = (i + 1) % MAX_INTERVAL) {
#if SHOW_BATTERY_INFO != 0
        if (i % BATTERY_UPDATE_INTERVAL == 0) {
            get_battery_status(&battery);
        }
#endif
#if SHOW_TEMPERATURE_SENSORS != 0
        if (i % TEMPERATURE_UPDATE_INTERVAL == 0) {
            get_temperatures(temperatures);
        }
#endif

        if (get_date_time(date_time, sizeof date_time) < 0) {
            strncpy(date_time, DATE_ERR, MAX_DATE_LENGTH);
        }

/* Print sensor information */
#if SHOW_TEMPERATURE_SENSORS != 0
        fprintf(stdout, "(");
        for (int n = 0; n < num_sensors; ++n) {
            if (n + 1 == num_sensors)
                fprintf(stdout, "%d)\u00B0C", temperatures[n]);
            else
                fprintf(stdout, "%d|", temperatures[n]);
        }
#endif

/* Print battery information */
#if SHOW_BATTERY_INFO != 0
        fprintf(stdout, " %d%%(%d hours %d mins)", battery.percent,
                battery.time_left.hours, battery.time_left.minutes);
#endif
        /* Print date and time */
        fprintf(stdout, " %s\n", date_time);

        /* Sleep */
        sleep(1);
    }

    return 0;
}

/* get_battery_status
 * Gets the battery statistics from the filesystem, and
 * translates them into a battery structure describing the
 * status of the battery. Returns 0 on success, and positive
 * integers otherwise.
 */
uint8_t get_battery_status(battery_status_t *status) {
    int64_t capacity, energy, power;

    if (read_file_as_int(BATTERY_PATH CAPACITY_FILE, &capacity)) {
        ERROR("error obtaining remaining battery capacity");
        capacity = 0;
        return 1;
    } else if (read_file_as_int(BATTERY_PATH POWER_REMAINING_FILE, &energy)) {
        ERROR("error obtaining remaining battery energy");
        energy = 1;
        return 2;
    } else if (read_file_as_int(BATTERY_PATH POWER_USAGE_FILE, &power)) {
        ERROR("error obtaining battery power usage");
        power = 1;
        return 3;
    } else if (power == 0) {
        INFO("Power read of 0. Ignoring and continuing.");
        return 4;
    } else {
        status->percent = (uint8_t)capacity;
        status->time_left =
            hours_to_hours_and_minutes((float)energy / (float)power);
        return 0;
    }
}

/* get_date_time
 * Fill the passed buffer with a formatted string representing
 * the current local date and time. Returns 0 on success, and
 * positive otherwise.
 */
uint8_t get_date_time(char *buffer, uint8_t max_len) {
    time_t now;
    if ((now = time(NULL)) < 0) {
        ERROR("get_date_time: error getting time");
        return 1;
    } else {
        struct tm *now_local = localtime(&now);
        if (strftime(buffer, max_len, DATE_FMT, now_local) == 0) {
            ERROR("get_date_time: ran out of space in date format buffer");
            return 2;
        } else {
            return 0;
        }
    }
}

/* read_file_as_int
 * Take a filename and write it's integer value to the result
 * pointer passed. Returns 0 on success, and positive integers
 * otherwise.
 */
uint8_t read_file_as_int(char *filename, int64_t *result) {
    FILE *fp;
    char *buf = NULL;
    size_t len = 0;

    if ((fp = fopen(filename, "r")) == NULL) {
        ERROR("Error opening %s", filename);
        return 1;
    } else {
        DEBUG("Opened %s for reading", filename);
        if (getline(&buf, &len, fp) < 1) {
            ERROR("Error reading capacity from %s", filename);
            fclose(fp);
            return 2;
        } else {
            fclose(fp);
            *result = atoi(buf);
            return 0;
        }
    }
}

/* hours_to_hours_and_minutes
 * converts the passed number of hours into an hours and minutes
 * structure
 */
hours_and_minutes_t hours_to_hours_and_minutes(float hours) {
    uint8_t whole_hours = floor(hours);
    return (hours_and_minutes_t){
        whole_hours, (uint8_t)decimal_to_minutes(hours - whole_hours)};
}

/* decimal_to_minutes
 * Takes a fraction of an hour (passed as a decimal), and converts
 * to minutes. For example 0.5 hours is converted to 30 minutes
 */
uint8_t decimal_to_minutes(float hours) {
    assert(0 <= hours && hours < 1);
    return floor(hours * 60);
}

/* get_temperatures
 * Read temperatures from each file specified in the
 * temperature_sensors array, and write it to the passed
 * temperatures array.
 */
uint8_t get_temperatures(int16_t *temperatures) {
    int64_t temperature;
    char filename[1024];
    for (int i = 0; i < num_sensors; ++i) {
        strcpy(filename, TEMPERATURE_SENSOR_PATH);
        strcat(filename, temperature_sensors[i].file);
        if (!read_file_as_int(filename, &temperature))
            temperatures[i] = (int16_t)(temperature / 1000);
        else {
            ERROR("failed to read temperature for %s",
                  temperature_sensors[i].name);
            temperatures[i] = -999;
        }
    }
    return 0;
}

/* log_source_infomration
 * Writes information about the sources of data to stderr.
 */
void log_source_information() {
#if SHOW_BATTERY_INFO != 0
    INFO("Reading battery details from %s", BATTERY_PATH);
#endif
#if SHOW_TEMPERATURE_SENSORS != 0
    for (int i = 0; i < num_sensors; ++i)
        INFO("Reading temperature information for %s from %s%s",
             temperature_sensors[i].name, TEMPERATURE_SENSOR_PATH,
             temperature_sensors[i].file);
#endif
}
