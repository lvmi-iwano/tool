
#include <linux/platform_device.h>
#include "flashlight_list.h"
#include <linux/delay.h>
#include <linux/hct_board_config.h>
#include <linux/kernel.h>
//#include <linux/gpio.h>
//#include <linux/gpio/consumer.h>
#define PK_DBG(fmt, arg...)    pr_debug(CONFIG_HCT_FLASHLIGHT"%s: "fmt, __func__ , ##arg)



static	int fl_init(struct platform_device *dev){


	return 0;
}


static	int fl_enable(int g_duty){
	if(g_duty){
	

	}else{
	

	}
	return 0;
}

static	int fl_disable(void){

return 0;
}


flashlight_list_t flashlight_list = {
	.fl_init = fl_init,
	.fl_enable = fl_enable,
	.fl_disable = fl_disable,
	.fl_name = CONFIG_HCT_FLASHLIGHT
};
