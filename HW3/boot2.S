
.intel_syntax noprefix
.global k_print
.global k_scroll
.global go
.global dispatch
.global init_timer_dev
.extern pop_current
.extern enqueue_current
.extern get_esp_current
.extern save_esp_current
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

go:
  /* dequeue pcb from queue and make it the current */
  call pop_current

  /* set esp register to esp value in the current processes pcb */
  call get_esp_current
  mov esp, eax
  
  /* restore registers */
  pop gs
  pop fs
  pop es
  pop ds
  popad
  iret

dispatch:
  /* save registers */
  pushad
  push ds
  push es
  push fs
  push gs

  /* save esp register in the current processes pcb */
  push esp
  call save_esp_current
  add esp, 4

  /* enqueue pcb for the current process */
  call enqueue_current

  /* dequeue pcb for the next process and make it current */
  call pop_current

  /* set the esp register to esp value in the new current processes pcb */
  call get_esp_current
  mov esp, eax

  /* restore registers of new stack */
  pop gs
  pop fs
  pop es
  pop ds
  popad

  /* send EOI to PIC */
  push eax
  mov al, 0x20
  out 0x20, al
  pop eax

  iret

init_timer_dev:
  /* save prev state */
  push ebp
  mov ebp, esp
  pushad

  /* move the ms argument value into a register */
  mov edx, [ebp+8]

  /* multiply dx (only the bottom 16 bits will be used) by 1193. */
  imul dx, 1193

  /* send the command word to the PIT (Programmable Interval Timer) that initializes Counter 0 */
  mov al, 0b00110110 /* 0x43 is the Write control word */
  out 0x43, al

  /* Load the LSB first then the MSB. */
  /* 0x40 = Load counter 0 with the following code: */
  mov ax, dx
  out 0x40, al /* LSB */
  xchg ah, al
  out 0x40, al /* MSB */
  
  /* restore prev state */
  popad
  pop ebp
  ret

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