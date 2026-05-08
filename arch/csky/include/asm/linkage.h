/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_CSKY_LINKAGE_H
#define __ASM_CSKY_LINKAGE_H

#define _THIS_IP_ ({ unsigned long __ip; asm volatile("grs %0, ." : "=r" (__ip)); __ip; })

#endif /* __ASM_CSKY_LINKAGE_H */
