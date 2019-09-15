#include <Time.h>             //https://github.com/PaulStoffregen/Time with patch from https://github.com/PaulStoffregen/Time/pull/131/files
#include <DS1307RTC.h>        //https://github.com/PaulStoffregen/DS1307RTC
#include <Timezone.h>         //https://github.com/JChristensen/Timezone
#include <TinyGPS++.h>        //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <Wire.h>

////////////////////////////////////
// INIT - GPS & REAL-TIME CLOCK
////////////////////////////////////

// Serial config for the GPS module
static const int RXPin = 3, TXPin = 13; // receive on pin 3, second pin is irrelevant
static const uint32_t GPSBaud = 4800; // "BR-355" GPS module uses 4800 baud speed. Cheaper "Neo6Mv2" uses 9600 baud. 

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial GPS_Serial(RXPin, TXPin);

tmElements_t tm;
bool rtcSet = false;

//Timezone stuff
int tzOff = 3; // plus three hours from UTC to Moscow time zone
TimeChangeRule ruMSK = {"MSK", Last, Sun, Mar, 1, tzOff * 60}; // number of hours * 60 mins in each hour = 180 minutes between UTC and Moscow
Timezone tzMSK(ruMSK, ruMSK); // For a time zone that does not change to daylight/summer time, we pass a single rule to the constructor

TimeChangeRule *tcr; //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;

//variables for time counting
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long syncTimer = 0;
unsigned long scheduledTimer = 0;

bool newboot = true;
int prevSecond = 0; // for debugging

////////////////////////////////////
// INIT - NEOPIXEL
////////////////////////////////////

#include <Adafruit_NeoPixel.h> // Adafruit NeoPixel library
#define PIN 2 // LED strip is wired to pin #2
#define NUMPIXELS 288 // total amount of NeoPixels attached to the Arduino

/*
  LED strip is connected as following. Please note that all four digits are wired identically, 
  so drawing numbers in any of the four available slots is done same way - you only have to shift
  by a necessary offset. I.e., to draw "1" in first or third slot you need to light up same amount of
  LEDs (twenty), the only difference is that you need to shift by 148 LEDs.

  1st digit - LEDs   0 to  69
  2nd digit - LEDs  70 to 139 (offset =  70)
  upper dot - LEDs 140 to 143 
  lower dot - LEDs 144 to 147
  3rd digit - LEDs 148 to 217 (offset = 148)
  4th digit - LEDs 218 to 287 (offset = 218)

      237 236 235 234 233 232 231 230 229 228             167 166 165 164 163 162 161 160 159 158                     89  88  87  86  85  84  83  82  81  80                  19  18  17  16  15  14  13  12  11  10  
  238                                         227     168                                         157             90                                          79          20                                          9
  239                                         226     169                                         156             91                                          78          21                                          8
  240                                         225     170                                         155             92                                          77          22                                          7
  241                                         224     171                                         154             93                                          76          23                                          6
  242                                         223     172                                         153             94                                          75          24                                          5
  243                                         222     173                                         152             95                                          74          25                                          4
  244                                         221     174                                         151             96                                          73          26                                          3
  245                                         220     175                                         150             97                                          72          27                                          2
  246                                         219     176                                         149             98                                          71          28                                          1
  247                                         218     177                                         148             99                                          70          29                                          0
      248 249 250 251 252 253 254 255 256 257             178 179 180 181 182 183 184 185 186 187                     100 101 102 103 104 105 106 107 108 109                 30  31  32  33  34  35  36  37  38  39  
  287                                         258     217                                         188     140     139                                         110         69                                          40
  286                                         259     216                                         189     141     138                                         111         68                                          41
  285                                         260     215                                         190     142     137                                         112         67                                          42
  284                                         261     214                                         191     143     136                                         113         66                                          43
  283                                         262     213                                         192             135                                         114         65                                          44
  282                                         263     212                                         193     144     134                                         115         64                                          45
  281                                         264     211                                         194     145     133                                         116         63                                          46
  280                                         265     210                                         195     146     132                                         117         62                                          47
  279                                         266     209                                         196     147     131                                         118         61                                          48
  278                                         267     208                                         197             130                                         119         60                                          49
      277 276 275 274 273 272 271 270 269 268             207 206 205 204 203 202 201 200 199 198                     129 128 127 126 125 124 123 122 121 120                 59  58  57  56  55  54  53  52  51  50  

*/

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// default colors:
byte default_red   =   0;
byte default_green =  90;
byte default_blue  = 255;

