#ifndef _SHIFTREGISTER_H_
#define _SHIFTREGISTER_H_

#include <stdint.h>

#define DIRECTION_READ  1
#define DIRECTION_WRITE 0

extern void sr_init(void);
extern void sr_out(uint16_t value);
extern uint16_t sr_in(void);

#endif // _SHIFTREGISTER_H_
