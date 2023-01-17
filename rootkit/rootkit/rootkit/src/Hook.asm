extern halCounterQueryRoutine:QWORD
extern circularKernelContextLogger:QWORD
extern testHook:PROC
.code

checkLogger PROC
	push rcx
	mov rcx, rsp
	call testHook
	pop rcx
exit:
	mov rax, halCounterQueryRoutine
	jmp rax
checkLogger ENDP
end