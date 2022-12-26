/* 
  Alarm-Match-Seconds
  This program demonstrates generating an interrupt at 5-second intervals
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

DS3231_TIME myClock (12, 25, 22, 18, 43, 0, MODE24HR);

// function prototypes
void showTime ();         // to output time via Serial
void handleAlarm (void);  // user-ISR for this project
void manageAlarm1 ();     // performs all the alarm operations

// global variables
volatile bool alarmReceived = false;

void setup() {

  Wire.begin();       // I2C
  
  Serial.begin(9600);
  while (!Serial);    // wait for serial port to open

  Serial.print    ("myClock object memory size: ");
  Serial.println  (sizeof(myClock));
  Serial.print    ("Initial time: ");
  showTime();

  // store the initial "time" seconds as the Alarm 1 seconds
  myClock.setA1second (myClock.getSeconds ());
  // set Alarm 1 to signal when alarm and time seconds match
  myClock.setA1matchbits (DS3231_A1MATCHSEC);
  // call a subroutine to advance the alarm 1 seconds value
  // and activate Alarm 1
  manageAlarm1 ();

  pinMode(LED_BUILTIN, OUTPUT);   // use the onboard led to signal alarm events
  digitalWrite(LED_BUILTIN, LOW); // set its initial state

  // set up the interrupt, here using digital pin 2
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), soundTheAlarm, FALLING);
}

void loop() {

  // at 1-second intervals...
  delay(1000);
  // bring the time registers into local storage
  myClock.downloadTime ();
  // and display the contents
  showTime();

  // check for and respond to alarm events
  if (alarmReceived) {
    manageAlarm1 ();              // turn off, advance and reset the alarm
    alarmReceived = false;        // turn off the interupt semaphore
    digitalWrite (LED_BUILTIN, 
    ! digitalRead (LED_BUILTIN)); // toggle the LED
    Serial.println("Alarm!");     // announce it, too
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
void manageAlarm1 () {
  // download the control and status registers to local storage
  myClock.downloadCS ();
  // turn off all the alarm-related bits in the local store
  myClock.disableAlarm1interrupt ();
  myClock.disableAlarm2interrupt ();
  myClock.resetAlarmFlags ();
  // upload the revised registers to the device to turn off the alarms
  myClock.uploadCS ();
  // add 5 seconds to the alarm time, handle overflow beyond 60
  myClock.setA1second (( myClock.getA1second() + 5) % 60);
  // upload the updated alarm time registers
  myClock.uploadAlarm1 ();
  // set the relevant alarm-related bits in the local store
  myClock.enableAlarm1interrupt ();
  // upload the control and status registers to re-enable the alarm
  myClock.uploadCS ();
}
