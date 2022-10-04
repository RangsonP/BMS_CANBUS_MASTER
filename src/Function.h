#ifndef FUNCTION_H
#define FUNCTION_H

#include <Arduino.h>
#include "Pins.h"
#include "Variables.h"
#include "CANID.h"
#include "CANStatus.h"
#include "DataConv.h"
#include "driver/can.h"
#include "driver/gpio.h"
#include <Preferences.h>
#include <SerialMessage.h>

Preferences preferences;

void output_high(uint16_t _pin){
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void output_low(uint16_t _pin){
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void power_control(){
  output_high(relay1);
  output_high(relay2);
}

void power_fault(){
  output_low(relay1);
  output_low(relay2);
}

void setup_can_driver(){
  can_general_config_t g_config = { // Create g_config for setup CAN Parameter
    .mode = CAN_MODE_NORMAL,
    .tx_io = GPIO_NUM_26, // CAN TX Pin
    .rx_io = GPIO_NUM_27, // CAN RX Pin
    .clkout_io = ((gpio_num_t)CAN_IO_UNUSED),
    .bus_off_io = ((gpio_num_t)CAN_IO_UNUSED),
    .tx_queue_len = 5,
    .rx_queue_len = 5,
    .alerts_enabled = CAN_ALERT_ALL,
    .clkout_divider = 0
  };
  can_timing_config_t t_config = CAN_TIMING_CONFIG_250KBITS(); // Set baud rate 250Kbps
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

  if (can_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    Serial.println("Failed to install driver");
    return;
  }
  
  if (can_start() != ESP_OK) {
    Serial.println("Failed to start driver");
    return;
  }
}

void read_bms_state(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == bms_state_id){
      Serial.println("-----START-----");
      sys_voltage     = sys_voltage_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS))); Serial.print("System Voltage = "); Serial.println(sys_voltage, DEC);
      sys_current     = sys_current_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS))); Serial.print("System Current = "); Serial.println(sys_current, DEC);
      sys_soc         = sys_soc_conv(rxmessage.data[4] & MASK_8BITS); Serial.print("System SoC = "); Serial.println(sys_soc, DEC);
      bms_state_b5    = rxmessage.data[5] & MASK_8BITS; Serial.print("BMS byte5 = "); Serial.println(bms_state_b5, BIN);
      bms_state_b6    = rxmessage.data[6] & MASK_8BITS; Serial.print("BMS byte6 = "); Serial.println(bms_state_b6, BIN);
      can_counter     = rxmessage.data[7] & MASK_8BITS; Serial.print("Can counter = "); Serial.println(can_counter, DEC);
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_cell_vol_1(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == cell_vol_1_id){
      //Serial.println("-----START-----");
      cell_max_vol      = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("Cell max voltage = "); Serial.println(cell_max_vol, DEC);
      cell_min_vol      = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("Cell min voltage = "); Serial.println(cell_min_vol, DEC);
      max_vol_cell_num  = rxmessage.data[4] & MASK_8BITS;
      max_vol_grp_num   = rxmessage.data[5] & MASK_8BITS;
      min_vol_cell_num  = rxmessage.data[6] & MASK_8BITS;
      min_vol_grp_num   = rxmessage.data[7] & MASK_8BITS;
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_cell_vol_2(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == cell_vol_2_id){
      //Serial.println("-----START-----");
      cell_ave_vol        = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("Cell avg voltage = "); Serial.println(cell_ave_vol, DEC);
      cell_dev_vol        = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("Cell dev voltage = "); Serial.println(cell_dev_vol, DEC);
      chg_current_limit   = chg_current_limit_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); Serial.print("CHG Current limit = "); Serial.println(chg_current_limit, DEC);
      dchg_current_limit  = dchg_current_limit_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("DCHG Current limit = "); Serial.println(dchg_current_limit, DEC);
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_cell_temperature(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == cell_temperature_id){
      //Serial.println("-----START-----");
      cell_max_temp     = cell_max_temp_conv(rxmessage.data[0] & MASK_8BITS);
      max_temp_cell_num = rxmessage.data[1] & MASK_8BITS;
      max_temp_grp_num  = rxmessage.data[2] & MASK_8BITS;
      cell_min_temp     = cell_min_temp_conv(rxmessage.data[3] & MASK_8BITS);
      min_temp_cell_num = rxmessage.data[4] & MASK_8BITS;
      min_temp_grp_num  = rxmessage.data[5] & MASK_8BITS;
      cell_ave_temp     = cell_min_temp_conv(rxmessage.data[6] & MASK_8BITS);
      cell_dev_temp     = rxmessage.data[7] & MASK_8BITS;
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_battery_sys_state(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == battery_sys_state_id){
      //Serial.println("-----START-----");
      battery_sys_state_b1    = rxmessage.data[0] & MASK_8BITS;
      battery_sys_state_b2    = rxmessage.data[1] & MASK_8BITS;
      battery_sys_state_b3    = rxmessage.data[2] & MASK_8BITS;
      battery_sys_state_b4    = rxmessage.data[3] & MASK_8BITS;
      battery_sys_state_b5    = rxmessage.data[4] & MASK_8BITS;
      battery_sys_state_b6    = rxmessage.data[5] & MASK_8BITS;
      max_chg_current  = max_chg_current_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("MAX CHG Current = "); Serial.println(max_chg_current, DEC);
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_battery_spec(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == battery_spec_id){
      //Serial.println("-----START-----");
      batt_vendor_nr      = rxmessage.data[0];
      bms_serial_nr       = ((rxmessage.data[1] << 8 & MASK_16BITS) + (rxmessage.data[2] & MASK_16BITS));
      v_max_cell_spec     = ((rxmessage.data[3] << 8 & MASK_16BITS) + (rxmessage.data[4] & MASK_16BITS));
      t_max_cell_spec     = rxmessage.data[5] & MASK_8BITS;
      I_ch_max_cell_spec  = I_ch_max_cell_spec_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS)));
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_accumulate_data(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == accumulate_data_id){
      //Serial.println("-----START-----");
      acc_charge     = acc_charge_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)));
      acc_discharge  = acc_discharge_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)));
      master_timer   = master_timer_conv(((rxmessage.data[4] << 32 & MASK_32BITS) + (rxmessage.data[5] << 16 & MASK_32BITS) + (rxmessage.data[6] << 8 & MASK_32BITS) + (rxmessage.data[7] & MASK_32BITS)));
      //Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

