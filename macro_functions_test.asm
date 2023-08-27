;Macro Functions test
;Macro's have a limit of 4 arguments.

;Author: Wesley Marlborow
;Date:   27/08/2023
;title:  Macro Functions test

;INFOMATION:

;you create a macro with '%macro [label] [argument size]'
;you reference the arguments with '%1, %2, %3 or %4'
;you end a macro with '%endmacro'

;Macro registers are like variables that hold the variable data
;of the arguments used to invoke the macro.

;There's no need to ret within the macro as once it reaches the line "%endmacro"
;ret is invoked.

;Example bellow: (Print function example)
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