// Variables to hold timetable of lessons:
time_t  break1start ;
time_t  break1end   ;
time_t  break2start ;
time_t  break2end   ;
time_t  break3start ;
time_t  break3end   ;
time_t  break4start ;
time_t  break4end   ;
time_t  break5start ;
time_t  break5end   ;
time_t  break6start ;
time_t  break6end   ;
time_t  break7start ;
time_t  break7end   ;

unsigned int isSchoolbreak; // Are we in between lessons?
unsigned long countdownToBreak; // Countdown to next lesson' start.

////////////////////////////////////
// SETUP
////////////////////////////////////

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(GPSBaud);

  pixels.begin(); // Initializes NeoPixel strip object
  pixels.clear(); // Resets LED strip (turns all LEDs off)
  draw_GPS(default_red, default_green, default_blue);
  pixels.show(); // Sends the updated pixel colors to the hardware.
}

////////////////////////////////////
// MAIN LOOP
////////////////////////////////////

void loop() { 
  currentMillis = millis();
  if(!rtcSet){
    syncOnBoot();
  } else {
    setSyncProvider(RTC.get); // tells Arduino to get the time from the RTC
    time_t utc = now();
    local = tzMSK.toLocal(utc, &tcr);
    isSchoolbreak = 0;
    countdownToBreak = 0;
    schoolBreak(); // checks whether we are in between lessons
    handleLED(); // subroutine with all stuff that handles LED display
    debugTime(); // prints time to serial monitor
  }
}

////////////////////////////////////
// FUNCTIONS - TIMETABLE
////////////////////////////////////

void schoolBreak() { // sets times of lessons and breaks on current day (in UTC time zone)

  // #  Lesson starts Lesson ends Break starts  Break ends
  // 1 09:00 09:45 09:45 09:55
  // 2 09:55 10:40 10:40 10:55
  // 3 10:55 11:40 11:40 11:55
  // 4 11:55 12:40 12:40 12:55
  // 5 12:55 13:40 13:40 13:55
  // 6 13:55 14:40 14:40 14:55
  // 7 14:55 15:40 15:40 15:55
  // 8 15:55 16:40 16:40 

  // Times entered here need to be in the local timezone (I don't know why)
  break1start = makeTime(year(local), month(local), day(local),  9, 45,  0);
  break1end   = makeTime(year(local), month(local), day(local),  9, 54, 59);
  break2start = makeTime(year(local), month(local), day(local), 10, 40,  0);
  break2end   = makeTime(year(local), month(local), day(local), 10, 54, 59);
  break3start = makeTime(year(local), month(local), day(local), 11, 40,  0);
  break3end   = makeTime(year(local), month(local), day(local), 11, 54, 59);
  break4start = makeTime(year(local), month(local), day(local), 12, 40,  0);
  break4end   = makeTime(year(local), month(local), day(local), 12, 54, 59);
  break5start = makeTime(year(local), month(local), day(local), 13, 40,  0);
  break5end   = makeTime(year(local), month(local), day(local), 13, 54, 59);
  break6start = makeTime(year(local), month(local), day(local), 14, 40,  0);
  break6end   = makeTime(year(local), month(local), day(local), 14, 54, 59);
  break7start = makeTime(year(local), month(local), day(local), 15, 40,  0);
  break7end   = makeTime(year(local), month(local), day(local), 15, 54, 59);

  if ((local >= break1start) && (break1end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break1end - local);
  };

  if ((local >= break2start) && (break2end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break2end - local);
  };
  
  if ((local >= break3start) && (break3end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break3end - local);
  };
  if ((local >= break4start) && (break4end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break4end - local);
  };
  
  if ((local >= break5start) && (break5end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break5end - local);
  };
  
  if ((local >= break6start) && (break6end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break6end - local);
  };
  
  if ((local >= break7start) && (break7end >= local)) {
    isSchoolbreak = 1;
    countdownToBreak = long(break7end - local);
  };

}

////////////////////////////////////
// FUNCTIONS - CLOCK
////////////////////////////////////

