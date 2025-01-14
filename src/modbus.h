#pragma once
#include <Arduino.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

SoftwareSerial rs485(16,17);
ModbusMaster node, node_s;

void setup_bodbus() {
  // Initialize RS-485 serial communication
  rs485.begin(4800); // RX, TX pins
  // Initialize ModbusMaster instance
  node_s.begin(1,rs485); // 1 адрес датчика почвы
  node.begin(2,rs485); // 2 адрес датчика воздуха
}

void readParameter(uint16_t registerAddress, const char* name, float conversionFactor) { //Функция чтения данных датчика воздуха
  uint8_t result;
  
  // Read data from the specified register address
  result = node.readHoldingRegisters(registerAddress, 2);

  if (result == node.ku8MBSuccess) {
    // Convert and print the received data
    float value = node.getResponseBuffer(0) * conversionFactor;
    Serial.print(name);
    Serial.print(": ");
    Serial.println(value);
  } else {
    Serial.print("Error reading ");
    Serial.print(name);
    Serial.print(": ");
    Serial.println(result);
  }
}

void readParameter_Soil(uint16_t registerAddress, const char* name, float conversionFactor) { //Функция чтения данных датчика почвы
  uint8_t result_soil;
  
  // Read data from the specified register address
  result_soil = node_s.readHoldingRegisters(registerAddress, 2);

  if (result_soil == node_s.ku8MBSuccess) {
    // Convert and print the received data
    float value = node_s.getResponseBuffer(0) * conversionFactor;
    Serial.print(name);
    Serial.print(": ");
    Serial.println(value);
  } else {
    Serial.print("Error reading ");
    Serial.print(name);
    Serial.print(": ");
    Serial.println(result_soil);
  }
}

void riadSensor() { // вызов функции чтение данных с датчиков
  // Read and print each parameter
  readParameter(0x0000, "Влажность   воздуха", 0.1); // 0.1%RH
  readParameter(0x0001, "Температура воздуха", 0.1); // 0.1°C
  Serial.println("");
  delay(10);
  readParameter_Soil(0x0000, "Влажность   почвы", 0.1); // 0.1%RH
  readParameter_Soil(0x0001, "Температура почвы", 0.1); // 0.1°C
  Serial.println("");
  
}