#include "settings.h"

#include <GyverNTP.h>
#include <LittleFS.h>
#include <SettingsGyver.h>
#include <WiFiConnector.h>

#include "data.h"  // тут лежит структура data по кошерному
#include "nastroyki.h"
//#include "userTimers.h"

// GyverDBFile db(&LittleFS, "/data.db");
// static SettingsESP sett(PROJECT_NAME " v" PROJECT_VER, &db);

GyverDBFile db(&LittleFS, "/nicelight.db");      // база данных для хранения настроек будет автоматически записываться в файл при изменениях
SettingsGyver sett("Горошек для любимой", &db);  // указывается заголовок меню, подключается база данных
Datime curDataTime(NTP);                         // NTP это объект типа GyverNTPClient, наследует stampticker (С) Гайвер
static bool notice_f;                            // флаг на отправку уведомления о подключении к wifi

static const char *const WEEKdays[] = {
    "как будто бы вчера",
    "Понедельник",
    "Вторник",
    "Среда",
    "Четверг",
    "Пятница",
    "Суббота",
    "Воскресенье"};
// это апдейтер. Функция вызывается, когда вебморда запрашивает обновления

sets::Logger logger(150);

void update(sets::Updater &upd) {
    // отправляем свежие значения по имени (хэшу) виджета

    upd.update(kk::secondsNow, data.secondsNow);
    upd.update(kk::secondsUptime, data.secondsUptime);

    // upd.update(kk::datime, String(curDataTime)); // старое
    upd.update(kk::datime, NTP.dateToString());
    // upd.update(kk::testlabel,  NTP.dateToString()); //https://github.com/GyverLibs/Stamp

    if (!data.uptime_Days) {
        upd.update(kk::uptimeDays, (String)("0 дней"));
    } else if (data.uptime_Days == 1)
        upd.update(kk::uptimeDays, (String)("1 день"));
    else if (data.uptime_Days < 5)
        upd.update(kk::uptimeDays, (String)(data.uptime_Days + String(" дня")));
    else if (data.uptime_Days >= 5)
        upd.update(kk::uptimeDays, (String)(data.uptime_Days + String(" дней")));
    // день недели выводим, оч красиво, Гайвер посоветовал

    upd.update(kk::dayofweek, (String)(WEEKdays[curDataTime.weekDay]));

    upd.update("sens_air_t"_h, (String)(data.value_temp_air + String(" °C")));
    upd.update("sens_air_h"_h, (String)(data.value_hum_air + String(" %RH")));
    upd.update("sens_soil_t"_h, (String)(data.value_temp_soil + String(" °C")));
    upd.update("sens_soil_h"_h, (String)(data.value_hum_soil + String(" %RH")));

 

    upd.update("lbl1"_h, (String)(curDataTime.weekDay + String(" день недели")));
    upd.update("lbl2"_h, millis());
    if (notice_f)  // уведомление при вводе wifi данных
    {
        notice_f = false;
        upd.notice("Если не ошибся с вводом, устройство подключится к  wifi сети, светодиод медленно замигает");
        //    upd.alert("Ошибка");
    }
}  // update
//
//
//

