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
Strings Example:
```asm
variables:
	var array (str)"Hello_World!"
	var array_len ~[array]
	ret

_main:
	jmp variables
	mov ebx 0
lp:
	mov ecx [array_len]
	cmp ebx ecx
	je fin
	mov eax [array+ebx]
	cmp eax edx
	print (str)eax
	add ebx 1
	jmp lp
fin:
	print newline
	hlt
```
output 
```
Hello_World!
```
Strings Example (with spaces):
```asm
variables:
	var array (str)"Hello_World!" ;store ascii string into array
	var array_len ~[array]        ;get the length of the array
	var hackyspaces (str)"_"      ;get a reference to the character that will be replaced for a space
	ret

_main:
	jmp variables 
	mov ebx 0
	mov edx [hackyspaces]
lp:
	mov ecx [array_len] 	
	cmp ebx ecx 		;compare the current iteration of the array with the array length
	je fin				;jump to fin label if it matches     
	mov eax [array+ebx] ;push the current array index value to eax
	cmp eax edx         ;compare the current value with the value of hackyspaces
	je space   
	print (str)eax      ;print ascii value of eax
	jmp after_space
space:
	mov eax 32            
	print (str)eax      ;print the ascii value of 32 (blank space character)
after_space:
	add ebx 1           ;increment the iteration counter
	jmp lp              ;loop once again
fin:
	print newline       ;print a new line once looped over the whole array
	hlt                 ;stop the program
```
output
```
Hello World!
```

