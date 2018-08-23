// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <inttypes.h>

namespace tvm
{

template<typename TType,
         unsigned int TIndex>
class raiiValue
{
public:
	using type = raiiValue<TType, TIndex>;

public:
	inline raiiValue() :
	        value(0)
	{
	}

	inline raiiValue(const TType& value) :
	        value(value)
	{
	}

	inline operator TType() const
	{
		return value;
	}

	inline type& operator+=(const type& rhs)
	{
		this->value += rhs.value;
		return *this;
	}

	inline type& operator-=(const type& rhs)
	{
		this->value -= rhs.value;
		return *this;
	}

	inline type& operator++()
	{
		this->value++;
		return *this;
	}

public:
	TType value;
};

using tFlowId = uint32_t;
using tMemoryModuleId = uint32_t;
using tLogicModuleId = uint32_t;
using tMemoryModuleType = uint32_t;
using tRootModuleType = uint32_t;
using tLogicModuleType = uint32_t;
using tRootSignalExitId = uint32_t;
using tRootMemoryExitId = uint32_t;
using tSignalEntryId = uint32_t;
using tSignalExitId = uint32_t;
using tMemoryEntryId = uint32_t;
using tMemoryExitId = uint32_t;
using tMemoryEntryExitId = uint32_t;
using tSchemeId = uint32_t;
using tUniqueId = size_t;

class cConverterStringHelper;
using tCaptionName = cConverterStringHelper;
using tModuleName = cConverterStringHelper;
using tSignalEntryName = cConverterStringHelper;
using tMemoryEntryName = cConverterStringHelper;
using tSignalExitName = cConverterStringHelper;
using tMemoryExitName = cConverterStringHelper;
using tMemoryEntryExitName = cConverterStringHelper;
using tMemoryTypeName = cConverterStringHelper;

enum class eMemoryPosition : uint8_t
{
	scheme,
	global,
	root,
	constant,
};

}
