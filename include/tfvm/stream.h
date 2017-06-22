// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_STREAM_H
#define TFVM_STREAM_H

#include <string>
#include <vector>
#include <map>

#include "type.h"

namespace nVirtualMachine
{

class cStreamIn
{
public:
	cStreamIn(const std::vector<char>& buffer)
	{
		this->in.buffer = buffer;
		this->in.position = 0;
		failed = false;
	}

	inline void pop(uint8_t& value)
	{
		using tType = uint8_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(uint16_t& value)
	{
		using tType = uint16_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(uint32_t& value)
	{
		using tType = uint32_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(uint64_t& value)
	{
		using tType = uint64_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(int8_t& value)
	{
		using tType = int8_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(int16_t& value)
	{
		using tType = int16_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(int32_t& value)
	{
		using tType = int32_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(int64_t& value)
	{
		using tType = int64_t;

		if (in.buffer.size() - in.position < sizeof(tType))
		{
			in.buffer.clear();
			value = 0;
			failed = true;
			return;
		}

		value = *(tType*)&in.buffer[in.position];

		in.position += sizeof(tType);
	}
	inline void pop(bool& value)
	{
		pop((uint8_t&)value);
	}
	inline void pop(char* buffer, uint64_t bufferSize)
	{
		if (in.buffer.size() - in.position < bufferSize)
		{
			in.buffer.clear();
			failed = true;
			return;
		}

		memcpy(buffer, &in.buffer[in.position], bufferSize);

		in.position += bufferSize;
	}
	inline void pop(std::string& value)
	{
		uint32_t size;
		pop(size);
		value.reserve(size + 1);
		value.resize(size);
		pop(&value[0], size);
		value[size] = 0;
	}
	inline void pop(std::vector<char>& value)
	{
		uint32_t size;
		pop(size);
		value.reserve(size);
		value.resize(size);
		pop(&value[0], size);
	}

	template<typename TFirstType, typename TSecondType>
	inline void pop(std::pair<TFirstType, TSecondType>& pair)
	{
		pop(pair.first);
		pop(pair.second);
	}

	template<typename TType, std::size_t TSize>
	inline void pop(std::array<TType, TSize>& array)
	{
		for (uint64_t i = 0; i < TSize; i++)
		{
			TType value;
			pop(value);
			array[i] = value;
		}
	}

	template<typename TVectorType>
	inline void pop(std::vector<TVectorType>& vector)
	{
		uint32_t count;
		pop(count);
		for (uint64_t i = 0; i < count; i++)
		{
			TVectorType vectorValue;
			pop(vectorValue);
			vector.push_back(vectorValue);
		}
	}

	template<typename TMapFirstType, typename TMapSecondType>
	inline void pop(std::map<TMapFirstType, TMapSecondType>& map)
	{
		uint32_t count;
		pop(count);
		for (uint64_t i = 0; i < count; i++)
		{
			TMapFirstType firstValue;
			pop(firstValue);
			pop(map[firstValue]);
		}
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	popTuple(std::tuple<TArgs ...>& tuple)
	{
	}
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), void>::type
	popTuple(std::tuple<TArgs ...>& tuple)
	{
		pop(std::get<TTupleIndex>(tuple));
		popTuple<TTupleIndex + 1>(tuple);
	}
	template<typename ... TArgs>
	inline void pop(std::tuple<TArgs ...>& tuple)
	{
		popTuple<0, TArgs ...>(tuple);
	}

	inline void pop(...)
	{
	}

	bool isFailed()
	{
		return failed;
	}

private:
	struct
	{
		std::vector<char> buffer;
		uint64_t position;
	} in;

	bool failed;
};

class cStreamOut
{
public:
	cStreamOut()
	{
	}

	inline void push(const uint8_t& value)
	{
		using tType = uint8_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const uint16_t& value)
	{
		using tType = uint16_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const uint32_t& value)
	{
		using tType = uint32_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const uint64_t& value)
	{
		using tType = uint64_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const int8_t& value)
	{
		using tType = int8_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const int16_t& value)
	{
		using tType = int16_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const int32_t& value)
	{
		using tType = int32_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const int64_t& value)
	{
		using tType = int64_t;

		uint64_t size = out.buffer.size();

		out.buffer.resize(size + sizeof(tType));

		*(tType*)(&out.buffer[size]) = value;
	}
	inline void push(const bool& value)
	{
		push((const uint8_t&)value);
	}
	inline void push(const char* buffer, uint64_t bufferSize)
	{
		uint64_t size = out.buffer.size();

		out.buffer.resize(size + bufferSize);

		memcpy(&out.buffer[size], buffer, bufferSize);
	}
	inline void push(const std::string& value)
	{
		uint32_t size = value.length();
		push(size);
		push(value.c_str(), size);
	}
	inline void push(const std::vector<char>& value)
	{
		uint32_t size = value.size();
		push(size);
		push(value.data(), size);
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
		for (uint64_t i = 0; i < TSize; i++)
		{
			push(array[i]);
		}
	}

	template<typename TVectorType>
	inline void push(const std::vector<TVectorType>& vector)
	{
		uint32_t count = vector.size();
		push(count);
		for (uint64_t i = 0; i < count; i++)
		{
			push(vector[i]);
		}
	}

	template<typename TMapFirstType, typename TMapSecondType>
	inline void push(const std::map<TMapFirstType, TMapSecondType>& map)
	{
		uint32_t count = map.size();
		push(count);
		for (auto& iter : map)
		{
			push(iter.first);
			push(iter.second);
		}
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	pushTuple(const std::tuple<TArgs ...>& tuple)
	{
	}
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), void>::type
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

	inline void push(...)
	{
	}

	const std::vector<char>& getBuffer()
	{
		return out.buffer;
	}

private:
	static constexpr uint32_t chunk = 8192;

	struct
	{
		std::vector<char> buffer;
	} out;
};

}

#endif // TFVM_STREAM_H
