#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <stdio.h>
#include <stdint.h>

typedef uint8_t bool;

#define TRUE          1
#define FALSE         0

#define CRASH()                 \
{                               \
  char*   __bullshit = NULL;    \
  *__bullshit = 0;              \
}

#define NARRAY(a)       (sizeof(a)/sizeof(a[0]))
#define UNUSED(a)       (void)(a)

#define HI_BYTE(w)                    ((uint8_t)((w >> 8) & 0xff))
#define LO_BYTE(w)                    ((uint8_t)(w  & 0xff))

#define BUFFER_TO_U16(b)              ((uint16_t)b[0] << 8 | (uint16_t)b[1])

#define U16_TO_BUFFER(w, b)           \
  b[0] = HI_BYTE(w);                  \
  b[1] = LO_BYTE(w);

#define MIN(a,b)      (a < b ? a : b)
#define MAX(a,b)      (a > b ? a : b)


#endif /* !__COMMON_DEF_H__ */
