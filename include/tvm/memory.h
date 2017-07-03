// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_MEMORY_H
#define TVM_MEMORY_H

#include "module.h"

namespace nVirtualMachine
{

class cMemory
{
public:
	using tVariables = std::map<tVariableName,
	                            std::tuple<std::type_index,
	                                       void*>>;

public:
	virtual ~cMemory() = default;

	virtual cMemory* clone() const = 0;
	virtual void* getValue() = 0;
	const tVariables& getVariables() const;

	virtual bool setVariables(const std::vector<uint8_t>& buffer) = 0;

protected:
	template<typename TType>
	bool registerVariable(const tVariableName& variableName,
	                      TType& variable)
	{
		if (variables.find(variableName) != variables.end())
		{
			return false;
		}

		variables.insert(std::make_pair(variableName,
		                                std::make_tuple(std::type_index(typeid(TType)),
		                                                &variable)));
		return true;
	}

	template<typename TType, typename TDefaultType>
	bool registerVariable(const tVariableName& variableName,
	                      TType& variable,
	                      const TDefaultType& defaultValue)
	{
		variable = defaultValue;
		return registerVariable<TType>(variableName, variable);
	}

private:
	tVariables variables;
};

template<typename TType>
class cMemoryVariable : public cMemory
{
public:
	cMemoryVariable();
	cMemoryVariable(const TType& value);

	cMemory* clone() const override;

	void* getValue() override;

	bool setVariables(const std::vector<uint8_t>& buffer) override;

private:
	TType value;
};

inline const cMemory::tVariables& cMemory::getVariables() const
{
	return variables;
}

template<typename TType>
cMemoryVariable<TType>::cMemoryVariable()
{
	registerVariable("value", this->value);
}

template<typename TType>
cMemoryVariable<TType>::cMemoryVariable(const TType& value)
{
	this->value = value;
	registerVariable("value", this->value);
}

template<typename TType>
cMemory* cMemoryVariable<TType>::clone() const
{
	return new cMemoryVariable(this->value);
}

template<typename TType>
void* cMemoryVariable<TType>::getValue()
{
	return &value;
}

template<typename TType>
bool cMemoryVariable<TType>::setVariables(const std::vector<uint8_t>& buffer)
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

}

#endif // TVM_MEMORY_H
