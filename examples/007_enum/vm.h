#ifndef VM_H
#define VM_H

#include <iostream>

#include <tvm/register.h>

namespace tvm
{

class cMemoryEnumType
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("type");

		registerModule.setMemoryAsEnumInsertValue("one", one);
		registerModule.setMemoryAsEnumInsertValue("two", two);
		registerModule.setMemoryAsEnumInsertValue("three", three);
		registerModule.setMemoryAsEnumInsertValue("four", four);
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
	enum eEnum
	{
		one = 12309,
		two = -1,
		three = 8800,
		four = 5553535,
	};

public:
	cMemoryEnumType() = default;

	cMemoryEnumType(eEnum value) :
	        value(value)
	{
	}

	bool operator==(const eEnum& secondValue) const
	{
		return value == secondValue;
	}

protected:
	eEnum value;
};

class cRootStart ///< root module
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("start");

		registerModule.insertRootSignalExit("signal", ///< signal name
		                                    &cRootStart::rootSignalExit); ///< store signal id
	}

public:
	tRootSignalExitId rootSignalExit;
};

class cLogicPrint ///< logic module
{
public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("print");

		registerModule.insertSignalEntry("signal", ///< signal name
		                                 &cLogicPrint::signalEntryPrint); ///< callback

		registerModule.insertMemoryEntry("type", ///< memory entry name
		                                 "type", ///< memory module name
		                                 &cLogicPrint::type); ///< store pointer
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (type)
		{
			if (*type == cMemoryEnumType::one)
			{
				std::cout << "cMemoryEnumType::one" << std::endl;
			}
			else if (*type == cMemoryEnumType::two)
			{
				std::cout << "cMemoryEnumType::two" << std::endl;
			}
			else if (*type == cMemoryEnumType::three)
			{
				std::cout << "cMemoryEnumType::three" << std::endl;
			}
			else if (*type == cMemoryEnumType::four)
			{
				std::cout << "cMemoryEnumType::four" << std::endl;
			}
			else
			{
				std::cout << "fail" << std::endl;
			}
		}
		return flowNowhere;
	}

protected:
	const cMemoryEnumType* type;
};

eResult registerModules(cRegister& virtualMachine)
{
	/// @todo: use registerMemoryModuleBase() for example

	virtualMachine.registerMemoryModule<cMemoryEnumType>();
	virtualMachine.registerRootModule<cRootStart>();
	virtualMachine.registerLogicModule<cLogicPrint>();

	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
