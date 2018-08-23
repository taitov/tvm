// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <stdio.h>
#include <endian.h>

#include <tuple>
#include <vector>

#include "type.h"

#define TVM_LOG_DEBUG(message, args ...) printf("%s:%d: " message, __FILE__, __LINE__, ##args)
#define TVM_LOG_ERROR(message, args ...) printf("%s:%d: " message, __FILE__, __LINE__, ##args)

#ifdef XXX_DEVELOP
#define TVM_UNUSED
#else
#define TVM_UNUSED [[maybe_unused]]
#endif

namespace tvm
{

template<class ...>
using void_t = void;

/// do not look at this code.
#define hasFunctionHelper(functionName, args ...) \
	template<class, class = void> \
	struct has_##functionName : std::false_type {}; \
	template<class TType> \
	struct has_##functionName<TType, \
	                          void_t<decltype(std::declval<TType>().functionName(args))>> : std::true_type {}

#define hasFunctionHelperWithDefaultValue(valueType, functionName, defaultValue) \
	hasFunctionHelper(functionName); \
	template<typename TType> \
	valueType functionName() \
	{ \
		if constexpr (has_##functionName<TType>::value) \
		{ \
			return TType::functionName(); \
		} \
		return defaultValue; \
	}

constexpr tSchemeId schemeRootMemories = ((tSchemeId)-1);
constexpr tSchemeId schemeGlobalMemories = ((tSchemeId)-2);
constexpr tSchemeId schemeConstMemories = ((tSchemeId)-3);

constexpr tFlowId flowShift = (sizeof(tFlowId) * 8 - 1);
constexpr tFlowId flowMask = (std::numeric_limits<tFlowId>::max() << flowShift);
constexpr tFlowId flowNowhere = ((tFlowId)1 | flowMask);
constexpr tFlowId flowStop = ((tFlowId)2 | flowMask);
constexpr tFlowId flowSync = ((tFlowId)3 | flowMask);

/// @todo: delete?
constexpr tLogicModuleType logicModuleTypeLock = ((tLogicModuleType)-1);
constexpr tLogicModuleType logicModuleTypeUnlock = ((tLogicModuleType)-2);

/// something like typeid(), but without RTTI
/// https://stackoverflow.com/questions/8001207/compile-time-typeid-without-rtti-with-gcc
template<typename TType>
static tUniqueId getUniqueId() ///< @todo: constexpr
{
	constexpr std::string_view uniqueString(__PRETTY_FUNCTION__);
	constexpr std::hash<std::string_view> hash;
	return hash(uniqueString);
}

}
