
.intel_syntax noprefix
.global k_print
.global k_scroll
.global kbd_enter
.global _kbd_skip
.global outportb
.global enableSTI
.global lidtr
.extern myString
.extern kbd_handler
.extern myString2
.global test
.extern println
got: .asciz "suffering"

kbd_enter:
	pushf
    pusha
    cli

    mov eax, 0

	in al, 0x64
	and al, 0x01
	jz _kbd_skip
	in al, 0x60

    /* mov ebx, eax
    push myString
    call println
    add esp, 4

    push myString2
    push ebx
    call convert_num
    pop ebx
    add esp, 4

    push myString2
    call println
    add esp, 4 */

	push eax
	call kbd_handler
    add esp, 4

_kbd_skip:
	mov al, 0x20
	out 0x20, al
	popa
	popf
	iret

test:
    push OFFSET got
    call println
    add esp, 4
    ret

outportb:
    push ebp
    mov ebp, esp
    push dx
    push ax
    mov dx, [ebp+8]
    mov al, [ebp+12]
    out dx, al
    pop ax
    pop dx
    pop ebp
    ret

enableSTI:
    sti
    ret

lidtr:
    push ebp
    mov ebp, esp
    pusha
    pushf
    mov eax, [ebp+8]
    lidt [eax]
    popf
    popa
    pop ebp
    ret

k_scroll:
    pushad
    pushf
    mov esi, 80* 2 + 0xb8000
    mov edi, 0xb8000
    mov ecx, 80 * 24 * 2
    rep movsb
    mov ecx, 80
    mov al, ' '
    mov ah, 31
    rep stosw
    popf
    popad
    ret

k_print:
    push ebp
    mov ebp, esp
    pushad
    pushf
    mov ecx, [ebp+12]
    mov eax, [ebp+16]
    mov edx, [ebp+20]
    imul eax, 80
    add eax, edx
    imul eax, 2
    add eax, 0xb8000
    mov edi, eax
    mov esi, [ebp+8]
    
loop:
    cmp ecx, 0
    je _done_printing
    cmp edi, (0xb8000+ 2*(24*80+79))
    jg _done_printing
    movsb 
    movb [edi], 31
    inc edi
    dec ecx
    cmp ecx, 0
    jg loop

_done_printing:
    popf
    popad
    pop ebp
    ret