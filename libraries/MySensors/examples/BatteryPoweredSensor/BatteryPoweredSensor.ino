/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * DESCRIPTION
 *
 * This is an example that demonstrates how to report the battery level for a sensor
 * Instructions for measuring battery capacity on A0 are available here:
 * http://www.mysensors.org/build/battery
 * 
 */


#include <SPI.h>
#include <MySensor.h>

int BATTERY_SENSE_PIN = A0;  // select the input pin for the battery sense point

MySensor gw;
unsigned long SLEEP_TIME = 900000;  // sleep time between reads (seconds * 1000 milliseconds)
int oldBatteryPcnt = 0;

void setup()  
{
   // use the 1.1 V internal reference
#if defined(__AVR_ATmega2560__)
   analogReference(INTERNAL1V1);
#else
   analogReference(INTERNAL);
#endif
   gw.begin();

   // Send the sketch version information to the gateway and Controller
   gw.sendSketchInfo("Battery Meter", "1.0");
}

void loop()
{
   // get the battery Voltage
   int sensorValue = analogRead(BATTERY_SENSE_PIN);
   #ifdef DEBUG
   Serial.println(sensorValue);
   #endif
   
   // 1M, 470K divider across battery and using internal ADC ref of 1.1V
   // Sense point is bypassed with 0.1 uF cap to reduce noise at that point
   // ((1e6+470e3)/470e3)*1.1 = Vmax = 3.44 Volts
   // 3.44/1023 = Volts per bit = 0.003363075
   float batteryV  = sensorValue * 0.003363075;
   int batteryPcnt = sensorValue / 10;
   
   //Some code I needed to get this to work with my battery set up
   //I'll leave it commented for others to approve
   // If voltage is 3.4 the batteryPcnt variable gives a result of 102% and Vera UI7 does render any battery symbol
   //To fix it I added the bit below
   // if (batteryPcnt > 100) {batteryPcnt = 100} else {batteryPcnt = batteryPcnt};

   #ifdef DEBUG
   Serial.print("Battery Voltage: ");
   Serial.print(batteryV);
   Serial.println(" V");

   Serial.print("Battery percent: ");
   Serial.print(batteryPcnt);
   Serial.println(" %");
   #endif

   if (oldBatteryPcnt != batteryPcnt) {
     // Power up radio after sleep
     gw.sendBatteryLevel(batteryPcnt);
     oldBatteryPcnt = batteryPcnt;
   }
   gw.sleep(SLEEP_TIME);
}
