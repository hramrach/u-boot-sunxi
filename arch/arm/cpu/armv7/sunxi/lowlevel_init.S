/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <version.h>
#include <linux/linkage.h>

#if defined CONFIG_MACH_SUN6I || defined CONFIG_MACH_SUN7I || \
    defined CONFIG_MACH_SUN8I
ENTRY(soc_init)
	mov	r9, lr
	bl	cpu_init_cortex_a7
	bl	cpu_init_cp15
	mov	pc, r9			@ back to my caller
ENDPROC(soc_init)
#endif
