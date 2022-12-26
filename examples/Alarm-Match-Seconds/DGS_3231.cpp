

#include "DGS_3231.h"

#define A1_Pos (0x07)
#define A1_Length (4)
#define A2_Pos (0x0b)
#define A2_Length (3)
#define CTRL_Pos (0x0e)
#define STATUS_Pos (0x0f)
#define CS_Length (2)
#define AGING (0x10)
#define TEMP_Pos (0x11)
#define TEMP_Length (2)


DS3231_TIME::DS3231_TIME() {
  for (int i = 0; i < 7; i++) buff[i] = 0;
}

DS3231_TIME::DS3231_TIME( 
    uint8_t month,
    uint8_t date,
    uint8_t year,
    uint8_t hour,
    uint8_t minute,
    uint8_t second,
    bool hr12,
    bool isAM 
  ) {
    setMode12 (hr12);
    setMonth (month);
    setDoW (0);
    setDate (date);
    setYear (year);
    setHour (hour, isAM);
    setMinutes(minute);
    setSeconds(second);
}

// time-related member functions

void DS3231_TIME::setSeconds (uint8_t seconds) {
  part.seconds = seconds % 10;
  part.sec10 = seconds / 10;
}

uint8_t DS3231_TIME::getSeconds () {
  return ((part.sec10 * 10) + part.seconds);
}

void DS3231_TIME::setMinutes (uint8_t minutes) {
  part.minutes = minutes % 10;
  part.min10 = minutes / 10;
}

uint8_t DS3231_TIME::getMinutes () {
  return ((part.min10 * 10) + part.minutes);
}

void DS3231_TIME::setDoW (uint8_t dow) {
  part.dow = dow & 7;
}

uint8_t DS3231_TIME::getDoW () {
  return part.dow;
}

void DS3231_TIME::setDate (uint8_t date) {
  part.date = date % 10;
  part.dat10 = date / 10;
}

uint8_t DS3231_TIME::getDate () {
  return ((part.dat10 * 10) + part.date); 
}

void DS3231_TIME::setMonth (uint8_t month) {
  part.month = month % 10;
  part.mon10 = month / 10;
}

uint8_t DS3231_TIME::getMonth () {
  return ((part.mon10 * 10) + part.month);
}

void DS3231_TIME::setYear (uint8_t year) {
  part.year = year % 10;
  part.yr10 = year / 10;
}

uint8_t DS3231_TIME::getYear () {
  return ((part.yr10 * 10) + part.year);
}

void DS3231_TIME::setHour (uint8_t hour, bool isAM) {
  // separate action into two paths
  if (part.hr12) { // path #1 = 12-hour time mode
    // expects 1 <= hour <= 12, works for both a.m.and p.m.
    part.hours = hour % 10;
    part.hour10 = hour / 10;
    part.hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour time mode
    // expects 0 <= hour < 24
    part.hours = hour % 10;
    if (hour < 20) { 
      part.hour10 = hour / 10;
      part.hr20pa = 0;
    } else {
      part.hour10 = 0;
      part.hr20pa = 1;
    }
  }
}

uint8_t DS3231_TIME::getHour () {
  // separate action into two paths
  if (part.hr12) { // path #1 = 12-hour mode
    return ((part.hour10 * 10) + part.hours);
  } else { // path #2 = 24-hour mode
    return (
        (part.hr20pa * 20) 
      + (part.hour10 * 10) 
      + part.hours
    );
  }
}

bool DS3231_TIME::getIsAM () {
  //separate action into two parts
  if (part.hr12) { // path #1 = 12-hour mode
    return (bool) part.hr20pa;
  } else { // path #2 = 24-hour mode
    if (getHour() < 12) return true; 
    return false;
  }
}
  
void DS3231_TIME::setMode12 (bool mode) {
  part.hr12 = (uint8_t) mode;
}

bool DS3231_TIME::getMode12 () {
  return (bool) part.hr12;
}

uint8_t DS3231_TIME::uploadTime () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (0); // target register 0
  countOfBytesSent = Wire.write (buff, 7); // send 7 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent;
}

uint8_t DS3231_TIME::downloadTime () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (0); // target register 0
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 7); // request 7 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < 7)) {
    buff[i] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 7 indicates a problem
}

// alarm 1 member functions
 // member functions relating to alarm 1
void DS3231_TIME::setA1second (uint8_t second) {
  part.a1seconds = second % 10;
  part.a1sec10 = second / 10;
}

uint8_t DS3231_TIME::getA1second () {
  return ((part.a1sec10 * 10) + part.a1seconds);
}

