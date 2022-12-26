/* 
  Alarm-Every-Minute
  This program demonstrates generating an interrupt at one-minute intervals
  using a DS3231 Real Time Clock module attached to an Arduino.
  Copyright (C) 2022  David G Sparks

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

  Contact the author by opening an issue in the Github repository
  https://github.com/IowaDave/DGS_3231
*/

// This example program requires connecting the DS3231 to the Arduino
// for I2C communications. In addition, a connection is required
// between digital pin 2 of the Arduino and the SQW/INT pin of the DS3231.


#include "DGS_3231.h"

// initialize locally stored register bytes 
// current with time values, why not?
DS3231_TIME myClock (12, 25, 22, 20, 48, 0, MODE24HR);

// function prototypes
void showTime ();         // to output time via Serial
void handleAlarm (void);  // user-ISR for this project
void manageAlarm1 ();     // performs all the alarm operations

// global variables
volatile bool alarmReceived = false;

void setup() {

  Wire.begin();       // I2C

  myClock.uploadTime ();      // send the local store to the device
  
  Serial.begin(9600);
  while (!Serial);    // wait for serial port to open

  Serial.print    ("myClock object memory size: ");
  Serial.println  (sizeof(myClock));
  Serial.print    ("Begin at time: ");
  showTime();

  // encode Alarm 2 in local store to signal every minute
  myClock.setA2matchbits (DS3231_A2EVERYMIN);
  // upload the local store to the device
  myClock.uploadAlarm2 ();
  
  // set up the interrupt, here using digital pin 2
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), soundTheAlarm, FALLING);

  // start the interrupts
    manageAlarm2 ();              
  
}

void loop() {

  // check for and respond to alarm events
  if (alarmReceived) {
    manageAlarm2 ();              // turn off and reset the alarm
    alarmReceived = false;        // turn off the interupt semaphore
    myClock.downloadTime ();              // get current time from the device...
    Serial.print("Alarm at time: ");  // ...and share it with the world.
    showTime ();
  }

}

// interrupt service routine
void soundTheAlarm (void) {
  // turn on a semaphore when interrupt occurs
  alarmReceived = true;
}

// output time values via Serial
void showTime () {
  Serial.print(myClock.getMonth());
  Serial.print("/");
  Serial.print(myClock.getDate());
  Serial.print("/");
  Serial.print(myClock.getYear());
  Serial.print(" ");
  Serial.print(myClock.getHour());
  Serial.print(":");
  Serial.print(myClock.getMinutes());
  Serial.print(":");
  Serial.print(myClock.getSeconds());
  Serial.print(" ");
  if(myClock.getIsAM()) {
    Serial.println("a.m.");
  } else {
    Serial.println("p.m.");
  }  
}

// do all things relating to the alarm
void manageAlarm2 () {
  // download the control and status registers to local storage
  myClock.downloadCS ();
  // turn off all the alarm-specific bits in the local store
  myClock.disableAlarm1interrupt ();
  myClock.disableAlarm2interrupt ();
  myClock.resetAlarmFlags ();
  // upload the revised registers to the device to turn off the alarms
  myClock.uploadCS ();
  // set the relevant alarm-related bits in the local store
  myClock.enableAlarm2interrupt ();
  // upload the control and status registers to re-enable the alarm
  myClock.uploadCS ();
}
