GLOBAL _start
EXTERN main 
EXTERN exit 
EXTERN __libc_init 

_start:
	MOV EBP,ESP 
	MOV EAX, [EBP]
	MOV EBX, [EBP + 4]
	MOV ECX, [EBP + 8]

	PUSH ECX 
	PUSH EBX 
	PUSH EAX 
	CALL __libc_init 
	ADD ESP, 12 


	MOV EAX, [EBP]
	MOV EBX, [EBP + 4]
	MOV ECX, [EBP + 8]

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
