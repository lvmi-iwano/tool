#ifndef __FLASHLIAHT_LIST_H__
#define __FLASHLIAHT_LIST_H__

typedef struct {
	int (*fl_init)(struct platform_device *);
	int (*fl_enable)(int);
	int (*fl_disable)(void);
	// int (*fl_open)(void *);
	// int (*fl_close)(void *);
	const char * fl_name;
} flashlight_list_t;

#define FLASHLIGHT_TEST

#endif


