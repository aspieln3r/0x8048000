/*
 * gcc calloop.c -o - -S | llvm-mca -timeline			# with 4 dummy write
 * 
                    0123456789
Index     0123456789d
[0,5]     D======eER.    .    .    .    .    .    .    .    .    ..   movq	%rax, counter(%rip)
[0,6]     .D======eER    .    .    .    .    .    .    .    .    ..   movl	%ebx, dummy(%rip)
[0,7]     .D=======eER   .    .    .    .    .    .    .    .    ..   movl	%ebx, dummy1(%rip)
[0,8]     .DeE-------R   .    .    .    .    .    .    .    .    ..   addl	$1, %ebx
[0,9]     .D=eE------R   .    .    .    .    .    .    .    .    ..   movl	%ebx, %eax
[0,10]    .DeE-------R   .    .    .    .    .    .    .    .    ..   movl	$1000000000, %edx
[0,11]    .D==eE-----R   .    .    .    .    .    .    .    .    ..   cmpq	%rdx, %rax
[0,12]    . D==eE----R   .    .    .    .    .    .    .    .    ..   jb	.L4
[1,0]     . D=======eER  .    .    .    .    .    .    .    .    ..   movl	%ebx, dummy(%rip)
[1,1]     . D========eER .    .    .    .    .    .    .    .    ..   movl	%ebx, dummy1(%rip)
[1,2]     . DeE--------R .    .    .    .    .    .    .    .    ..   movl	%ebx, %edx
[1,3]     . DeeeeeE----R .    .    .    .    .    .    .    .    ..   movq	counter(%rip), %rax

 * [0,5] - write to counter on first iteration
 * [1,3] - read from previously written counter
 * difference in no of clockcycles = 4 cycles
 * result - faster than tightloop
 *
---------------------------------------------------------------------------------------------------
 *
 * gcc tight.c -S -o - | llvm-mca -timeline
 *
                    0123456789
Index     0123456789
[0,2]     D======eER.    .   .   movq	%rax, counter(%rip)
[0,3]     DeE------R.    .   .   addq	$1, %rbx
[0,4]     DeE------R.    .   .   movl	$1000000000, %eax
[0,5]     D=eE-----R.    .   .   cmpq	%rax, %rbx
[0,6]     .D=eE----R.    .   .   jb	.L3
[1,0]     .DeeeeeE-R.    .   .   movq	counter(%rip), %rax

 * [0,2] - write to counter on first iteration
 * [1,0] - read from previously written counter
 * difference in no of clockcycles = 0 cycles
 * result - slow execution
 *
 *
 * ------------------------------------------------------------------------------------------------
 *
 * gcc calloop.c -o - -S | llvm-mca -timeline                 # with 2 dummy write
 *
                    0123456789          0123456
Index     0123456789          0123456789       

[0,5]     D======eER.    .    .    .    .    ..   movq	%rax, counter(%rip)
[0,6]     .DeE-----R.    .    .    .    .    ..   addl	$1, %ebx
[0,7]     .D=eE----R.    .    .    .    .    ..   movl	%ebx, %eax
[0,8]     .DeE-----R.    .    .    .    .    ..   movl	$1000000000, %edx
[0,9]     .D==eE---R.    .    .    .    .    ..   cmpq	%rdx, %rax
[0,10]    .D===eE--R.    .    .    .    .    ..   jb	.L4
[1,0]     .D======eER    .    .    .    .    ..   movl	%ebx, dummy(%rip)
[1,1]     . D======eER   .    .    .    .    ..   movl	%ebx, dummy1(%rip)
[1,2]     . DeE------R   .    .    .    .    ..   movl	%ebx, %edx
[1,3]     . DeeeeeE--R   .    .    .    .    ..   movq	counter(%rip), %rax

 * [0,5] - write to counter
 * [1,3] - read from previously written counter
 * difference in no of clockcycles = 2 cycles
 * result - fastest execution time
 * adding only one dummy read makes difference of one cycle and no performance gain is observed
 */
 const unsigned long N = 1000 * 1000 * 1000;

 unsigned long long counter = 0;

// Don't inline the benchmarking code into main
//void __attribute__((noinline)) tightloop();
//void __attribute__((noinline)) loop_with_extra_call();

//volatile unsigned dummy = 0;
//volatile unsigned dummy1 = 0;

void tightloop() {
  for (register unsigned long j = 0; j < N; ++j) {
	  __asm volatile("# LLVM-MCA-BEGIN");
	  counter += j;
  }
	  __asm volatile("# LLVM-MCA-END");
}

int main(){
	tightloop();
}
