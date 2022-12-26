

#include "DGS_3231.h"

#define TIME_Pos (0)
#define ALARM1_Pos (0x07)
#define ALARM2_Pos (0x0b)
#define CONTROL_Pos (0x0e)
#define STATUS_Pos (0x0f)
#define AGING_Pos (0x10)
#define TEMPERATURE_Pos (0x11)


DGS_3231::DGS_3231() {
  for (int i = 0; i < 7; i++) buff[i] = 0;
}

DGS_3231::DGS_3231( 
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

void DGS_3231::setSeconds (uint8_t seconds) {
  part.TIME.field.seconds = seconds % 10;
  part.TIME.field.sec10 = seconds / 10;
}

uint8_t DGS_3231::getSeconds () {
  return ((part.TIME.field.sec10 * 10) + part.TIME.field.seconds);
}

void DGS_3231::setMinutes (uint8_t minutes) {
  part.TIME.field.minutes = minutes % 10;
  part.TIME.field.min10 = minutes / 10;
}

uint8_t DGS_3231::getMinutes () {
  return ((part.TIME.field.min10 * 10) + part.TIME.field.minutes);
}

void DGS_3231::setDoW (uint8_t dow) {
  part.TIME.field.dow = dow & 7;
}

uint8_t DGS_3231::getDoW () {
  return part.TIME.field.dow;
}

void DGS_3231::setDate (uint8_t date) {
  part.TIME.field.date = date % 10;
  part.TIME.field.dat10 = date / 10;
}

uint8_t DGS_3231::getDate () {
  return ((part.TIME.field.dat10 * 10) + part.TIME.field.date); 
}

void DGS_3231::setMonth (uint8_t month) {
  part.TIME.field.month = month % 10;
  part.TIME.field.mon10 = month / 10;
}

uint8_t DGS_3231::getMonth () {
  return ((part.TIME.field.mon10 * 10) + part.TIME.field.month);
}

void DGS_3231::setYear (uint8_t year) {
  part.TIME.field.year = year % 10;
  part.TIME.field.yr10 = year / 10;
}

uint8_t DGS_3231::getYear () {
  return ((part.TIME.field.yr10 * 10) + part.TIME.field.year);
}

void DGS_3231::setHour (uint8_t hour, bool isAM) {
  // separate action into two paths
  if (part.TIME.field.hr12) { // path #1 = 12-hour time mode
    // expects 1 <= hour <= 12, works for both a.m.and p.m.
    part.TIME.field.hours = hour % 10;
    part.TIME.field.hour10 = hour / 10;
    part.TIME.field.hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour time mode
    // expects 0 <= hour < 24
    part.TIME.field.hours = hour % 10;
    if (hour < 20) { 
      part.TIME.field.hour10 = hour / 10;
      part.TIME.field.hr20pa = 0;
    } else {
      part.TIME.field.hour10 = 0;
      part.TIME.field.hr20pa = 1;
    }
  }
}

uint8_t DGS_3231::getHour () {
  // separate action into two paths
  if (part.TIME.field.hr12) { // path #1 = 12-hour mode
    return ((part.TIME.field.hour10 * 10) + part.TIME.field.hours);
  } else { // path #2 = 24-hour mode
    return (
        (part.TIME.field.hr20pa * 20) 
      + (part.TIME.field.hour10 * 10) 
      + part.TIME.field.hours
    );
  }
}

bool DGS_3231::getIsAM () {
  //separate action into two parts
  if (part.TIME.field.hr12) { // path #1 = 12-hour mode
    return (bool) part.TIME.field.hr20pa;
  } else { // path #2 = 24-hour mode
    if (getHour() < 12) return true; 
    return false;
  }
}
  
void DGS_3231::setMode12 (bool mode) {
  part.TIME.field.hr12 = (uint8_t) mode;
}

bool DGS_3231::getMode12 () {
  return (bool) part.TIME.field.hr12;
}

uint8_t DGS_3231::uploadTime () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (TIME_Pos); // target register 0
  countOfBytesSent = Wire.write (part.TIME.buff, sizeof(part.TIME.buff)); // send 7 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent;
}

