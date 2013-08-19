#include "UInt32Vector.hpp"

#include <cassert>
#include <iostream>


UInt32Vector::UInt32Vector(size_t size)
{
	assert(size > 0);

	this->count = 0;
	this->items = NULL;
	this->setSize(size); // Uses realloc, which on a null pointer behaves like malloc 
}

UInt32Vector::~UInt32Vector()
{
	free(this->items);
}

UInt32Vector *UInt32Vector::initWithSize(size_t size)
{
	return new UInt32Vector(size);
}


UInt32Vector *UInt32Vector::copy() const
{
	UInt32Vector *copy = UInt32Vector::initWithSize(this->size());

	for (size_t i = 0; i < copy->size(); ++i)
	{
		copy->setItem(i, this->item(i));
	}

	return copy;
}

void UInt32Vector::setSize(size_t size)
{
	this->items = (uint32_t*)realloc(this->items, size * sizeof(uint32_t));

	size_t oldSize = this->count;
	this->count = size;

	for (size_t i = oldSize; i < size; ++i)
	{
		this->setItem(i, 0);
	}
}

void UInt32Vector::growToSize(size_t size)
{
	if (size > this->size())
	{
		this->setSize(size);
	}
}

uint32_t UInt32Vector::item(size_t index) const
{
	assert(index < this->size());
	return this->items[index];
}

void UInt32Vector::setItem(size_t index, uint32_t value)
{
	assert(index < this->size());
	this->items[index] = value;
}

size_t UInt32Vector::size() const
{
	return this->count;
}

void UInt32Vector::debug() const
{
	std::cout << "UInt32Vector:";

	for (size_t i = 0; i < this->size(); ++i)
	{
		std::cout << "\n\t[" << i << "]\t" << this->item(i);
	}

	std::cout << std::endl;
}
