<H2 align='center'>Часы-будильник с подсветкой и звуковой сигнализацией</H2>

###### В проект включено: светодионое кольцо __WS2812B__ с 16 светодиодами, которое реализует световые эффекты, плеер **DFPlayer Mini** для воспроизведения музыки или других звуковых эффектов, часы реального времени **RTC DS3231** и для отображении информации 4-х сегментный жидкокристаллический индикатор **TM1637**.

###### Для функционирования mp3-плеер нужно в корневую директорию карты памяти microSD поместить файл с именем 001.mp3 который Вы хотите услышать при срабатывании будильника. 
На панели управления часами находятся 4 кнопки (Left, Right, Set, Cancel) и один сенсорный индикатор. При включении часы находятся в рабочем режиме. 
При нажатии на кнопку Cancel, отобразится время будильника.
Касаясь сенсорного индикатора происходит демонстрация эффекта в течении 10 секунд (зависит от установок в setup.h файле при компиляции). При повторном касании индикатора, эффект переключается (их всего шесть, можно добавлять) и сохраняется в памяти для последующего включения его при срабатывании будильника.
Если нажать кнопку Left или кнопку Right перейдем в режим настройки часов (3 опции). Установка времени, установка будильника, установка яркости. Перемещение по меню осуществляется этими же кнопками. Для выхода в рабочий режим, нажимаем кнопку Cancel.
Set времени осуществляется нажатием кнопки Set. Устанавливаемый параметр будет отмечем миганием. Кнопками Left и Right устанавливаем необходимую величину параметра и нажатием кнопки Set, фиксируем изменение.
При срабатывании будильника запустится звуковое сопровождение и включится световой эффект. Чтобы остановить будильник нужно коснуться сенсорного индикатора один раз либо он прекратит сигнализировать спустя 5 минут (длительность устанавливается в setup.h).
Для установки громкости, в рабочем режиме нажмите кнопку Set и кнопками Left и Right выбираем необходимую громкость. Нажимаем кнопку Set еще раз для фиксации уровня громкости.
