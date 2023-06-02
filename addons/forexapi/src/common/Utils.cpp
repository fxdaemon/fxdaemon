/*
 * Copyright 2023 fxdaemon.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../stdafx.h"
#include "Utils.h"

static const char *TimeFormat = "%Y/%m/%d %H:%M:%S";

void Utils::time2Tm(time_t t, tm *st)
{
  if (t == 0) {
    t = time(NULL);
  }
  localtime_s(st, &t);
}

void Utils::time2UTCTm(time_t t, tm *st)
{
  if (t == 0) {
    t = time(NULL);
  }
  gmtime_s(st, &t);
}

long Utils::getTimeZone()
{
#ifdef WIN32
  TIME_ZONE_INFORMATION timeZoneInfo;
  DWORD ret = ::GetTimeZoneInformation(&timeZoneInfo);
  if (ret == TIME_ZONE_ID_STANDARD || ret == TIME_ZONE_ID_UNKNOWN) {
    return timeZoneInfo.Bias * 60;
  }
  return 0;
#else
  time_t t = time(NULL);
  struct tm st;
  localtime_r(&t, &st);
  return -st.tm_gmtoff;
#endif
}

time_t Utils::oleTime2Time(DATE date)
{
  tm st;
  if (CO2GDateUtils::OleTimeToCTime(date, &st)) {
    return mktime(&st) - getTimeZone();
  } else {
    return 0;
  }
}

void Utils::oleTime2Tm(DATE date, tm *st)
{
  if (!CO2GDateUtils::OleTimeToCTime(date, st)) {
    memset(st, 0, sizeof(tm));
  }
}

DATE Utils::time2OleTime(time_t t)
{
  tm st;
  time2UTCTm(t, &st);
  return tm2OleTime(&st);
}

DATE Utils::tm2OleTime(tm *st)
{
  DATE dt = 0;
  CO2GDateUtils::CTimeToOleTime(st, &dt);
  return dt;
}

INT64 Utils::oleTime2Minute(DATE date)
{
  double msec = date * 86400 * 1000;
  return (INT64)(floor(msec + 0.5)) / 60000;
}

std::string Utils::tm2String(tm *st, const char* format)
{
  char str[128];
  strftime(str, sizeof(str), format ? format : TimeFormat, st);
  return str;
}

std::string Utils::time2String(time_t t, const char* format)
{
  tm st;
  gmtime_s(&st, &t);
  return tm2String(&st, format);
}

std::string Utils::oleTime2String(DATE date, const char* format)
{
  tm st;
  oleTime2Tm(date, &st);
  return tm2String(&st, format);
}
