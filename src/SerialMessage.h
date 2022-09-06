#ifndef SERIALMESSAGE_H
#define SERIALMESSAGE_H

#include <Arduino.h>
#include <CANID.h>

#define SERIAL_MAX_DATA_LEN     16
#define START_BYTE              0x1F
#define END_BYTE                0x9F

byte byteArray[SERIAL_MAX_DATA_LEN] = {};

typedef struct{
    uint8_t data_length_code;
    uint8_t messageId;
    uint8_t data[SERIAL_MAX_DATA_LEN];
    uint32_t bytein;
    void writeMessage(){
        data[0] = START_BYTE;
        data[1] = messageId;
        data[2] = data_length_code;
        data[sizeof(data) - 1] = END_BYTE;
        for(int i=0; i<sizeof(data); i++){
            Serial.write(data[i]);
        }
    }
    void readMessgae(){
        for(int i=0; i<sizeof(data); i++){
            data[i] = Serial.read();
        }
    }
} serial_message_t;

void getbmsstateId(){
    serial_message_t txmessage;
    txmessage.messageId = 0x01;
    txmessage.data_length_code = 4;
    txmessage.data[3] = bms_state_id >> 24 & MASK_8BITS;
    txmessage.data[4] = bms_state_id >> 16 & MASK_8BITS;
    txmessage.data[5] = bms_state_id >> 8 & MASK_8BITS;
    txmessage.data[6] = bms_state_id & MASK_8BITS;
    txmessage.writeMessage();
}

void getcellvol1Id(){
    serial_message_t txmessage;
    txmessage.messageId = 0x02;
    txmessage.data_length_code = 4;
    txmessage.data[3] = cell_vol_1_id >> 24 & MASK_8BITS;
    txmessage.data[4] = cell_vol_1_id >> 16 & MASK_8BITS;
    txmessage.data[5] = cell_vol_1_id >> 8 & MASK_8BITS;
    txmessage.data[6] = cell_vol_1_id & MASK_8BITS;
    txmessage.writeMessage();
}

void getcellvol2Id(){
    serial_message_t txmessage;
    txmessage.messageId = 0x03;
    txmessage.data_length_code = 4;
    txmessage.data[3] = cell_vol_2_id >> 24 & MASK_8BITS;
    txmessage.data[4] = cell_vol_2_id >> 16 & MASK_8BITS;
    txmessage.data[5] = cell_vol_2_id >> 8 & MASK_8BITS;
    txmessage.data[6] = cell_vol_2_id & MASK_8BITS;
    txmessage.writeMessage();
}

void getcelltempId(){
    serial_message_t txmessage;
    txmessage.messageId = 0x04;
    txmessage.data_length_code = 4;
    txmessage.data[3] = cell_temperature_id >> 24 & MASK_8BITS;
    txmessage.data[4] = cell_temperature_id >> 16 & MASK_8BITS;
    txmessage.data[5] = cell_temperature_id >> 8 & MASK_8BITS;
    txmessage.data[6] = cell_temperature_id & MASK_8BITS;
    txmessage.writeMessage();
}

void getbattsysstateId(){
    serial_message_t txmessage;
    txmessage.messageId = 0x05;
    txmessage.data_length_code = 4;
    txmessage.data[3] = battery_sys_state_id >> 24 & MASK_8BITS;
    txmessage.data[4] = battery_sys_state_id >> 16 & MASK_8BITS;
    txmessage.data[5] = battery_sys_state_id >> 8 & MASK_8BITS;
    txmessage.data[6] = battery_sys_state_id & MASK_8BITS;
    txmessage.writeMessage();
}




#endif