//Bradley Harpr
//#include<stdio.h>
//#include<string.h>
//#include<stdint.h>

#include <stdint.h>

#define NULL ((void *)0)

//Struct to define an entry in the interrupt descriptor table
struct idt_entry
{
    unsigned short baselow;
    unsigned short selector;
    unsigned char always0;
    unsigned char access;
    unsigned short base_hi16;
}__attribute__((packed));
typedef struct idt_entry idt_entry_t;

//struct for lidtr
struct gdt_r_s
{
    unsigned short limit;
    idt_entry_t* base;
}__attribute__((packed));
typedef struct gdt_r_s gdt_r_t;

struct PCB_s {
  uint32_t esp;  // stack ptr
  int pid;       // process id
  struct PCB_s* next;
};
typedef struct PCB_s PCB_t;

//old global variables
idt_entry_t idt[256]; //array containing 256 entries for the descriptor table
char* myString = "Got a key: ";
char* myString2 = "          ";
char* charBuffer;
int head = 0;
int tail = 0;
int row = 0;
int col = 0;
int bufferMax = 0;

//old prototypes
void initIDTEntry(idt_entry_t *entry, void* base, unsigned short selector, unsigned char access);
void kbd_handler(unsigned int scancode);
void kbd_enter();
char translate_scancode(int what);
char k_getchar();
void default_handler();
void setupPIC();
void initIDT();
void enableSTI();
void lidtr(gdt_r_t* idtr);
void outportb(unsigned short t, unsigned char x);
void print(char *string);
void k_scroll();
void k_print(char *string, int string_length, int row, int col);
void k_clearscr();
void println(char *string);

//New ptypes
void init_timer_device(uint32_t);
int create_process(uint32_t processEntry);
PCB_t* allocate_PCB();
uint32_t* allocate_stack();
void pop_current();
void enqueue_current();
uint32_t get_esp_current();
void save_esp_current(uint32_t);
int create_process(uint32_t);
void p1();
void p2();
void p3();
void p4();
void p5();
void process_idle();
void go();
void dispatch();
void init_timer_dev(uint32_t);
void q_enqueue(PCB_t *);
PCB_t *q_pop();
int q_isEmpty();

//new globals
int numProcesses = 5;
void* processList[] = {p1, p2, p3, p4, p5};

uint32_t numStacks[6][1024];
pcb_t PCBs[6];

int s_allocated = 0;
pcb_t *currPCB;

static PCB_t *head;
int length = 0;

PCB_t *allocate_PCB() { return &PCBs[s_allocated]; };
uint32_t *allocate_stack() { return numStacks[s_allocated]; }

void pop_current() { currPCB = q_pop(); }
void enqueue_current() { q_enqueue(currPCB); }

uint32_t get_esp_current() { return currPCB->esp; }
void save_esp_current(uint32_t esp) { currPCB->esp = esp; }

//given converters
int convert_num_h(unsigned int num, char buf[])
{
    if(num == 0)
    {
        return 0;
    }

    int idx = convert_num_h(num / 10, buf);
    buf[idx] = num % 10 + '0';
    buf[idx+1] = '\0';
    return idx + 1;
}

void convert_num(unsigned int num, char buf[])
{
    if(num == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';

    }
    else 
    {
        convert_num_h(num, buf);
    }
}

//given
enum CSET_1 {
  Q_PRESSED = 0x10, W_PRESSED = 0x11, E_PRESSED = 0x12, R_PRESSED = 0x13,
  T_PRESSED = 0x14, Y_PRESSED = 0x15, U_PRESSED = 0x16, I_PRESSED = 0x17,
  O_PRESSED = 0x18, P_PRESSED = 0x19
};
static char* cset_1_chars = "qwertyuiop";

enum CSET_2 {
  A_PRESSED = 0x1E, S_PRESSED = 0x1F, D_PRESSED = 0x20, F_PRESSED = 0x21,
  G_PRESSED = 0x22, H_PRESSED = 0x23, J_PRESSED = 0x24, K_PRESSED = 0x25,
  L_PRESSED = 0x26
};
static char *cset_2_chars = "asdfghjkl";

enum CSET_3 {
  Z_PRESSED = 0x2C, X_PRESSED = 0x2D, C_PRESSED = 0x2E, V_PRESSED = 0x2F,
  B_PRESSED = 0x30, N_PRESSED = 0x31, M_PRESSED = 0x32,
};
static char *cset_3_chars = "zxcvbnm";

enum CSET_NUMBERS {
  ONE_PRESSED = 0x2, TWO_PRESSED = 0x3, THREE_PRESSED = 0x4,
  FOUR_PRESSED = 0x5, FIVE_PRESSED = 0x6, SIX_PRESSED = 0x7,
  SEVEN_PRESSED = 0x8, EIGHT_PRESSED = 0x9, NINE_PRESSED = 0xA
};
static char *cset_4_nums = "123456789";

