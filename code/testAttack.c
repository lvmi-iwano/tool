#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define ROUNDUP(x, align) \
	({ \
		unsigned long _x = (x); \
		unsigned long _align = (align); \
		(_x + _align - 1) / _align * _align; \
	})
#define PAGE_SIZE 4096

int main(int argc, char **argv) {
    int ret, fd;
    char cmd = *(argv[1]);
    char arg = *(argv[2]);
    unsigned long addr;
    volatile unsigned int * ptr = NULL;

    fd = open("/dev/attack", O_RDWR);
    if (fd < 0) {
        perror("fail to open device\n");
        return -1;
    }
    ret = write(fd, &cmd, sizeof(cmd));
    if (ret < 0) {
        printf("fail to write device\n");
        return -1;
    } else
        printf("Attack %c %c begin...\n", cmd, arg);

    /*ret = read(fd, &addr, sizeof(addr));
    if (ret < 0) {
        printf("fail to read device\n");
        return -1;
    }*/
    if (cmd == '4' || cmd == '5' || cmd == '6') {
        ptr = (volatile unsigned int *)malloc(PAGE_SIZE * 2);
        memset(ptr,1,PAGE_SIZE * 2);
        printf("alloc ptr %p\n", ptr);
        addr = (unsigned long) ROUNDUP((unsigned long)ptr, PAGE_SIZE);//we should make addr aligned to ensure no other va can be used to access the page
        ptr = (volatile unsigned int *)addr;
        printf("ptr %p addr 0x%lx value %x\n", ptr, addr, *ptr);
        ret = write(fd, &addr, sizeof(addr));//pass the addr to device, kernel module will map the va to kernel memory
        if (ret < 0) {
            printf("fail to write device\n");
            return -1;
        }
    }
    switch(cmd) {
      case '1'://let kernel module modify the permission for text section and modify the code
      case '2'://let kernel module modify the permission for rodata section and modify rodata
      case '3'://let kernel module execute on data section
        break; 
      case '4': {//read and write execute kernel code
        if (arg == 'r') {//read
            printf("try to read code:\n");
            printf("read value 0x%x successfully\n", *ptr);
        } else if (arg == 'w') {
            printf("try to write value 0xffffffff to code\n");
            *(ptr) = 0xffffffff;
            if (*ptr == 0xffffffff)
                printf("write successfully value %u\n", *ptr);
            else
                printf("fail to wirte\n");
        } else if (arg == 'x') {
            printf("try to execute code\n");
            asm volatile ("blr %x0"::"r"(addr):);
            printf("execute successfully\n");
        }
        break; }
      case '5': {// r/w kernel ro data
        if (arg == 'r') {
            printf("try to read rodata:\n");
            printf("read value 0x%x successfully\n", *ptr);
        } else if (arg == 'w') {
            printf("try to write value 0xffffffff to rodata\n");
            *(ptr) = 0xffffffff;
            if (*ptr == 0xffffffff)
                printf("write successfully value %u\n", *ptr);
            else
                printf("fail to wirte\n");
        } 
        break; }
      case '6': {//r/w/x kernel data/bss
        if (arg == 'r') {
            printf("try to read data:\n");
            printf("read value 0x%x successfully\n", *ptr);
        } else if (arg == 'w') {
            printf("try to write value 0xffffffff to data\n");
            *(ptr) = 0xffffffff;
            if (*ptr == 0xffffffff)
                printf("write successfully value %u\n", *ptr);
            else
                printf("fail to wirte\n");
        } else if (arg == 'x') {
            printf("try to execute code\n");
            asm volatile ("blr %x0"::"r"(addr):);
            printf("execute successfully\n");
        }
        //printf("recieve addr 0x%lx size %lu from device\n", addr, sizeof(addr));
        break; }
      //default:
      //  printf("unkown cmd %c\n",cmd);
    }
    return 0;
}
