#ifndef USBRH_USBLIB_HIDAPI_H
#define USBRH_USBLIB_HIDAPI_H

//
#define ID_VENDOR_USBRH         0x1774
#define ID_PRODUCT_USBRH        0x1001
#define OPTION_SERIAL_NUMBER    NULL

/* Converting Output to Physical Values 
 * see http://www.sensirion.com/en/pdf/product_information/Data_Sheet_humidity_sensor_SHT1x_SHT7x_E.pdf
 */
#define COPV_RH_C1_12b          (-4.0)
#define COPV_RH_C2_12b          (0.0405)
#define COPV_RH_C3_12b          (-0.0000028)
#define COPV_RH_T1_12b          (0.01)
#define COPV_RH_T2_12b          (0.00008)
#define COPV_RH_C1_8b           (-4.0)
#define COPV_RH_C2_8b           (0.648)
#define COPV_RH_C3_8b           (-0.00072)
#define COPV_RH_T1_8b           (0.01)
#define COPV_RH_T2_8b           (0.00128)

#define COPV_TC_D1_5_0          (-40.00)
#define COPV_TC_D1_4_0          (-39.75)
#define COPV_TC_D1_3_5          (-39.66)
#define COPV_TC_D1_3_0          (-39.60)
#define COPV_TC_D1_2_5          (-39.55)

#define COPV_TF_D1_5_0          (-40.00)
#define COPV_TF_D1_4_0          (-39.55)
#define COPV_TF_D1_3_5          (-39.39)
#define COPV_TF_D1_3_0          (-39.28)
#define COPV_TF_D1_2_5          (-39.19)

#define COPV_TC_D2_14b          (0.01)
#define COPV_TC_D2_12b          (0.04)

#define COPV_TF_D2_14b          (0.018)
#define COPV_TF_D2_12b          (0.072)

/* * */
typedef struct _copv_rht {
    double c1;
    double c2;
    double c3;
    double t1;
    double t2;
    double d1;
    double d2;

    int ofs_rhr;
    int ofs_tt;
    int ofs_tr;

    int dbg;

} COPV_RHT;

/* * */
#define SIZE_RHT_PATH      32
typedef struct _rht_path {
    char path[SIZE_RHT_PATH];
} RHT_PATH;

/* * */
typedef struct _rht_value {
    double humidity;              // humidity_true
    double humidity_linear;
    double temperature;
} RHT_VALUE;

/* * */
static double COPV_RH_C1[] = { COPV_RH_C1_12b, COPV_RH_C1_8b };
static double COPV_RH_C2[] = { COPV_RH_C2_12b, COPV_RH_C2_8b };
static double COPV_RH_C3[] = { COPV_RH_C3_12b, COPV_RH_C3_8b };
static double COPV_RH_T1[] = { COPV_RH_T1_12b, COPV_RH_T1_8b };
static double COPV_RH_T2[] = { COPV_RH_T2_12b, COPV_RH_T2_8b };
static double COPV_T_D1[][5] = { { COPV_TC_D1_5_0, COPV_TC_D1_4_0, COPV_TC_D1_3_5, COPV_TC_D1_3_0, COPV_TC_D1_2_5 },
                                { COPV_TF_D1_5_0, COPV_TF_D1_4_0, COPV_TF_D1_3_5, COPV_TF_D1_3_0, COPV_TF_D1_2_5 } };
static double COPV_T_D2[][5] = { { COPV_TC_D2_14b, COPV_TC_D2_12b },
                                { COPV_TF_D2_14b, COPV_TF_D2_12b } };

class UsbrhUsblibHidapi {

public:
    UsbrhUsblibHidapi();
    ~UsbrhUsblibHidapi();

    void setHumidityResolution(int mode);
    void setTemperatureResolution(int mode);
    void setTemperatureMode(int mode);
    void setLogLevel(int lev);

    int countPath();
    int listPath(int max_list, RHT_PATH list[]);
    int get(int n, RHT_VALUE *val);

private:
    void log(int lev, char *msg);
    void setup();

    double param_c1;
    double param_c2;
    double param_c3;
    double param_t1;
    double param_t2;
    double param_d1;
    double param_d2;

    int mode_rhr;
    int mode_tt;
    int mode_tr;

    int level_log;
};

#endif
