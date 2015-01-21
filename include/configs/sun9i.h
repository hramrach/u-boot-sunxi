/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * Configuration settings for the Allwinner A80 (sun9i) CPU
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * A80 specific configuration
 */

#define CONFIG_SYS_PROMPT	"sun9i# "

/*
 * Include common sunxi configuration where most the settings are
 */
#include <configs/sunxi-common.h>

#endif /* __CONFIG_H */
