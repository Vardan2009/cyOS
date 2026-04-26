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

static inline uint32_t syscall1(uint32_t num, uint32_t a1) {
    uint32_t ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(num), "b"(a1) : "memory");
    return ret;
}

static inline uint32_t syscall2(uint32_t num, uint32_t a1, uint32_t a2) {
    uint32_t ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(num), "b"(a1), "c"(a2)
                 : "memory");
    return ret;
}

#define SYSCALL_EXIT 1
#define SYSCALL_EXEC 2
#define SYSCALL_WRITE 3

static inline void exit(int code) { syscall1(SYSCALL_EXIT, (uint32_t)code); }

static inline int exec(const char *path) {
    return (int)syscall1(SYSCALL_EXEC, (uint32_t)path);
}

static inline void write(const char *text) {
    syscall1(SYSCALL_WRITE, (uint32_t)text);
}

void _start() {
    write("Loaded second program!\n");
    exit(777);
}
