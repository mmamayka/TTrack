
push 1
pop ax

loop:
	push ax
	push ax
	mul
	out

	push ax
	push 1
	add
	pop ax

	push ax
	push 10
	je exit
	jmp loop

exit:

hlt
