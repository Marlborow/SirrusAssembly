# Simple Interpreted Pseudo Assembler Language
Bask in the glory of my terrible coding practices!
It has four registers `eax, ebx, ecx, edx`.
Functions include:
`mov,add,mul,sub,var,jmp,jl,jg,je,ret,hlt`

Currently increment and decrement does not work.

**Planned features:**
 - Support for variables to hold a wide range of data types, not limited to just integers.
 - Adding the capability to import external files to improve organization and leverage libraries effectively.
 - Resolve issues with increment and decrement.
 - JIT (joking)

**EXAMPLES**

Moving data around:
```asm
_main:                  ;start of program
	var temp 10 		;initialize temp with the value of 10
	mov  eax [temp]		;move the value of temp into eax register
	add  eax  10        ;add 10 to the eax register
	mov  ebx  2         ;move 2 to the ebx register
	mul  eax  ebx       ;multiply eax with ebx
	mov [temp] eax      ;move the value of eax back to the temp variable
	print eax           ;print the result within the eax register
	hlt                 ;end of program
```
output:
```
40
```

Jumping to specific lines:
```asm
variables:
	var test  10	;initialze variable test to the value of 10
	var i 0			;initialize a variable to be used as a loop counter
	ret				;return to the main function
  
_main:
	jmp variables  	;initialize variables
	mov  ebx [test]
	mov  eax [i]
lp:
	add  eax  1
	cmp  eax  ebx 	;compare eax with ebx
	print eax
	je fin			;jump if equals to the fin label
	jmp lp          ;else jump back to lp label and continue the loop
fin:
	hlt				;end of program
```
output:
```
1
2
3
4
5
6
7
8
9
10
```