void DS3231_TIME::setA1minute (uint8_t minute) {
  part.a1minutes = minute % 10;
  part.a1min10 = minute / 10;
}

uint8_t DS3231_TIME::getA1minute () {
  return ((part.a1min10 * 10) + part.a1minutes);
}

void DS3231_TIME::setA1hour (uint8_t hour, bool hr12, bool isAM) {
  part.a1hr12 = hr12;
  part.a1hours = hour % 10;
  // divide into two paths
  if (hr12) { // path #1 = 12-hour mode
    part.a1hour10 = hour / 10;
    part.a1hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour mode
    if (hour < 20) {
      part.a1hour10 = hour / 10;
      part.a1hr20pa = 0;
    } else {
      part.a1hour10 = 0;
      part.a1hr20pa = 1;
    }
  }
}

uint8_t DS3231_TIME::getA1hour () {
  // separate action onto two paths
  if (part.a1hr12) { // path #1 = 12-hour mode
    return ((part.a1hour10 * 10) + part.a1hours);
  } else {
    return (
        (part.a1hr20pa * 20)
      + (part.a1hour10 * 10)
      + part.a1hours
    );  
  }
}

bool DS3231_TIME::getA1hr12 () {
  return (bool) part.a1hr12;
}

bool DS3231_TIME::getA1isAM () {
  // separate action onto two paths
  if (part.a1hr12) { // path #1 = 12-hour mode
    return (bool) part.a1hr20pa;
  } else {
    if (getA1hour() < 12) return true;
    return false;
  }
}

void DS3231_TIME::setA1date (uint8_t date) {
  part.a1daydate = date % 10;
  part.a1dat10 = date / 10;
  part.a1dydt = false;
}

uint8_t DS3231_TIME::getA1date () {
  return ((part.a1dat10 * 10) + part.a1daydate);
}

void DS3231_TIME::setA1dayOfWeek (uint8_t day) {
  part.a1daydate = day % 7;
  part.a1dydt = true;
}

uint8_t DS3231_TIME::getA1dayOfWeek () {
  return part.a1daydate;
}

bool DS3231_TIME::getA1dydt () {
  return (bool) part.a1dydt;
}

void DS3231_TIME::setA1matchbits (uint8_t bits) {
  part.a1m1 =    (bits & 0b00001);
  part.a1m2 =    (bits & 0b00010) >> 1;
  part.a1m3 =    (bits & 0b00100) >> 2;
  part.a1m4 =    (bits & 0b01000) >> 3;
  part.a1dydt =  (bits & 0b10000) >> 4;
}

uint8_t DS3231_TIME::getA1matchbits () {
  return (
      part.a1m1
    | (part.a1m2 << 1)
    | (part.a1m3 << 2)
    | (part.a1m4 << 3)
    | (part.a1dydt << 4)
  );
}


uint8_t DS3231_TIME::uploadAlarm1 () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (A1_Pos); // target registers for Alarm 1
  countOfBytesSent = Wire.write (buff+A1_Pos, A1_Length); // send 4 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
}

uint8_t DS3231_TIME::downloadAlarm1 () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (A1_Pos); // target registers for Alarm 1
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, A1_Length); // request 4 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < A1_Length)) {
    buff[i + A1_Pos] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 4 indicates a problem
}

// alarm 2 member functions
 // member functions relating to alarm 2

void DS3231_TIME::setA2minute (uint8_t minute) {
  part.a2minutes = minute % 10;
  part.a2min10 = minute / 10;
}

uint8_t DS3231_TIME::getA2minute () {
  return ((part.a2min10 * 10) + part.a2minutes);
}

void DS3231_TIME::setA2hour (uint8_t hour, bool hr12, bool isAM) {
  part.a2hr12 = hr12;
  part.a2hours = hour % 10;
  // divide into two paths
  if (hr12) { // path #1 = 12-hour mode
    part.a2hour10 = hour / 10;
    part.a2hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour mode
    if (hour < 20) {
      part.a2hour10 = hour / 10;
      part.a2hr20pa = 0;
    } else {
      part.a2hour10 = 0;
      part.a2hr20pa = 1;
    }
  }
}

uint8_t DS3231_TIME::getA2hour () {
  // separate action onto two paths
  if (part.a2hr12) { // path #1 = 12-hour mode
    return ((part.a2hour10 * 10) + part.a2hours);
  } else {
    return (
        (part.a2hr20pa * 20)
      + (part.a2hour10 * 10)
      + part.a2hours
    );  
  }
}

