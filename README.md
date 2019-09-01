# Прошивка к часам в холле http://sch57.ru

LED-часы с установкой точного времени по GPS из пятиметровой полосы LED "WS2812b" ("Neopixel"). 

Общий вид (видео) - https://www.instagram.com/p/BxpXTDVDegR/ 

![Instagram](https://scontent-arn2-1.cdninstagram.com/vp/4b3fefc065ee42ab763cbce7da96b675/5D6E7A5E/t51.2885-15/e35/58775460_2222468581172818_1322464915588762730_n.jpg?_nc_ht=scontent-arn2-1.cdninstagram.com)

## Подключение:

* GPS подключается (через MAX232) к входу Arduino «D3».
* Лента (288 светодиодов WS2812) – к выводу «D2».
  * Каждая цифра – 7 сегментов по 10 LED. 
  * «Точки» посередине – два сегмента по 4 LED.
* Контроллер – Arduino Nano (Atmega 328P, "old bootloader")

![Wiring](https://github.com/Bougakov/wallclock/blob/master/Wall%20clock%20schematics.png)

## Текущие проблемы

Часы "убегают" по неизвестной причине:

```02:03:07.024 -> ---RTC TIME---
02:03:07.024 -> UTC: 2019/6/23 23:3:6
02:03:07.024 -> RTC has set the system time
02:03:07.024 -> 2:03:06
02:03:07.278 -> GPS not ready yet. Waiting for fix. Sat count: 0
02:03:07.646 -> GPS not ready yet. Waiting for fix. Sat count: 10
02:03:07.856 -> New boot. Need to update RTC with GPS time. Sat count: 10
02:03:07.856 -> Setting RTC from GPS
02:03:07.856 -> 
02:03:07.856 -> RTC set from GPS
02:03:08.227 -> 2:03:07
02:03:09.461 -> 2:03:08
02:03:10.982 -> 2:03:09
02:03:12.174 -> 2:03:10
02:03:13.341 -> 2:03:11
02:03:14.646 -> 2:03:12
02:03:16.096 -> 2:03:13
02:03:17.274 -> 2:03:14
02:03:18.541 -> 2:03:15
```

Если в начале расхождение между RTC и GPS составляет секунду, то в конце "набегает" уже три секунды разницы.
