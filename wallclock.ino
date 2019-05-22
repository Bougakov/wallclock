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
  LED strip is connected as following:

  1st digit -   0 to  69
  2nd digit -  70 to 139
  upper dot - 140 to 143
  lower dot - 140 to 143
  3rd digit - 148 to 217
  4th digit - 218 to 287

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
  SerialGPS.begin(4800); // BR-355 uses 4800 baud speed. Neo6Mv2 uses 9600 baud.
  Serial.println("Initializing LED strip ... ");
  pixels.begin(); // Initializes NeoPixel strip object
  pixels.clear(); // Resets LED strip
  draw_GPS();
  Serial.println("Waiting for GPS time ... ");
}

void draw_GPS() {
  // Draws "GPS..." on the LED strip while we get time from satellites
  /*
    G: 228-237, 238-247, 278-287, 268-277, 258-267
    P: 148-157, 158-167, 168-177, 178-187, 208-217
    S: 80-89, 90-99, 100-109, 110-119, 120-129
    hellip: 50-51, 54-55, 58-59
  */

  for (int i = 228; i <= 237; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 238; i <= 247; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 278; i <= 287; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 267; i <= 277; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 258; i <= 267; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 148; i <= 157; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 158; i <= 167; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 168; i <= 177; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 178; i <= 187; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 208; i <= 217; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i =  80; i <=  89; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i =  90; i <=  99; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 100; i <= 109; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 110; i <= 119; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i = 120; i <= 129; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i =  50; i <=  51; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i =  54; i <=  55; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  for (int i =  58; i <=  59; i = i + 1)    { pixels.setPixelColor(i, pixels.Color(  0,  90,  90 )); }
  pixels.show();   // Send the updated pixel colors to the hardware.
}

void loop() {
  
  if (timeStatus() == timeNotSet) {
    setGPS();
  } else { 
    digitalClockDisplay();  
  } 
}

void setGPS() {
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
  pixels.clear(); // Set all pixel colors to 'off'
  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(255, 255, 255 ));
    pixels.show();   // Send the updated pixel colors to the hardware.
  }
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits) {
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

