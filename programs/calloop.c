const unsigned long N = 1000 * 1000 * 1000;

 unsigned long long counter = 0;

// Don't inline the benchmarking code into main
//void __attribute__((noinline)) tightloop();
//void __attribute__((noinline)) loop_with_extra_call();

volatile unsigned dummy = 0;
volatile unsigned dummy1 = 0;

void foo(){}

void callloop() {
  for (register unsigned j = 0; j < N; ++j) {
	__asm volatile("# LLVM-MCA-BEGIN");
	dummy=j;
	dummy1=j;
	counter += j;
  }
	__asm volatile("# LLVM-MCA-END");
}

int main(){
	callloop();
}
