#ifndef UTILS_H_230426_
#define UTILS_H_230426_

#include "ForexConnect.h"

class Utils
{
public:
  static void time2Tm(time_t t, tm *st);
  static void time2UTCTm(time_t t, tm *st);
  static long getTimeZone();
  static time_t oleTime2Time(DATE date);
  static void oleTime2Tm(DATE date, tm *st);
  static DATE time2OleTime(time_t t);
  static DATE tm2OleTime(tm *st);
  static INT64 oleTime2Minute(DATE date);
  static std::string tm2String(tm *st, const char* format = NULL);
  static std::string time2String(time_t t, const char* format = NULL);
  static std::string oleTime2String(DATE date, const char* format = NULL);
};

#endif
