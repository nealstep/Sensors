#include "s_mlx90641.h"
#ifdef S_MLX90641
#include <sensors.h>

#include <Wire.h>
#include <MLX90641_API.h>
#include <MLX9064X_I2C_Driver.h>

const byte MLX90641_address = 0x33;
#define TA_SHIFT 8

uint16_t eeMLX90641[832];
float mlx90641To[192];
uint16_t mlx90641Frame[242];
paramsMLX90641 mlx90641;
bool therm_inited;

boolean therm_is_connected() {
    Wire.beginTransmission((uint8_t)MLX90641_address);
    if (Wire.endTransmission() != 0) {
        return (false);
    }
    return (true);
}

void s_mlx90641_init(void) {
    lg.debug("s_mlx90641 init");

    int status;

    Wire.begin();
    therm_inited = false;
    Wire.setClock(400000);
    if (therm_is_connected() == false) {
        lg.error("Therm: MLX90640 not detected at default I2C address. Please check wiring. Freezing.");
        return;
    }
    status = MLX90641_DumpEE(MLX90641_address, eeMLX90641);
    if (status != 0) {
        lg.error("Therm: Failed to load system parameters");
        lg.value("Therm Error", status, logger_error);
        return;
    }
    status = MLX90641_ExtractParameters(eeMLX90641, &mlx90641);
    if (status != 0) {
        lg.error("Therm: Parameter extraction failed");
        lg.value("Therm Error", status, logger_error);
        return;
    }
    lg.info("Therm: Inited");
    MLX90641_SetRefreshRate(MLX90641_address, 0x00); // .5hz (1=1hz,2=2hz,3=4hz)
    therm_inited = true;
}

void s_mlx90641_check(void) {
    lg.debug("s_mlx90641 check");

    byte x;
    int status;
    float vdd, Ta, tr, emissivity;
    uint32_t line[192];

    if (!therm_inited) {
        lg.error("Therm: not initied");
        return;
    }

    for (x=0;x<2;x++) {
        status = MLX90641_GetFrameData(MLX90641_address, mlx90641Frame);
        vdd = MLX90641_GetVdd(mlx90641Frame, &mlx90641);
        Ta = MLX90641_GetTa(mlx90641Frame, &mlx90641);
        tr = Ta - TA_SHIFT; //Reflected temperature based on the sensor ambient temperature
        emissivity = 0.95;
        MLX90641_CalculateTo(mlx90641Frame, &mlx90641, emissivity, tr, mlx90641To);
    }
    if (status != 0) {
        lg.error("Therm: read failed");
        lg.value("Therm Error", status, logger_error);
        return;
    }
	lg.report("therm.vdd", vdd);

    for (x=0;x<192;x++) {
		line[x] = conv_float_uint32(mlx90641To[x]);
	}  
    // lg.report_long(line, 12, 16);  
}

#endif // S_MLX90641
