#pragma once
#include <Arduino.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include "data.h"

SoftwareSerial rs485(16,17);
ModbusMaster node, node_s;

void setup_modbus() { // Initialize RS-485 serial communication
                         
  rs485.begin(4800);     // RX, TX pins                      
  node_s.begin(1,rs485); // 1 адрес датчика почвы
  node.begin(2,rs485);   // 2 адрес датчика воздуха
}

  void read_Air_Hum() { //Чтения данных датчика влажность воздуха
    uint8_t result_air_h;
    result_air_h = node.readHoldingRegisters(0x000, 1);

    if (result_air_h == node.ku8MBSuccess) {
      // Convert and print the received data
      data.value_hum_air = (node.getResponseBuffer(0) * 0.1);

      
      Serial.print("Влажность воздуха: ");
      Serial.println(data.value_hum_air);
  
    } else {
      Serial.print("Error Влажность воздуха. ");
    
      Serial.print(": ");
      Serial.println(result_air_h);
    }
  
    }


  void read_Air_Temp() { //Чтения данных датчика температура воздуха
    uint8_t result_air_t;
    result_air_t = node.readHoldingRegisters(0x001, 1);
    
  if (result_air_t == node.ku8MBSuccess) {
      // Convert and print the received data
      data.value_temp_air = (node.getResponseBuffer(0) * 0.1);

      
      Serial.print("Температура воздуха: ");
      Serial.println(data.value_temp_air);
      Serial.println("");
    } else {
      Serial.print("Error Температура воздуха. ");
      Serial.print(": ");
      Serial.println(result_air_t);
    }
  }


  void read_Soil_Hum() { //Функция чтения данных датчика влажность почвы
    uint8_t result_soil_h;
    result_soil_h = node_s.readHoldingRegisters(0x000, 1);

    if (result_soil_h == node_s.ku8MBSuccess) {
      // Convert and print the received data
      data.value_hum_soil = (node_s.getResponseBuffer(0) * 0.1);

      
      Serial.print("Влажность почвы: ");
      Serial.println(data.value_hum_soil);
  
    } else {
      Serial.print("Error Влажность почвы. ");
      Serial.print(": ");
      Serial.println(result_soil_h);
    }
  
    }


  void read_Soil_Temp() { //Функция чтения данных датчика температуры почвы
    uint8_t result_soil_t;
    result_soil_t = node_s.readHoldingRegisters(0x001, 1);
    
  if (result_soil_t == node_s.ku8MBSuccess) {
      // Convert and print the received data
      data.value_temp_soil = (node_s.getResponseBuffer(0) * 0.1);

      
      Serial.print("Температура почвы: ");
      Serial.println(data.value_temp_soil);
      Serial.println("");
    } else {
      Serial.print("Error Температура почвы почвы ");
      Serial.print(": ");
      Serial.println(result_soil_t);
    }
  }

  // Тут будет про реле