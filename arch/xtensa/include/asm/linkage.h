/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#define __ALIGN		.align 4
#define __ALIGN_STR	".align 4"

#define _THIS_IP_ ({				\
	unsigned long __ip;			\
	asm volatile(".align 4\n\t"		\
		     "call0 1f\n\t"		\
		     ".align 4\n\t"		\
		     "1: mov %0, a0"		\
		     : "=r" (__ip) : : "a0"	\
	);					\
	__ip;					\
})

#endif