bool DS3231_TIME::getA2hr12 () {
  return (bool) part.a2hr12;
}

bool DS3231_TIME::getA2isAM () {
  // separate action onto two paths
  if (part.a2hr12) { // path #1 = 12-hour mode
    return (bool) part.a2hr20pa;
  } else {
    if (getA2hour() < 12) return true;
    return false;
  }
}

void DS3231_TIME::setA2date (uint8_t date) {
  part.a2daydate = date % 10;
  part.a2dat10 = date / 10;
  part.a2dydt = false;
}

uint8_t DS3231_TIME::getA2date () {
  return ((part.a2dat10 * 10) + part.a2daydate);
}

void DS3231_TIME::setA2dayOfWeek (uint8_t day) {
  part.a2daydate = day % 7;
  part.a2dydt = true;
}

uint8_t DS3231_TIME::getA2dayOfWeek () {
  return part.a2daydate;
}

bool DS3231_TIME::getA2dydt () {
  return (bool) part.a2dydt;
}

void DS3231_TIME::setA2matchbits (uint8_t bits) {
  part.a2m2 =    (bits & 0b0001);
  part.a2m3 =    (bits & 0b0010) >> 1;
  part.a2m4 =    (bits & 0b0100) >> 2;
  part.a2dydt =  (bits & 0b1000) >> 3;
}

uint8_t DS3231_TIME::getA2matchbits () {
  return (
      part.a2m2
    | (part.a2m3 << 1)
    | (part.a2m4 << 2)
    | (part.a2dydt << 3)
  );
}

uint8_t DS3231_TIME::uploadAlarm2 () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (A2_Pos); // target registers for Alarm 2
  countOfBytesSent = Wire.write (buff+A2_Pos, A2_Length); // send 3 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
}

uint8_t DS3231_TIME::downloadAlarm2 () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (A2_Pos); // target registers for Alarm 2
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, A2_Length); // request 3 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < A2_Length)) {
    buff[i + A2_Pos] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 3 indicates a problem
}

// Control and Status register functions

void DS3231_TIME::enableAlarm1interrupt () {
  part.CTRL.bit.intcn = 1; //enable interrupt output
  part.CTRL.bit.a1enable = 1; // enable output when a1flag = 1
}

void DS3231_TIME::disableAlarm1interrupt () {
  part.CTRL.bit.a1enable = 0;
}

bool DS3231_TIME::getAlarm1flag () {
  return (bool) part.STATUS.bit.a1flag;
}

void DS3231_TIME::clearAlarm1flag () {
  part.STATUS.bit.a1flag = 0;
}

void DS3231_TIME::enableAlarm2interrupt () {
  part.CTRL.bit.intcn = 1; //enable interrupt output
  part.CTRL.bit.a2enable = 1; // enable output when a1flag = 1
}

void DS3231_TIME::disableAlarm2interrupt () {
  part.CTRL.bit.a2enable = 0;
}

bool DS3231_TIME::getAlarm2flag () {
  return (bool) part.STATUS.bit.a2flag;
}

void DS3231_TIME::clearAlarm2flag () {
  part.STATUS.bit.a2flag = 0;
}

void DS3231_TIME::resetAlarmFlags () {
  clearAlarm1flag ();
  clearAlarm2flag ();
}

void DS3231_TIME::outputSQW (uint8_t freqCode) { // 0, 1, 2 or 3
  part.CTRL.bit.rs = freqCode;
}

void DS3231_TIME::enable32K () {
  part.STATUS.bit.en32khz = 1;
}

void DS3231_TIME::disable32K () {
  part.STATUS.bit.en32khz = 0;
}

void DS3231_TIME::requestTemperature () {
  part.CTRL.bit.conv = 1;
}

bool DS3231_TIME::temperatureReady () {
  if (part.CTRL.bit.conv || part.STATUS.bit.bsy) return false;
  return true;
}

uint8_t DS3231_TIME::uploadCS () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (CTRL_Pos); // target Control register
  countOfBytesSent = Wire.write (buff+CTRL_Pos, CS_Length); // send 2 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
  
}
uint8_t DS3231_TIME::downloadCS () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (CTRL_Pos); // target Control register
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, CS_Length); // request 2 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < CS_Length)) {
    buff[i + CTRL_Pos] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 2 indicates a problem
  
}

uint8_t DS3231_TIME::downloadTemperature () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (TEMP_Pos); // target Temperature registers
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, TEMP_Length); // request 2 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < TEMP_Length)) {
    buff[i + TEMP_Pos] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 2 indicates a problem
  
}
