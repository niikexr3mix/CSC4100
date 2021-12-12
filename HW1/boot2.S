
.intel_syntax noprefix
.global k_print
.global k_scroll


//given
k_scroll:
    pushad
    pushf
    mov esi, 80* 2 + 0xb8000
    mov edi, 0xb8000
    mov ecx, 80 * 24 * 2
    rep movsb
    mov ecx, 80
    mov al, ' '
    mov ah, 32
    rep stosw
    popf
    popad
    ret


k_print:
    push ebp // make stack
    mov ebp, esp
    pushad // dr rogers made these so i will too
    pushf
    mov ecx, [ebp+12] // second var aka strlen
    mov eax, [ebp+16] // row
    mov edx, [ebp+20] // col
    imul eax, 80
    add eax, edx
    imul eax, 2
    add eax, 0xb8000 // given math
    mov edi, eax
    mov esi, [ebp+8] // char

//do a loopty loop and pull and your shoes are lookin cool    
loop:
    cmp ecx, 0 //compare length to zero
    je _done_printing   //if so we are done
    cmp edi, (0xb8000+ 2*(24*80+79)) // compare to end of screen
    jg _done_printing // if so done
    movsb 
    movb [edi], 32
    inc edi
    dec ecx
    cmp ecx, 0
    jg loop

_done_printing:
    popf
    popad
    pop ebp
    retS