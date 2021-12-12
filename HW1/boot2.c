#include<stdio.h>
#include<string.h>

//given protoypes
void k_scroll();
void k_print(char *string, int string_length, int row, int col);
void k_clearscr();
void println(char *string);

//made this to check for prime numbers
int primeChecker(unsigned int numToCheck);

//global variable to send to asm file
int row = 0;
//int col;

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

int main()
{

    k_clearscr(); //hope this clears the screen, pray to dr rogers himself

    //keep track of numbers printed
    int printedNums = 0;
    //array to hold the "string"
    char testArray[80];
    //start at 3 since we know 2 is prime.
    int starterPokemon = 3;

    //we know 2 is prime
    convert_num(2, testArray);
    println(testArray);

    //go through other 29 primes
    while(printedNums < 29)
    {
        //print if its prime
        if(primeChecker(starterPokemon))
        {
            printedNums++;

            convert_num(starterPokemon, testArray);
            println(testArray);

            starterPokemon++;
        }
        else
        {
            //increase the num if it's not
            starterPokemon++;
        }
    }

    while(1)
    {
        //its a never ending storrryyyyyyyyyyyyyyyyyy
    }

    return 0;
}


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

//check if prime
int primeChecker(unsigned int numToCheck)
{
    int x = 1;
    for(int i = 2; i <= numToCheck/2; i++)
    {
        if(numToCheck%i == 0)
        {
            x = 0;
            break;
        }
    }

    return x;
}