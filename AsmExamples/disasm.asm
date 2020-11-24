	call L_000000
	hlt 
L_000001:
	pop ax
	push ax
	push ax
	mul 
	ret 
L_000000:
	in 
	pop bx
	in 
	pop cx
L_000003:
	push bx
	call L_000001
	out 
	push bx
	push 1.000000
	add 
	pop bx
	push bx
	push cx
	jl L_000002
	jmp L_000003
L_000002:
	ret 
