/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 *
 * Copyright (c) 2011 Allwinner Technology
 * Copyright (c) 2014 Michal Suchanek <hramrach@gmail.com>
 *
 * ChangeLog
 *
 * + Adapt allwinner legacy driver for u-boot
 */
#define  KEY_DEBUG
#define  KEY_DEBUG_LEVEL2

#ifdef KEY_DEBUG
#define	PRINTF(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF(fmt,args...)
#endif

#ifdef KEY_DEBUG_LEVEL2
#define	PRINTF2(fmt,args...)	printf (fmt ,##args)
#else
#define PRINTF2(fmt,args...)
#endif

#include <common.h>
#include <dt-bindings/input/input.h>
#include <asm/io.h>
#include <sun4i_keyboard.h>
#ifdef CONFIG_AXP209_POWER
#include <axp209.h>
#else
static inline int axp209_power_button(void) { return 0; }
#endif
#ifdef CONFIG_AXP152_POWER
#include <axp152.h>
#else
static inline int axp152_power_button(void) { return 0; }
#endif

#define	DEVNAME			"sun4i-kbd"

#define  KEY_MAX_CNT  		(13)

#define  KEY_BASSADDRESS	(0xf1c22800)
#define  LRADC_CTRL		(0x00)
#define  LRADC_INTC		(0x04)
#define  LRADC_INT_STA 		(0x08)
#define  LRADC_DATA0		(0x0c)
#define  LRADC_DATA1		(0x10)

#define  FIRST_CONCERT_DLY		(2<<24)
#define  CHAN				(0x3)
#define  ADC_CHAN_SELECT		(CHAN<<22)
#define  LRADC_KEY_MODE		(0)
#define  KEY_MODE_SELECT		(LRADC_KEY_MODE<<12)
#define  LEVELB_VOL			(0<<4)

#define  LRADC_HOLD_EN		(1<<6)

#define  LRADC_SAMPLE_32HZ		(3<<2)
#define  LRADC_SAMPLE_62HZ		(2<<2)
#define  LRADC_SAMPLE_125HZ		(1<<2)
#define  LRADC_SAMPLE_250HZ		(0<<2)


#define  LRADC_EN			(1<<0)

#define  LRADC_ADC1_UP_EN		(1<<12)
#define  LRADC_ADC1_DOWN_EN		(1<<9)
#define  LRADC_ADC1_DATA_EN		(1<<8)

#define  LRADC_ADC0_UP_EN		(1<<4)
#define  LRADC_ADC0_DOWN_EN		(1<<1)
#define  LRADC_ADC0_DATA_EN		(1<<0)

#define  LRADC_ADC1_UPPEND		(1<<12)
#define  LRADC_ADC1_DOWNPEND	(1<<9)
#define  LRADC_ADC1_DATAPEND		(1<<8)


#define  LRADC_ADC0_UPPEND 		(1<<4)
#define  LRADC_ADC0_DOWNPEND	(1<<1)
#define  LRADC_ADC0_DATAPEND		(1<<0)

#define EVB
#define ONE_CHANNEL
#define MODE_0V2
//#define MODE_0V15
//#define TWO_CHANNEL

#ifdef MODE_0V2
//standard of key maping
//0.2V mode

#define REPORT_START_NUM			(5)
#define REPORT_KEY_LOW_LIMIT_COUNT		(3)
#define MAX_CYCLE_COUNTER			(100)
#define REPORT_REPEAT_KEY_FROM_HW
#define INITIAL_VALUE				(0Xff)
#define KBD_BUFFER_LEN				(0x20)
static void sun4i_kbd_read_input(void);

/******************************************************************
 * Queue handling from keyboard.c
 ******************************************************************/

static volatile char kbd_buffer[KBD_BUFFER_LEN];
static volatile int in_pointer = 0;
static volatile int out_pointer = 0;

/* puts character in the queue and sets up the in and out pointer */
static void kbd_put_queue(char data)
{
	if((in_pointer+1)==KBD_BUFFER_LEN) {
			if(out_pointer==0) {
					return; /* buffer full */
			} else{
					in_pointer=0;
			}
	} else {
			if((in_pointer+1)==out_pointer)
				return; /* buffer full */
			in_pointer++;
	}
	kbd_buffer[in_pointer]=data;
	return;
}

/* test if a character is in the queue */
static int sun4i_kbd_testc(struct stdio_dev *dev)
{
	sun4i_kbd_read_input();
	if(in_pointer==out_pointer)
		return(0); /* no data */
	else
		return(1);
}

/* gets the character from the queue */
static int sun4i_kbd_getc(struct stdio_dev *dev)
{
	char c;
	while(in_pointer==out_pointer) {
			sun4i_kbd_read_input();
	}
	if((out_pointer+1)==KBD_BUFFER_LEN)
		out_pointer=0;
	else
		out_pointer++;
	c=kbd_buffer[out_pointer];
	return (int)c;
}

