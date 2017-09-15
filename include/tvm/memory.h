// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_MEMORY_H
#define TVM_MEMORY_H

#include "module.h"

namespace nVirtualMachine
{

class cMemory
{
public:
	virtual ~cMemory() = default;

	virtual cMemory* clone() const = 0;
	virtual void* getPointer() = 0;

	virtual bool write(const std::vector<uint8_t>& buffer) = 0;
	virtual std::vector<uint8_t> read() = 0;
};

template<typename TType>
class cMemoryVariable : public cMemory
{
public:
	cMemoryVariable()
	{
	}

	cMemoryVariable(const TType& value)
	{
		this->value = value;
	}

	cMemory* clone() const override
	{
		return new cMemoryVariable(this->value);
	}

	void* getPointer() override
	{
		return &value;
	}

	bool write(const std::vector<uint8_t>& buffer) override
	{
		if (!buffer.size())
		{
			/** @todo: temporary */
			return true;
		}

		cStreamIn stream(buffer);

		stream.pop(value);

		if (stream.isFailed())
		{
			return false;
		}

		return true;
	}

	std::vector<uint8_t> read() override
	{
		cStreamOut buffer;
		buffer.push(value);
		return buffer.getBuffer();
	}

protected:
	TType value;
};

}

#endif // TVM_MEMORY_H
