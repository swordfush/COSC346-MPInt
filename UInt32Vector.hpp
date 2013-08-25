#ifndef _UINT32VECTOR_HPP
#define _UINT32VECTOR_HPP

#include <cstdlib>
#include <cstdint>

/**
 * A vector that holds 32 bit unsigned integers.
 */
struct UInt32Vector
{
	/**
	 * Creates a vector with the capacity specified. All elements will be
	 * set to zero.
	 */
	static UInt32Vector *initWithSize(size_t size);

	~UInt32Vector();

	/**
	 * Creates a copy of the vector.
	 */
	UInt32Vector *copy() const;

	/**
	 * Grows the vector to the capacity specified as long as it is larger than
	 * its current capacity.
	 */
	void growToSize(size_t size);
	/**
	 * Shrinks the vector to discard any leading (high indices) zeros.
	 */
	void discardLeadingZeros();

	/**
	 * Obtains the item at the index specified.
	 */
	uint32_t item(size_t index) const;
	/**
	 * Sets the item at the index specified.
	 */
	void setItem(size_t index, uint32_t value);

	/**
	 * Returns the capacity of the vector.
	 */
	size_t size() const;

	/**
	 * Returns the index of the highest non-zero element in the vector.
	 */
	size_t highestNonZeroIndex() const;


	private:
		UInt32Vector(size_t size);

		void setSize(size_t size);

		uint32_t *items;
		size_t count;

};


#endif
