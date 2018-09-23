#include <android/log.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>


#define LOG_TAG "TrustKernelUserTest"

#define LOG_D(...) {printf(__VA_ARGS__);__android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__);}
#define LOG_I(...) {printf(__VA_ARGS__);__android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__);}
#define LOG_E(...) {printf(__VA_ARGS__);__android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__);}
#define LOG(...)   {printf(__VA_ARGS__);__android_log_print(ANDROID_LOG_ERROR,LOG_TAG ,__VA_ARGS__);}


static void log_to_file(const char* prefix, char *buffer, uint32_t size)
{
	FILE *log_file;
	char* buf;

	buf = (char*)malloc(size + strlen(prefix));
	if(!buf)
		return;

	log_file = fopen("/sdcard/ifaa_data.dat", "a");

	if (log_file != NULL) {
		memcpy(buf, prefix, strlen(prefix));
		memcpy(buf + strlen(prefix), buffer, size);
		fwrite(buf, size, 1, log_file);
		fclose(log_file);
		log_file = NULL;
	}else {
		LOG_E("failed to open log file /sdcard/ifaa_data.dat\n");
		LOG_E("%s\n", strerror(errno));
	}

	free(buf);
}

static void hex_dump(const char* desc, void* addr, int len)
{
    uint8_t buff[17];
    uint8_t* pc = (uint8_t*)addr;
    int i = 0;

    if (desc != NULL)
        LOG_I("[" LOG_TAG "]%s: (len: 0x%04x)\n", desc, len);

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).
        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0) LOG_I("  %s\n", buff);
            // Output the offset.
            LOG_I("[" LOG_TAG "]  %04x ", i);
        }
        // Now the hex code for the specific character.
        LOG_I(" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        LOG_I("   ");
        i++;
    }

    // And print the final ASCII bit.
    LOG_I("  %s\n", buff);
}

char buf[2000];
char title[64];
unsigned long pbase;

int main(int argc, char** argv) {
	int fd;
	unsigned long vaddr;
	int offset = 0;
	fd = open("/dev/tk_mem", O_RDWR);
	if(fd == -1) {
		LOG_E("Failed to open /dev/tk_mem\n");
		return 1;
	}
	LOG_I("open /dev/tk_mem OK\n");
	// ATF_BOOT_ARG_ADDR, return 0 
	//pbase = 0x0011c000;
	// PRELOADER, stuck
	pbase = 0x00237000;
	// NON_SECURE_SRAM, return 0
	//pbase = 0x0012A000;
	// TEE
	//pbase = 0xBC000000;
	// CFG_BOOTA64_MEMADDR, failed
	//pbase = 0x40000000;
	*(unsigned long*)buf = pbase;

	vaddr = write(fd, buf, 0x9000);
	if(vaddr <=0) {
		LOG_E("Failed to remap addr");
		return 1;
	}
	LOG_I("write /dev/tk_mem OK, return vaddr: 0x%lx\n", vaddr);
	sleep(2);

	*(unsigned long*)buf = vaddr;
	if(read(fd, buf, 32) >=0) {
		LOG_I("read /dev/tk_mem OK\n");
		hex_dump("predump", buf, 32);
	}
	sleep(2);
	return 0;
	
	for(offset = 0; offset < 0x02000; offset += 2000) {
		*(unsigned long*)buf = vaddr + offset;
		if(read(fd, buf, 2000) >=0) {
			sprintf(title, "paddr dump from 0x%lx", pbase + offset);
			hex_dump(title, buf, 2000);
		}else {
			LOG_E("Failed to read addr data\n");
		}
	}
	close(fd);
	LOG_I("utest ends\n");
	return 0;
}
