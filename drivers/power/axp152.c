/*
 * (C) Copyright 2012
 * Henrik Nordstrom <henrik@henriknordstrom.net>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */
#include <common.h>
#include <i2c.h>
#include <axp152.h>

enum axp152_reg {
	AXP152_CHIP_VERSION = 0x3,
	AXP152_DCDC2_VOLTAGE = 0x23,
	AXP152_DCDC3_VOLTAGE = 0x27,
	AXP152_DCDC4_VOLTAGE = 0x2B,
	AXP152_LDO2_VOLTAGE = 0x2A,
	AXP152_SHUTDOWN = 0x32,
	AXP152_REG_INTSTS1 = 0x48,
	AXP152_REG_INTSTS2 = 0x49,
	AXP152_REG_INTSTS3 = 0x4a,

};

#define AXP152_POWEROFF			(1 << 7)

/* interrupt bits */
#define AXP152_IRQ_PEKLONG      (1 << 8)
#define AXP152_IRQ_PEKSHORT     (1 << 9)


static int axp152_write(enum axp152_reg reg, u8 val)
{
	return i2c_write(0x30, reg, 1, &val, 1);
}

static int axp152_read(enum axp152_reg reg, u8 *val)
{
	return i2c_read(0x30, reg, 1, val, 1);
}

static int axp152_write_interrupts(int irqs)
{
	u8 val[3] = { irqs, irqs >> 8, irqs >> 16 };
	int ret, i;
	for(i = 0; i < 3 ; i++) {
			ret = i2c_write(0x30, AXP152_REG_INTSTS1 + i, 1, val + i, 1);
			if (ret < 0 )
				return ret ;
	}
	return 0;
}

static int axp152_read_interrupts(void)
{
	u8 val[3] = { 0, 0, 0 };
	int ret, i;
	for(i = 0; i < 3 ; i++) {
			ret = i2c_read(0x30, AXP152_REG_INTSTS1 + i, 1, val + i, 1);
			if (ret < 0 )
				return ret ;
	}
	return (val[2] << 16) | (val[1] << 8) | val[0];
}

int axp152_power_button(void)
{
	int irqs;
	irqs = axp152_read_interrupts();
	if (irqs < 0)
		return 0;
	axp152_write_interrupts(irqs);
	return irqs & (AXP152_IRQ_PEKLONG | AXP152_IRQ_PEKSHORT);
}

static u8 axp152_mvolt_to_target(int mvolt, int min, int max, int div)
{
	if (mvolt < min)
		mvolt = min;
	else if (mvolt > max)
		mvolt = max;

	return (mvolt - min) / div;
}

int axp152_set_dcdc2(int mvolt)
{
	int rc;
	u8 current, target;

	target = axp152_mvolt_to_target(mvolt, 700, 2275, 25);

	/* Do we really need to be this gentle? It has built-in voltage slope */
	while ((rc = axp152_read(AXP152_DCDC2_VOLTAGE, &current)) == 0 &&
	       current != target) {
		if (current < target)
			current++;
		else
			current--;
		rc = axp152_write(AXP152_DCDC2_VOLTAGE, current);
		if (rc)
			break;
	}
	return rc;
}

int axp152_set_dcdc3(int mvolt)
{
	u8 target = axp152_mvolt_to_target(mvolt, 700, 3500, 50);

	return axp152_write(AXP152_DCDC3_VOLTAGE, target);
}

int axp152_set_dcdc4(int mvolt)
{
	u8 target = axp152_mvolt_to_target(mvolt, 700, 3500, 25);

	return axp152_write(AXP152_DCDC4_VOLTAGE, target);
}

int axp152_set_ldo2(int mvolt)
{
	u8 target = axp152_mvolt_to_target(mvolt, 700, 3500, 100);

	return axp152_write(AXP152_LDO2_VOLTAGE, target);
}

int axp152_init(void)
{
	u8 ver;
	int rc;

	rc = axp152_read(AXP152_CHIP_VERSION, &ver);
	if (rc)
		return rc;

	if (ver != 0x05)
		return -1;

	/* enable power key */
	axp152_write_interrupts(AXP152_IRQ_PEKLONG | AXP152_IRQ_PEKSHORT);
	axp152_write_interrupts(AXP152_IRQ_PEKLONG | AXP152_IRQ_PEKSHORT);

	return 0;
}
