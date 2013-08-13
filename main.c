#include <stdlib.h>
#include <stdio.h>

#include "UInt32Vector.h"




int main(const int argc, const char **argv)
{
	size_t i;

	UInt32Vector vector = UInt32Vector_createWithUInt32(1234);
	UInt32Vector_growToSize(vector, 4);

	UInt32Vector_setItemAtIndex(vector, 2, 5678);

	for (i = 0; i < UInt32Vector_count(vector); ++i)
	{
		printf("%u\n", UInt32Vector_itemAtIndex(vector, i));
	}

	UInt32Vector_delete(vector);

	return EXIT_SUCCESS;
}
