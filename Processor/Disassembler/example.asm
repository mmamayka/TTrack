	in 
	pop bx
	in 
	pop cx
L_000002:
	push bx
	call L_000000
	out 
	push bx
	push 1.000000
	add 
	pop bx
	push bx
	push cx
	jl L_000001
	jmp L_000002
L_000001:
	hlt 
L_000000:
	pop ax
	push ax
	push ax
	mul 
	ret 
