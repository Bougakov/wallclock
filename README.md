# Firmware for the clock in the hallway of http://sch57.ru

This is a firmware for a giant clock made of multiple layers of plywood and acrylic and five meters of "WS2812b" ("Neopixel") LED strip. It uses GPS module to set the correct time from satellites on the startup. RTC module ("DS1307") is used for timekeeping. 

During the lessons and in after-school hours it displays current time; during the breaks it acts as a count-down timer.

This clock is a part of a large-scale redesign of School 57 by a Russian architect and designer, Mia Karlova, which won the "Best public space" award by AD Magazine in https://www.admagazine.ru/ad-design-award/novyj-oblik-moskovskoj-shkoly-57 and was featured in FRAME magazine: https://www.frameweb.com/news/moscow-school-57-mia-karlova

![Assembled clock on the wall](https://user-images.githubusercontent.com/1763243/65143810-5d81f380-da05-11e9-9e41-a72968dae387.png)

[Click to view video on Instagram: ![Preview - Instagram](https://user-images.githubusercontent.com/1763243/64928813-a28f0580-d80c-11e9-9330-3cba9d5aab31.png)](https://www.instagram.com/p/BxpXTDVDegR/)

Initally, a large TV screen was used instead of this clock:

![Old TV was replaced with this clock](https://user-images.githubusercontent.com/1763243/64928726-cf8ee880-d80b-11e9-99cf-2e64978a1c44.png)

The clock can be assembled from laser-cut sheets of 3-4mm plywood. The [layouts (in AutoCAD format)](https://github.com/Bougakov/wallclock/tree/master/CAD%20files%20%28plywood%20lasercut%29) are designed in such a way that any laser cutter with working area of as low as 300x600mm could be used. Few dozen of 40mm M3 screws are needed for assembly. Diffusors are made of 2mm matte acrylic.

[Click to view video on Instagram: ![Countdown - Instagram](https://user-images.githubusercontent.com/1763243/64928825-ceaa8680-d80c-11e9-9caf-27436787bc27.png)](https://www.instagram.com/p/B2cq6O_Ikv1/)

Click the preview of the AutoCAD drawing to read more on the assembly:

[![AutoCAD drawing in full glory](https://user-images.githubusercontent.com/1763243/65395618-083f3c80-dd8d-11e9-930d-021e4f74db41.png)](https://github.com/Bougakov/wallclock/tree/master/CAD%20files%20%28plywood%20lasercut%29)

## Wiring

* GPS is connected (via "MAX232" RS232 to TTL converter) to `D3` pin of Arduino. Don't forget to double-check the baud rate of your GPS.
* LED strip (288 "WS2812" LEDs) – to pin `D2`.
  * Each "digit" is made of 7 segments, 10 LEDs each. 
  * "Colon" in the middle is made of 2 segments, 4 LEDs each.
* Controller – an Arduino Nano (Atmega 328P, "old bootloader")

![Wiring](https://github.com/Bougakov/wallclock/blob/master/Wall%20clock%20schematics.png)

## Important notes

Avoid voltage drop by wiring LED strip to external power source in multiple points.
For heat dissipation use 5m of 10mm aluminum strip cut to length of LED fragments. There are vent holes on the back that allow aluminum strips to dissipate heat to the wall. Fix the LEDs and strips using zip ties.

To radically simplify the programming of the clock make sure you connect the segments in each digit in the identical order. Check out attached Excel file to get the idea on how to do it. 

## Adjusting time table for your school:

To change the time ranges check out this code fragment:

```
  break2start = makeTime(year(local), month(local), day(local), 10, 40,  0);
  break2end   = makeTime(year(local), month(local), day(local), 10, 54, 59);
```

It tells Arduino that the break after the 2nd lesson is between 10:40am and 10:55. Please note that times entered here need to be in the local timezone, not UTC.
