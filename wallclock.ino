// #define DEBUG 1

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

// default colors:
byte default_red   =   0;
byte default_green = 90;
byte default_blue  = 255;
// HUE variation range (in HSV model, as in https://www.ginifab.com/feeds/pms/rgb_to_hsv_hsl.html x)
byte hue_from = 209;
byte hue_to = 229;

/* =============================================================== */

static void draw_GPS(int red, int green, int blue) {
  // Draws "GPS..." on the LED strip while we get correct time from satellites
  /*
    G: 228-237, 238-247, 278-287, 268-277, 258-267
    P: 148-157, 158-167, 168-177, 178-187, 208-217
    S: 80-89, 90-99, 100-109, 110-119, 120-129
    hellip: 50-51, 54-55, 58-59
  */
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
}

static void drawtime(int red, int green, int blue) {
  int h =   hour();
  int m = minute();
  int s = second();

  // placeholders for upper and lower digits of time - i.e. "11:23" - "3" is lower minute digit, "2" is upper
  int h_lo_digit = h % 10;
  int h_hi_digit = (h - h_lo_digit) / 10;
  int m_lo_digit = m % 10;
  int m_hi_digit = (m - m_lo_digit) / 10;
  
  drawdigit(h_hi_digit, 4, red, green, blue);  // leftmost position (4th from right)
  drawdigit(h_lo_digit, 3, red, green, blue);  
  drawdigit(m_hi_digit, 2, red, green, blue);  
  drawdigit(m_lo_digit, 1, red, green, blue);  // rightmost position

  if ((millis() % 600) >= 300) {
    // blinks with colon every even second
    draw_upper_dot(red, green, blue); 
    draw_lower_dot(red, green, blue); 
  }
}

