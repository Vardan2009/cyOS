#include "rtc.h"

#include <io.h>
#include <stdint.h>

#define CMOS_PORT 0x70
#define CMOS_DATA 0x71

static uint8_t RTCWait() {
    outb(CMOS_PORT, 0x0A);
    return (inb(CMOS_DATA) & 0x80);
}

static uint8_t RTCRegRead(int reg) {
    outb(CMOS_PORT, reg);
    return inb(CMOS_DATA);
}

DateTime RTCGetDateTime() {
    while (RTCWait());

    DateTime result;

    result.second = RTCRegRead(0x00);
    result.minute = RTCRegRead(0x02);
    result.hour = RTCRegRead(0x04);
    result.day = RTCRegRead(0x07);
    result.month = RTCRegRead(0x08);
    result.year = RTCRegRead(0x09);

    uint8_t regB = RTCRegRead(0x0B);

    if (!(regB & 0x04)) {
        result.second = (result.second & 0x0F) + ((result.second / 16) * 10);
        result.minute = (result.minute & 0x0F) + ((result.minute / 16) * 10);

        result.hour =
            ((result.hour & 0x0F) + (((result.hour & 0x70) / 16) * 10)) |
            (result.hour & 0x80);

        result.day = (result.day & 0x0F) + ((result.day / 16) * 10);

        result.month = (result.month & 0x0F) + ((result.month / 16) * 10);

        result.year = (result.year & 0x0F) + ((result.year / 16) * 10);
    }

    if (!(regB & 0x02) && (result.hour & 0x80))
        result.hour = ((result.hour & 0x7F) + 12) % 24;
    result.year += 2000;

    return result;
}
