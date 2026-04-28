#ifndef CY_CONSOLE_H
#define CY_CONSOLE_H

typedef void (*ConsolePutCFn)(char);
extern ConsolePutCFn ConsolePutC;

typedef void (*ConsolePutSFn)(char *);
extern ConsolePutSFn ConsolePutS;

#endif  // CY_CONSOLE_H
