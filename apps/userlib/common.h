

#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>


#if CONFIG_USERLIB
#undef __syscall
#define __syscall
#endif

#endif /* _COMMON_H_ */

