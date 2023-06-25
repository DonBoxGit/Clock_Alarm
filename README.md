<H2 align='center'>Часы-будильник с подсветкой и звуковой сигнализацией</H2>


##### В проект включено: светодионое кольцо ***WS2812B*** с 16 светодиодами, которое реализует световые эффекты, плеер ***DFPlayer Mini*** для воспроизведения музыки или других звуковых эффектов, часы реального времени ***RTC DS3231*** и для отображении информации 4-х сегментный жидкокристаллический индикатор ***TM1637***. <br><br>Используются библиотеки EncButton, GyverTM1637 и microDS3231 от AlexGyver, DFRobotDFPlayerMini от DFRobot и FastLED от Даниэля Гарсии. Также использовались библиотеки EEPROM, оригинальные авторские права Дэвида А. Меллиса (Новая версия Кристофера Эндрюса, 2015 г.) и TwoWire.h — библиотека для Arduino & Wiring авторские прав Николаса Замбетти.
------------------------
###### Для функционирования ***mp3-плеер*** нужно в корневую директорию карты памяти microSD поместить файл с именем ***001.mp3*** который Вы хотите услышать при срабатывании будильника. <br><br>На панели управления часами находятся 4 кнопки `Left` `Right`, `Set`, `Cancel`) и один сенсорный индикатор. При включении часы находятся в ***рабочем*** режиме. <br><br>Находясь в ***рабочем*** режиме, однократное нажатие на кнопку `Cancel`, отобразит (на 5 секунд) время будильника. При касании сенсорного индикатора происходит демонстрация эффекта в течении 10 секунд (зависит от установок в ***config.h*** файле при компиляции). Во время демонстрации эффекта, при повторном касании индикатора, пресет эффекта переключается (их всего шесть, можно добавлять) и сохраняется в памяти для последующего включения его при срабатывании будильника. <br><br>Для установки громкости, в ***рабочем*** режиме нажмите кнопку `Set` и кнопками `Left` и `Right` устанавливаем необходимую громкость. Нажимаем кнопку `Set` еще раз для фиксации и сохранения уровня громкости или нажимаем кнопку `Cancel` для выхода. <br><br>Если нажать кнопку `Left` или кнопку `Right` находясь в ***рабочем*** режиме, то перейдем в режим ***настройки*** часов, где имеется 3 опции. ***Установка времени***, ***установка будильника***, ***установка яркости***. Перемещение по меню осуществляется этими же кнопками. Установка значений в опциях, осуществляется нажатием кнопки `Set`. Устанавливаемый параметр будет отмечем миганием. Кнопками `Left` и `Right` устанавливается необходимая величина параметра, кнопка `Set`, фиксирует изменение. Нажатие кнопки `Cancel` - выход в ***рабочий*** режим. <br><br>При срабатывании будильника запустится звуковое сопровождение и включится световой эффект. Чтобы остановить будильник нужно коснуться сенсорного индикатора один раз либо он прекратит сигнализировать спустя 5 минут (длительность устанавливается в ***config.h***).
