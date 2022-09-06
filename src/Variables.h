#ifndef VARIABLES_H
#define VARIABLES_H

#include <Arduino.h>

// BMS State data assign
int16_t sys_voltage; // 16-bit variables

#define MASK_8BITS   0xFF
#define MASK_16BITS  0xFFFF
#define MASK_32BITS  0xFFFFFFFF

uint16_t sys_current;
uint8_t can_counter; // 8-bit variables

int sys_soc;
int sys_status;
int chg_link;
int chg_indication;
int err_level;
int bms_self_check;
int power_off_rqst;
int insu_fade;

// Cell_vol_1, Cell_Vol_2 data assign
uint16_t cell_ave_vol, cell_dev_vol, chg_current_limit, dchg_current_limit; // 16-bit data
uint16_t cell_max_vol; // cell max voltage
uint16_t cell_min_vol; // cell min voltage

uint8_t max_vol_cell_num, max_vol_grp_num, min_vol_cell_num, min_vol_grp_num; // 8-bit data

// Cell Temperature data assign
uint8_t max_temp_cell_num, max_temp_grp_num, min_temp_cell_num, min_temp_grp_num, cell_ave_temp, cell_dev_temp; // 8-bit data
uint8_t cell_max_temp;
uint8_t cell_min_temp;

// Battery Sys State data assign
uint16_t max_chg_current; // 16-bit data
int cell_vol_high, cell_vol_low, tem_high, tem_low; // byte 1 data
int sum_vol_high, sum_vol_low, dis_current_over, chg_current_over; // byte 2 data
int soc_high, soc_low, det_temp, det_cell_vol; // byte 3 data
int bms_commu_fault, vcu_timeout_malf, sum_vol_diff_malf, port_temp_high; // byte 4 data
int st_pos_relay, st_chg_relay, st_neg_relay, st_prechg_relay, dc_dc_relay; // byte 5 data
int air_pump_relay, port_temp_relay; // byte 6 data

// Battery Spec data assign
uint16_t bms_serial_nr, v_max_cell_spec, I_ch_max_cell_spec; // 16-bit data
uint8_t batt_vendor_nr, t_max_cell_spec; // 8-byte data

// Accumulate Spec data assign
uint32_t master_timer; // 32-bit data
uint16_t acc_charge, acc_discharge; // 16-bit data

// SOH data assign
uint16_t soh_data;





#endif