#ifndef _UINT32VECTOR_H
#define _UINT32VECTOR_H

#include <stdlib.h>
#include <stdint.h>


typedef struct UInt32Vector *UInt32Vector;

UInt32Vector UInt32Vector_create();
UInt32Vector UInt32Vector_createWithUInt32(uint32_t value);
UInt32Vector UInt32Vector_copy(UInt32Vector vector);
void UInt32Vector_delete(UInt32Vector vector);

void UInt32Vector_growToSize(UInt32Vector vector, size_t size);

void UInt32Vector_setItemAtIndex(UInt32Vector vector, size_t index, uint32_t value);
uint32_t UInt32Vector_itemAtIndex(UInt32Vector vector, size_t index);

size_t UInt32Vector_count(UInt32Vector vector);



#endif
