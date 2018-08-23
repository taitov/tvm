#ifndef VM_H
#define VM_H

#include <iostream>
#include <string>

#include <tvm/register.h>

namespace tvm
{

class cMemoryString : public std::string
{
public:
	tvmRegisterMemoryModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_1_");
		registerModule.setMemoryAsString();
	}

public:
	bool restore(cStreamIn& stream)
	{
		stream.pop(*(std::string*)this);
		return true;
	}

	void save(cStreamOut& stream)
	{
		stream.push(*(std::string*)this);
	}

public:
	cMemoryString(const char* string) :
	        std::string(string)
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_8_" << std::endl;
	}
};

class cRootStart
{
public:
	cRootStart(const std::string& string) :
	        string4(string)
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_6_" << std::endl;
	}

public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_3_");
		registerModule.setCaptionName("_TEXT_ONLY_FOR_CONVERTER_4_");

		registerModule.insertRootSignalExit("_TEXT_ONLY_FOR_CONVERTER_5_",
		                                    &cRootStart::signal);

		registerModule.insertRootMemoryExit("_TEXT_ONLY_FOR_CONVERTER_17_",
		                                    "_TEXT_ONLY_FOR_CONVERTER_18_",
		                                    &cRootStart::string);
	}

public:
	tRootSignalExitId signal;

public:
	cMemoryString* string;

protected:
	std::string string4;
};

class cLogicPrint
{
public:
	cLogicPrint(const std::string& string) :
	        string4(string)
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_7_" << std::endl;
	}

public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_6_");
		registerModule.setCaptionName("_TEXT_ONLY_FOR_CONVERTER_7_");

		registerModule.insertSignalEntry("_TEXT_ONLY_FOR_CONVERTER_8_",
		                                 &cLogicPrint::signalPrint1);

		registerModule.insertSignalEntry("_TEXT_ONLY_FOR_CONVERTER_9_",
		                                 &cLogicPrint::signalPrint2);

		registerModule.insertMemoryEntry("_TEXT_ONLY_FOR_CONVERTER_10_",
		                                 "_TEXT_ONLY_FOR_CONVERTER_11_",
		                                 &cLogicPrint::string1);

		registerModule.insertSignalExit("_TEXT_ONLY_FOR_CONVERTER_12_",
		                                &cLogicPrint::flow);

		registerModule.insertMemoryExit("_TEXT_ONLY_FOR_CONVERTER_13_",
		                                "_TEXT_ONLY_FOR_CONVERTER_14_",
		                                &cLogicPrint::string2);

		registerModule.insertMemoryEntryExit("_TEXT_ONLY_FOR_CONVERTER_15_",
		                                     "_TEXT_ONLY_FOR_CONVERTER_16_",
		                                     &cLogicPrint::string3);
	}

protected:
	tFlowId signalPrint1()
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_1_"
		          << *string1
		          << *string2
		          << *string3
		          << string4
		          << std::endl;
		return flow;
	}

	tFlowId signalPrint2()
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_2_"
		          << *string1
		          << *string2
		          << *string3
		          << string4
		          << std::endl;
		return flowNowhere;
	}

protected:
	const cMemoryString* string1;
	cMemoryString* string2;
	cMemoryString* string3;

protected:
	tFlowId flow;

protected:
	std::string string4;
};

eResult registerModules(cRegister& virtualMachine)
{
	virtualMachine.registerMemoryModule<cMemoryString>("_TEXT_ONLY_FOR_ENGINE_3_");
	virtualMachine.registerRootModule<cRootStart>("_TEXT_ONLY_FOR_ENGINE_5_");
	virtualMachine.registerLogicModule<cLogicPrint>("_TEXT_ONLY_FOR_ENGINE_4_");
	return virtualMachine.getRegisterResult();
}

}

#endif // VM_H
