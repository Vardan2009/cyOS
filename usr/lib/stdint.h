#ifndef CYLIBC_STDINT_H
#define CYLIBC_STDINT_H

typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed long int int32_t;
typedef unsigned long int uint32_t;

typedef signed long long int int64_t;
typedef unsigned long long int uint64_t;

typedef unsigned long int addr_t;

#define NULL 0

typedef int32_t ssize_t;
typedef uint32_t size_t;

#endif  // CYLIBC_STDINT_H
