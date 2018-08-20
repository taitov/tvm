#ifndef MEMORY_H
#define MEMORY_H

#include <tvm/register.h>

namespace nVirtualMachine
{

class cMemoryEnumPrintType
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "printType";

		valueType.enumInsertValue("oneLine", oneLine);
		valueType.enumInsertValue("oneLineWithEndLine", oneLineWithEndLine);
		valueType.enumInsertValue("multipleLine", multipleLine);
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(value);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(value);
	}

public:
	enum eEnum : uint8_t ///< @todo: int16_t
	{
		oneLine,
		oneLineWithEndLine,
		multipleLine,
	};

public:
	cMemoryEnumPrintType() = default;

	cMemoryEnumPrintType(eEnum value) :
	        value(value)
	{
	}

	bool operator==(eEnum secondValue) const
	{
		return value == secondValue;
	}

protected:
	eEnum value;
};

class cMemoryInteger
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "integer";
		valueType.valueSetType("int32");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(value);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(value);
	}

public:
	cMemoryInteger()
	{
	}

	cMemoryInteger(int32_t value) :
	        value(value)
	{
	}

public:
	int32_t value;
};

class cMemoryString : public std::string
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "string";
		valueType.valueSetType("string");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::string*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::string*)this);
	}

public:
	using std::string::string;
};

class cMemoryLine : public std::tuple<cMemoryInteger, cMemoryString>
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "line";

		valueType.tupleInsertMemoryModule("id", "integer");
		valueType.tupleInsertMemoryModule("login", "string");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::tuple<cMemoryInteger, cMemoryString>*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::tuple<cMemoryInteger, cMemoryString>*)this);
	}

public:
	using std::tuple<cMemoryInteger, cMemoryString>::tuple;
};

class cMemoryVectorLines : public std::vector<cMemoryLine>
{
public:
	tvmRegisterMemoryModule()
	{
		moduleName = "lines";
		valueType.vectorSetMemoryModule("line");
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::vector<cMemoryLine>*)this);
		return true;
	}

	void save(cStreamOut& stream) const
	{
		stream.push(*(std::vector<cMemoryLine>*)this);
	}

public:
	using std::vector<cMemoryLine>::vector;
};

}

#endif // MEMORY_H
