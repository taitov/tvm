#ifndef VM_H
#define VM_H

#include <iostream>
#include <string>

#include <tvm/register.h>

namespace tvm
{

class cRootStart
{
/// @todo: cRootStart()
public:
	tRootSignalExitId rootSignalExit;

public:
	std::string* string;
};

class cLogicPrint
{
public:
	cLogicPrint(const std::string& string) :
	        string4(string)
	{
		std::cout << "_TEXT_ONLY_FOR_ENGINE_5_" << std::endl;
	}

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

public:
	const std::string* string1;
	std::string* string2;
	std::string* string3;

public:
	tFlowId flow;

protected:
	std::string string4;
};

eResult registerModules(cRegister& virtualMachine)
{
	eResult result;

	result = virtualMachine.registerMemoryModuleBase<std::string>("_TEXT_ONLY_FOR_CONVERTER_1_",
	                                                              {"_TEXT_ONLY_FOR_CONVERTER_2_",
	                                                               "_TEXT_ONLY_FOR_CONVERTER_17_"},
	                                                              "_TEXT_ONLY_FOR_ENGINE_3_");
	if (result != eResult::success)
	{
		return result;
	}

	/// @todo: vector, tuple, enum

	result = virtualMachine.registerRootModuleBase<cRootStart>("_TEXT_ONLY_FOR_CONVERTER_3_",
	                                                           "_TEXT_ONLY_FOR_CONVERTER_4_",
	                                                           {{"_TEXT_ONLY_FOR_CONVERTER_5_",
	                                                             &cRootStart::rootSignalExit}},
	                                                           {{"_TEXT_ONLY_FOR_CONVERTER_17_",
	                                                             "_TEXT_ONLY_FOR_CONVERTER_18_",
	                                                             reinterpret_cast<void* (cRootStart::*)>(&cRootStart::string)}});
	if (result != eResult::success)
	{
		return result;
	}

	result = virtualMachine.registerLogicModuleBase<cLogicPrint>("_TEXT_ONLY_FOR_CONVERTER_6_",
	                                                             "_TEXT_ONLY_FOR_CONVERTER_7_",
	                                                             {{"_TEXT_ONLY_FOR_CONVERTER_8_",
	                                                               &cLogicPrint::signalPrint1},
	                                                              {"_TEXT_ONLY_FOR_CONVERTER_9_",
	                                                               &cLogicPrint::signalPrint2}},
	                                                             {{"_TEXT_ONLY_FOR_CONVERTER_10_",
	                                                               "_TEXT_ONLY_FOR_CONVERTER_11_",
	                                                               reinterpret_cast<const void* (cLogicPrint::*)>(&cLogicPrint::string1)}},
	                                                             {{"_TEXT_ONLY_FOR_CONVERTER_12_",
	                                                               &cLogicPrint::flow}},
	                                                             {{"_TEXT_ONLY_FOR_CONVERTER_13_",
	                                                               "_TEXT_ONLY_FOR_CONVERTER_14_",
	                                                               reinterpret_cast<void* (cLogicPrint::*)>(&cLogicPrint::string2)}},
	                                                             {{"_TEXT_ONLY_FOR_CONVERTER_15_",
	                                                               "_TEXT_ONLY_FOR_CONVERTER_16_",
	                                                               reinterpret_cast<void* (cLogicPrint::*)>(&cLogicPrint::string3)}},
	                                                             "_TEXT_ONLY_FOR_ENGINE_4_");
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

}

#endif // VM_H
