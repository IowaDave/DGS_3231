

#ifndef DGS_3231_H
#define DGS_3231_H

#include <stdint.h>
#include <Wire.h>

// time-related defines
#define MODE24HR (false)
#define MODE12HR (true)

// alarm defines
#define DS3231_A1EVERYSEC  (0b01111) // alarm 1 every second
#define DS3231_A1MATCHSEC     (0b01110) // alarm 1 when seconds match
#define DS3231_A1MINSEC  (0b01100) // alarm 1 when minutes and seconds match
#define DS3231_A1HMS     (0b01000) // alarm 1 when hours, minutes and seconds match
#define DS3231_A1DATE    (0b00000) // alarm 1 when date, hours, minutes and seconds match
#define DS3231_A1DOW     (0b10000) // alarm 1 when day of week, hours, minutes and seconds match

#define DS3231_A2EVERYMIN  (0b0111)  // alarm 2 every minute
#define DS3231_A2MATCHMIN     (0b0110)  // alarm 2 when minutes match
#define DS3231_A2HM      (0b0100)  // alarm 2 when hours and minutes match
#define DS3231_A2DATE    (0b0000)  // alarm 2 when date, hours and minutes match
#define DS3231_A2DOW     (0b1000)  // alarm 2 when day of week, hours and minutes match

// CONTROL register defines
#define DS3231_CTRL_A1IE_pos   0
#define DS3231_CTRL_A1IE       (1<<DS3231_CTRL_A1IE_pos)
#define DS3231_CTRL_A2IE_pos   1
#define DS3231_CTRL_A2IE       (1<<DS3231_CTRL_A2IE_pos)
#define DS3231_CTRL_INTCN_pos  2
#define DS3231_CTRL_INTCN      (1<<DS3231_CTRL_INTCN_pos)
#define DS3231_CTRL_RS_pos     4
#define DS3231_CTRL_RS_mask    (0b11ul << DS3231_CTRL_RS_pos)
#define DS3231_CTRL_RS(value)  (DS3231_CTRL_RS_mask & ((value) << DS3231_CTRL_RS_pos)) 
#define DS3231_CTRL_CONV_pos   5
#define DS3231_CTRL_CONV       (1<<DS3231_CTRL_CONV_pos)
#define DS3231_CTRL_BBSQW_pos  6
#define DS3231_CTRL_BBSQW      (1<<DS3231_CTRL_BBSQW_pos)
#define DS3231_CTRL_EOSC_pos   7
#define DS3231_CTRL_EOSC       (1<<DS3231_CTRL_EOSC_pos)
#define DS3231_CTRL_RS_Hz0001_val  0
#define DS3231_CTRL_RS_Hz1024_val  1
#define DS3231_CTRL_RS_Hz4096_val  2
#define DS3231_CTRL_RS_Hz8192_val  3
#define DS3231_SQW_1HZ        0
#define DS3231_SQW_1024HZ     1
#define DS3231_SQW_4096HZ     2
#define DS3231_SQW_8192HZ     3

// STATUS register defines
#define DS3231_STATUS_A1FLAG_pos   0
#define DS3231_STATUS_A1FLAG       (1<<DS3231_STATUS_AIFLAG_pos)
#define DS3231_STATUS_A2FLAG_pos   1
#define DS3231_STATUS_A2FLAG       (1<<DS3231_STATUS_A2FLAG_pos)
#define DS3231_STATUS_BSY_pos      2
#define DS3231_STATUS_BSY          (1<<DS3231_STATUS_BSY_pos)
#define DS3231_STATUS EN32KHZ_pos  3  
#define DS3231_STATUS_EN32KHZ      (1<<DS3231_STATUS_EN32KHZ_pos)
#define DS3231_STATUS_OSF_pos      7
#define DS3231_STATUS_OSF          (1<<DS3231_STATUS_OSF_pos)

union DGS_3231 {
  // data
  struct __attribute__((__packed__)) {
    union {
      struct {
        // byte 0x00 time seconds
        uint8_t seconds :4; // 1s of seconds, 0 - 9
        uint8_t sec10   :3; // 10s of seconds, 0 - 5
        uint8_t         :1; // reserved
    
        // byte 0x01 time minutes
        uint8_t minutes :4; // 1s of minutes, 0 - 9
        uint8_t min10   :3; // 10s of minutes, 0 - 5
        uint8_t         :1; // reserved
    
