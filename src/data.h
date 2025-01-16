/* как пользоваться кошерно данными
читаем тут
 https://alexgyver.ru/docs/program-flow/cpp-files/#9-toc-title
пункт Составные типы

вот пример организации файлов
// data.h
#pragma once
struct Data {};
extern Data data;

// data.cpp
#include "data.h"
Data data;

//main.cpp
#include "data.h"
// переменная data уже видна тут
*/

#pragma once
#include <Arduino.h>


struct Data  // обьявляем класс структуры
{
    uint32_t secondsNow = 44000ul;
    uint32_t secondsUptime = 1;
    byte uptime_Days = 0;
    // uint32_t secondsUptime = 86390;
    // byte uptime_Days = 1;


    // Переменные датчиков
    float value_temp_air = -100.0;  // датчик температуры воздуха rs485
    float value_hum_air = -100.0;   // датчик влажности воздуха rs485
    float value_temp_soil = -100.0; // датчик температуры почвы rs485
    float value_hum_soil = -100.0;  // датчик влажности почвы rs485

};

// for cpp
extern Data data;  // объявляем что у нас будет переменная data класса Data
