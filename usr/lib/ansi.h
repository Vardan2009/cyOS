#ifndef CYLIBC_ANSI_H
#define CYLIBC_ANSI_H

#include <stdio.h>

#include "syscall.h"

#define ANSI_UP(n) "\x1B[" #n "A"
#define ANSI_DOWN(n) "\x1B[" #n "B"
#define ANSI_RIGHT(n) "\x1B[" #n "C"
#define ANSI_LEFT(n) "\x1B[" #n "D"
#define ANSI_HOME "\x1B[H"
#define ANSI_CLEAR "\x1B[2J\x1B[H"
#define ANSI_CLEAR_LINE "\x1B[2K\r"
#define ANSI_ERASE_TO_END "\x1B[K"
#define ANSI_SAVE_CURSOR "\x1B[s"
#define ANSI_RESTORE_CURSOR "\x1B[u"
#define ANSI_HIDE_CURSOR "\x1B[?25l"
#define ANSI_SHOW_CURSOR "\x1B[?25h"

#define ANSI_RESET "\x1B[0m"
#define ANSI_BOLD "\x1B[1m"
#define ANSI_REVERSE "\x1B[7m"

#define ANSI_FG_BLACK "\x1B[30m"
#define ANSI_FG_RED "\x1B[31m"
#define ANSI_FG_GREEN "\x1B[32m"
#define ANSI_FG_YELLOW "\x1B[33m"
#define ANSI_FG_BLUE "\x1B[34m"
#define ANSI_FG_MAGENTA "\x1B[35m"
#define ANSI_FG_CYAN "\x1B[36m"
#define ANSI_FG_WHITE "\x1B[37m"
#define ANSI_FG_BRIGHT_RED "\x1B[91m"
#define ANSI_FG_BRIGHT_GREEN "\x1B[92m"
#define ANSI_FG_BRIGHT_CYAN "\x1B[96m"
#define ANSI_FG_BRIGHT_WHITE "\x1B[97m"

#define ANSI_BG_BLACK "\x1B[40m"
#define ANSI_BG_RED "\x1B[41m"
#define ANSI_BG_BLUE "\x1B[44m"
#define ANSI_BG_WHITE "\x1B[47m"

static inline void ANSIMove(int row, int col) {
    char buf[16];
    int i = 0;
    buf[i++] = '\x1B';
    buf[i++] = '[';
    if (row >= 10) buf[i++] = '0' + row / 10;
    buf[i++] = '0' + row % 10;
    buf[i++] = ';';
    if (col >= 10) buf[i++] = '0' + col / 10;
    buf[i++] = '0' + col % 10;
    buf[i++] = 'H';
    buf[i] = '\0';
    write(FD_STDOUT, buf, i);
}

#endif  // CYLIBC_ANSI_H
