#include <stdio.h>
#include <stdint.h>
#include <assert.h>

void func1();
void DoCheck(uint32_t dwSomeValue);
void do_print(uint32_t dwSomeValue);
void func4();
void symbolic();

int main(int argc, char** argv) {
  //func1();
  //DoCheck(10000);
  //do_print(30);
  //func4();
  symbolic();
  return 0;
}

void DoCheck(uint32_t dwSomeValue) {
  uint32_t dwRes;

  // Assumes dwSomeValue is not zero.
  asm( "bsfl %1, %0"
       :"=r" (dwRes)
       :"r" (dwSomeValue)
       :"cc");
  //assert(dwRes > 3);
  printf("%d\n", dwRes);
}

void func1() {
  int src = 1;
  int dst;
  asm("mov %1, %0\n\t"
      "add $1, %0"
      : "=r" (dst)
      : "r" (src));
  printf("%d\n", dst);
}

void do_print(uint32_t dwSomeValue) {
  uint32_t dwRes;
  for(uint32_t x = 0; x < 5; x++) {
    // Assumes dwSomeValue is not zero.
    asm ("bsfl %1, %0"
	 : "=r" (dwRes)
	 : "r" (dwSomeValue)
	 : "cc");
    printf("%u: %u %u\n", x, dwSomeValue, dwRes);
  }
}

void func4() {
  uint64_t msr;

  asm volatile ("rdtsc\n\t"                // Returns the time in EDX:EAX.
		"shl $32, %%rdx\n\t"       // Shift the upper bits left.
		"or %%rdx, %0"             // 'Or' in the lower bits.
		: "=a" (msr)
		:
		: "rdx");

  printf("msr: %llx\n", msr);

  // Do other work...

  // Reprint the timestamp
  asm volatile ("rdtsc\n\t"                // Returns the time in EDX:EAX.
		"shl $32, %%rdx\n\t"       // Shift the upper bits left.
		"or %%rdx, %0"             // 'Or' in the lower bits.
		: "=a" (msr)
		:
		: "rdx");

  printf("msr: %llx\n", msr);
}

void symbolic() {
  uint32_t c = 1;
  uint32_t d;
  uint32_t *e = &c;
  
  asm ("mov %[e], %[d]"
      :[d] "=m" (d)
      :[e] "m" (*e));
  
  printf("%d\n", d);
}
