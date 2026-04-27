#ifndef CY_ENV_H
#define CY_ENV_H

#include "process.h"

void EnvSet(Process *proc, const char *key, const char *value);
char *EnvGet(Process *proc, const char *key);
void EnvUnset(Process *proc, const char *key);
void EnvInherit(Process *child, Process *parent);

#endif  // CY_ENV_H
