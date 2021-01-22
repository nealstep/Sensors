#include <sensors.h>

void sensor_init(void) {
    lg.debug("sensor init");

// init begin flag
#ifdef S_DUMMY
    s_dummy_init();
#endif // S_DUMMY

#ifdef S_BME280
    s_bme280_init();
#endif // S_BME280

#ifdef S_AHT20
    s_aht20_init();
#endif // S_AHT20

#ifdef S_DS18B20
    s_ds18b20_init();
#endif // S_DS18B20

#ifdef S_MHZ16
    s_mhz16_init();
#endif // S_MHZ16
// init end flag
}

void sensor_check(void) {
    lg.debug("sensor check");

// check begin flag
#ifdef S_DUMMY
    s_dummy_check();
#endif // S_DUMMY

#ifdef S_BME280
    s_bme280_check();
#endif // S_BME280

#ifdef S_AHT20
    s_aht20_check();
#endif // S_AHT20

#ifdef S_DS18B20
    s_ds18b20_check();
#endif // S_DS18B20

#ifdef S_MHZ16
    s_mhz16_check();
#endif // S_MHZ16
// check end flag
}

void noop(void) {
    lg.debug("NoOp");
}

uint16_t conv_float_uint16(float arg, uint16_t acc) {
    if (arg < 0) {
        lg.error("Not a positive number");
        return -1;
    }
    return static_cast<uint32_t>(std::round(arg * acc));
}

uint32_t conv_float_uint32(float arg, uint16_t acc) {
    if (arg < 0) {
        lg.error("Not a positive number");
        return -1;
    }
    return static_cast<uint32_t>(std::round(arg * acc));
}

uint16_t ana_read(void) {
    uint16_t adc;
    uint8_t i;

    for (i=0;i<ana_igno;i++) {
        analogRead(pin_ana);
        delay(time_ana_delay);
    }
    adc = 0;
    for (i=0;i<ana_samp;i++) {
        adc += analogRead(pin_ana);
        delay(time_ana_delay);
    }
    return conv_float_uint16(((float)adc)/ana_samp, 1);
}
