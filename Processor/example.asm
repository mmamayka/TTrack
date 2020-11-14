call main
hlt

sqr:
	; copy value to the stack twice
	pop ax
	push ax
	push ax

	; find sqr
	mul

	; that is all
	ret

; emtry point
main:
	; initialize counter and store it in bx
	in
	pop bx

	; initialize max counter value and store it in cx
	in
	pop cx

loop:
	; copy counter to the stack to compute sqr
	push bx
	call sqr
	; print sqr value
	out

	; copy counter to the stack to increment it
	push bx
	push 1
	; increment counter
	add
	; store counter in bx
	pop bx

	; test if counter less of equal to 10
	push bx
	push cx
	; halt otherwise
	jl exit
	
	; keep counting squares
	jmp loop

exit:
	ret