        // byte 0x02 time hours
        uint8_t hours   :4; // 1s of hours, 0 - 9
        uint8_t hour10  :1; // 10s of hours, 0 - 1
        uint8_t hr20pa  :1; // 20s of hours, 0 - 1, or PM/AM(low)
        uint8_t hr12    :1; // 12hr/24hr(low) mode
        uint8_t         :1; // reserved
    
        // byte 0x03 time day of week
        uint8_t dow     :3; // day of week, 0 - 6
        uint8_t         :5; // reserved
    
        // byte 0x04 time date in month
        uint8_t date    :4; // 1s of date, 0 - 9
        uint8_t dat10   :2; // 10s of date, 0 - 3
        uint8_t         :2; // reserved
    
        // byte 0x05 time month
        uint8_t month   :4; // 1s of month, 0 - 9
        uint8_t mon10   :1; // 10s of month, 0 - 1
        uint8_t         :2; // reserved
        uint8_t         :1; // century
    
        // byte 0x06 time year
        uint8_t year    :4; // 1s of year, 0 - 9
        uint8_t yr10    :4; // 10s of year, 0 - 9
      } field;
      uint8_t buff[7];
    } TIME;

    union {
      struct {
        // byte 0x07 alarm 1 seconds
        uint8_t seconds :4; // 1s of alarm 1 seconds
        uint8_t sec10   :3; // 10s of alarm 1 seconds
        uint8_t a1m1      :1; // alarm 1 match bit 1

        // byte 0x08 alarm 1 minutes
        uint8_t minutes :4; // 1s of alarm 1 minutes
        uint8_t min10   :3; // 10s of alarm 1 minutes
        uint8_t a1m2      :1; // alarm 1 match bit 2
    
        // byte 0x09 alarm 1 hours
        uint8_t hours   :4; // 1s of alarm 1 hours, 0 - 9
        uint8_t hour10  :1; // 10s of alarm 1 hours, 0 - 1
        uint8_t hr20pa  :1; // 20s of alarm 1 hours, 0 - 1, or PM/AM(low)
        uint8_t hr12    :1; // alarm 1 12hr/24hr(low) mode
        uint8_t a1m3      :1; // alarm 1 match bit 3
    
        // byte 0x0a alarm 1 day/date
        uint8_t daydate :4; // 1s of alarm 1 day or date
        uint8_t dat10   :2; // 10s of alarm 1 date
        uint8_t dydt    :1; // alarm 1 day/date(low) flag
        uint8_t a1m4      :1; // alarm 1 match bit 4
      } field;
      uint8_t buff[4];
    } ALARM1;
    
    union {
      struct {
        // byte 0x0b alarm 2 minutes
        uint8_t minutes :4; // 1s of alarm 2 minutes
        uint8_t min10   :3; // 10s of alarm 2 minutes
        uint8_t a2m2      :1; // alarm 2 match bit 2 (there is no bit 1)
    
        // byte 0x0c alarm 2 hours
        uint8_t hours   :4; // 1s of alarm 2 hours, 0 - 9
        uint8_t hour10  :1; // 10s of alarm 2 hours, 0 - 1
        uint8_t hr20pa  :1; // 20s of alarm 2 hours, 0 - 1, or PM/AM(low)
        uint8_t hr12    :1; // alarm 2 12hr/24hr(low) mode
        uint8_t a2m3      :1; // alarm 2 match bit 3
    
        // byte 0x0d alarm 2 day/date
        uint8_t daydate :4; // 1s of alarm 2 day or date
        uint8_t dat10   :2; // 10s of alarm 2 date
        uint8_t dydt    :1; // alarm 2 day/date(low) flag
        uint8_t a2m4      :1; // alarm 2 match bit 4
      } field;
      uint8_t buff[3];
    } ALARM2;

    // byte 0x0e DS3231 Control register
    union {
      struct {
        uint8_t a1enable  :1; // alarm 1 enable/disable(low)
        uint8_t a2enable  :1; // alarm 2 enable/disable(low)
        uint8_t intcn     :1; // sqw/int(low) output select
        uint8_t rs        :2; // square wave rate select 0=1Hz, 1=1,024Hz, 2=4,096Hz, 3=8,192Hz
        uint8_t conv      :1; // start and monitor temperature conversion
        uint8_t bbsqw     :1; // enable/disable(low) square wave output during battery poswer
        uint8_t eosc      :1; // DISABLE/enable(low) oscillator operation during battery power
      } field;
      uint8_t buff[1]; // packs it to 8-bit size and makes buff be a pointer
    } CONTROL;
    
