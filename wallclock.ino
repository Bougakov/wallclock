/* =============================================================== */

#include <TimeLib.h> // https://github.com/PaulStoffregen/Time
#include <TinyGPS.h> // http://arduiniana.org/libraries/TinyGPS/
#include <SoftwareSerial.h>

// GPS module is handled by SoftwareSerial
SoftwareSerial SerialGPS = SoftwareSerial(3, 13);  // receive on pin 3, second pin is irrelevant
TinyGPS gps; 
const int offset = 3;   // Moscow Time

/* =============================================================== */

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

/* =============================================================== */

void setup() {
  Serial.begin(115200);
  // while (!Serial) ; // Needed for Leonardo only
  SerialGPS.begin(4800); // "BR-355" GPS module uses 4800 baud speed. Cheaper "Neo6Mv2" uses 9600 baud. Check the datasheet for your model.
  Serial.println("Initializing LED strip ... ");
  pixels.begin(); // Initializes NeoPixel strip object
  pixels.clear(); // Resets LED strip (turns all LEDs off)
  draw_GPS();
}

void draw_GPS() {
  // Draws "GPS..." on the LED strip while we get correct time from satellites
  /*
    G: 228-237, 238-247, 278-287, 268-277, 258-267
    P: 148-157, 158-167, 168-177, 178-187, 208-217
    S: 80-89, 90-99, 100-109, 110-119, 120-129
    hellip: 50-51, 54-55, 58-59
  */

  strip(228, 237, 0, 0,  90,  90 );
  strip(238, 247, 0, 0,  90,  90 );
  strip(278, 287, 0, 0,  90,  90 );
  strip(267, 277, 0, 0,  90,  90 );
  strip(258, 267, 0, 0,  90,  90 );
  strip(148, 157, 0, 0,  90,  90 );
  strip(158, 167, 0, 0,  90,  90 );
  strip(168, 177, 0, 0,  90,  90 );
  strip(178, 187, 0, 0,  90,  90 );
  strip(208, 217, 0, 0,  90,  90 );
  strip( 80,  89, 0, 0,  90,  90 );
  strip( 90,  99, 0, 0,  90,  90 );
  strip(100, 109, 0, 0,  90,  90 );
  strip(110, 119, 0, 0,  90,  90 );
  strip(120, 129, 0, 0,  90,  90 );
  strip( 50,  50, 0, 0,  90,  90 );
  strip( 54,  54, 0, 0,  90,  90 );
  strip( 58,  58, 0, 0,  90,  90 );
  pixels.show();   // Sends the updated pixel colors to the hardware.
}

void loop() {
  if (timeStatus() == timeNotSet) {
    Serial.println("Waiting for GPS time ... ");
    setGPS();
  } else { 
    digitalClockDisplay();
    // showpixels(); // debug
    pixels.clear(); // Set all pixel colors to 'off'
    drawdigit(1, 4, 0, 240, 255);  // debug
    drawdigit(2, 3, 0, 240, 255);  // debug 
    drawdigit(3, 2, 0, 240, 255);  // debug
    drawdigit(4, 1, 0, 240, 255);  // debug
    pixels.show();   // Sends the updated pixel colors to the hardware.
  } 
}

void strip(int start, int end, int offset, int red, int green, int blue) {
  // Sub-routine to draw a given sequence of LEDs in a given RGB color
  for (int i = (start + offset); i <= (end + offset); i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

void drawcolon(int red, int green, int blue) {
  // Sub-routine to draw a colon in the center of the clock (eight LEDs) in a given RGB color
  for (int i = 140; i <= 147; i = i + 1) { 
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

void setGPS() {
  // Reads input from GPS and waits until there is enough data from satellites to set the clock correctly.
  // Unless correct time is obtained, "timeStatus()" is set to "timeNotSet".
  // Getting correct data from GPS might take up to 60-90 seconds ("cold start"), so this needs to be invoked in a main loop.
  while (SerialGPS.available()) {
    if (gps.encode(SerialGPS.read())) { // process gps messages
      unsigned long age;
      int Year;
      byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
      }
    }
  }
}

void showpixels() {
  // Simple test - sequentailly lights up all LEDs
  pixels.clear(); // Set all pixel colors to 'off'
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 255, 255) );
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 0, 0) );
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 255, 0) );
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 0, 255) );
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
}

void digitalClockDisplay(){
  // Debug routine to display GPS time in the serial port output
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print("/");
  Serial.print(month());
  Serial.print("/");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits) {
  // utility sub-routine for digital clock display: prints preceding colon and pads with zeroes if necessary
  Serial.print(":");
  if (digits < 10) {
    Serial.print('0');
  }
  Serial.print(digits);
}

