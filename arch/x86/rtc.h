#ifndef CY_RTC_H
#define CY_RTC_H

#include "datetime.h"

DateTime RTCGetDateTime();
void RTCSetDateTime(DateTime dt);

#endif  // CY_RTC_H