void build(sets::Builder &b) {
    // обработка действий от виджетов:
    switch (b.build.id) {
        // case kk::logUpdate:  // если ввели импут
        //     // logger.println(b.build.id, HEX);
        //     logger.print("старт таймера 1 в ");
        //     logger.println(db[kk::t1Discr_startTime]);
        //     logger.print(" curDataTime.weekDay:");
        //     logger.println(curDataTime.weekDay);
        //     break;
    }

   
    // WEB интерфейс ВЕБ морда формируется здесь
    {
        sets::Group g(b, "Nicelight");
        if (NTP.synced()) {
            {
                sets::Row g(b);
                // sets::Row g(b, "Row");
                // b.DateTime(kk::datime, "Сегодня ");
                b.Label(kk::dayofweek, "Сегодня");  // текущая дата
                b.Label(kk::datime, " ");           // текущая дата
            }
        }  // NTP.synced()

        {
            sets::Row g(b);
            // sets::Row g(b, "Row");
            b.Label(kk::uptimeDays, "Аптайм");
            b.Time(kk::secondsUptime, " ");
        }

        b.Time(kk::secondsNow, "Времечко");
        // b.Label(kk::secondsNow, "Времечко");
    }

    {
        sets::Group g(b, "Воздух");
        {
            sets::Row g(b);
            b.Label(kk::sens_air_t, "Температура воздуха","", sets::Colors::Green);    // RS485 
            
        }
        {
            sets::Row g(b);
            b.Label(kk::sens_air_h, "Влажность воздуха","", sets::Colors::Green);    // RS485
        }
       
    }  //"Воздух"
    //
    //

    {  // "Почва"
        sets::Group g(b, "Почва");
        {
            sets::Row g(b);
            b.Label(kk::sens_soil_h, "Влажность почвы","", sets::Colors::Aqua);    // RS485
             
        }
        {
            sets::Row g(b);
            b.Label(kk::sens_soil_t, "Температура почвы","", sets::Colors::Aqua);    // RS485 
            
        }
        
    }  //"Почва"

   
    /* Настройки , внизу страницы*/
    {
        sets::Group g(b, " ");
        {
            sets::Menu g(b, "Опции");

            {
                sets::Menu g(b, "Интерфейс");
                /*
                      //пример изменения имени виджета
                      b.Input(kk::btnName, "новое имя кнопки:");
                      if(b.Button(kk::btnflex, db[kk::btnName], db[kk::btnColor])) b.reload();
                */

                b.Input(kk::t1Discr_name, "Имя Реле1:");
                b.Input(kk::t2Discr_name, "Имя Реле2:");
                b.Input(kk::t3Discr_name, "Имя Реле3:");
                b.Input(kk::t4Discr_name, "Имя Реле4:");
                b.Input(kk::t5Discr_name, "Имя Реле5:");
                b.Input(kk::t6Discr_name, "Имя Реле6:");
                b.Input(kk::dht1name, "dht22 1:");
                b.Input(kk::dht2name, "dht22 2:");
                b.Input(kk::DS1name, "DS18B20 1:");
                b.Input(kk::DS2name, "DS18B20 2:");
            }
            {
                sets::Menu g(b, "Расширенные");
                /// провалились в расширенные пристройки
                {
                    sets::Group g(b, "настройки WiFi");
                    b.Input(kk::wifi_ssid, "SSID");
                    b.Pass(kk::wifi_pass, "Password");
                    if (b.Button(kk::apply, "Save & Restart")) {
                        db.update();  // сохраняем БД не дожидаясь таймаута
                        WiFiConnector.connect(db[kk::wifi_ssid], db[kk::wifi_pass]);
                        notice_f = true;
                        //          ESP.restart();
                    }  // button Save
                }  // настройки wifi
                b.Input(kk::ntp_gmt, "Часовой пояс");
                

                // // и просто виджеты
                // b.Label("lbl3"_h, "Another label", "Val", sets::Colors::Green);
                // b.Label("lbl4"_h, "Привет", "Val", sets::Colors::Blue);

                // кнопки являются "групповым" виджетом, можно сделать несколько кнопок в одной строке
                if (b.beginButtons()) {
                    // кнопка вернёт true при клике
                    // if (b.Button(kk::btn1, "-")) {
                    //     Serial.println("reload");
                    //     b.reload();
                    // }

                    if (b.Button(kk::btn2, "стереть базу(аккуратно!)", sets::Colors::Red)) {
                        Serial.println("could clear db");
                        // db.clear();
                        // db.update();
                    }
                    b.endButtons();  // завершить кнопки
                }
            }  // Расширенные
        }
    }  // Подстройки

}  // builder
