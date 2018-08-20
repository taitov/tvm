// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_STREAM_H
#define TVM_STREAM_H

#include <string.h>
#include <inttypes.h>

#include <string>
#include <vector>
#include <map>
#include <type_traits>
#include <variant>

#include "common.h"

namespace nVirtualMachine
{

/// file format virtual machine to host endian
template<typename TType>
inline typename std::enable_if<sizeof(TType) == 1, void>::type
fvmtoh(const uint8_t* from, TType& to)
{
	to = *(TType*)from;
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 2, void>::type
fvmtoh(const uint8_t* from, TType& to)
{
	to = (TType)be16toh(*(uint16_t*)from);
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 4, void>::type
fvmtoh(const uint8_t* from, TType& to)
{
	to = (TType)be32toh(*(uint32_t*)from);
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 8, void>::type
fvmtoh(const uint8_t* from, TType& to)
{
	to = (TType)be64toh(*(uint64_t*)from);
}

/// host to file format virtual machine endian
template<typename TType>
inline typename std::enable_if<sizeof(TType) == 1, void>::type
htofvm(const TType& from, uint8_t* to)
{
	*(TType*)to = from;
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 2, void>::type
htofvm(const TType& from, uint8_t* to)
{
	*(TType*)to = (TType)htobe16(*(uint16_t*)&from);
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 4, void>::type
htofvm(const TType& from, uint8_t* to)
{
	*(TType*)to = (TType)htobe32(*(uint32_t*)&from);
}

template<typename TType>
inline typename std::enable_if<sizeof(TType) == 8, void>::type
htofvm(const TType& from, uint8_t* to)
{
	*(TType*)to = (TType)htobe64(*(uint64_t*)&from);
}

class cStreamIn
{
public:
	using tInteger = uint64_t;

public:
	cStreamIn(const std::vector<uint8_t>& buffer);

	template<typename TType>
	inline void pop(TType& value);

	inline void pop(char* buffer, uint64_t bufferSize);
	inline void pop(std::string& value);
	inline void pop(std::vector<uint8_t>& value);

	template<typename TFirstType, typename TSecondType>
	inline void pop(std::pair<TFirstType, TSecondType>& pair);

	template<typename TType, std::size_t TSize>
	inline void pop(std::array<TType, TSize>& array);

	template<typename TVectorType>
	inline void pop(std::vector<TVectorType>& vector);

	template<typename TMapFirstType, typename TMapSecondType>
	inline void pop(std::map<TMapFirstType, TMapSecondType>& map);

	template<typename ... TArgs>
	inline void pop(std::tuple<TArgs ...>& tuple);

	template<typename ... TArgs>
	inline void pop(std::variant<TArgs ...>& variant);

	inline bool isFailed();

protected:
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	popTuple(std::tuple<TArgs ...>& tuple);

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<(TTupleIndex < sizeof...(TArgs)), void>::type
	popTuple(std::tuple<TArgs ...>& tuple);

	template<size_t TVariantIndex, typename ... TArgs>
	inline typename std::enable_if<TVariantIndex == sizeof...(TArgs), void>::type
	popVariant(std::variant<TArgs ...>& variant, uint32_t index);

	template<size_t TVariantIndex, typename ... TArgs>
	inline typename std::enable_if<(TVariantIndex < sizeof...(TArgs)), void>::type
	popVariant(std::variant<TArgs ...>& variant, uint32_t index);

protected:
	const std::vector<uint8_t>& inBuffer;
	uint64_t inPosition;
	bool failed;
};

//

class cStreamOut
{
public:
	using tInteger = uint64_t;

public:
	cStreamOut()
	{
	}

	template<typename TType>
	inline void push(const TType& value);

	/// @todo
	inline void push(const char* buffer, uint64_t bufferSize)
	{
		tInteger size = outBuffer.size();
		outBuffer.resize(size + bufferSize);
		memcpy(&outBuffer[size], buffer, bufferSize);
	}
	inline void push(const std::string& value)
	{
		tInteger size = value.length();
		push(size);
		push(value.c_str(), size);
	}
	inline void push(const std::vector<uint8_t>& value)
	{
		tInteger size = value.size();
		push(size);
		push((char*)value.data(), size);
	}

	template<typename TFirstType, typename TSecondType>
	inline void push(const std::pair<TFirstType, TSecondType>& pair)
	{
		push(pair.first);
		push(pair.second);
	}

	template<typename TType, std::size_t TSize>
	inline void push(const std::array<TType, TSize>& array)
	{
		for (std::size_t i = 0; i < TSize; i++)
		{
			push(array[i]);
		}
	}

	template<typename TVectorType>
	inline void push(const std::vector<TVectorType>& vector)
	{
		tInteger size = vector.size();
		push(size);
		for (tInteger i = 0; i < size; i++)
		{
			push(vector[i]);
		}
	}

	template<typename TMapFirstType, typename TMapSecondType>
	inline void push(const std::map<TMapFirstType, TMapSecondType>& map)
	{
		tInteger size = map.size();
		push(size);
		for (auto& iter : map)
		{
			push(iter.first);
			push(iter.second);
		}
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	pushTuple(TVM_UNUSED const std::tuple<TArgs ...>& tuple)
	{
	}
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<(TTupleIndex < sizeof...(TArgs)), void>::type
	pushTuple(const std::tuple<TArgs ...>& tuple)
	{
		push(std::get<TTupleIndex>(tuple));
		pushTuple<TTupleIndex + 1>(tuple);
	}
	template<typename ... TArgs>
	inline void push(const std::tuple<TArgs ...>& tuple)
	{
		pushTuple<0, TArgs ...>(tuple);
	}

	template<typename ... TArgs>
	inline void push(const std::variant<TArgs ...>& variant)
	{
		tInteger index = variant.index();
		push(index);
		std::visit([this](auto&& arg){push(arg);}, variant);
	}

	inline const std::vector<uint8_t>& getBuffer()
	{
		return outBuffer;
	}

private:
	std::vector<uint8_t> outBuffer;
};

//

namespace nPopHelper
{
hasFunctionHelper(restore,
                  std::declval<cStreamIn&>());
}

//

inline cStreamIn::cStreamIn(const std::vector<uint8_t>& buffer) :
        inBuffer(buffer),
        inPosition(0),
        failed(false)
{
}

template<typename TType>
inline void cStreamIn::pop(TType& value)
{
	if constexpr (nPopHelper::has_restore<TType>::value)
	{
		if (isFailed())
		{
			return;
		}

		failed = !value.restore(*this);
	}
	else
	{
		if (inBuffer.size() - inPosition < sizeof(TType))
		{
			inPosition = this->inBuffer.size();
			failed = true;
			return;
		}

		fvmtoh(&inBuffer[inPosition], value);

		inPosition += sizeof(TType);
	}
}

inline void cStreamIn::pop(char* buffer, uint64_t bufferSize)
{
	if (this->inBuffer.size() - inPosition < bufferSize)
	{
		inPosition = this->inBuffer.size();
		failed = true;
		return;
	}

	memcpy(buffer, &this->inBuffer[inPosition], bufferSize);

	inPosition += bufferSize;
}

inline void cStreamIn::pop(std::string& value)
{
	tInteger size;
	pop(size);

	if (isFailed())
	{
		return;
	}

	value.reserve(size + 1);
	value.resize(size);
	pop(&value[0], size);
	value[size] = 0;
}

inline void cStreamIn::pop(std::vector<uint8_t>& value)
{
	tInteger size;

	pop(size);
	if (isFailed())
	{
		return;
	}

	value.reserve(size);
	value.resize(size);
	pop((char*)&value[0], size);
}

template<typename TFirstType, typename TSecondType>
inline void cStreamIn::pop(std::pair<TFirstType, TSecondType>& pair)
{
	pop(pair.first);
	pop(pair.second);
}

template<typename TType, std::size_t TSize>
inline void cStreamIn::pop(std::array<TType, TSize>& array)
{
	for (std::size_t i = 0; i < TSize; i++)
	{
		pop(array[i]);
	}
}

template<typename TVectorType>
inline void cStreamIn::pop(std::vector<TVectorType>& vector)
{
	tInteger count;

	pop(count);
	if (isFailed())
	{
		return;
	}

	for (tInteger i = 0; i < count; i++)
	{
		TVectorType vectorValue;

		pop(vectorValue);
		if (isFailed())
		{
			return;
		}

		vector.emplace_back(vectorValue);
	}
}

template<typename TMapFirstType, typename TMapSecondType>
inline void cStreamIn::pop(std::map<TMapFirstType, TMapSecondType>& map)
{
	tInteger count;

	pop(count);
	if (isFailed())
	{
		return;
	}

	for (tInteger i = 0; i < count; i++)
	{
		TMapFirstType firstValue;
		pop(firstValue);
		pop(map[firstValue]);
	}
}

template<typename ... TArgs>
inline void cStreamIn::pop(std::tuple<TArgs ...>& tuple)
{
	popTuple<0>(tuple);
}

template<typename ... TArgs>
inline void cStreamIn::pop(std::variant<TArgs ...>& variant)
{
	tInteger index;

	pop(index);
	if (isFailed())
	{
		return;
	}

	popVariant<0>(variant, index);
}

inline bool cStreamIn::isFailed()
{
	return failed;
}

template<size_t TTupleIndex, typename ... TArgs>
inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
cStreamIn::popTuple(TVM_UNUSED std::tuple<TArgs ...>& tuple)
{
}

template<size_t TTupleIndex, typename ... TArgs>
inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), void>::type
cStreamIn::popTuple(std::tuple<TArgs ...>& tuple)
{
	pop(std::get<TTupleIndex>(tuple));
	popTuple<TTupleIndex + 1>(tuple);
}

template<size_t TVariantIndex, typename ... TArgs>
inline typename std::enable_if<TVariantIndex == sizeof...(TArgs), void>::type
cStreamIn::popVariant(TVM_UNUSED std::variant<TArgs ...>& variant,
                      TVM_UNUSED uint32_t index)
{
	inPosition = this->inBuffer.size();
	failed = true;
}

template<size_t TVariantIndex, typename ... TArgs>
inline typename std::enable_if<TVariantIndex < sizeof...(TArgs), void>::type
cStreamIn::popVariant(std::variant<TArgs ...>& variant,
                      uint32_t index)
{
	if (index == TVariantIndex)
	{
		typename std::tuple_element<TVariantIndex, std::tuple<TArgs ...>>::type value;

		pop(value);
		if (isFailed())
		{
			return;
		}

		variant = value;
	}
	else
	{
		popVariant<TVariantIndex + 1>(variant, index);
	}
}

//

namespace nPushHelper
{
hasFunctionHelper(save,
                  std::declval<cStreamOut&>());
}

//

template<typename TType>
inline void cStreamOut::push(const TType& value)
{
	if constexpr (nPushHelper::has_save<TType>::value)
	{
		value.save(*this);
	}
	else
	{
		tInteger size = outBuffer.size();
		outBuffer.resize(size + sizeof(TType));
		htofvm(value, &outBuffer[size]);
	}
}

}

#endif // TVM_STREAM_H
