#ifndef SYSTEM_DATA_H
#define SYSTEM_DATA_H

#include <stdint.h>
#include "bsp_afe.h"

typedef enum {
    COMP_RESISTOR = 0,
    COMP_CAPACITOR = 1,
    COMP_INDUCTOR = 2,
    COMP_UNKNOWN = 3,
} ComponentType_t;

typedef struct {
    float re_ohm;
    float im_ohm;
    float z_mag_ohm;
    float phase_deg;

    float primary_value;
    float q_factor;
    float d_factor;

    float freq_hz;
    ComponentType_t comp_type;
    AFE_Rref_t rref_range;
    AFE_PGA_Gain_t pga_gain;

    uint8_t valid;
} MeasureResult_t;

#endif