    // byte 0x0f DS3231 Status register
    union {
      struct {
        uint8_t a1flag    :1; // alarm 1 match flag
        uint8_t a2flag    :1; // alarm 2 match flag
        uint8_t bsy       :1; // busy during temperature conversion
        uint8_t en32khz   :1; // enable/disable(low) output of 32kHz oscillator signal
        uint8_t           :3; // reserved
        uint8_t osf       :1; // oscillator stop flag to monitor DS3231 health
      } field;
      uint8_t buff[1]; // packs it to 8-bit size and makes buff be a pointer
    } STATUS;
    
    // byte 0x10 DS3231 aging offset register
    int8_t aging;         //`signed, two's complement value of aging data

    union {
      struct {
        // byte 0x11 temperature register upper byte, integer value of temperature
        uint8_t tempint   :7; // absolute value of integer part
        uint8_t tempsign  :1; // two's complement sign bit of the temperature
        // byte 0x12 fraction of degree of temperature
        uint8_t           :6; // reserved
        uint8_t tempfrac  :2; // fractional part, where 0=0, 1=0.25, 2=0.50, 3=0.75
      } field;
      uint8_t buff[2];
    } TEMPERATURE;
  } part;
  uint8_t buff[0x12];

  // constructors
  DGS_3231();
  DGS_3231( 
    uint8_t month,
    uint8_t date,
    uint8_t year,
    uint8_t hour,
    uint8_t minute,
    uint8_t second,
    bool hr12,
    bool isAM = false
  );

  // member functions relating to clock time and date
  void setSeconds (uint8_t seconds);
  uint8_t getSeconds () ; 
  void setMinutes (uint8_t seconds);
  uint8_t getMinutes () ; 
  void setDoW (uint8_t dow);
  uint8_t getDoW ();
  void setDate (uint8_t date);
  uint8_t getDate ();
  void setMonth (uint8_t month);
  uint8_t getMonth ();
  void setYear (uint8_t year);
  uint8_t getYear ();
  void setHour (uint8_t hour, bool isAM);
  uint8_t getHour (); 
  bool getIsAM (); // true = a.m. in 12-hour mode
  void setMode12 (bool mode);
  bool getMode12 (); // true = using 12-hour mode

  uint8_t uploadTime ();
  uint8_t downloadTime ();

  // member functions relating to alarm 1
  void setA1second (uint8_t second);
  uint8_t getA1second ();
  void setA1minute (uint8_t minute);
  uint8_t getA1minute ();
  void setA1hour (uint8_t hour, bool hr12, bool isAM = false);
  uint8_t getA1hour ();
  bool getA1hr12 ();
  bool getA1isAM ();
  void setA1date (uint8_t date);
  uint8_t getA1date ();
  void setA1dayOfWeek (uint8_t day);
  uint8_t getA1dayOfWeek ();
  bool getA1dydt ();
  void setA1matchbits (uint8_t bits);
  uint8_t getA1matchbits ();

  uint8_t uploadAlarm1 ();
  uint8_t downloadAlarm1 ();


  // member functions for alarm 2
  void setA2minute (uint8_t minute);
  uint8_t getA2minute ();
  void setA2hour (uint8_t hour, bool hr12, bool isAM = false);
  uint8_t getA2hour ();
  bool getA2hr12 ();
  bool getA2isAM ();
  void setA2date (uint8_t date);
  uint8_t getA2date ();
  void setA2dayOfWeek (uint8_t day);
  uint8_t getA2dayOfWeek ();
  bool getA2dydt ();
  void setA2month (uint8_t month);
  uint8_t getA2month ();
  void setA2year (uint8_t year);
  uint8_t getA2year ();
  void setA2matchbits (uint8_t bits);
  uint8_t getA2matchbits ();

  uint8_t uploadAlarm2 ();
  uint8_t downloadAlarm2 ();

  // member functions for the control and status registers
  void enableAlarm1interrupt ();
  void disableAlarm1interrupt ();
  bool getAlarm1flag ();
  void clearAlarm1flag ();
  void enableAlarm2interrupt ();
  void disableAlarm2interrupt ();
  bool getAlarm2flag ();
  void clearAlarm2flag ();
  void resetAlarmFlags ();
  void outputSQW (uint8_t freqCode); // 0, 1, 2 or 3
  void enable32K ();
  void disable32K ();
  uint8_t uploadControl ();
  uint8_t uploadStatus ();
  uint8_t downloadControl ();
  uint8_t downloadStatus ();
  void requestTemperature ();
  bool temperatureReady ();
  uint8_t downloadTemperature ();

  
}; // union DGS_3231

#endif
