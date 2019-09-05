#include <DS1307RTC.h>        //https://github.com/PaulStoffregen/DS1307RTC
#include <Time.h>             //https://github.com/PaulStoffregen/Time
#include <Timezone.h>         //https://github.com/JChristensen/Timezone
#include <TinyGPS++.h>        //https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <Wire.h>
#include <TimeLib.h>

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
TimeChangeRule msk = {"MSK", Last, Sun, Mar, 1, 180}; // 3 * 60 = 180 minutes between UTC and Moscow
Timezone tzMSK(msk);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;

//variables for time counting
unsigned long previousMillis = 0;
unsigned long currentMillis = 0;
unsigned long syncTimer = 0;
unsigned long scheduledTimer = 0;

bool newboot = true;
int prevSecond = 0; // for debugging

////////////////////////////////////
// SETUP
////////////////////////////////////

void setup() {
  Serial.begin(115200);
  GPS_Serial.begin(GPSBaud);
}

////////////////////////////////////
// MAIN LOOP
////////////////////////////////////

void loop() { 
  currentMillis = millis();
  if(!rtcSet){
    syncOnBoot();
  } else {
    setSyncProvider(RTC.get); // forces Arduino to get the time from the RTC
    utc = now(); // Grab the current time
    local = tzMSK.toLocal(utc, &tcr);
    DisplayTime(); // Shows clock
  }
}

////////////////////////////////////
// FUNCTIONS
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

        Serial.print(F("Time from GPS is: "));
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

void DisplayTime() {
  if(!rtcSet)  {
    Serial.println(F("Waiting for GPS Fix"));
  }else{
    if (second(local) != prevSecond){
      padZero(  hour(local));
      Serial.print(":");
      padZero(minute(local)); 
      Serial.print(":");
      padZero(second(local)); 
      Serial.println();
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