void read_soh(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    if(rxmessage.identifier == soh_id){
      //Serial.println("-----START-----");
      soh_b1   = rxmessage.data[0];
      soh_b2   = rxmessage.data[1];
      soh_b3   = rxmessage.data[2];
      soh_b4   = rxmessage.data[3];
      soh_data = soh_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); Serial.print("SoH = "); Serial.println(soh_data, DEC);
      soh_b7   = rxmessage.data[6];
      soh_b8   = rxmessage.data[7];
      Serial.println("-----END-----");
    }
    delayMicroseconds(1);
  }
}

// Read CAN Message via Serial port monitor
void readCANmessage(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    output_high(led_status);
    if(rxmessage.identifier == bms_state_id){
      sys_voltage     = sys_voltage_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS))); Serial.print("System Voltage = "); Serial.println(sys_voltage, DEC);
      sys_current     = sys_current_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS))); Serial.print("System Current = "); Serial.println(sys_current, DEC);
      sys_soc         = sys_soc_conv(rxmessage.data[4] & MASK_8BITS); Serial.print("System SoC = "); Serial.println(sys_soc, DEC);
      bms_state_b5    = rxmessage.data[5] & MASK_8BITS; Serial.print("BMS byte5 = "); Serial.println(bms_state_b5, BIN);
      bms_state_b6    = rxmessage.data[6] & MASK_8BITS; Serial.print("BMS byte6 = "); Serial.println(bms_state_b6, BIN);
      can_counter     = rxmessage.data[7] & MASK_8BITS; Serial.print("Can counter = "); Serial.println(can_counter, DEC);
    }
    if(rxmessage.identifier == cell_vol_1_id){
      cell_max_vol      = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("Cell max voltage = "); Serial.println(cell_max_vol, DEC);
      cell_min_vol      = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("Cell min voltage = "); Serial.println(cell_min_vol, DEC);
      max_vol_cell_num  = rxmessage.data[4] & MASK_8BITS; Serial.print("Max voltage cell number = "); Serial.println(max_vol_cell_num, DEC);
      max_vol_grp_num   = rxmessage.data[5] & MASK_8BITS; Serial.print("Max voltage group number = "); Serial.println(max_vol_grp_num, DEC);
      min_vol_cell_num  = rxmessage.data[6] & MASK_8BITS; Serial.print("Min voltage cell number = "); Serial.println(min_vol_cell_num, DEC);
      min_vol_grp_num   = rxmessage.data[7] & MASK_8BITS; Serial.print("Min voltage group number = "); Serial.println(min_vol_grp_num, DEC);
    }
    if(rxmessage.identifier == cell_vol_2_id){
      cell_ave_vol        = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("Cell avg voltage = "); Serial.println(cell_ave_vol, DEC);
      cell_dev_vol        = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("Cell dev voltage = "); Serial.println(cell_dev_vol, DEC);
      chg_current_limit   = chg_current_limit_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); Serial.print("CHG Current limit = "); Serial.println(chg_current_limit, DEC);
      dchg_current_limit  = dchg_current_limit_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("DCHG Current limit = "); Serial.println(dchg_current_limit, DEC);
    }
    if(rxmessage.identifier == cell_temperature_id){
      cell_max_temp     = cell_max_temp_conv(rxmessage.data[0] & MASK_8BITS); Serial.print("Cell max temperature = "); Serial.println(cell_max_temp, DEC);
      max_temp_cell_num = rxmessage.data[1] & MASK_8BITS; Serial.print("Max cell temperature number = "); Serial.println(max_temp_cell_num, DEC);
      max_temp_grp_num  = rxmessage.data[2] & MASK_8BITS; Serial.print("Max group temperature number = "); Serial.println(max_temp_grp_num, DEC);
      cell_min_temp     = cell_min_temp_conv(rxmessage.data[3] & MASK_8BITS); Serial.print("Cell min temperature = "); Serial.println(cell_min_temp, DEC);
      min_temp_cell_num = rxmessage.data[4] & MASK_8BITS; Serial.print("Min cell temperature number = "); Serial.println(min_temp_cell_num, DEC);
      min_temp_grp_num  = rxmessage.data[5] & MASK_8BITS; Serial.print("Min group temperature number = "); Serial.println(min_temp_grp_num, DEC);
      cell_ave_temp     = cell_ave_temp_conv(rxmessage.data[6] & MASK_8BITS); Serial.print("Cell average temperature = "); Serial.println(cell_ave_temp, DEC);
      cell_dev_temp     = rxmessage.data[7] & MASK_8BITS; Serial.print("Cell dev temperature = "); Serial.println(cell_dev_temp, DEC);
    }
    if(rxmessage.identifier == battery_sys_state_id){
      battery_sys_state_b1    = rxmessage.data[0] & MASK_8BITS; Serial.print("Battery system state byte1 = "); Serial.println(battery_sys_state_b1, BIN);
      battery_sys_state_b2    = rxmessage.data[1] & MASK_8BITS; Serial.print("Battery system state byte2 = "); Serial.println(battery_sys_state_b2, BIN);
      battery_sys_state_b3    = rxmessage.data[2] & MASK_8BITS; Serial.print("Battery system state byte3 = "); Serial.println(battery_sys_state_b3, BIN);
      battery_sys_state_b4    = rxmessage.data[3] & MASK_8BITS; Serial.print("Battery system state byte4 = "); Serial.println(battery_sys_state_b4, BIN);
      battery_sys_state_b5    = rxmessage.data[4] & MASK_8BITS; Serial.print("Battery system state byte5 = "); Serial.println(battery_sys_state_b5, BIN);
      battery_sys_state_b6    = rxmessage.data[5] & MASK_8BITS; Serial.print("Battery system state byte6 = "); Serial.println(battery_sys_state_b6, BIN);
      max_chg_current  = max_chg_current_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("MAX CHG Current = "); Serial.println(max_chg_current, DEC);
    }
    if(rxmessage.identifier == battery_spec_id){
      batt_vendor_nr      = rxmessage.data[0];
      bms_serial_nr       = ((rxmessage.data[1] << 8 & MASK_16BITS) + (rxmessage.data[2] & MASK_16BITS));
      v_max_cell_spec     = ((rxmessage.data[3] << 8 & MASK_16BITS) + (rxmessage.data[4] & MASK_16BITS));
      t_max_cell_spec     = rxmessage.data[5] & MASK_8BITS;
      I_ch_max_cell_spec  = I_ch_max_cell_spec_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS)));
    }
    if(rxmessage.identifier == accumulate_data_id){
      acc_charge     = acc_charge_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)));
      acc_discharge  = acc_discharge_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)));
      master_timer   = master_timer_conv(((rxmessage.data[4] << 32 & MASK_32BITS) + (rxmessage.data[5] << 16 & MASK_32BITS) + (rxmessage.data[6] << 8 & MASK_32BITS) + (rxmessage.data[7] & MASK_32BITS)));
    }
    if(rxmessage.identifier == soh_id){
      soh_b1   = rxmessage.data[0];
      soh_b2   = rxmessage.data[1];
      soh_b3   = rxmessage.data[2];
      soh_b4   = rxmessage.data[3];
      soh_data = soh_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); Serial.print("SoH = "); Serial.println(soh_data, DEC);
      soh_b7   = rxmessage.data[6];
      soh_b8   = rxmessage.data[7];
    }
    output_low(led_status);
  }
}