uint8_t DGS_3231::downloadTime () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (TIME_Pos); // target register 0
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, sizeof(part.TIME.buff)); // request 7 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < sizeof(part.TIME.buff))) {
    part.TIME.buff[i] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 7 indicates a problem
}

// alarm 1 member functions
 // member functions relating to alarm 1
void DGS_3231::setA1second (uint8_t second) {
  part.ALARM1.field.seconds = second % 10;
  part.ALARM1.field.sec10 = second / 10;
}

uint8_t DGS_3231::getA1second () {
  return ((part.ALARM1.field.sec10 * 10) + part.ALARM1.field.seconds);
}

void DGS_3231::setA1minute (uint8_t minute) {
  part.ALARM1.field.minutes = minute % 10;
  part.ALARM1.field.min10 = minute / 10;
}

uint8_t DGS_3231::getA1minute () {
  return ((part.ALARM1.field.min10 * 10) + part.ALARM1.field.minutes);
}

void DGS_3231::setA1hour (uint8_t hour, bool hr12, bool isAM) {
  part.ALARM1.field.hr12 = hr12;
  part.ALARM1.field.hours = hour % 10;
  // divide into two paths
  if (hr12) { // path #1 = 12-hour mode
    part.ALARM1.field.hour10 = hour / 10;
    part.ALARM1.field.hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour mode
    if (hour < 20) {
      part.ALARM1.field.hour10 = hour / 10;
      part.ALARM1.field.hr20pa = 0;
    } else {
      part.ALARM1.field.hour10 = 0;
      part.ALARM1.field.hr20pa = 1;
    }
  }
}

uint8_t DGS_3231::getA1hour () {
  // separate action onto two paths
  if (part.ALARM1.field.hr12) { // path #1 = 12-hour mode
    return ((part.ALARM1.field.hour10 * 10) + part.ALARM1.field.hours);
  } else {
    return (
        (part.ALARM1.field.hr20pa * 20)
      + (part.ALARM1.field.hour10 * 10)
      + part.ALARM1.field.hours
    );  
  }
}

bool DGS_3231::getA1hr12 () {
  return (bool) part.ALARM1.field.hr12;
}

bool DGS_3231::getA1isAM () {
  // separate action onto two paths
  if (part.ALARM1.field.hr12) { // path #1 = 12-hour mode
    return (bool) part.ALARM1.field.hr20pa;
  } else {
    if (getA1hour() < 12) return true;
    return false;
  }
}

void DGS_3231::setA1date (uint8_t date) {
  part.ALARM1.field.daydate = date % 10;
  part.ALARM1.field.dat10 = date / 10;
  part.ALARM1.field.dydt = false;
}

uint8_t DGS_3231::getA1date () {
  return ((part.ALARM1.field.dat10 * 10) + part.ALARM1.field.daydate);
}

void DGS_3231::setA1dayOfWeek (uint8_t day) {
  part.ALARM1.field.daydate = day % 7;
  part.ALARM1.field.dydt = true;
}

uint8_t DGS_3231::getA1dayOfWeek () {
  return part.ALARM1.field.daydate;
}

bool DGS_3231::getA1dydt () {
  return (bool) part.ALARM1.field.dydt;
}

void DGS_3231::setA1matchbits (uint8_t bits) {
  part.ALARM1.field.a1m1 =    (bits & 0b00001);
  part.ALARM1.field.a1m2 =    (bits & 0b00010) >> 1;
  part.ALARM1.field.a1m3 =    (bits & 0b00100) >> 2;
  part.ALARM1.field.a1m4 =    (bits & 0b01000) >> 3;
  part.ALARM1.field.dydt =  (bits & 0b10000) >> 4;
}

uint8_t DGS_3231::getA1matchbits () {
  return (
      part.ALARM1.field.a1m1
    | (part.ALARM1.field.a1m2 << 1)
    | (part.ALARM1.field.a1m3 << 2)
    | (part.ALARM1.field.a1m4 << 3)
    | (part.ALARM1.field.dydt << 4)
  );
}


