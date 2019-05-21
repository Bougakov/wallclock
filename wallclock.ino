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
#define PIN        2 // LED strip is wired to pin #2

#define NUMPIXELS 288 // How many NeoPixels are attached to the Arduino?
// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/* =============================================================== */

time_t prevDisplay = 0; // when the digital clock was displayed

void setup() {
  
  Serial.begin(115200);
  // while (!Serial) ; // Needed for Leonardo only
  SerialGPS.begin(4800); // BR-355 uses 4800 baud speed. Neo6Mv2 uses 9600 baud.

  Serial.println("Initializing LED strip ... ");
  pixels.begin(); // Initializes NeoPixel strip object
  pixels.clear(); // Resets LED strip
  showpixels();


  Serial.print("TinyGPS library v. "); 
  Serial.println(TinyGPS::library_version());
  Serial.println("Waiting for GPS time ... ");
  
}

void loop() {
  setGPS();
  if (timeStatus()!= timeNotSet) {
    if (now() != prevDisplay) { //update the display only if the time has changed
      prevDisplay = now();
      digitalClockDisplay();  
      showpixels();
    } else {

    }
  }
}

void setGPS() {
  while (SerialGPS.available()) {
    if (gps.encode(SerialGPS.read())) { // process gps messages
      // when TinyGPS reports new data...
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