static void strip(int start, int end, int offset, int red, int green, int blue) {
  // Sub-routine to draw a given sequence of LEDs in a given RGB color
  for (int i = (start + offset); i <= (end + offset); i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

// Sub-routines to draw a colon in the center of the clock (eight LEDs) in a given RGB color
static void draw_upper_dot(int red, int green, int blue) {
  for (int i = 140; i <= 143; i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}

static void draw_lower_dot(int red, int green, int blue) {
  for (int i = 144; i <= 147; i = i + 1) { 
    pixels.setPixelColor(i, pixels.Color(red, green, blue)); 
  }
}


static void drawdigit(int digit, int location, int red, int green, int blue) {
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

static void setGPS() {
  // Reads input from GPS and waits until there is enough data from satellites to set the clock correctly.
  // Unless correct time is obtained, "timeStatus()" is set to "timeNotSet".
  // Getting correct data from GPS might take up to 60-90 seconds ("cold start"), so this needs to be invoked in a main loop.
  while (SerialGPS.available()) {
    if (
      gps.encode(SerialGPS.read())
    ) { 
      // process gps messages
      unsigned long age, date, time, chars = 0;
      #ifdef DEBUG
        Serial.print("Satellites: ");
        print_int(gps.satellites(), TinyGPS::GPS_INVALID_SATELLITES, 5);
        unsigned short sentences = 0, failed = 0;
        gps.stats(&chars, &sentences, &failed);
        Serial.print("; Chars processed: ");
        print_int(chars, 0xFFFFFFFF, 6);
        Serial.print("; Sentences processed: ");
        print_int(sentences, 0xFFFFFFFF, 10);
        Serial.print("; Fails: ");
        print_int(failed, 0xFFFFFFFF, 9);
        Serial.println();
      #endif
      int Year;
      byte Month, Day, Hour, Minute, Second;
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime(offset * SECS_PER_HOUR);
        setSyncInterval(15); // corrects internal clock every 15 seconds 
      }
    }
  }
}


static void debug_time_toserial(){
  // Debug routine to display GPS time in the serial port output
  #ifdef DEBUG 
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
  #endif
}

static void printDigits(int digits) {
  // utility sub-routine for digital clock display: prints preceding colon and pads with zeroes if necessary
  #ifdef DEBUG
    Serial.print(":");
    if (digits < 10) {
      Serial.print('0');
    }
    Serial.print(digits);
  #endif
}

void setup() {
  #ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) ; // Needed for Leonardo only
  #endif
  SerialGPS.begin(4800); // "BR-355" GPS module uses 4800 baud speed. Cheaper "Neo6Mv2" uses 9600 baud. Check the datasheet for your model.
  pixels.begin(); // Initializes NeoPixel strip object
  pixels.clear(); // Resets LED strip (turns all LEDs off)
  draw_GPS(default_red, default_green, default_blue);
  pixels.show(); // Sends the updated pixel colors to the hardware.
  #ifdef DEBUG
    Serial.println("Waiting for GPS time ... ");
  #endif
}

void loop() {
  if (timeStatus() == timeNotSet) {
    setGPS();
  } else {
      setGPS();
      if (timeStatus() == timeNeedsSync) {
        #ifdef DEBUG
          Serial.println("Re-syncing clock with GPS time ... ");
        #endif
        draw_GPS(default_red, default_green, default_blue);
        setGPS();
      } else {
        #ifdef DEBUG
          debug_time_toserial();
        #endif
        pixels.clear(); // Set all pixel colors to 'off'
        if ( second() < 30) {
          // first half of minute we iterate hue forward,
          setColorHSV(map(second(),   0, 29, hue_from, hue_to), 255, 255);
          //default_green = map(second(),  0, 29, 0, 255);
        } else {
          // second half of minute we iterate backwards
          setColorHSV(map(second(),  30, 59, hue_to, hue_from), 255, 255);
          //default_green = map(second(), 30, 59, 255, 0); 
        }      
        drawtime(default_red, default_green, default_blue); // Displays current time
        pixels.show();   // Sends the updated pixel colors to the hardware.
      }
  } 
}

void setColorHSV(byte h, byte s, byte v) {
  // An HSV to RGB converter using byte and integer arithmetic. 
  // Called with HSV arguments of from 0 to 255. 
  // To cycle through a color palette, call it with hues from 0 through 255 and saturation and value of 255.
  // Source: https://github.com/judge2005/arduinoHSV

  // this is the algorithm to convert from RGB to HSV
  h = (h * 192) / 256;  // 0..191
  unsigned int i = h / 32;   // We want a value of 0 thru 5
  unsigned int f = (h % 32) * 8;   // 'fractional' part of 'i' 0..248 in jumps

  unsigned int sInv = 255 - s;  // 0 -> 0xff, 0xff -> 0
  unsigned int fInv = 255 - f;  // 0 -> 0xff, 0xff -> 0
  byte pv = v * sInv / 256;  // pv will be in range 0 - 255
  byte qv = v * (256 - s * f / 256) / 256;
  byte tv = v * (256 - s * fInv / 256) / 256;

  switch (i) {
  case 0:
    default_red = v;
    default_green = tv;
    default_blue = pv;
    break;
  case 1:
    default_red = qv;
    default_green = v;
    default_blue = pv;
    break;
  case 2:
    default_red = pv;
    default_green = v;
    default_blue = tv;
    break;
  case 3:
    default_red = pv;
    default_green = qv;
    default_blue = v;
    break;
  case 4:
    default_red = tv;
    default_green = pv;
    default_blue = v;
    break;
  case 5:
    default_red = v;
    default_green = pv;
    default_blue = qv;
    break;
  }
}

static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

static void print_float(float val, float invalid, int len, int prec) {
  #ifdef DEBUG
  if (val == invalid)
  {
    while (len-- > 1)
    Serial.print('*');
    Serial.print(' ');
  }
  else
  {
    Serial.print(val, prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      Serial.print(' ');
  }
  #endif
  smartdelay(0);
}

static void print_int(unsigned long val, unsigned long invalid, int len) {
  #ifdef DEBUG
  char sz[32];
  if (val == invalid)
    strcpy(sz, "*******");
  else
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i=strlen(sz); i<len; ++i)
    sz[i] = ' ';
  if (len > 0) 
    sz[len-1] = ' ';
  Serial.print(sz);
  #endif
  smartdelay(0);
}

static void print_date(TinyGPS &gps) {
  #ifdef DEBUG
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned long age;
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  if (age == TinyGPS::GPS_INVALID_AGE)
    Serial.print("********** ******** ");
  else
  {
    char sz[32];
    sprintf(sz, "%02d/%02d/%02d %02d:%02d:%02d ",
        month, day, year, hour, minute, second);
    Serial.print(sz);
  }
  print_int(age, TinyGPS::GPS_INVALID_AGE, 5);
  #endif
  smartdelay(0);
}

static void print_str(const char *str, int len) {
  #ifdef DEBUG
  int slen = strlen(str);
  for (int i=0; i<len; ++i)
  Serial.print(i<slen ? str[i] : ' ');
  #endif
  smartdelay(0);
}
