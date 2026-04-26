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

static inline uint32_t syscall3(uint32_t num, uint32_t a1, uint32_t a2,
                                uint32_t a3) {
    uint32_t ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(num), "b"(a1), "c"(a2), "d"(a3)
                 : "memory");
    return ret;
}

#define SYSCALL_EXIT 1
#define SYSCALL_EXEC 2
#define SYSCALL_READ 3
#define SYSCALL_WRITE 4
#define SYSCALL_OPEN 5
#define SYSCALL_CLOSE 6
#define SYSCALL_SEEK 7

#define FD_STDIN 0
#define FD_STDOUT 1
#define FD_STDERR 2

static inline void exit(int code) { syscall1(SYSCALL_EXIT, (uint32_t)code); }

static inline int exec(const char *path) {
    return (int)syscall1(SYSCALL_EXEC, (uint32_t)path);
}

static inline int open(const char *path, int flags) {
    return (int)syscall2(SYSCALL_OPEN, (uint32_t)path, (uint32_t)flags);
}

static inline int close(int fd) {
    return (int)syscall1(SYSCALL_CLOSE, (uint32_t)fd);
}

static inline int read(int fd, void *buf, uint32_t count) {
    return (int)syscall3(SYSCALL_READ, (uint32_t)fd, (uint32_t)buf, count);
}

static inline int write(int fd, const void *buf, uint32_t count) {
    return (int)syscall3(SYSCALL_WRITE, (uint32_t)fd, (uint32_t)buf, count);
}

static inline void puts(const char *s) {
    uint32_t len = 0;
    while (s[len]) len++;
    write(FD_STDOUT, s, len);
}

static inline char *gets(char *buf, int size) {
    int n = read(FD_STDIN, buf, size - 1);
    if (n <= 0) {
        buf[0] = '\0';
        return buf;
    }

    if (buf[n - 1] == '\n') n--;
    buf[n] = '\0';
    return buf;
}

void _start() {
    char buf[512];

    puts("Hello! What is your name?: ");

    if (gets(buf, 512)) {
        puts("Hello, ");
        puts(buf);
        puts("!\n");
    }

    exit(7);
}
