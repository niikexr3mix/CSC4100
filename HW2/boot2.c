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

//new global variables
idt_entry_t idt[256]; //array containing 256 entries for the descriptor table
char* myString = "Got a key: ";
char* myString2 = "          ";
char* charBuffer;

int head = 0;
int tail = 0;


//old global variables
int row = 0;
int col = 0;
int bufferMax = 0;

//new prototypes
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

//old prototypes
void k_scroll();
void k_print(char *string, int string_length, int row, int col);
void k_clearscr();
void println(char *string);

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

    initIDT();
    setupPIC();
    enableSTI();

    char charToPrint;
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
    
    return 0;
}


//new functions*********************************************************************************************
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