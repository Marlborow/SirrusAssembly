;Macro Function Reference Test

;Author: Wesley Marlborow
;Date:   27/08/2023
;title:  Macro Function Reference Test

;INFOMATION
;This is a working test of macro's using references to variables
;rather then copying its values.

;to reference a variable instead of copying when using macro's
;simply invoke the macro and when passing arguments use the '&' symbol to 
;specify that it's a reference.

;example '[macro] [&argument,&argument]'

;this tells the code to copy the label to the macro registers
;instead of the value of the variable passed as an argument.

;Working Example below:
%macro increment 1
	mov eax [%1]
	mov ebx 1
	add eax ebx
	mov [%1] eax
%endmacro


_main:
	var test 0
	increment [&test]
	mov eax [test]
	print eax
	newline
	hlt