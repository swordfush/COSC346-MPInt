#include "UInt32Vector.h"

#include <assert.h>


struct UInt32Vector
{
	uint32_t *items;
	size_t count;
};


UInt32Vector UInt32Vector_create()
{
	return UInt32Vector_createWithUInt32(0u);
}

UInt32Vector UInt32Vector_createWithUInt32(uint32_t value)
{
	UInt32Vector vector = malloc(sizeof(struct UInt32Vector));
	vector->items = malloc(sizeof(uint32_t));
	vector->items[0] = value;
	vector->count = 1u;
	return vector;
}

UInt32Vector UInt32Vector_copy(UInt32Vector vector)
{
	UInt32Vector new = malloc(sizeof(struct UInt32Vector));
	size_t size = UInt32Vector_count(vector);
	size_t i;
	new->items = malloc(sizeof(uint32_t) * size);
	new->count = size;

	for (i = 0u; i < size; ++i)
	{
		new->items[i] = UInt32Vector_itemAtIndex(vector, i);
	}

	return new;
}

void UInt32Vector_delete(UInt32Vector vector)
{
	free(vector->items);
	free(vector);
}

void UInt32Vector_growToSize(UInt32Vector vector, size_t size)
{
	size_t i;
	size_t vector_length = UInt32Vector_count(vector);
	if (vector_length < size)
	{
		vector->items = realloc(vector->items, sizeof(uint32_t) * size);

		for (i = vector_length; i < size; ++i)
		{
			UInt32Vector_setItemAtIndex(vector, i, 0u);
		}

		vector->count = size;
	}
}

void UInt32Vector_setItemAtIndex(UInt32Vector vector, size_t index, uint32_t value)
{
	assert(index < UInt32Vector_count(vector));
	vector->items[index] = value;
}

uint32_t UInt32Vector_itemAtIndex(UInt32Vector vector, size_t index)
{
	assert(index < UInt32Vector_count(vector));
	return vector->items[index];
}

size_t UInt32Vector_count(UInt32Vector vector)
{
	return vector->count;
}
