
variables:
    var test 10
    ret

_main:

    jmp variables
    mov eax 1
    mov ecx 404
    mov ebx [test]

    print ebx
lp:
    ;;this is a test
    add eax 1 ;test
    print eax ;test test test
    cmp eax ebx
    je fin
    jmp lp
;test mov ebx eax;
fin:    
    print ecx
    hlt