// Read CAN Message via Serial port monitor
void readCANmessageSerial(){
  can_message_t rxmessage;
  if (can_receive(&rxmessage, pdMS_TO_TICKS(1000)) == ESP_OK) {
    output_high(led_status);
    if(rxmessage.identifier == bms_state_id){
      sys_voltage     = sys_voltage_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS))); Serial.print("SYSVT = "); Serial.println(sys_voltage, DEC);
      sys_current     = sys_current_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS))); Serial.print("SYSCR = "); Serial.println(sys_current, DEC);
      sys_soc         = sys_soc_conv(rxmessage.data[4] & MASK_8BITS); Serial.print("SYSSC = "); Serial.println(sys_soc, DEC);
      bms_state_b5    = rxmessage.data[5] & MASK_8BITS; Serial.print("BMSB5 = "); Serial.println(bms_state_b5, BIN);
      bms_state_b6    = rxmessage.data[6] & MASK_8BITS; Serial.print("BMSB6 = "); Serial.println(bms_state_b6, BIN);
      can_counter     = rxmessage.data[7] & MASK_8BITS; Serial.print("CANCN = "); Serial.println(can_counter, DEC);
    }
    if(rxmessage.identifier == cell_vol_1_id){
      cell_max_vol      = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("CLMXV = "); Serial.println(cell_max_vol, DEC);
      cell_min_vol      = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("CLMNV = "); Serial.println(cell_min_vol, DEC);
      max_vol_cell_num  = rxmessage.data[4] & MASK_8BITS; Serial.print("MXVCN = "); Serial.println(max_vol_cell_num, DEC);
      max_vol_grp_num   = rxmessage.data[5] & MASK_8BITS; Serial.print("MXVGN = "); Serial.println(max_vol_grp_num, DEC);
      min_vol_cell_num  = rxmessage.data[6] & MASK_8BITS; Serial.print("MNVCN = "); Serial.println(min_vol_cell_num, DEC);
      min_vol_grp_num   = rxmessage.data[7] & MASK_8BITS; Serial.print("MNVGN = "); Serial.println(min_vol_grp_num, DEC);
    }
    if(rxmessage.identifier == cell_vol_2_id){
      cell_ave_vol        = ((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)); Serial.print("CLAVV = "); Serial.println(cell_ave_vol, DEC);
      cell_dev_vol        = ((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)); Serial.print("CLDVV = "); Serial.println(cell_dev_vol, DEC);
      chg_current_limit   = chg_current_limit_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); Serial.print("CHGCL = "); Serial.println(chg_current_limit, DEC);
      dchg_current_limit  = dchg_current_limit_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("DCHCL = "); Serial.println(dchg_current_limit, DEC);
    }
    if(rxmessage.identifier == cell_temperature_id){
      cell_max_temp     = cell_max_temp_conv(rxmessage.data[0] & MASK_8BITS); Serial.print("CLMXT = "); Serial.println(cell_max_temp, DEC);
      max_temp_cell_num = rxmessage.data[1] & MASK_8BITS; Serial.print("MXTCN = "); Serial.println(max_temp_cell_num, DEC);
      max_temp_grp_num  = rxmessage.data[2] & MASK_8BITS; Serial.print("MXTGN = "); Serial.println(max_temp_grp_num, DEC);
      cell_min_temp     = cell_min_temp_conv(rxmessage.data[3] & MASK_8BITS); Serial.print("CLMNT = "); Serial.println(cell_min_temp, DEC);
      min_temp_cell_num = rxmessage.data[4] & MASK_8BITS; Serial.print("MNTCN = "); Serial.println(min_temp_cell_num, DEC);
      min_temp_grp_num  = rxmessage.data[5] & MASK_8BITS; Serial.print("MNTGN = "); Serial.println(min_temp_grp_num, DEC);
      cell_ave_temp     = cell_ave_temp_conv(rxmessage.data[6] & MASK_8BITS); Serial.print("CLAVT = "); Serial.println(cell_ave_temp, DEC);
      cell_dev_temp     = rxmessage.data[7] & MASK_8BITS; Serial.print("CLDVT = "); Serial.println(cell_dev_temp, DEC);
    }
    if(rxmessage.identifier == battery_sys_state_id){
      battery_sys_state_b1    = rxmessage.data[0] & MASK_8BITS; Serial.print("BSSB1 = "); Serial.println(battery_sys_state_b1, BIN);
      battery_sys_state_b2    = rxmessage.data[1] & MASK_8BITS; Serial.print("BSSB2 = "); Serial.println(battery_sys_state_b2, BIN);
      battery_sys_state_b3    = rxmessage.data[2] & MASK_8BITS; Serial.print("BSSB3 = "); Serial.println(battery_sys_state_b3, BIN);
      battery_sys_state_b4    = rxmessage.data[3] & MASK_8BITS; Serial.print("BSSB4 = "); Serial.println(battery_sys_state_b4, BIN);
      battery_sys_state_b5    = rxmessage.data[4] & MASK_8BITS; Serial.print("BSSB5 = "); Serial.println(battery_sys_state_b5, BIN);
      battery_sys_state_b6    = rxmessage.data[5] & MASK_8BITS; Serial.print("BSSB6 = "); Serial.println(battery_sys_state_b6, BIN);
      max_chg_current  = max_chg_current_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS))); Serial.print("MXCHC = "); Serial.println(max_chg_current, DEC);
    }
    if(rxmessage.identifier == battery_spec_id){
      batt_vendor_nr      = rxmessage.data[0];
      bms_serial_nr       = ((rxmessage.data[1] << 8 & MASK_16BITS) + (rxmessage.data[2] & MASK_16BITS));
      v_max_cell_spec     = ((rxmessage.data[3] << 8 & MASK_16BITS) + (rxmessage.data[4] & MASK_16BITS));
      t_max_cell_spec     = rxmessage.data[5] & MASK_8BITS;
      I_ch_max_cell_spec  = I_ch_max_cell_spec_conv(((rxmessage.data[6] << 8 & MASK_16BITS) + (rxmessage.data[7] & MASK_16BITS)));
    }
    if(rxmessage.identifier == accumulate_data_id){
      acc_charge     = acc_charge_conv(((rxmessage.data[0] << 8 & MASK_16BITS) + (rxmessage.data[1] & MASK_16BITS)));
      acc_discharge  = acc_discharge_conv(((rxmessage.data[2] << 8 & MASK_16BITS) + (rxmessage.data[3] & MASK_16BITS)));
      master_timer   = master_timer_conv(((rxmessage.data[4] << 32 & MASK_32BITS) + (rxmessage.data[5] << 16 & MASK_32BITS) + (rxmessage.data[6] << 8 & MASK_32BITS) + (rxmessage.data[7] & MASK_32BITS)));
    }
    if(rxmessage.identifier == soh_id){
      soh_b1   = rxmessage.data[0];
      soh_b2   = rxmessage.data[1];
      soh_b3   = rxmessage.data[2];
      soh_b4   = rxmessage.data[3];
      soh_data = soh_conv(((rxmessage.data[4] << 8 & MASK_16BITS) + (rxmessage.data[5] & MASK_16BITS))); //Serial.print("SoH = "); Serial.println(soh_data, DEC);
      soh_b7   = rxmessage.data[6];
      soh_b8   = rxmessage.data[7];
    }
    output_low(led_status);
  }
}