#define CSET_ZERO 0x0B

#define CSET_NL 0x1C
#define CSET_SPC 0x39
#define CSET_RET 0xE
#define CSET_POINT_PRESSED 0x34
#define CSET_SLASH_PRESSED 0x35

int main()
{
	k_clearscr();

    k_print("Running processes: ", 19, 0, 0);

    __asm__("cli");

    initIDT();
    setupPIC();
    //enableSTI();

    init_timer_device(50);

    int retVal = create_process((uint32_t)process_idle);
    numProcesses++;

    create_process(p1);
    numProcesses++;
    create_process(p2);
    numProcesses++;
    create_process(p3);
    numProcesses++;
    create_process(p4);
    numProcesses++;
    create_process(p5);
    numProcesses++;


    go();

    while(1)
    {

    }


    /*char charToPrint;
    charBuffer = " ";
    
    
    while(1)
    {
        charBuffer[0] = k_getchar();

        if(charBuffer[0] == '\n')
        {
            row++;
            col = 0;
            continue;
        }
        if(charBuffer[0] == 0)
        {
            continue;
        }

        print(charBuffer);

        if(col == 80)
        {
            row++;
            col = 0;
        }
        
        if(row == 25)
        {
            k_scroll();
            row = 0;
            col = 0;
        }
    }
    
    return 0;*/
}


//new functions*********************************************************************************************
void q_enqueue(PCB_t *pcb) {
  if (length == 0) {
    head = pcb;
  } else {
    int i = length;
    PCB_t *temp = head;
    while (--i != 0) temp = temp->next;
    temp->next = pcb;
  }
  length++;
}

PCB_t *q_pop() {
  PCB_t *val = head;
  head = head->next;
  length--;
  return val;
}

int q_isEmpty() {
  if (length == 0)
    return 1;
  else
    return 0;
}

int create_process(uint32_t processEntry)
{
    if (s_allocated >= MAX_P) {
    return 1;  // error
  }

  // set stackptr to return value from allocate_stack()
  uint32_t *stackptr = allocate_stack();

  // set st (a pointer to unit32_t) to stackptr + SIZE_OF_STACK
  uint32_t *st = stackptr + SIZE_OF_STACK;

  // STACK SETUP
  // go
  st--;
  *st = (uint32_t)go;
  // 0x200
  st--;
  *st = 0x200;
  // CS
  st--;
  *st = 16;
  // Address of process
  st--;
  *st = processEntry;
  // EBP
  st--;
  *st = 0;
  // ESP
  st--;
  *st = 0;
  // EDI
  st--;
  *st = 0;
  // ESI
  st--;
  *st = 0;
  // EDX
  st--;
  *st = 0;
  // ECX
  st--;
  *st = 0;
  // EBX
  st--;
  *st = 0;
  // EAX
  st--;
  *st = 0;
  // DS
  st--;
  *st = 8;
  // ES
  st--;
  *st = 8;
  // FS
  st--;
  *st = 8;
  // GS
  st--;
  *st = 8;

  PCB_t *pcb = allocate_PCB();
  pcb->esp = (uint32_t)st;
  pcb->pid = s_allocated;
  pcb->next = NULL;

  s_allocated++;

  q_enqueue(pcb);

  return 0;  // no errors
}

void process_idle()
{
    while(1)
    {

    }
}



void p1()
{
    int x = 0;
    while(1)
    {
        char* msg = "Process p1: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 2, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 2, 13);
        x = ((x+1) % 500);
    }
}

void p2()
{
    int x = 0;
    while(1)
    {
        char* msg = "Process p2: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 3, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 3, 13);
        x = ((x+1) % 500);
    }
}

void p3()
{
    int x = 0;
    while(1)
    {
        char* msg = "Process p3: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 4, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 4, 13);
        x = ((x+1) % 500);
    }
}

void p4()
{
    int x = 0;
    while(1)
    {
        char* msg = "Process p4: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 5, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 5, 13);
        x = ((x+1) % 500);
    }
}

void p5()
{
    int x = 0;
    while(1)
    {
        char* msg = "Process p5: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 6, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 6, 13);
        x = ((x+1) % 500);
    }
}

//old functions*********************************************************************************************
char k_getchar()
{


	if(bufferMax == 0) //kbd_buffer empty
	{
		return 0;
    }

    char returnVal = charBuffer[head];
    head++;
    head = head % 256;
    bufferMax--;

	//returnVal = kbd_buffer; // something like this

	return returnVal;
}

