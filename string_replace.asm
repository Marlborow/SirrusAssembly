;This is a very messy example of basic string manipulation
;it swaps the word "oranges" for "apples!" 

variables:
	var array (str)"how_do_you_like_them_oranges"
	var swap (str)"apples!"
	var hackyspaces (str)"_"  
	var array_len ~[array]
	var swap_len ~[swap]
	var swap_index 0
	var i 0
	ret

_main:
	jmp variables 

	mov eax [array_len]
	sub eax [swap_len]
	sub eax 1
	mov [swap_index] eax

	mov ebx 0
lp:
	mov ecx [array_len] 	
	cmp ebx ecx 	
	je fin			   
	mov ecx [swap_index]
	cmp ebx ecx
	jg swap
	jmp noswap
swap:
	mov edx [i]
	mov eax [swap+edx]
	add edx 1
	mov [i+0] edx         ;weird bug, needs to reference the index of the variable.
	mov edx [hackyspaces]
	jmp endswap
noswap:
	mov eax [array+ebx] 
	mov edx [hackyspaces]
endswap:
	cmp eax edx        
	je space   
	print (str)eax    
	jmp after_space
space:
	mov eax 32            
	print (str)eax     
after_space:
	add ebx 1        
	jmp lp       
fin:
	print newline   
	hlt             
