#include <stdio.h>

typedef struct mm{
    unsigned long v;
    char u;
}mm;

#define STR_POST_XN1(name, xn, simm) \
    switch (xn) { \
    case 0: \
        asm volatile("str "#name", [x0], %0" \
            ::"r" (simm));  \      
    break; }

int main()\ {
    mm m;
    m.v = 0;
    m.u = 0x3;
    int instr = 0xc0c00000;
    unsigned char temp = ~(0x1100 & 0xff);
    int simm = -(temp + 1), simm2 = 0xff;
    unsigned int temp1 = 4095, imm = temp1 << 3;
    unsigned int xn = 0, sp;
    unsigned long value= 0x1020, status, mem, *pmem = &mem;
    //printf("size: %ld v:0x%lx u:0x%x\n", sizeof(struct mm), m.v, m.u);
    //m.u = 4;
    //printf("size: %ld v:0x%lx u:0x%x\n", sizeof(struct mm), m.v, m.u);
	char size_V_opc = ((instr >> 27) & 0x18) | ((instr >> 24) & 4) | ((instr >> 22) & 3);
    m.u += -1;
    printf("%x\n", m.u);
/*    asm volatile("str x0, [x1], #255");
    asm volatile("str x0, [x1], #-256");
    asm volatile("str x0, [x1,#255]!");
    asm volatile("str x0, [x1,#-256]!");
    asm volatile("str x0, [x1,#1024]");
    asm volatile("str x0, [x1,#8]");*/
    asm volatile("msr %x0, spsel":"=r"(sp));

    asm volatile("mov x0, %1\n\t"
                 "mov x1, %2\n\t"
                 "stxrb w2, w0, [x1]\n\t"
                 "mov %0, x2\n\t"
                :"=r" (status):"r" (value), "r" (pmem));
    printf("value: 0x%lx in mem: 0x%lx status: %ld sp %u\n", value, mem, status, sp);
    return 0;
}
