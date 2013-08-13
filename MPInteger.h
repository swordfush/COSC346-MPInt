#ifndef _MPINTEGER_H
#define _MPINTEGER_H


#include <stdint.h>


typedef struct MPInteger *MPInteger;


MPInteger MPInteger_createWithString(const char *str);
MPInteger MPInteger_createWithUInt32(uint32_t value, bool isSigned);
MPInteger MPInteger_copy(const MPInteger integer);

void MPInteger_delete(MPInteger integer);

char *MPInteger_description(const MPInteger integer);

MPInteger MPInteger_add(const MPInteger integer, const MPInteger other);
MPInteger MPInteger_subract(const MPInteger integer, const MPInteger other);
MPInteger MPInteger_multiply(const MPInteger integer, const MPInteger other);
MPInteger MPInteger_divide(const MPInteger integer, const MPInteger other);
MPInteger MPInteger_modulus(const MPInteger integer, const MPInteger other);

bool MPInteger_isLessThan(const MPInteger integer, const MPInteger other);


#endif
