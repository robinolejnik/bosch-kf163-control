#ifndef _SHIFTREGISTER_H_
#define _SHIFTREGISTER_H_

#include <stdint.h>

#define DIRECTION_READ  1
#define DIRECTION_WRITE 0

extern void shiftregister_init(void);
extern void shiftregister_out(uint16_t value);
extern uint16_t shiftregister_in(void);

#endif // _SHIFTREGISTER_H_
