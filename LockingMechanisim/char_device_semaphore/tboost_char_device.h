#ifndef _TBOOST_CHAR_DEVICE_H_
#define _TBOOST_CHAR_DEVICE_H_

#include <asm/ioctl.h>

struct my_data
{
    int i;
    long x;
    char s[256];
};


#define TBOOST_MAGIC 'L'

#endif /* _TBOOST_CHAR_DEVICE_H_ */
