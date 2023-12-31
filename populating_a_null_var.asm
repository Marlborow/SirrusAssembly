;Author: Wesley Marlborow
;Date:   24/08/2023
;title:  Populating a NULL Variable

;INFOMATION:
;when input is made it stores the value you enter into a temporary stack like this:

;example 'test'
; [length] 4
; [character/number] t
; [character/number] e
; [character/number] s
; [character/number] t

; you get the values from this by poping the stack
; into a register 'pop [register]'

; utilizing the length got from the stack, a loop is made and
; that value gets pushed/appened into the variable with
; 'mov [variable++] [register]'

;finally a new variable is created to hold the new value
;of the target variables size
;using this as a confirmation that all went correct and
;a new loop to print out the final result, perfectly matching
;the input.

_main:
	var o nullptr
	var i 0
	var l 0

	;if the variable in question is not null then skip everything.
	mov eax [o]
	cmp eax -1
	jg fin2

	input
	pop eax
	mov [l] eax

lp:
	mov edx 32
	mov eax [i]
	mov ebx [l]
	cmp eax ebx
	
	mov ecx 0
	je fin

	pop eax
	mov [o++] eax  ;repurposing push to be more faithful to how x86asm works, appending to variable now works like this
	mov eax [i]
	add eax 1
	mov [i] eax
	jmp lp
fin:
	var osize ~[o]
lp2:
	mov edx [osize]

	cmp ecx edx
	je fin2

	mov eax [o+ecx]
	print (str)eax

	add ecx 1
	jmp lp2

fin2:
	print newline
	hlt
