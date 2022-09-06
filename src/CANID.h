#ifndef CANID_H
#define CANID_H

#include <Arduino.h>

// Assign message id for can transmitter

// #define bms_state_id 0x10FF50F4 // bms state id
// #define cell_vol_1_id 0x18FF51F4 // cell vol 1 id
// #define cell_vol_2_id 0x18FF52F4 // cell vol 2 id
// #define cell_temperature_id 0x18FF53F4 // cell temperature id
// #define battery_sys_state_id 0x18FF54F4 // battery system state id
// #define battery_spec_id 0x18FF60F4 // battery spec id
// #define accumulate_data_id 0x18F009A0 // accumulate data id
// #define soh_id 0x18F001A0 // state of health id

uint32_t bms_state_id;
uint32_t cell_vol_1_id;
uint32_t cell_vol_2_id;
uint32_t cell_temperature_id;
uint32_t battery_sys_state_id;
uint32_t battery_spec_id;
uint32_t accumulate_data_id;
uint32_t soh_id;




#endif