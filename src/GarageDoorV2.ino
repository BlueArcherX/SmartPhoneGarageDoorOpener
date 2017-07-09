// ************************************************************
// This is the latest and likely final rev of my Smartphone Garage Door Opener sketch
// using the Adafruit Feather Huzzah board and the Blynk framework. It's pretty robust 
// and yells at you when you leave the garage door open 
// ************************************************************
// Tyler Winegarner, 2017
// Forked and modified by Brad Calvert, 2017

// #include <Arduino.h>
// #include <Blynk.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Comment this out to disable prints and save space
// #define BLYNK_PRINT Serial

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "***REMOVED***";
// char auth2[] = "***REMOVED***";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "***REMOVED***";
char pass[] = "***REMOVED***";

// Select your pin with physical button
const int doorPin = 2;
// warnPin is the virtual button in the phone app to enable or disable the notifications
int warnPin;
// warnThreshold is the number ot ticks that must pass after opening the door
// before the phone app will notify
// 400 = 1 minute
// 2000 = 5 minutes
// 4000 = 10 minutes
// 12000 = 30 minutes
int warnThreshold = 2000;
// tick increments every cycle of the loop
int tick = 0;
WidgetLCD lcd(V3);

// not sure what this does
BlynkTimer timer;

BLYNK_WRITE(V1) {
  // I don't really understand this, but I think it is setting warnPin to be virtual pin 1 (V1) from the blynk app
  warnPin = param.asInt();
}

// V3 LED Widget represents the physical button state
boolean btnState = false;
void buttonLedWidget()
{
  // Read door pin state
  // check if pin doorPin (2) is high
  if (digitalRead(doorPin) == HIGH) {
    // if door pin is high, print open status to phone app
    lcd.print(0,0, "Door Open  ");
    // increment tick, this will increment every loop while the door is open
    // once the tick > warnThreshold, notification is sent to phone
    tick++;
  }  else {
    // if door pin is not high, print closed status to phone app
    lcd.print(0,0, "Door Closed");
    // reset tick to 0 since door was closed
    tick = 0;
    Blynk.virtualWrite(warnPin, LOW);
  }
  if (tick > warnThreshold) {
    if (warnPin == LOW) {
      Blynk.notify("Garage door is open!");
      // if I understand this correctly, then the number subtracted from tick
      // below is how long to wait before throwing another active alert on the phone?
      // should be at least 10 minutes
      tick = (tick - 2000);
    }
  }
  Blynk.virtualWrite(V2, tick);
}

void uptimeWidget()
{
  // writes the elapsed time to Blynk pin V4
  // / 60000 to convert milliseconds to minutes
  // / 1000 to convert milliseconds to seconds
  Blynk.virtualWrite(V4, millis() / 60000);
}

void setup(void) 
{
  // Debug console
  // Serial.begin(9600);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  // Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  // Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

  // Setup physical button pin (active low)
  pinMode(doorPin, INPUT);
  // this timer only allows the function that checks the door state to run every 10 ms by default
  // I will experiement with this value and check for power savings
  timer.setInterval(500, buttonLedWidget);
  // timer to send the uptime to pin V4 every 5 seconds
  timer.setInterval(5000, uptimeWidget);
}

void loop(void)
{
  Blynk.run();
  timer.run();
}
