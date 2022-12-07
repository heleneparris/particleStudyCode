/*CODE FOR SENSOR #1

  - Baud rate needs to be 9600 in the serial monitor to see the values
  - Serial monitor is printing out the values in CSV format as "Sensor #, PM 0.1, PM 2.5, PM 1.0"
  - One blue blink means the wifi is succesfully connected, two blue blinks means a sensor reading is being taken
*/

// WiFi Libraries
#include <WiFi.h>                               // WiFi Library
#include <HTTPClient.h>                         // Server Library
const char* ssid = "EZConnect";                 // Connect to Berry EZConnect WiFi
const char* password =  "mDx773HAt3";           // Insert your EZConnect Password Here

int size1;                                      //vars to store PM 0.1
int size2;                                      //vars to store PM 2.5
int size3;                                      //vars to store PM 1.0
int sensor = 1;                                 //vars to store the senosr #

#define ONBOARD_LED  2

// Sensor Libraries
#include <Wire.h>

// Sleep Settings
#define uS_TO_S_FACTOR 1000000  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  600      // Time ESP32 will go to sleep (in seconds)

//PARTICLE SENSOR STUFF
#ifndef ESP32
#include <SoftwareSerial.h>
#endif
#include <PMserial.h> // Arduino library for PM sensors with serial interface

#if !defined(PMS_RX) && !defined(PMS_TX)
constexpr auto PMS_RX = 16;
constexpr auto PMS_TX = 17;
#endif

#ifndef ESP32
SerialPM pms(PMS5003, PMS_RX, PMS_TX); // PMSx003, RX, TX

#else
SerialPM pms(PMS5003, PMS_RX, PMS_TX); // PMSx003, RX, TX
#endif

void setup()
{
  //sensor setup stuff
  Serial.begin(9600);
  Serial.println(F("Booted"));
  Serial.println(F("PMS sensor on SWSerial"));
  Serial.print(F("  RX:"));
  Serial.println(PMS_RX);
  Serial.print(F("  TX:"));
  Serial.println(PMS_TX);
  pms.init();
  pinMode(ONBOARD_LED, OUTPUT);
  sampleReading();
  delay(1000);
}

void loop()
{
  sampleReading();
}

//wifi reading function
void sampleReading() {

  // read the PM sensor
  pms.read();
  if (pms)
  { // successfull read
    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);
    digitalWrite(ONBOARD_LED, LOW);
    delay(100);
    digitalWrite(ONBOARD_LED, HIGH);
    delay(100);
    digitalWrite(ONBOARD_LED, LOW);
    size1 = pms.pm01;     // Get first value from the sensor
    size2 = pms.pm25;    // Get second value from the sensor
    size3 = pms.pm10;    // Get third value from the sensor
#ifdef ESP8266
    // print formatted results
    Serial.printf(sensor, size1, size2, size3);
#else
    // print the results
    Serial.print(size1);
    Serial.print(F(", "));
    Serial.print(size2);
    Serial.print(F(", "));
    Serial.print(size3);
    Serial.println();
#endif
  }
  else
  { // something went wrong
    switch (pms.status)
    {
      case pms.OK: // should never come here
        break;     // included to compile without warnings
      case pms.ERROR_TIMEOUT:
        Serial.println(F(PMS_ERROR_TIMEOUT));
        break;
      case pms.ERROR_MSG_UNKNOWN:
        Serial.println(F(PMS_ERROR_MSG_UNKNOWN));
        break;
      case pms.ERROR_MSG_HEADER:
        Serial.println(F(PMS_ERROR_MSG_HEADER));
        break;
      case pms.ERROR_MSG_BODY:
        Serial.println(F(PMS_ERROR_MSG_BODY));
        break;
      case pms.ERROR_MSG_START:
        Serial.println(F(PMS_ERROR_MSG_START));
        break;
      case pms.ERROR_MSG_LENGTH:
        Serial.println(F(PMS_ERROR_MSG_LENGTH));
        break;
      case pms.ERROR_MSG_CKSUM:
        Serial.println(F(PMS_ERROR_MSG_CKSUM));
        break;
      case pms.ERROR_PMS_TYPE:
        Serial.println(F(PMS_ERROR_PMS_TYPE));
        break;
    }
  }



  delay(500);
  openWiFi();                 // Open WiFi Connection
  HTTPClient http;
  String url = "http://10.40.6.2/399/groups/callisto/capture.php?sensor=" + String(sensor) + "&size1=" + String(size1) + "&size2=" + String(size2) + "&size3=" + String(size3); // Send a string to your server
  http.begin(url);
  int httpCode = http.GET();
  closeWiFi();
  delay(500);              // Small delay to allow the server to finish

  // wait for 20 minutes ---- can be changed to however long we want to wait between readings
  //change to a for loop that runs 40 times and delays for 30,000 ms
  for (int i = 0 ; i <= 40; i++) {
    delay(30000);
  }
}
