# Прошивка к часам в холле http://sch57.ru

LED-часы с установкой точного времени по GPS из пятиметровой полосы LED "WS2812b" ("Neopixel"). 

Общий вид (видео) - https://www.instagram.com/p/BxpXTDVDegR/ 

![Instagram](https://scontent-arn2-1.cdninstagram.com/vp/4b3fefc065ee42ab763cbce7da96b675/5D6E7A5E/t51.2885-15/e35/58775460_2222468581172818_1322464915588762730_n.jpg?_nc_ht=scontent-arn2-1.cdninstagram.com)

* GPS подключается (через MAX232) к входу Arduino «D3».
* Лента (288 светодиодов WS2812) – к выводу «D2».
  * Каждая цифра – 7 сегментов по 10 LED. 
  * «Точки» посередине – два сегмента по 4 LED.
* Контроллер – Arduino Nano (Atmega 328P, "old bootloader")

Подключение:

![Wiring](https://github.com/Bougakov/wallclock/blob/master/Wall%20clock%20schematics.png)
