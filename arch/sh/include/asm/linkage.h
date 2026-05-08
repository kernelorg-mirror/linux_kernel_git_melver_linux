/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#define __ALIGN .balign 4
#define __ALIGN_STR ".balign 4"

#define _THIS_IP_ ({ unsigned long __ip; asm volatile("mova 1f, %0\n1:" : "=z" (__ip)); __ip; })

#endif
