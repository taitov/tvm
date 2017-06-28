// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_TYPE_H
#define TFVM_TYPE_H

#include <map>
#include <string>

#include <inttypes.h>

#define MAKE_TYPE(type, name) \
struct name \
{ \
	type value; \
 \
	inline constexpr name() : \
		value() \
	{ \
	} \
 \
	inline constexpr name(const type& value) : \
		value(value) \
	{ \
	} \
 \
	inline constexpr operator type&() \
	{ \
		return value; \
	} \
 \
	inline constexpr operator const type&() const \
	{ \
		return value; \
	} \
 \
	inline bool operator==(const type& value) const \
	{ \
		return (this->value == value); \
	} \
 \
	inline bool operator==(const name& second) const \
	{ \
		return (this->value == second.value); \
	} \
 \
	inline bool operator<(const name& second) const \
	{ \
		return (this->value < second.value); \
	} \
} __attribute__((__aligned__(sizeof(type))))

#define MAKE_STRING_TYPE(type, name) \
struct name \
{ \
	type value; \
 \
	inline name() = default; \
 \
	inline name(const type& value) \
	{ \
		this->value = value; \
	} \
 \
	inline name(const char* value) \
	{ \
		this->value = value; \
	} \
 \
	inline operator type&() \
	{ \
		return value; \
	} \
 \
	inline operator const type&() const \
	{ \
		return value; \
	} \
 \
	inline bool operator==(const type& value) const \
	{ \
		return (this->value == value); \
	} \
 \
	inline bool operator==(const name& second) const \
	{ \
		return (this->value == second.value); \
	} \
 \
	inline bool operator<(const name& second) const \
	{ \
		return (this->value < second.value); \
	} \
} __attribute__((__aligned__(sizeof(type))))

namespace nVirtualMachine
{

MAKE_TYPE(uint32_t, tRootSignalExitId);
MAKE_TYPE(uint32_t, tRootMemoryExitId);
MAKE_TYPE(uint32_t, tSignalEntryId);
MAKE_TYPE(uint32_t, tSignalExitId);
MAKE_TYPE(uint32_t, tMemoryEntryId);
MAKE_TYPE(uint32_t, tMemoryExitId);

MAKE_STRING_TYPE(std::string, tLibraryName);
MAKE_STRING_TYPE(std::string, tRootModuleName);
MAKE_STRING_TYPE(std::string, tModuleName);
MAKE_STRING_TYPE(std::string, tCustomModuleName);
MAKE_STRING_TYPE(std::string, tCaptionName);
MAKE_STRING_TYPE(std::string, tModuleTypeName);
MAKE_STRING_TYPE(std::string, tSignalEntryName);
MAKE_STRING_TYPE(std::string, tSignalExitName);
MAKE_STRING_TYPE(std::string, tMemoryEntryName);
MAKE_STRING_TYPE(std::string, tMemoryExitName);
MAKE_STRING_TYPE(std::string, tMemoryName);
MAKE_STRING_TYPE(std::string, tMemoryTypeName);
MAKE_STRING_TYPE(std::string, tVariableName);
MAKE_STRING_TYPE(std::string, tSchemeName);

MAKE_TYPE(uint32_t, tModuleId);

}

#endif // TFVM_TYPE_H