uint8_t DGS_3231::uploadAlarm1 () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (ALARM1_Pos); // target registers for Alarm 1
  countOfBytesSent = Wire.write (part.ALARM1.buff, sizeof(part.ALARM1.buff)); // send 4 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
}

uint8_t DGS_3231::downloadAlarm1 () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (ALARM1_Pos); // target registers for Alarm 1
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, sizeof(part.ALARM1.buff)); // request 4 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < sizeof(part.ALARM1.buff))) {
    part.ALARM1.buff[i] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 4 indicates a problem
}

// alarm 2 member functions
 // member functions relating to alarm 2

void DGS_3231::setA2minute (uint8_t minute) {
  part.ALARM2.field.minutes = minute % 10;
  part.ALARM2.field.min10 = minute / 10;
}

uint8_t DGS_3231::getA2minute () {
  return ((part.ALARM2.field.min10 * 10) + part.ALARM2.field.minutes);
}

void DGS_3231::setA2hour (uint8_t hour, bool hr12, bool isAM) {
  part.ALARM2.field.hr12 = hr12;
  part.ALARM2.field.hours = hour % 10;
  // divide into two paths
  if (hr12) { // path #1 = 12-hour mode
    part.ALARM2.field.hour10 = hour / 10;
    part.ALARM2.field.hr20pa = (uint8_t) isAM;
  } else { // path #2 = 24-hour mode
    if (hour < 20) {
      part.ALARM2.field.hour10 = hour / 10;
      part.ALARM2.field.hr20pa = 0;
    } else {
      part.ALARM2.field.hour10 = 0;
      part.ALARM2.field.hr20pa = 1;
    }
  }
}

uint8_t DGS_3231::getA2hour () {
  // separate action onto two paths
  if (part.ALARM2.field.hr12) { // path #1 = 12-hour mode
    return ((part.ALARM2.field.hour10 * 10) + part.ALARM2.field.hours);
  } else {
    return (
        (part.ALARM2.field.hr20pa * 20)
      + (part.ALARM2.field.hour10 * 10)
      + part.ALARM2.field.hours
    );  
  }
}

bool DGS_3231::getA2hr12 () {
  return (bool) part.ALARM2.field.hr12;
}

bool DGS_3231::getA2isAM () {
  // separate action onto two paths
  if (part.ALARM2.field.hr12) { // path #1 = 12-hour mode
    return (bool) part.ALARM2.field.hr20pa;
  } else {
    if (getA2hour() < 12) return true;
    return false;
  }
}

void DGS_3231::setA2date (uint8_t date) {
  part.ALARM2.field.daydate = date % 10;
  part.ALARM2.field.dat10 = date / 10;
  part.ALARM2.field.dydt = false;
}

uint8_t DGS_3231::getA2date () {
  return ((part.ALARM2.field.dat10 * 10) + part.ALARM2.field.daydate);
}

void DGS_3231::setA2dayOfWeek (uint8_t day) {
  part.ALARM2.field.daydate = day % 7;
  part.ALARM2.field.dydt = true;
}

uint8_t DGS_3231::getA2dayOfWeek () {
  return part.ALARM2.field.daydate;
}

bool DGS_3231::getA2dydt () {
  return (bool) part.ALARM2.field.dydt;
}

void DGS_3231::setA2matchbits (uint8_t bits) {
  part.ALARM2.field.a2m2 =    (bits & 0b0001);
  part.ALARM2.field.a2m3 =    (bits & 0b0010) >> 1;
  part.ALARM2.field.a2m4 =    (bits & 0b0100) >> 2;
  part.ALARM2.field.dydt =  (bits & 0b1000) >> 3;
}

uint8_t DGS_3231::getA2matchbits () {
  return (
      part.ALARM2.field.a2m2
    | (part.ALARM2.field.a2m3 << 1)
    | (part.ALARM2.field.a2m4 << 2)
    | (part.ALARM2.field.dydt << 3)
  );
}

uint8_t DGS_3231::uploadAlarm2 () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (ALARM2_Pos); // target registers for Alarm 2
  countOfBytesSent = Wire.write (part.ALARM2.buff, sizeof(part.ALARM2.buff)); // send 3 bytes
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
}