void syncOnBoot() {
  if(newboot) {

    // Read GPS data from serial connection until no more data is available
    while (GPS_Serial.available() > 0) {
      gps.encode(GPS_Serial.read());
    }

    if (currentMillis > 5000 && gps.charsProcessed() < 10) {
      Serial.println(F("No GPS detected: check wiring."));
      while(true); // halt
    }
 
    int satcount = gps.satellites.value();
    if (gps.date.isValid() && gps.time.isValid() && satcount >= 4) {
      Serial.print(F("New boot. Need to update RTC with GPS time. Sat count: "));
      Serial.println(satcount);

      Serial.println(F("Setting RTC from GPS"));
      if (gps.date.isValid() && gps.time.isValid()) {
        tm.Year   = gps.date.year();
        tm.Month  = gps.date.month();
        tm.Day    = gps.date.day();
        tm.Hour   = gps.time.hour();
        tm.Minute = gps.time.minute();
        tm.Second = gps.time.second();

        Serial.print(F("UTC time from GPS is: "));
        padZero(gps.time.hour());
        Serial.print(":");
        padZero(gps.time.minute()); 
        Serial.print(":");
        padZero(gps.time.second()); 
        Serial.println();
        
        // Saves GPS time to RTC
        if (RTC.write(tm)) {
          Serial.println(F("RTC is now set from GPS."));
          rtcSet = true;
          newboot = false;
          syncTimer = 0;
          GPS_Serial.end(); // stops processing GPS data until clock is rebooted (useful since SoftwareSerial messes with hardware timers and causes clock to drift)
          setSyncProvider(RTC.get); // tells Arduino to get the time from the RTC
          time_t utc = now();
          local = tzMSK.toLocal(utc, &tcr);
          if(timeStatus()!= timeSet) {
            Serial.println(F("Unable to sync with the RTC, check wiring! "));
            while(true); // halt
          } else {
            Serial.println(F("Initialized the RTC."));
          }
        } else {
          Serial.println(F("Error setting RTC from GPS values: check wiring."));
          while(true); // halt
        }
      } else {
        Serial.println(F("No GPS fix yet. Can't set RTC yet."));
      }

    } else {
      if(currentMillis > (syncTimer + 250)) {
        Serial.print(F("GPS not ready yet. Waiting for fix. Sat count: "));
        Serial.println(satcount);
        syncTimer = currentMillis;
      }
    }
  }
}

void debugTime() {
  if(!rtcSet)  {
    Serial.println(F("Waiting for GPS Fix"));
  } else {
    if (second(local) != prevSecond){
      padZero(  hour(local));
      Serial.print(":");
      padZero(minute(local)); 
      Serial.print(":");
      padZero(second(local)); 
      Serial.println();
    
      // Serial.print(F("Are we in a break? "));
      // Serial.println(isSchoolbreak);
      // Serial.print(F("Countdown to next lesson: "));
      // Serial.println(countdownToBreak);
    }
    prevSecond = second(local);
  }
}

void padZero(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  if(digits < 10) {
    Serial.print('0');
  }
  Serial.print(digits);
}

////////////////////////////////////
// FUNCTIONS - LED STRIP
////////////////////////////////////

void draw_GPS(int red, int green, int blue) {
  // Draws four characters, "GPS…", on the LED strip while we get correct time from satellites
  /*
    G: 228-237, 238-247, 278-287, 268-277, 258-267
    P: 148-157, 158-167, 168-177, 178-187, 208-217
    S: 80-89, 90-99, 100-109, 110-119, 120-129
    hellip: 50-51, 54-55, 58-59
  */
  pixels.clear(); // Set all pixel colors to 'off'
  strip(228, 237, 0, red, green, blue); 
  strip(238, 247, 0, red, green, blue); 
  strip(278, 287, 0, red, green, blue); 
  strip(267, 277, 0, red, green, blue); 
  strip(258, 267, 0, red, green, blue); 
  strip(148, 157, 0, red, green, blue); 
  strip(158, 167, 0, red, green, blue); 
  strip(168, 177, 0, red, green, blue); 
  strip(178, 187, 0, red, green, blue); 
  strip(208, 217, 0, red, green, blue); 
  strip( 80,  89, 0, red, green, blue); 
  strip( 90,  99, 0, red, green, blue); 
  strip(100, 109, 0, red, green, blue); 
  strip(110, 119, 0, red, green, blue); 
  strip(120, 129, 0, red, green, blue); 
  strip( 50,  50, 0, red, green, blue); 
  strip( 54,  54, 0, red, green, blue); 
  strip( 58,  58, 0, red, green, blue); 
  pixels.show();
}

void handleLED() { // Main routine that handles displaying time on LED strip:
  pixels.clear(); // Set all pixel colors to 'off'
  if(isSchoolbreak == 0) {
    drawtime(default_red, default_green, default_blue); // Displays current time
  } else {
    drawcountdown(int(countdownToBreak));
  }
  pixels.show();   // Sends the updated pixel colors to the hardware.  
}

