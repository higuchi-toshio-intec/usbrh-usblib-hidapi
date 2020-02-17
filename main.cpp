#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usbrh-usblib-hidapi.h"

typedef struct _params {
    bool list_device;
    int selected_device;
    int display_mode;
    bool display_unit;
    int unit_temperature;
} PARAMS;

static char *UNIT_TEMPERATURE[2] = { "C", "F" };

/* * */
void print_usage(char *cmd) {
    char *cmdname = basename(cmd);
    printf("Usage: %s [OPTION]...\n"
               " -h      display this help and exit\n"
               " -rhr x  select humidity resolution\n"
               "           0 : 12bit [default]\n"
               "           1 :  8bit\n"
               " -tt x   select type of temperature\n"
               "           0 : C(Celsius) [default]\n"
               "           1 : F(Fahrenheit)\n"
               " -tr x   select temperature resolution\n"
               "           0 : 14bit [default]\n"
               "           1 : 12bit\n"
               " -ld     display list device(s)\n"
               " -sd x   select device (0 - N)\n"
               "           0 : all devices\n"
               "           N : Nth device found\n"
               "               [default] 1\n"
               " -dm     display mode\n"
               "           0 : h_val t_val\n"
               "           1 : H=h_val T=t_val  [default]\n"
               "           2 : Humidity=h_val Temperature=t_val\n"
               "           3 : Humidity=h_val\n"
               "               Temperature=t_val\n"
               " -du     display unit(%%/C/F)\n"
               " -d x    log level (0-255)\n"
               "           [default] 0\n"
               "", cmdname);
    return;
}

/* * */
void parse_args(int argc, char *argv[], UsbrhUsblibHidapi *usbrh, PARAMS *params) {

    // set defaults
    params->list_device = false;
    params->selected_device = 1;
    params->display_mode = 0;
    params->display_unit = false;
    params->unit_temperature = 0;

    //
    for(int i=1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            exit(0);
        }
        else if (strcmp(argv[i], "-rhr") == 0) {
            int val = 0;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if ((0 <= val) && (val <= 1)) {
                usbrh->setHumidityResolution(val);
            }
        }
        else if (strcmp(argv[i], "-tt") == 0) {
            int val = 0;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if ((0 <= val) && (val <= 1)) {
                usbrh->setTemperatureMode(val);
                params->unit_temperature = val;
            }
        }
        else if (strcmp(argv[i], "-tr") == 0) {
            int val = 0;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if ((0 <= val) && (val <= 1)) {
               usbrh->setTemperatureResolution(val);
            }
        }
        else if (strcmp(argv[i], "-ld") == 0) {
            params->list_device = true;
        }
        else if (strcmp(argv[i], "-sd") == 0) {
            int val = 1;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if (0 <= val) {
                params->selected_device = val;
            }
        }
        else if (strcmp(argv[i], "-dm") == 0) {
            int val = 1;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if (0 <= val) {
                params->display_mode = val;
            }
        }
        else if (strcmp(argv[i], "-du") == 0) {
            params->display_unit = true;
        }
        else if (strcmp(argv[i], "-d") == 0) {
            int val = 0;
            if ((i + 1 != argc) && (strncmp(argv[i + 1], "-", 1) != 0)) {
                val = atoi(argv[i + 1]);
                i++;
            }
            if (0 <= val) {
                usbrh->setLogLevel(val);
            }
        }
    }

    return;
}

/* * */
int main(int argc, char* argv[]) {

    //
    UsbrhUsblibHidapi *usbrh = new UsbrhUsblibHidapi();
    PARAMS params;
    parse_args(argc, argv, usbrh, &params);

    if (params.list_device == true) {
        RHT_PATH listPath[10];
        int n = usbrh->listPath(10, listPath);
        for (int i = 0; i < n; i++) {
            printf("%s\n", listPath[i].path);
        }
        exit(0);
    }

    //
    RHT_VALUE rht_value;
    int st = params.selected_device;
    int ed = params.selected_device;
    if (params.selected_device == 0) {
        int n = usbrh->countPath();
        st = 1;
        ed = n;
    }

    //
    char *unit_h = "";
    char *unit_t = "";
    if (params.display_unit == true) {
        unit_h = "%";
        unit_t = UNIT_TEMPERATURE[0];
        if (params.unit_temperature != 0) {
            unit_t = UNIT_TEMPERATURE[1];
        }
    }
    for (int i = st; i <= ed; i++) {
        usbrh->get(i, &rht_value);
        switch (params.display_mode) {
        case 0:
            printf("%f%s %f%s\n", rht_value.humidity, unit_h, rht_value.temperature, unit_t);
            break;
        case 2:
            printf("Humidity=%f%s Temperature=%f%s\n", rht_value.humidity, unit_h, rht_value.temperature, unit_t);
            break;
        case 3:
            printf("Humidity=%f%s\n", rht_value.humidity, unit_h);
            printf("Temperature=%f%s\n", rht_value.temperature, unit_t);
            break;
        case 1:
        default:
            printf("H=%f%s T=%f%s\n", rht_value.humidity, unit_h, rht_value.temperature, unit_t);
            break;
        }
    }

    return 0;
}

/* eof */
