GLOBAL SyscallStub 
EXTERN SyscallDispatch

SyscallStub:
	PUSHA 
	PUSH ESP 

	CALL SyscallDispatch 
	ADD ESP, 4 

	MOV [ESP + 28], EAX 
	POPA 
	IRETD
