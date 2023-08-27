;Macro Functions test
;Macro's have a limit of 4 arguments.

;To make it simple they're just like registers only
;they are arrays rather then only being able to hold
;an integer.

;Future optimizations required.

;DO NOT USE LABELS IN MACRO FUNCTIONS AS I'VE NOT CODED RETURNS
;TO WORK WELL WITHIN MACRO'S.
;	Instead specify the reletive line with +/- number
; 	example to jump 5 lines ahead "jmp +5"

%macro _print 2
	mov ebx 0
	mov edx [hackyspaces]
mf_print_lp:
	mov ecx [%2]
	cmp ebx ecx
	je mf_print_fin

	mov eax [%1+ebx]
	cmp eax edx
	je mf_print_space

	print (str)eax
	jmp mf_print_after_space

mf_print_space:
	mov eax 32
	print (str)eax
mf_print_after_space:
	add ebx 1
	jmp mf_print_lp

mf_print_fin:
	print newline
%endmacro


_main:
	var t (str)"this_is_a_test"
	var t_len ~[t]
	var hackyspaces (str)"_"

	_print [t,t_len]
	hlt
