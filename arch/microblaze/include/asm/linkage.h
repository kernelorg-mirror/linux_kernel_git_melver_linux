/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_MICROBLAZE_LINKAGE_H
#define _ASM_MICROBLAZE_LINKAGE_H

#define _THIS_IP_ ({ unsigned long __ip; asm volatile("mfs %0, rpc" : "=r" (__ip)); __ip; })

#endif /* _ASM_MICROBLAZE_LINKAGE_H */
