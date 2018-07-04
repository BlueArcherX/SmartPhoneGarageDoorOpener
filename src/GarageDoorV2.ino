// ************************************************************
// ESP8266-based smartphone garage door opener using the
// Adafruit Feather Huzzah board and the Blynk framework.
// Will fire alerts on your smart phone Blynk App when the
// garage door is left open for a defined ammount of time.
// ************************************************************
// Tyler Winegarner, 2017 (Original author)
// Forked and modified by Brad Calvert, 2017

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <sensitiveData.h>

// sensitiveData.h needs to exist in your include path and contain the following variables:
// your Blynk auth token
//char auth[] = "";
// your WiFi SSID
//char ssid[] = "";
// your WiFi password
//char pass[] = "";

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

// The physical pin that receives the input from the door switch.
const int doorPin = 2;
// The virtual button in the Blynk app to enable or disable the notifications.
int warnPin;

// warnThreshold is the number of ticks that must pass after opening the door before the phone app will notify
// This is highly dependent on the buttonLedWidget timer value near the bottom of this file
// for buttonLedWidget =  500   | 2000  length
//        warnThreshold = 400   | 100   1 minute
//        warnThreshold = 2000  | 500   5 minutes
//        warnThreshold = 4000  | 1000  10 minutes
//        warnThreshold = 12000 | 3000  30 minutes
int warnThreshold = 1000;
// tick increments every cycle of the loop
int tick = 0;
// Output to the Blynk app virtual LCD screen on virtual pin 3 (V3)
WidgetLCD lcd(V3);

// Create a BlynkTimer object called timer
BlynkTimer timer;

// This checks virtual pin1 (V1) for data written from the Blynk App
// V1 is a boolean set to turn on and off notifications on the phone with a toggle switch
BLYNK_WRITE(V1) {
  // Sets warnPin to the value of the toggle switch in Blynk app
  // 0 = phone notifications off, 1 = phone notifications on
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
    lcd.print(0,0, "Door Open");
    Serial.println("Door Open");
    // increment tick, this will increment every loop while the door is open
    // once the tick > warnThreshold, notification is sent to phone
    tick++;
  }  else {
    // if door pin is not high, print closed status to phone app
    lcd.print(0,0, "Door Closed");
    Serial.println("Door Closed");
    // reset tick to 0 since door was closed
    tick = 0;
    Blynk.virtualWrite(warnPin, LOW);
  }
  Serial.println("BEGIN Notify Loop");
  Serial.println("Tick:");
  Serial.println(tick);
  Serial.println("Warn Pin State:");
  Serial.println(warnPin);
    if (tick > warnThreshold) {
    if (warnPin == HIGH) {
      Blynk.notify("Garage door is open!");
      Serial.println("SENT Notification");
      // Reset tick to zero.This should cause a new alarm to fire at a rate equal to warnThreshold (4000 = 10 minutes)
      tick = 0;
    }
  }
  Serial.println("END Notify Loop");
  Serial.println();

  // N1ot exaclty sure what this does... Writes the value of tick to virtual pin 2?
  // But nothing in the app is dealing with that pin, so I am just going to comment that out.
  // I suppose it would be possible to display how long the door has been open on the LCD.
  //Blynk.virtualWrite(V2, tick);
}

void uptimeWidget()
{
  // Writes the elapsed device uptime to Blynk pin V4
  // 60000 to convert milliseconds to minutes
  // 1000 to convert milliseconds to seconds
  Blynk.virtualWrite(V4, millis() / 60000);
}

void setup(void)
{
  // Debug console
  Serial.begin(9600); //serial for USB serial
  Serial.println("Serial enabled");

  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
  Blynk.begin(auth, ssid, pass);

  // Setup physical button pin (active low)
  pinMode(doorPin, INPUT);
  // this timer only allows the function that checks the door state to run every 500 ms by default
  // Experiement with this value and check for power savings
  timer.setInterval(2000, buttonLedWidget);
  // timer to send the uptime to pin V4 every 60 seconds
  timer.setInterval(60000, uptimeWidget);
}

// Main loop to execute all the functions defined above
void loop(void)
{
  Blynk.run();
  timer.run();
}