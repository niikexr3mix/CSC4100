#include<stdio.h>

/*void k_clearscr();
void println(char *string);
void k_print(char *string, int string_length, int row, int col);
void k_scroll();
*/

int main()
{
	int i,j,n = 0;

	//k_clearscr();

	printf("Prime numbers are:-\n");    
    for(i=3;n<20;i++)
    {
        int c=0;
        for(j=1;j<=i;j++)
        {
            if(i%j==0)
            {
                c++;
            }
        }
         
       if(c==2)
        {
        	printf("\n%d ",n+1);
        	printf(": ");
            printf("%d ",i);
            n++;
        }
    }
}