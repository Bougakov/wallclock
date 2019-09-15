# Firmware for the clock in the hallway of http://sch57.ru

This is a firmware for a giant clock made of multiple layers of plywood and acrylic and five meters of "WS2812b" ("Neopixel") LED strip. It uses GPS module to set the correct time from satellites on the startup. RTC module ("DS1307") is used for timekeeping. 

During the lessons and in after-school hours it displays current time; during the breaks it acts as a count-down timer.

This clock is a part of a large-scale redesign of School 57 by a Russian architect and designer, Mia Karlova, which won the "Best public space" award by AD Magazine in https://www.admagazine.ru/ad-design-award/novyj-oblik-moskovskoj-shkoly-57 and was featured in FRAME magazine: https://www.frameweb.com/news/moscow-school-57-mia-karlova

Initally, a large TV screen was used instead of this clock:

![Old TV was replaced with this clock](https://user-images.githubusercontent.com/1763243/64928726-cf8ee880-d80b-11e9-99cf-2e64978a1c44.png)

The clock can be assembled from laser-cut sheets of 4mm plywood. The layouts (in AutoCAD format) are designed in such a way that any laser cutter with working area of as low as 300x600mm could be used. Few dozen of 40mm M3 screws are needed for assembly. Diffusors are made of 2mm matte acrylic.

The test of the clock's hardware - https://www.instagram.com/p/BxpXTDVDegR/ 

Countdown feature in action - https://www.instagram.com/p/B2cq6O_Ikv1/

## Wiring

* GPS is connected (via "MAX232" RS232 to TTL converter) to `D3` pin of Arduino.
* LED strip (288 "WS2812" LEDs) – to pin `D2`.
  * Each "digit" is made of 7 segments, 10 LEDs each. 
  * "Colon" in the middle is made of 2 segments, 4 LEDs each.
* Controller – an Arduino Nano (Atmega 328P, "old bootloader")

![Wiring](https://github.com/Bougakov/wallclock/blob/master/Wall%20clock%20schematics.png)

## Important notes

Avoid voltage drop by wiring LED strip to external power source in multiple points.
For heat dissipation use 5m of 10mm aluminum strip cut to length of LED fragments. There are vent holes on the back that allow aluminum strips to dissipate heat to the wall. Fix the LEDs and strips using zip ties.
