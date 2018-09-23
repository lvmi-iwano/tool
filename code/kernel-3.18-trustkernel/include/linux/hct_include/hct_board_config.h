#ifndef __HCT_BOARD_CONFIG_H__
#define __HCT_BOARD_CONFIG_H__


#undef HCT_YES
#define HCT_YES 1
#undef HCT_NO
#define HCT_NO 0


#define __HCT_SD_CARD_HOTPLUG_SUPPORT__  	HCT_YES

#define __HCT_SIM_HOTPLUG_SUPPORT__  		HCT_YES


/*audio releated */
/* phone mic mode*/
#define __HCT_PHONE_MIC_MODE__   2
 
 /*phone use exp audio pa*/
#define __HCT_USING_EXTAMP_HP__  HCT_YES

#if  __HCT_USING_EXTAMP_HP__  
   #define __HCT_EXTAMP_HP_MODE__    3
#endif
#define __HCT_EXTAMP_GPIO_NUM__	100

/****POWER RELEATED****/
#define __HCT_CAR_TUNE_VALUE__   88
#define HCT_5G_WIFI_SUPPORT HCT_NO

#define __HCT_R_SENSE_VALUE__    33
/*############### USB OTG releated config--START #####################*/

#define __HCT_USB_MTK_OTG_SUPPORT__  HCT_YES
#define __HCT_GTP_VIO28_SUPPORT__  HCT_YES

/*############### USB OTG releated config--END #####################*/

/**************************FlashLight Related*********/
#define  __HCT_SUB_FLASHLIGHT_PWM_F_DUTY__      10        
#define  __HCT_SUB_FLASHLIGHT_PWM_T_DUTY__      10

#define  __HCT_GPIO_FLASHLIGHT_EN_NUM__         8
#define  __HCT_GPIO_FLASHLIGHT_MODE_NUM__       9
#define  __HCT_GPIO_FLASHLIGHT_SUB_EN_NUM__     12
#endif
