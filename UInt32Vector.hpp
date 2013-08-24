#ifndef _UINT32VECTOR_HPP
#define _UINT32VECTOR_HPP

#include <cstdlib>
#include <cstdint>

struct UInt32Vector
{
	static UInt32Vector *initWithSize(size_t size);
	~UInt32Vector();

	UInt32Vector *copy() const;

	void growToSize(size_t size);
	void discardLeadingZeros();

	uint32_t item(size_t index) const;
	void setItem(size_t index, uint32_t value);

	size_t size() const;

	size_t highestNonZeroIndex() const;

	void debug() const;


	private:
		UInt32Vector(size_t size);

		void setSize(size_t size);

		uint32_t *items;
		size_t count;

};


#endif