static unsigned char keypad_mapindex[64] =
{
	0,0,0,0,0,0,0,0,               //key 1, 8个， 0-7
	1,1,1,1,1,1,1,                 //key 2, 7个， 8-14
	2,2,2,2,2,2,2,                 //key 3, 7个， 15-21
	3,3,3,3,3,3,                   //key 4, 6个， 22-27
	4,4,4,4,4,4,                   //key 5, 6个， 28-33
	5,5,5,5,5,5,                   //key 6, 6个， 34-39
	6,6,6,6,6,6,6,6,6,6,           //key 7, 10个，40-49
	7,7,7,7,7,7,7,7,7,7,7,7,7,7    //key 8, 17个，50-63
};
#endif

#ifdef MODE_0V15
//0.15V mode
static unsigned char keypad_mapindex[64] =
{
	0,0,0,                      //key1
	1,1,1,1,1,                  //key2
	2,2,2,2,2,
	3,3,3,3,
	4,4,4,4,4,
	5,5,5,5,5,
	6,6,6,6,6,
	7,7,7,7,
	8,8,8,8,8,
	9,9,9,9,9,
	10,10,10,10,
	11,11,11,11,
	12,12,12,12,12,12,12,12,12,12 //key13
};
#endif

#ifdef EVB
static unsigned int sun4i_scankeycodes[KEY_MAX_CNT]=
{
	[0 ] = 0x10,
	[1 ] = 0xe,
	[2 ] = KEY_MENU,
	[3 ] = KEY_SEARCH,
	[4 ] = KEY_HOME,
	[5 ] = KEY_ESC,
	[6 ] = '\r',
	[7 ] = KEY_RESERVED,
	[8 ] = KEY_RESERVED,
	[9 ] = KEY_RESERVED,
	[10] = KEY_RESERVED,
	[11] = KEY_RESERVED,
	[12] = KEY_RESERVED,
};
#endif
/* keycode generated by AXP power button */
static unsigned int power_key = '\r';

static volatile unsigned int key_val;
static unsigned char scancode;

static unsigned char key_cnt = 0;
static unsigned char cycle_buffer[REPORT_START_NUM] = {0};
static unsigned char transfer_code = INITIAL_VALUE;



static void sun4i_kbd_read_input(void)
{
	unsigned int  reg_val;
	int judge_flag = 0;
	int loop = 0;

	reg_val  = readl(KEY_BASSADDRESS + LRADC_INT_STA);
	//writel(reg_val,KEY_BASSADDRESS + LRADC_INT_STA);
	if(reg_val&LRADC_ADC0_DOWNPEND)
		{
			PRINTF("key down\n");
		}

	if(reg_val&LRADC_ADC0_DATAPEND)
		{
			key_val = readl(KEY_BASSADDRESS+LRADC_DATA0);
			if(key_val < 0x3f)
				{
					/*key_val = readl(KEY_BASSADDRESS + LRADC_DATA0);
					  cancode = keypad_mapindex[key_val&0x3f];
					  PRINTF("raw data: key_val == %u , scancode == %u \n", key_val, scancode);
					  */
					cycle_buffer[key_cnt%REPORT_START_NUM] = key_val&0x3f;
					if((key_cnt + 1) < REPORT_START_NUM)
						{
							//do not report key message

						}else{
								//scancode = cycle_buffer[(key_cnt-2)%REPORT_START_NUM];
								if(cycle_buffer[(key_cnt - REPORT_START_NUM + 1)%REPORT_START_NUM] \
								   == cycle_buffer[(key_cnt - REPORT_START_NUM + 2)%REPORT_START_NUM])
									{
										key_val = cycle_buffer[(key_cnt - REPORT_START_NUM + 1)%REPORT_START_NUM];
										scancode = keypad_mapindex[key_val&0x3f];
										judge_flag = 1;

									}
								if((!judge_flag) && cycle_buffer[(key_cnt - REPORT_START_NUM + 4)%REPORT_START_NUM] \
								   == cycle_buffer[(key_cnt - REPORT_START_NUM + 5)%REPORT_START_NUM])
									{
										key_val = cycle_buffer[(key_cnt - REPORT_START_NUM + 5)%REPORT_START_NUM];
										scancode = keypad_mapindex[key_val&0x3f];
										judge_flag = 1;

									}
								if(1 == judge_flag)
									{
										PRINTF2("report data: key_val :%8d transfer_code: %8d , scancode: %8d\n",\
											key_val, transfer_code, scancode);

										if(transfer_code == scancode){
												//report repeat key value
#ifdef REPORT_REPEAT_KEY_FROM_HW
												kbd_put_queue(sun4i_scankeycodes[scancode]);
#else
												//do not report key value
#endif
										}else if(INITIAL_VALUE != transfer_code){
												//report previous key value up signal + report current key value down
												kbd_put_queue(sun4i_scankeycodes[scancode]);
												transfer_code = scancode;

										}else{
												//INITIAL_VALUE == transfer_code, first time to report key event
												kbd_put_queue(sun4i_scankeycodes[scancode]);
												transfer_code = scancode;
										}

									}

						}
					key_cnt++;
					if(key_cnt > 2 * MAX_CYCLE_COUNTER ){
							key_cnt -= MAX_CYCLE_COUNTER;
					}

				}
		}

	if(reg_val&LRADC_ADC0_UPPEND)
		{
			if(key_cnt > REPORT_START_NUM)
				{
					if(INITIAL_VALUE != transfer_code)
						{
							PRINTF2("report data: key_val :%8d transfer_code: %8d \n",key_val, transfer_code);
							kbd_put_queue(sun4i_scankeycodes[transfer_code]);
						}

				}else if((key_cnt + 1) >= REPORT_KEY_LOW_LIMIT_COUNT){
						//rely on hardware first_delay work, need to be verified!
						if(cycle_buffer[0] == cycle_buffer[1]){
								key_val = cycle_buffer[0];
								scancode = keypad_mapindex[key_val&0x3f];
								PRINTF2("report data: key_val :%8d scancode: %8d \n",key_val, scancode);
								kbd_put_queue(sun4i_scankeycodes[scancode]);
						}

				}

			PRINTF("key up \n");

			key_cnt = 0;
			judge_flag = 0;
			transfer_code = INITIAL_VALUE;
			for(loop = 0; loop < REPORT_START_NUM; loop++)
				{
					cycle_buffer[loop] = 0;
				}

		}

	writel(reg_val,KEY_BASSADDRESS + LRADC_INT_STA);
	/* handle AXP power button if axp is configured - normally used in menu as enter on tablets with lack of buttons */
	if(axp152_power_button())
		kbd_put_queue(power_key);
	if(axp209_power_button())
		kbd_put_queue(power_key);
}

