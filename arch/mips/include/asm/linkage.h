/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#ifdef __ASSEMBLER__
#include <asm/asm.h>
#endif

#define cond_syscall(x) asm(".weak\t" #x "\n" #x "\t=\tsys_ni_syscall")
#define SYSCALL_ALIAS(alias, name)					\
	asm ( #alias " = " #name "\n\t.globl " #alias)

#define _THIS_IP_ ({				\
	unsigned long __ip;			\
	asm volatile("bal 1f\n\t"		\
		     " nop\n\t"			\
		     "1: move %0, $ra"		\
		     : "=r" (__ip) : : "$31"	\
	);					\
	__ip;					\
})

#endif
