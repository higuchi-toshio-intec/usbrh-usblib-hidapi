#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hidapi.h"
#include "usbrh-usblib-hidapi.h"

#define SIZE_BUFF                7
#define TIMEOUT_HID_READ_MS      2000

//
UsbrhUsblibHidapi::UsbrhUsblibHidapi() {
    setHumidityResolution(0);
    setTemperatureResolution(0);
    setTemperatureMode(0);
    setLogLevel(0);
}

//
UsbrhUsblibHidapi::~UsbrhUsblibHidapi() {
}

//
void UsbrhUsblibHidapi::setHumidityResolution(int mode) {
    mode_rhr = mode;
}

//
void UsbrhUsblibHidapi::setTemperatureResolution(int mode) {
    mode_tr = mode;
}

//
void UsbrhUsblibHidapi::setTemperatureMode(int mode) {
    mode_tt = mode;
}

//
void UsbrhUsblibHidapi::setLogLevel(int lev) {
    level_log = lev;
}

//
void UsbrhUsblibHidapi::setup() {

    //
    param_c1 = COPV_RH_C1[mode_rhr];
    param_c2 = COPV_RH_C2[mode_rhr];
    param_c3 = COPV_RH_C3[mode_rhr];
    param_t1 = COPV_RH_T1[mode_rhr];
    param_t2 = COPV_RH_T2[mode_rhr];
    param_d1 = COPV_T_D1[mode_tt][mode_tr];
    param_d2 = COPV_T_D2[mode_tt][mode_tr];

    return;
}

/* * */
int UsbrhUsblibHidapi::countPath() {
    int stat;

    //
    setup();

    // Initialize the hidapi library
    stat = hid_init();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_init returns %d\n", stat);
        exit(-1);
    }

    //
    int n = 0;
    struct hid_device_info *devs = hid_enumerate(ID_VENDOR_USBRH, ID_PRODUCT_USBRH);
    struct hid_device_info *hid = devs;
    while (hid) {
        hid = hid->next;
        n++;
    }
    hid_free_enumeration(devs);

    // Finalize the hidapi library
    stat = hid_exit();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_exit returns %d\n", stat);
        exit(-1);
    }

    //
    return n;
}

 /* * */
int UsbrhUsblibHidapi::listPath(int max_list, RHT_PATH list[]) {
    int stat;

    //
    setup();

    // Initialize the hidapi library
    stat = hid_init();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_init returns %d\n", stat);
        exit(-1);
    }

    //
    int n = 0;
    struct hid_device_info *devs = hid_enumerate(ID_VENDOR_USBRH, ID_PRODUCT_USBRH);
    struct hid_device_info *hid = devs;
    while (hid) {
        if (max_list <= n) {
            break;
        }

        strncpy(list[n].path, hid->path, SIZE_RHT_PATH);
        hid = hid->next;
        n++;
    }
    hid_free_enumeration(devs);

    // Finalize the hidapi library
    stat = hid_exit();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_exit returns %d\n", stat);
        exit(-1);
    }

    //
    return n;
}

/* * */
int UsbrhUsblibHidapi::get(int selected_device, RHT_VALUE *value) {
    int stat;

    //
    setup();

    // Initialize the hidapi library
    stat = hid_init();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_init returns %d\n", stat);
        exit(-1);
    }

    //
    int n = 1;
    const char *path_to_open;
    struct hid_device_info *devs = hid_enumerate(ID_VENDOR_USBRH, ID_PRODUCT_USBRH);
    struct hid_device_info *hid = devs;
    while (hid) {
        if (n == selected_device) {
            char buff[SIZE_BUFF];
            memset(buff, 0, SIZE_BUFF);

            //
            path_to_open = hid->path;
            hid_device *hdh = hid_open_path(path_to_open);
            if (hdh == NULL) {
                fprintf(stderr, "[ERROR] hid_open_path(\"%s\") returns NULL\n", path_to_open);
                exit(-1);
            }

            //
            stat = hid_write(hdh, (const unsigned char *)buff, SIZE_BUFF);
            if (stat < 0) {
                fprintf(stderr, "[ERROR] hid_write returns %d\n", stat);
                exit(-1);
            }

            stat = hid_read_timeout(hdh, (unsigned char *)buff, SIZE_BUFF, TIMEOUT_HID_READ_MS);

            //
            double sorh = (double)(buff[0] * 256 + buff[1]);
            double sot  = (double)(buff[2] * 256 + buff[3]);
            value->humidity_linear = param_c1 + (param_c2 * sorh) + (param_c3 * sorh * sorh);
            value->temperature = param_d1 + (param_d2 * sot);

            //
            double tc = COPV_T_D1[0][mode_tr] + (COPV_T_D2[0][mode_tr] * sot);
            value->humidity = (tc - 25.0) * (param_t1 + param_t2 * sorh) + value->humidity_linear;
            hid_close(hdh);

            break;
        }

        hid = hid->next;
        n++;
    }
    hid_free_enumeration(devs);

    // Finalize the hidapi library
    stat = hid_exit();
    if (stat < 0) {
        fprintf(stderr, "[ERROR] hid_exit returns %d\n", stat);
        exit(-1);
    }

    //
    return 0;
}

/* eof */