static void sun4i_kbd_initialize(void)
{

#ifdef ONE_CHANNEL
	writel(LRADC_ADC0_DOWN_EN|LRADC_ADC0_UP_EN|LRADC_ADC0_DATA_EN,KEY_BASSADDRESS + LRADC_INTC);
	writel(FIRST_CONCERT_DLY|LEVELB_VOL|KEY_MODE_SELECT|LRADC_HOLD_EN|ADC_CHAN_SELECT|LRADC_SAMPLE_250HZ|LRADC_EN,KEY_BASSADDRESS + LRADC_CTRL);
	//writel(FIRST_CONCERT_DLY|LEVELB_VOL|KEY_MODE_SELECT|ADC_CHAN_SELECT|LRADC_SAMPLE_62HZ|LRADC_EN,KEY_BASSADDRESS + LRADC_CTRL);
#else
#error No code for TWO_CHANNEL
#endif
}

static int sun4i_kbd_init_hw(void)
{
#ifdef CONFIG_KEYBOARD_SUN4I_KEYBOARD_FEX /* TODO read from somewhere - DT? */
	int val, err = 0;
	char key[16];

	err = script_parser_fetch("tabletkeys_para", "tabletkeys_used", &i, 1);
	if (err != 0 || i == 0)
		return -ENODEV;

	for (i = 0; i < KEY_MAX_CNT; i++) {
			snprintf(key, sizeof(key), "key%d_code", i);
			err = script_parser_fetch("tabletkeys_para", key, &val, 1);
			if (err == 0)
				sun4i_scankeycodes[i] = val;
	}
#endif

	PRINTF("sun4ikbd_init \n");
	sun4i_kbd_initialize();
	PRINTF("sun4ikbd initialized \n");
	return 1;
}

/* registration magic from keyboard.c */
#ifdef CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE
extern int overwrite_console (void);
#define OVERWRITE_CONSOLE overwrite_console ()
#else
#define OVERWRITE_CONSOLE 0
#endif /* CONFIG_SYS_CONSOLE_OVERWRITE_ROUTINE */

int sun4i_kbd_init (void)
{
	int error;
	struct stdio_dev kbddev ;
	char *stdinname  = getenv ("stdin");

	if(sun4i_kbd_init_hw()==-1)
		return -1;
	memset (&kbddev, 0, sizeof(kbddev));
	strcpy(kbddev.name, DEVNAME);
	kbddev.flags =  DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
	kbddev.getc = sun4i_kbd_getc ;
	kbddev.tstc = sun4i_kbd_testc ;

	error = stdio_register (&kbddev);
	if(error==0) {
			/* check if this is the standard input device */
			if(strcmp(stdinname,DEVNAME)==0) {
					/* reassign the console */
					if(OVERWRITE_CONSOLE) {
							return 1;
					}
					error=console_assign(stdin,DEVNAME);
					if(error==0)
						return 1;
					else
						return error;
			}
			return 1;
	}
	return error;
}