void getCommand(){
  while(Serial.read() == 0x1a)
  while(true){
    byte cmd = Serial.read();
    if(cmd == 0x2a)
    getbmsstateId();            // Message ID=1
    if(cmd == 0x3a)
    getcellvol1Id();            // Message ID=2
    if(cmd == 0x4a)
    getcellvol2Id();            // Message ID=3
    if(cmd == 0x5a)
    getcelltempId();            // Message ID=4
    if(cmd == 0x6a)
    getbattsysstateId();        // Message ID=5
    if(cmd == 0x9a)
    break;
  }
}

void getParams(){
  for(int i=0; i<SERIAL_MAX_DATA_LEN; i++){
    byte byteIn = Serial.read();
    byteArray[i] = byteIn;
  }
}

// Default IDx
void setIDx(){
  preferences.begin("canbus-id", false);
  preferences.putInt("bms-state-id", 0x10FF50F4);
  preferences.putInt("cell-vol1-id", 0x18FF51F4);
  preferences.putInt("cell-vol2-id", 0x18FF52F4);
  preferences.putInt("cell-temp-id", 0x18FF53F4);
  preferences.putInt("batt-syst-id", 0x18FF54F4);
  preferences.putInt("batt-spec-id", 0x18FF60F4);
  preferences.putInt("accu-data-id", 0x18F009A0);
  preferences.putInt("soh-id", 0x18F001A0);
  preferences.end();
}

void getIDx(){
  preferences.begin("canbus-id", false);
  bms_state_id = preferences.getInt("bms-state-id");
  cell_vol_1_id = preferences.getInt("cell-vol1-id");
  cell_vol_2_id = preferences.getInt("cell-vol2-id");
  cell_temperature_id = preferences.getInt("cell-temp-id");
  battery_sys_state_id = preferences.getInt("batt-syst-id");
  battery_spec_id = preferences.getInt("batt-spec-id");
  accumulate_data_id = preferences.getInt("accu-data-id");
  soh_id = preferences.getInt("soh-id");
  preferences.end();
}




#endif