#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main () {

    // 0x0 is hex literal that defaults to signed integer
    // here we are casting it to a void pointer
    // and then assigning it to a value declared to be a void pointer
    // this is the correct way to create an arbitrary pointer in C
    long loc;
    char c1[30]="cat /proc/";
    char c2[]="/maps",pid[6];
    sprintf(pid,"%d",getpid());
    strcat (c1,pid);
    strcat (c1,c2);
    system(c1);
    printf("%p",&loc);
    printf("enter mem location:");
    scanf("%lx",&loc);
    printf("\nint loc(hex):%lx\n",loc);
    char * addr = (void *) loc;
    int i = 0;
    char ch;
    while(1){
	    printf("%c",addr[i]);
//	    ch=addr[i];
	    i++;
//	    printf("%d\n",i);
    }

}
