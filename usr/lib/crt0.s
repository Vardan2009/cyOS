GLOBAL _start
EXTERN main
EXTERN exit
EXTERN __libc_init

_start:
	MOV EAX, [ESP]
	MOV EBX, [ESP + 4]
	MOV ECX, [ESP + 8]

	PUSH ECX
	PUSH EBX
	PUSH EAX
	CALL __libc_init
	ADD ESP, 12

	PUSH ECX
	PUSH EBX
	PUSH EAX
	CALL main
	ADD ESP, 12

	PUSH EAX
	CALL exit
	.HANG:
		HLT
		JMP .HANG

