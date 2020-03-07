/*
 * A program that will read and print printable characters in it's memory given a memory address
 * until it segfaults
 */
#define _GNU_SOURCE /* Bring REG_XXX names from /usr/include/sys/ucontext.h */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>

int i,err=0;
void sighandler(int signum);
void* label;

void readmem(){
	long loc;
	i=0;
	err=0;
	printf("enter mem location:");
	scanf("%lx",&loc);
	printf("--dump begin--\n");			
	char * addr = (void *) loc;
	char ch;
	label=&&l; 
/* 	was kept there so that I can find how many bytes to increment rip to recover fro segfault
	address can be found using gdb also
*/
   	while(1){
		ch=addr[i];
l:
		printf("%c",ch);
		i++;
		if ( err == 1)
			break;
//		printf("%d\n",i);
	}
}

static void sigaction_segv(int signal, siginfo_t *si, void *arg)
{
    ucontext_t *ctx = (ucontext_t *)arg;

	/* We are on linux x86, the returning IP is stored in RIP (64bit) or EIP (32bit).
	In this example, the length of the offending instruction is 6 bytes.
	So we skip the offender ! 
	
	&&l will return address pointed by label l
	(gdb) disass readmem
	...
	0x0000555555555284 <+139>:	mov    -0x10(%rbp),%rax
	0x0000555555555288 <+143>:	add    %rdx,%rax
     => 0x000055555555528b <+146>:	movzbl (%rax),%eax    		    -> rip on segfault
	0x000055555555528e <+149>:	mov    %al,-0x19(%rbp)
	0x0000555555555291 <+152>:	movsbl -0x19(%rbp),%eax
	0x0000555555555295 <+156>:	mov    %eax,%edi
	0x0000555555555297 <+158>:	callq  0x555555555030 <putchar@plt>
	...
	>>
	(gdb) p (void*) label
	$1 = (void *) 0x555555555291 <readmem+152>		->address of next instruction
	>>
	
	we need to go to <readmem+152> ie, next instruction
	so we add decimal 6 to rip in sighandler

	*/
    #if __WORDSIZE == 64
        printf("\nCaught SIGSEGV, addr %p, RIP 0x%lx\n",si->si_addr,ctx->uc_mcontext.gregs[REG_RIP]);
        ctx->uc_mcontext.gregs[REG_RIP] += 6;
    #else
        printf("Caught SIGSEGV, addr %p, EIP 0x%x\n",si->si_addr,ctx->uc_mcontext.gregs[REG_EIP]);
        ctx->uc_mcontext.gregs[REG_EIP] += 6;
    #endif 
	err=1;
	printf("no of bytes read:%d\n",i);
}


int main () {

	// 0x0 is hex literal that defaults to signed integer
	// here we are casting it to a void pointer
	// and then assigning it to a value declared to be a void pointer
	// this is the correct way to create an arbitrary pointer in C
	struct sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = sigaction_segv;
	sa.sa_flags = SA_SIGINFO;
 	if (sigaction(SIGSEGV, &sa, NULL) == -1) {
		fprintf(stderr, "failed to setup SIGSEGV handler\n");
    		exit(1);
	}
	char c[25];
	sprintf(c,"cat /proc/%d/maps",getpid());
	system(c);
	while(1){
		readmem();
	}
}
