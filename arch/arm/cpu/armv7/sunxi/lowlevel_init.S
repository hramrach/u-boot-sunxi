/*
 * (C) Copyright 2015 Hans de Goede <hdegoede@redhat.com>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <config.h>
#include <version.h>
#include <linux/linkage.h>

#if defined CONFIG_MACH_SUN6I || defined CONFIG_MACH_SUN7I || \
    defined CONFIG_MACH_SUN8I || defined CONFIG_MACH_SUN9I
ENTRY(soc_init)
	mov	r9, lr
	bl	cpu_init_cortex_a7
#if defined CONFIG_MACH_SUN9I
	/*
	 * Dark magic poke of some register in the SYS_CTRL region.
	 * Allwinners own u-boot does this and without this u-boot causes a
	 * CPU / system reset as soon as it loads.
	 */
	ldr	r0, =0x008000e0
	ldr	r1, =0x16aa0001
	str	r1, [r0]
#endif
	bl	cpu_init_cp15
	mov	pc, r9			@ back to my caller
ENDPROC(soc_init)
#endif
