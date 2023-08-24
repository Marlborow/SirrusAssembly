;Author: Wesley Marlborow
;Date:   24/08/2023
;title:  Populating a NULL Variable

;INFOMATION:
;when input is called it stores the value you enter into a temporary stack like this:

;example 'test'
; [length] 4
; [character/number] t
; [character/number] e
; [character/number] s
; [character/number] t

; you get the values from this by poping the stack
; into a register 'pop [register]'

; utilizing the length gotten a loop is made and
; that value gets pushed into the variable with
; 'push [variable] [register]'

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

	mov eax [i]
	mov ebx [l]
	cmp eax ebx
	
	mov ecx 0
	je fin

	pop eax
	push [o] eax
	mov eax [i]
	add eax 1
	mov [i] eax
	jmp lp
fin:

	var osize ~[o]
	mov edx [osize]

	cmp ecx edx
	je fin2

	mov eax [o+ecx]
	print (str)eax

	add ecx 1
	jmp fin


fin2:
	print newline
	hlt

