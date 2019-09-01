# Прошивка к часам в холле http://sch57.ru

LED-часы с установкой точного времени по GPS из пятиметровой полосы LED "WS2812b" ("Neopixel"). Общий вид - https://www.instagram.com/p/BxpXTDVDegR/ 

* GPS подключается (через MAX232) к входу Arduino «D3».
* Лента (288 светодиодов WS2812) – к выводу «D2».
  * Каждая цифра – 7 сегментов по 10 LED. 
  * «Точки» посередине – два сегмента по 4 LED.
* Контроллер – Arduino Nano (Atmega 328P, "old bootloader")

Подключение:

![Wiring](https://github.com/Bougakov/wallclock/blob/master/Wall%20clock%20schematics.png)
