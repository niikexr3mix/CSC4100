//Bradley Harpr
//#include<stdio.h>
//#include<string.h>
//#include<stdint.h>

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

//struct for pcb
struct pcb
{
    unsigned int esp;
    unsigned int pid;
    //int pcb_next;
}__attribute__((packed));
typedef struct pcb pcb_t;

//struct for queue
//#define queueSize 6

struct queue
{
    int qHead;
    int qTail;
    pcb_t qPCBs[10]; // ????
}__attribute__((packed));
typedef struct queue queue_t;

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

//new global variables
int num_processes = 0;
unsigned int retVal = 0;
int stacks[5][1024];
int nextStack = 0;
pcb_t* currProcess;
int pcb_next = 0;
char* msg2 = "Process p2: ";
char* msg3 = "Process p3: ";
char* msg4 = "Process p4: ";
char* msg5 = "Process p5: ";
queue_t queues;

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

//new prototypes
void init_timer_dev(int x);
void go();
int create_process(unsigned int processEntry);
int* allocate_stack();
pcb_t* allocatePCB();
void p1();
void p2();
void p3();
void p4();
void p5();
void process_idle();
void dispatch();
void enqueue(pcb_t *qThis);
pcb_t* dequeue();

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
    //println("Running processes: ");
    char* first ="Running processes: ";
    int len = 0;

    //__asm__("cli");

    for(int dos = 0; first[dos]!='\0'; ++dos) 
    {
        len++;
    }

    k_print(first, len, 0, 0);

    initIDT();

    setupPIC();
    //enableSTI();

    //program the timer device
    init_timer_dev(50);

    //initialize RR queue data structures
    queues.qHead = 0;
    queues.qTail = 0;

    //set retval to create_process(process_idle)
    retVal = create_process((unsigned long) process_idle);

    //loop from 1 to num_processes, a global variable at the top
    /*for(int i = 1; i < num_processes; i++)
    {
        //set retval to the return value of create_process(process_fun)
        retVal = create_process(process_fun); // wtf is process fun
        //print error if retval is less than 0
        if(retVal < 0)
        {
            println("Error!");
        }
    }*/


    retVal = create_process((unsigned long) p1);
    //num_processes++;
    retVal = create_process((unsigned long) p2);
    //num_processes++;
    retVal = create_process((unsigned long) p3);
    //num_processes++;
    retVal = create_process((unsigned long) p4);
    //num_processes++;
    retVal = create_process((unsigned long) p5);
    //num_processes++;
    //end loop


    //enableSTI();

    //now begin running the first process
    go();

    //println("im here");

    while(1)
    {
        //go();
        //k_print("i hate this", 11, 13, 0);
    }

    return 0;

}

//new functions*********************************************************************************************
int create_process(unsigned int processEntry)
{
    //processEntry is the pointer to the function that contains the process code
    //begin
    //if no more pcbs are available or no stack can be allocated then return 1
    if(nextStack == 6)
    {
        return 1;
    }

    unsigned int *st = allocate_stack();

    //set cs to the process's code selector (which is 16, same as OS for now)
    int cs = 16;

    //set st (s pointer to unint32_t) to stackptr + SIZE_OF_STACK
    st = st + 1024;

    st--;
    *st = (unsigned long) go;

    st--;

    //set 32-bit word pointed at by st to 0x0200 (Eflags with interrupts disabled)
    //subtract 1 from st
    *st = 0x0200;
    st--;

    *st = cs;
    st--;

    //first process
    *st = processEntry;

    st--;
    *st = 0; // ebp
    st--;
    *st = 0; //esp
    st--;
    *st = 0; // edi
    st--;
    *st = 0; // esi
    st--;
    *st = 0; // edx
    st--;
    *st = 0; // ecx
    st--;
    *st = 0; // ebx
    st--;
    *st = 0; //eax

    st--;
    *st = 8; // ds
    st--;
    *st = 8; // es
    st--;
    *st = 8; // fs
    st--;
    *st = 8; // gs 

    pcb_t* myPCB = allocatePCB();
    myPCB->esp = (unsigned long)st;
    num_processes++;
    //get next pid
    myPCB->pid = num_processes;
    //myPCB->row = 0;
    //myPCB->col = 0;
    enqueue(myPCB);

    return 0;
}

pcb_t* allocatePCB()
{
    pcb_next++;
    return &(queues.qPCBs[pcb_next-1]);
}

int* allocate_stack()
{
    return stacks[nextStack++];
}


/*int* allocate_stack()
{
    if(numStacks > 6)
    {
        return -1;
    }

    int* allocatedStack = stackArray[numStacks];
    numStacks++;
    //allocatedStack = allocatedStack + 1024;

    return allocatedStack;
}

pcb_t* allocatePCB()
{
    if(numPCBs > 6)
    {
        return -1;
    }

    pcb_t* allocatedPCB = pcbArray[numPCBs];
    numPCBs++;
    return allocatedPCB;
}*/

void enqueue(pcb_t *qThis)
{
    queues.qPCBs[queues.qTail] = *qThis;
    queues.qTail = ++queues.qTail % 10;

    /*myQueue.qTail = (myQueue.qTail+1) % queueSize;
    myQueue.qPCBs[myQueue.qTail] = qThis;*/
}

pcb_t* dequeue()
{
    if(queues.qHead == -1)
    {
        return 0;
    }

    int head = queues.qHead;
    queues.qHead = ++queues.qHead % 10;

    return &(queues.qPCBs[head]);

    /*
    pcb_t* temp = (myQueue.qPCBs[head]);
    myQueue.qHead = (myQueue.qHead+1) % queueSize;
    //currPCB = (myQueue.qPCBs[temp]);
    return (temp);*/


    /*
    int temp = myQueue.qHead;
    myQueue.qHead = (myQueue.qHead+1) % queueSize;
    //currPCB = (myQueue.qPCBs[temp]);
    return (myQueue.qPCBs[temp]);*/
} 

void process_idle()
{
    while(1)
    {
        //println("im here");
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

void p2()
{
    char* msg2 = "Process p2: ";
    int x = 0;
    while(1)
    {
        //char* msg2 = "Process p2: ";
        int msg_len = 0;

        for(int i = 0; msg2[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        //println("Shits fucked");
        k_print(msg2, msg_len, 5, 0);
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

void p3()
{
    int x = 0;
    char* msg = "Process p3: ";
    while(1)
    {
        //char* msg = "Process p3: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 7, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 7, 13);
        x = ((x+1) % 500);
    }
}

void p4()
{
    int x = 0;
    char* msg = "Process p4: ";
    while(1)
    {
        //char* msg = "Process p4: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 9, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 9, 13);
        x = ((x+1) % 500);
    }
}

void p5()
{
    int x = 0;
    char* msg = "Process p5: ";
    while(1)
    {
        //char* msg = "Process p5: ";
        int msg_len = 0;

        for(int i = 0; msg[i]!='\0'; ++i) 
        {
            msg_len++;
        }

        k_print(msg, msg_len, 11, 0);
        msg_len = 0;

        char* msgI = "";
        convert_num(x, msgI);
        for(int i = 0; msgI[i]!='\0'; ++i) 
        {
            msg_len++;
        }
        k_print(msgI, msg_len, 11, 13);
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
            initIDTEntry(idt+i, dispatch, 16, 0x8e);
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

    //k_print("error", 5, 15, 15);

    //while(1);
    


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

    outportb(0x21, 0xfc);
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