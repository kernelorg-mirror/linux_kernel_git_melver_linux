/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2010-2011, The Linux Foundation. All rights reserved.
 */

#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#define __ALIGN		.align 4
#define __ALIGN_STR	".align 4"

#define _THIS_IP_ ({ unsigned long __ip; asm volatile("call 1f\n1: %0 = r31" : "=r" (__ip) : : "r31"); __ip; })

#endif
