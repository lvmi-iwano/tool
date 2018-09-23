#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define TEST1
typedef struct mm{
    unsigned long v;
    char u;
}mm;
struct cred{
    unsigned long v;
    char u;
};

typedef union{
    struct {
        unsigned long p : 1,
                      ap : 2,
                      res : 9,
                      vfn : 52;
    };
    unsigned long en;
} entry;

int main(){
#ifdef TEST1
    mm m;
    m.v = 1;
    m.u = 0x3;
    mm *pmm = &m;
    int instr = 0xc0c00000;
    unsigned char temp = ~(0x1100 & 0xff);
    int a = -1, simm = -(temp + 1), simm2 = -0xff, addr;
    unsigned int b = 2, temp1 = 4095, imm = temp1 << 3;
    unsigned int data[2]  = {1,2};
    unsigned long dest, dest1 = 1;
    unsigned long value= 0x1020, status, mem, *pmem = &mem;
    //printf("size: %ld v:0x%lx u:0x%x\n", sizeof(struct mm), m.v, m.u);
    //m.u = 4;
    //printf("size: %ld v:0x%lx u:0x%x\n", sizeof(struct mm), m.v, m.u);
	char size_V_opc = ((instr >> 27) & 0x18) | ((instr >> 24) & 4) | ((instr >> 22) & 3);
    unsigned long e = 0x1;
    //entry en = {e};
    entry en, *pte = &en;
    unsigned long va = 0xffffffc0fb807250;
    struct cred creden = {1,2};
    const struct cred creden1 = {2,3};
    const struct cred *pcreden = &creden;
    //*pcreden = creden1;
    b += a;
    pte->vfn = va >> 12;
    addr = 0xf00 + simm2;
    memcpy(&dest, (void *)data, 8);
    printf("pmm->v: %ld\n", pmm->v);
    pmm->v--;
    printf("short :%lu char: %lu int: %lu temp: %d simm: %d simm2 :%d temp1: %u imm: %u addr: 0x%x b: %u\n", sizeof(short), sizeof(char), sizeof(int), temp, simm, simm2, temp1, imm, addr, b);
    printf("size:0x%x v:0x%x opc:0x%x size_V_opc:0x%x dest: 0x%lx va:0x%lx\n", (instr >> 27) & 0x18, (instr >> 24) & 4, (instr >> 22) & 3, size_V_opc, dest, va);
    printf("va: 0x%lx en:0x%lx p:0x%hhx res:0x%x vfn:0x%lx (u64)vfn: 0x%lx (u64)(vfn)<<12: 0x%lx size: %lu\n", va, pte->en, pte->p, pte->res, pte->vfn, (unsigned long)(pte->vfn), (unsigned long)(pte->vfn) << 12, sizeof(entry));
    printf("pmm->v: %ld\n", pmm->v);
    memset((void*)(&e), 0, 8);
    printf("e: %ld\n", e);
      /*asm volatile("mov x0, %1\n\t"
                 "mov x1, %2\n\t"
                 "stxrb w2, w0, [x1]\n\t"
                 "mov %0, x2\n\t"
                :"=r" (status):"r" (value), "r" (pmem));
    printf("value: 0x%lx in mem: 0x%lx status: %ld\n", value, mem, status);*/
    unsigned long *ptr = &dest1;
    (*ptr)++;
    printf("%lu %lx\n", dest1, 0xfffffffff >> 18);
    //pcreden->v = 2;
    //pcreden->u = 2;
    unsigned long val = 0xffffffffffff3, shift = ~(1<<1), va_ = 0xbbbbbbbbb000;
    printf("val 0x%lx pmm->v 0x%u\n", val & ~(1<<1), pmm->v);
    pmm->v = pmm->v | 1;
    en.ap = 0x5;
    printf("val 0x%lx pmm->v 0x%u 0x%lx ap 0x%x\n", val & ~(1<<1), pmm->v, (va_ >> 39) &0x1ff, en.ap);
#endif
    int a = 510;
    unsigned long b = ((a & 0x1ff) << 30) & 0x7fffffffff; 
    struct cred en[10] = {0,0};
    char* array = malloc(10);
    memset(array,1,10);
    mm m;
    m.u = 0x3;
    m.u += -1;
    printf("b 0x%lx %x\n", b, m.u);
    for (int i = 0; i < 10; i++) {
        printf("en[%d] %lu %hhu array[%d]: %hhd\n", i, en[i].v,en[i].u, i, array[i]);
    }
    do {
    printf("ok\n");
    //break;
    printf("ok1 0x%x\n", 0x1000 >>30);
    }while(0);
    entry old;
    int ap=1;
    old.ap = 1;
    int idx = 10;
    int arr[10];
    int b_ = 0;
    int *p = &b_;
    *p |= 3;
    for (int i = 0; i < 10; i++) {
        arr[--idx] = i;
        printf("array[%d] %d\n", idx, arr[idx]);
    }
    (*p)++;
    if (old.ap == 1)
        printf("*p %d\n", b_);
    return 0;
}
