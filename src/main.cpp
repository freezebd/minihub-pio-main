

// прошивка тут
// C:\Users\Acer\Documents\PlatformIO\Projects\minihub\.pio\build\esp32doit-devkit-v1

#include <Arduino.h>

#include "led.h"
#include "timer.h"
#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <GyverDBFile.h>
//#include <GyverDS18.h>
#include <GyverNTP.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
#include <WiFiConnector.h>
//#include <modbus.h>

// #include "driver/temp_sensor.h"
#include "data.h"  // тут лежит структура data по кошерному
#include "nastroyki.h"
//#include "sensors.h"
#include "settings.h"
//#include "userTimers.h"

// обявление фкнций для их видимости из вкладок.

Timer each5Sec(5000ul);    // таймер раз в 10 сек
Timer each5min(300000ul);  // таймер раз в 5 мин
Timer eachSec(1000ul);     // таймер раз в сек

LED indikator(INDIKATOR, 300, 3, 50, 20);  // каждые 1000 милисек мигаем 3 раза каждых 50 мс, время горения 20 мсек
bool gotWifi = false;                      // если подключено было к сети
int valNum;
uint32_t startSeconds = 0;
uint32_t stopSeconds = 0;
byte initially = 5;        // первых 10 секунд приписываем время в переменную
// bool firstSlowSensor = 1;  // опрос датчиков по очереди
void setup() {
    each5min.rst();
   // init_pins();

    Serial.begin(115200);
    Serial.println("\n\n\n\t\t\t ESP STARTED !\n\n");

    // ======== SETTINGS ========
    WiFi.mode(WIFI_AP_STA);  // режим AP_STA. Вызываем перед sett.begin(), чтобы settings знал о текущем режиме wifi
    sett.begin();
    sett.onBuild(build);
    sett.onUpdate(update);
    //setup_modbus();          // настройка modbus

    // ======== DATABASE ========
#ifdef ESP32
    LittleFS.begin(true);
#else
    LittleFS.begin();
#endif
    db.begin();
    db.init(kk::wifi_ssid, WIFI);
    db.init(kk::wifi_pass, WIFIPASS);
    db.init(kk::ntp_gmt, 5);

   

    db.init(kk::t1f_enabled, (uint8_t)0);
    db.init(kk::t1f1_startTime, (uint32_t)21600ul);
    db.init(kk::t1f2_startTime, (uint32_t)25200ul);
    db.init(kk::t1f2_dim, (uint8_t)70);
    db.init(kk::t1f3_startTime, (uint32_t)43200ul);
    db.init(kk::t1f3_dim, (uint8_t)95);
    db.init(kk::t1f4_startTime, (uint32_t)64800ul);
    db.init(kk::t1f4_dim, (uint8_t)80);
    db.init(kk::t1f5_startTime, (uint32_t)72000ul);
    db.init(kk::t1_stopTime, (uint32_t)75600ul);

    

    
    // первый запуск всех исполнительных механизмов
    

    // ======== WIFI ========
    // подключение и реакция на подключение или ошибку
    WiFiConnector.setPass("12345678");  // пароль точки доступа
    WiFiConnector.setTimeout(10);       // сколько секунд пытаться приконнектиттся
    WiFiConnector.onConnect([]() {
        Serial.print("Con with IP: ");
        Serial.println(WiFi.localIP());
        indikator.setPeriod(3000, 1, 200, 150);  // раз в 000 сек, 0 раз взмигнем - по 00 милисек периоды, гореть будем 0 милисек
        gotWifi= true;
        
    });NTP.begin();
        NTP.setPeriod(600);  // обновлять раз в 600 сек
        NTP.tick();
        NTP.setGMT(db[kk::ntp_gmt]);
    WiFiConnector.onError([]() {
        Serial.print("Error! start AP ");
        Serial.println(WiFi.softAPIP());
        indikator.setPeriod(600, 2, 100, 50);  // раз в  секунду два раза взмигнем - по 200 милисек, гореть будем 50 милисек
        if (each5min.ready()) ESP.restart();   // через 5 минут ребутаемся
    });

    WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
    //  getdht1(); //опрос медленных датчиков
    //  delay(1);
    //   getdht2();
}  // setup

void loop() {
    // если wifi связь есть, сбрасываем вочдог таймер 5 минутный.
    // если нет связи, ждем 5 минут и ребутаемся, а то мало ли
    // если связь восстановилась после потери, снова мигаем плавно
    WiFiConnector.tick();  // поддержка wifi связи
    if (WiFiConnector.connected()) {
        if (!gotWifi) indikator.setPeriod(3000, 1, 200, 150);  // спокойное мигание после реконнекта к wifi
        gotWifi = true;
        each5min.rst();
    } else {
        if (gotWifi) {
            gotWifi = false;  // для запуска частой мигалки
            // общее время, кол-во, период одного, один зажжен на.
            indikator.setPeriod(1000, 10, 100, 70);  // часто мигаем
        }
        if (each5min.ready()) ESP.restart();
    }  // WiFi.connected()
    sett.tick();  // поддержка веб интерфейса
    NTP.tick();
    indikator.tick();  // in loop

    if (each5Sec.ready())  // раз в 5 сек
    {
        // поддержка NTP
        if (!NTP.status() && NTP.synced()) {
            data.secondsNow = NTP.daySeconds();
            curDataTime = NTP.getUnix();
        } else
            Serial.println("\n\n\t\t\t\tNTP not reached\n\n");
       
       // read_Soil_Hum();    // Опрос датчика почвы
      //  delay(1);   //  отдадим управление вайфаю
       // read_Air_Hum();  // Опрос датчика воздуха
      //  delay(1);   //  отдадим управление вайфаю
       // read_Soil_Temp();   // Опрос датчика почвы
      //  delay(1);   //  отдадим управление вайфаю
       // read_Air_Temp();  // Опрос датчика воздухаd
    }  // each5Sec

    if (eachSec.ready()) {                  // раз в 1 сек
        data.secondsNow++;                  // инкермент реалтайм
        data.secondsUptime++;               // инкермент аптайм
        if (data.secondsUptime == 86399) {  // инкремент дней аптайма
            data.secondsUptime = 0;
            data.uptime_Days++;
        }
      
    }
   // userDhtRelays();  // тут ничего медленного, можно часто
   // userDSRelays();   // тут ничего медленного, можно часто

    
}  // loop