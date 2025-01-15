
#pragma once
#include <Arduino.h>
#include <GyverDBFile.h>
#include <GyverNTP.h>
#include <SettingsGyver.h>


DB_KEYS(  // ключи по которым идет привязка к базе и к виджетам в web
    kk,
    wifi_ssid,
    wifi_pass,
    apply,
    ntp_gmt,
    secondsNow,
    secondsUptime,
    dayofweek,
    uptimeDays,

    sens_air_t,
    sens_air_h,
    sens_soil_t,
    sens_soil_h,

    t1Discr_name,
   
    t2Discr_name,
    
    t3Discr_name,
    
    t4Discr_name,
   
    t5Discr_name,
    
    t6Discr_name,
    

    t1f_enabled,
    t1f_led,
    t1f1_startTime,
    t1f2_startTime,
    t1f2_dim,
    t1f3_startTime,
    t1f3_dim,
    t1f4_startTime,
    t1f4_dim,
    t1f5_startTime,
    t1_stopTime,
    t1_btn_accept,

    
    date,
    datime,

    btn1,
    btn2);

extern GyverDBFile db;  // база данных для хранения настроек будет автоматически записываться в файл при изменениях
extern SettingsGyver sett;

extern Datime curDataTime;

// чтобы функции были видимыми, делаем их в *.cpp  не static
// и тут объявляем. Или же делаем их static ( не видимыми за пределами)
//  и тут не объявляем
void build(sets::Builder &b);
void update(sets::Updater &upd);