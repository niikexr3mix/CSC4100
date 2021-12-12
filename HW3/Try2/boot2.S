/*Need this*/
.intel_syntax noprefix

/*Project 1*/
.global k_print
.global k_scroll
.global kbd_enter
.global _kbd_skip

/*Project 2*/
.global outportb
.global enableSTI
.global lidtr
.extern myString
.extern kbd_handler
.extern myString2
.global test
.extern println
got: .asciz "suffering"

/*Project 3*/
.global go
.global dispatch
.global init_timer_dev
.extern enqueue
.extern dequeue
.extern currPCB

go:
    /*dequeue pcb from queue and make it the current*/
    call dequeue

    /*set esp register to esp value in the current processes pcb*/
    mov [currPCB], eax
    mov esp, [eax]
    //mov ss, [eax+4]

    /*restore (pop) gs fs es ds*/
    pop gs
    pop fs
    pop es
    pop ds

    /*restore all the general purpose registers (popad)*/
    popad

    /*(return from interrupt. Why does this work?)*/
    iret

dispatch:
    /* note that at this point, eflags, cs, and ip are on the stack */

    /*save all the general purpose registers (pushad)*/
    pushad

    /*save ds, es, fs, and gs*/
    push ds
    push es
    push fs
    push gs

    /*save esp register in the current processes pcb*/
    mov eax, [currPCB]
    mov [eax], esp
    push eax

    /*enqueue pcb for the current process*/
    call enqueue

    /*dequeue pcb for the next process and make it current*/
    call dequeue

    /*set the esp register to esp value in the new current processe pcb*/
    mov [currPCB], eax
    mov esp, [eax]
    mov ss, [eax+4]

    /*restore gs, fs, es, ds*/
    pop gs
    pop fs
    pop es
    pop ds

    /*restore the general purpose registers*/
    popad

    push eax

    // send EOI to PIC
    mov al, 0x20
    out 0x20, al

    pop eax
    iret

init_timer_dev:
    /*
    1) Do the normal preamble for assembly functions (set up ebp and save any registers that will be used). The first arg is time in ms

    2) move the ms argument value into a register (say, edx)
    
    3) Multiply dx (only the bottom 16 bits will be used) by 1193. 
    Why? because the timer cycles at 1193180 times a second, which is 1193 times a ms 
    note: The results must fit in 16 bits, so ms can't be more than 54.
    So, put your code for steps 1 - 3 HERE:*/

    push ebp
    mov ebp, esp
    pushad
    pushf
    mov edx, [ebp+8]
    imul dx, 1193

    /*The code for steps 4-6 is given to you for free below...
    4) Send the command word to the PIT (Programmable Interval Timer) that initializes Counter 0
    (there are three counters, but you will only use counter 0).
    The PIT will be initalized to mode 3, Read or Load LSB first then MSB, and
    Channel (counter) 0 with the following bits: 0b00110110 =
    Counter 0 |Read then load|Mode 3|Binary. So, the instructions will be:*/
    mov al, 0b00110110 /* 0x43 is the Write control word */
    out 0x43, al
    /* 5) Load the LSB first then the MSB.
    0x40 = Load counter 0 with the following code: */
    mov ax, dx
    out 0x40, al //LSB
    xchg ah, al
    out 0x40, al //MSB
    /* 6) clean up (pop ebp and other regs used) and return */

    popf
    popad
    pop ebp
    iret

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