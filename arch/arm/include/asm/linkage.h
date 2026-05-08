/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_LINKAGE_H
#define __ASM_LINKAGE_H

#define __ALIGN .align 0
#define __ALIGN_STR ".align 0"

#define ENDPROC(name) \
  .type name, %function; \
  END(name)

#define _THIS_IP_ ({ unsigned long __ip; asm volatile("1: adr %0, 1b" : "=r" (__ip)); __ip; })

#endif
