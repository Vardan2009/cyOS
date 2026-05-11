# cyOS
cyOS is a 32-bit operating system for the x86 CPU architecture.

<img width="720" height="400" alt="image" src="https://github.com/user-attachments/assets/1236354d-9211-4315-aa5e-9f843022442b" />

## Syscall Table

Syscalls can be invoked using the `INT 80h` instruction.

| Name/EAX  | EBX | ECX | EDX |
| --------- | --- | --- | --- |
| `EXIT/01` | exit code | ... | ... |
| `EXEC/02` | path | argv | ... |
| `READ/03` | fd | buffer | count |
| `WRITE/04` | fd | buffer | count |
| `OPEN/05` | path | flags | ... |
| `OPENDIR/06` | path | ... | ... |
| `CLOSE/07` | fd | ... | ... |
| `SEEK/08` | fd | offset | whence |
| `STAT/09` | path | info | ... |
| `GETENV/10` | key | buffer | size |
| `SETENV/11` | key | value | ... |
| `UNSETENV/12` | key | ... | ... |
| `SBRK/13` | increment | ... | ... |
| `GETCWD/14` | buffer | size | ... |
| `CHDIR/15` | path | ... | ... |

## Current features

- VGA Driver
- PS/2 Keyboard Driver
- Serial IO
- Disk IO (ATA/IDE)
- Filesystem support (FAT12, FAT16, FAT32, and exFAT)
- Paging and Upper-half kernel
- Kernel Syscalls
- User tools and libc
- ELF executables and user processes
- ANSI output rendering
- User-mode shell (cyShell)

## Future features

- Multitasking/multithreading
- Graphical Interface
