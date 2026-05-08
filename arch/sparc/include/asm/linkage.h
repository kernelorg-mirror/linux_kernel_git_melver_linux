/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_SPARC_LINKAGE_H
#define _ASM_SPARC_LINKAGE_H

#ifdef CONFIG_SPARC64
#define _THIS_IP_ ({ unsigned long __ip; asm volatile("rd %%pc, %0" : "=r" (__ip)); __ip; })
#else
#define _THIS_IP_ ({				\
	unsigned long __ip;			\
	asm volatile("call 1f\n\t"		\
		     " nop\n\t"			\
		     "1: mov %%o7, %0"		\
		     : "=r" (__ip) : : "o7"	\
	);					\
	__ip;					\
})
#endif

#endif /* _ASM_SPARC_LINKAGE_H */