uint8_t DGS_3231::downloadAlarm2 () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (ALARM2_Pos); // target registers for Alarm 2
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, sizeof(part.ALARM2.buff)); // request 3 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < sizeof(part.ALARM2.buff))) {
    part.ALARM2.buff[i] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 3 indicates a problem
}

// Control and Status register functions

void DGS_3231::enableAlarm1interrupt () {
  part.CONTROL.field.intcn = 1; //enable interrupt output
  part.CONTROL.field.a1enable = 1; // enable output when a1flag = 1
}

void DGS_3231::disableAlarm1interrupt () {
  part.CONTROL.field.a1enable = 0;
}

bool DGS_3231::getAlarm1flag () {
  return (bool) part.STATUS.field.a1flag;
}

void DGS_3231::clearAlarm1flag () {
  part.STATUS.field.a1flag = 0;
}

void DGS_3231::enableAlarm2interrupt () {
  part.CONTROL.field.intcn = 1; //enable interrupt output
  part.CONTROL.field.a2enable = 1; // enable output when a1flag = 1
}

void DGS_3231::disableAlarm2interrupt () {
  part.CONTROL.field.a2enable = 0;
}

bool DGS_3231::getAlarm2flag () {
  return (bool) part.STATUS.field.a2flag;
}

void DGS_3231::clearAlarm2flag () {
  part.STATUS.field.a2flag = 0;
}

void DGS_3231::resetAlarmFlags () {
  clearAlarm1flag ();
  clearAlarm2flag ();
}

void DGS_3231::outputSQW (uint8_t freqCode) { // 0, 1, 2 or 3
  part.CONTROL.field.rs = freqCode;
  part.CONTROL.field.intcn = 0; // output square wave, not alarm-match interrupt
}

void DGS_3231::enable32K () {
  part.STATUS.field.en32khz = 1;
}

void DGS_3231::disable32K () {
  part.STATUS.field.en32khz = 0;
}

uint8_t DGS_3231::uploadControl () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (CONTROL_Pos); // target Control register address
  countOfBytesSent = Wire.write (part.CONTROL.buff, 1); // send 1 byte
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
  
}

uint8_t DGS_3231::uploadStatus () {
  uint8_t countOfBytesSent;
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (STATUS_Pos); // target Status register address
  countOfBytesSent = Wire.write (part.STATUS.buff, 1); // send 1 byte
  return (Wire.endTransmission() * 10) + countOfBytesSent; 
  
}

uint8_t DGS_3231::downloadControl () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (CONTROL_Pos); // target Control register address
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 1); // request 1 byte
  uint8_t i = 0;
  if (Wire.available())  {
    part.CONTROL.buff[0] = Wire.read(); // read 1 byte
    i = 1; // increment index
  }
  // test results
  return i; // other than 1 indicates a problem
}

uint8_t DGS_3231::downloadStatus () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (STATUS_Pos); // target Status register address
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, 1); // request 1 byte
  uint8_t i = 0;
  if (Wire.available())  {
    part.STATUS.buff[0] = Wire.read(); // read 1 byte
    i = 1; // increment index
  }
  // test results
  return i; // other than 1 indicates a problem
}

void DGS_3231::requestTemperature () {
  part.CONTROL.field.conv = 1;
}

bool DGS_3231::temperatureReady () {
  if (part.CONTROL.field.conv || part.STATUS.field.bsy) return false;
  return true;
}

uint8_t DGS_3231::downloadTemperature () {
  Wire.beginTransmission(0b1101000); // DS3231 I2C address
  Wire.write (TEMPERATURE_Pos); // target Temperature registers
  Wire.endTransmission();
  Wire.requestFrom(0b1101000, sizeof(part.TEMPERATURE.buff)); // request 2 bytes
  uint8_t i = 0;
  while (Wire.available() && (i < sizeof(part.TEMPERATURE.buff))) {
    part.TEMPERATURE.buff[i] = Wire.read(); // read 1 byte
    i += 1; // increment index
  }
  // test results
  return i; // other than 2 indicates a problem
  
}