void drawtime(int red, int green, int blue) {
  int h =   hour(local);
  int m = minute(local);

  // placeholders for upper and lower digits of time - i.e. "11:23" - "3" is lower minute digit, "2" is upper
  int h_lo_digit = h % 10;
  int h_hi_digit = (h - h_lo_digit) / 10;
  int m_lo_digit = m % 10;
  int m_hi_digit = (m - m_lo_digit) / 10;

  drawdigit(h_hi_digit, 4, red, green, blue);  // leftmost position (4th from right)
  drawdigit(h_lo_digit, 3, red, green, blue);  
  drawdigit(m_hi_digit, 2, red, green, blue);  
  drawdigit(m_lo_digit, 1, red, green, blue);  // rightmost position

  if ((millis() % 800) >= 400) {
    // blinks with colon every 400ms
    draw_upper_dot(red, green, blue); 
    draw_lower_dot(red, green, blue); 
  }
}

void drawcountdown(int cnt) { // displays the countdown - time left till next lesson starts
  // placeholders for upper and lower digits of countdown timer
  int thousands = 0;
  int  hundreds = 0;
  int      tens = 0;
  int   singles = 0;
  
  int countdown_red   = 220;
  int countdown_green = 220;
  int countdown_blue  = 220;

  if (cnt < 60) {
    countdown_red = 255;
    countdown_green = 0;
    countdown_blue = 0;
  }

  // here we split the countdown timer in 3 digits to be displayed separately

  if (cnt >= 1000) {
    thousands = (cnt / 1000);
    cnt = cnt % 1000;
  }

  if (cnt >= 100) {
    hundreds = (cnt / 100);
    cnt = cnt % 100;
  }

  if (cnt >= 10) {
    tens = (cnt / 10);
    cnt = cnt % 10;
  }

  singles = cnt;

  if (thousands != 0) { // leftmost position (4th from right)
    drawdigit(thousands, 4, countdown_red, countdown_green, countdown_blue);
  }
  if (hundreds != 0) {
    drawdigit(hundreds, 3, countdown_red, countdown_green, countdown_blue);
  }
  else {
    if (thousands != 0) {
      drawdigit(hundreds, 3, countdown_red, countdown_green, countdown_blue);
    }
  }
  if (tens != 0) {
    drawdigit(tens, 2, countdown_red, countdown_green, countdown_blue);
  }
  else {
    if (hundreds != 0) {
      drawdigit(tens, 2, countdown_red, countdown_green, countdown_blue);
    }
  }
  drawdigit(singles, 1, countdown_red, countdown_green, countdown_blue); // rightmost position
}

void strip(int start, int end, int offset, int red, int green, int blue) {
  // Sub-routine to draw a given sequence of LEDs in a given RGB color
  for (int i = (start + offset); i <= (end + offset); i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

// Sub-routines to draw a colon in the center of the clock (eight LEDs) in a given RGB color
void draw_upper_dot(int red, int green, int blue) {
  for (int i = 140; i <= 143; i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

void draw_lower_dot(int red, int green, int blue) {
  for (int i = 144; i <= 147; i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

void drawdigit(int digit, int location, int red, int green, int blue) {
  // Sub-routine to draw a number (0 to 9) in any of 4 available locations in a given RGB color
  int offset = 0;
  switch (location) {
    // LEDs are identically placed, the only difference is the offset of the first LED in each of four digits:
    case 1:
      offset = 0;
      break;
    case 2:
      offset = 70;
      break;
    case 3:
      offset = 148;
      break;
    case 4:
      offset = 218;
      break;
  }

  switch (digit) {
    case 1:
      strip(  0,   9, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      break;
    case 2:
      strip( 10,  19, offset, red, green, blue);
      strip(  0,   9, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 60,  69, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      break;
    case 3:
      strip( 10,  19, offset, red, green, blue);
      strip(  0,   9, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      break;
    case 4:
      strip( 20,  29, offset, red, green, blue);
      strip(  0,   9, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      break;
    case 5:
      strip( 10,  19, offset, red, green, blue);
      strip( 20,  29, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      break;
    case 6:
      strip( 10,  19, offset, red, green, blue);
      strip( 20,  29, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      strip( 60,  69, offset, red, green, blue);
      break;
    case 7:
      strip(  0,   9, offset, red, green, blue);
      strip( 10,  19, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      break;
    case 8:
      strip(  0,   9, offset, red, green, blue);
      strip( 10,  19, offset, red, green, blue);
      strip( 20,  29, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      strip( 60,  69, offset, red, green, blue);
      break;
    case 9:
      strip(  0,   9, offset, red, green, blue);
      strip( 10,  19, offset, red, green, blue);
      strip( 20,  29, offset, red, green, blue);
      strip( 30,  39, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      break;
    case 0:
      strip(  0,   9, offset, red, green, blue);
      strip( 10,  19, offset, red, green, blue);
      strip( 20,  29, offset, red, green, blue);
      strip( 40,  49, offset, red, green, blue);
      strip( 50,  59, offset, red, green, blue);
      strip( 60,  69, offset, red, green, blue);
      break;
  }
}