//for phase2/3
void kbd_handler(unsigned int scancode)
{
    char charToPrint;
    //println("Got a key: ");
	if(scancode == 0 || bufferMax == 256) // kbd_buffer full
	{
		return;
	}

	//translate the scan code into a char
    charToPrint = translate_scancode(scancode);
    charBuffer[tail] = charToPrint;
    tail++;
    tail = tail % 256;
    bufferMax++;

    return;
    
}

void initIDTEntry(idt_entry_t *entry, void* base, unsigned short selector, unsigned char access)
{
    entry->baselow = (unsigned int) base & 0x0000FFFF;
    entry->base_hi16 = ((unsigned int) base & 0xFFFF0000) >> 16;
    entry->selector = selector;
    entry->access = access;
    entry->always0 = 0;
}

void initIDT() // idk what to do here
{
    for(int i = 0; i < 256; i++)
    {
        if(i < 32)
        {
    	   // for entries 0-31 setting these entries to point to the default handler
           initIDTEntry(idt+i, default_handler, 0x10, 0x8e); 
        }
        else if(i == 32)
        {
            // for entry 32 setting it to point to 0
            initIDTEntry(idt+i, 0, 0, 0);
        }
        else if(i == 33)
        {
            // for entry 33 setting it to point to keyboard handler (kbd_enter())
            initIDTEntry(idt+i, kbd_enter, 16, 0x8e); 
        }
        else if(i > 33)
        {
            // for entries 34 to 255, setting these entries to point to 0
            initIDTEntry(idt+i, 0, 0, 0); 
        }
    }

    gdt_r_t idtr;
    idtr.limit = (sizeof(idt_entry_t) * 256) - 1;
    idtr.base = idt;
	lidtr(&idtr);
}

void default_handler()
{
    //println("Error");
    while(1)
    {

    }
}

void setupPIC()
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);

    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);

    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);

    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);

    outportb(0x21, 0xfd);
    outportb(0xA1, 0xff);
    
}

char translate_scancode(int what)
{
    if(what >= Q_PRESSED && what <= P_PRESSED)
    {
        return cset_1_chars[what - Q_PRESSED];
    }
    else if(what >= A_PRESSED && what <= L_PRESSED)
    {
        return cset_2_chars[what - A_PRESSED];
    }
    else if(what >= Z_PRESSED && what <= M_PRESSED)
    {
        return cset_3_chars[what - Z_PRESSED];
    }
    else if(what >= ONE_PRESSED && what <= NINE_PRESSED)
    {
        return cset_4_nums[what - ONE_PRESSED];
    }
    else if(what >= ONE_PRESSED && what <= NINE_PRESSED)
    {
        return cset_4_nums[what - ONE_PRESSED];
    }
    else if (what == CSET_ZERO)
        return '0';
    else if (what == CSET_NL || what == CSET_RET) 
        return '\n';
    else if (what == CSET_SPC) 
        return ' '; 
    else if (what == CSET_POINT_PRESSED) 
        return '.';
    else if (what == CSET_SLASH_PRESSED) 
        return '/'; 
    else
    {

    }
}

void print(char *string2print) 
{

    int string_len = 0;

    for(int i=0; string2print[i]!='\0'; ++i) {
        string_len++;

    }

    if(string_len == 0)
    {
        return;
    }

    while( string_len != 0 )
    {
        if(string_len < 80)
        {
            k_print(string2print, string_len, row, col);
            col++;
            string_len = 0;
        }
        else
        {
            k_print(string2print, string_len, row, col);
            col++;
            //string_len = string_len - 80;
            string_len = string_len - (81 - col);
        }

        //row = row+1;
        if(row > 24)
        {
            k_scroll();
            row = 24;
        }


        /*if(col + string_len < 80)
        {
            k_print( string2print, string_len, row, col );
            col = col + string_len;
            string_len = 0;
        }
        else
        {
            //println("we are here");
            string_len = string_len - (81 - col);
            col = 0;
            row++;
            k_print(string2print, string_len, row, col);
            if ( row > 24 ) 
            {
                k_scroll(); 
                row = 24;
            }
        }*/
        

    }

    return; 

}


//old functions**********************************************************************************************************************
void println(char *s)
{

    int strlength = 0;
    for(int i = 0; s[i]!='\0'; ++i)
    {
        strlength++;

    } 

    if (strlength == 0)
    {
        return;
    }

    while(strlength != 0)
    {
        if(strlength < 80)
        {
            k_print(s, strlength, row, 0);
            strlength = 0;
        }
        else
        {
            k_print(s, strlength, row, 0);
            strlength = strlength - 80;
        }

        row = row+1;
        if(row > 24)
        {
            k_scroll();
            row = 24;
        }
    }
    return;
}


//clear the screen
void k_clearscr()
{
    for(int i = 0; i < 25; i++)
    {
        println("                                                                                ");
    }
    row = 0;
}