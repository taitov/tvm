#ifndef VM_H
#define VM_H

#include <iostream>
#include <string>

#include <tvm/register.h>

#include "root_start1.h"
#include "logic_print1.h"
#include "logic_print2.h"

namespace nVirtualMachine
{

eResult registerModules(cRegister& virtualMachine)
{
	eResult result;

	result = virtualMachine.registerMemoryModuleBase<std::string>("_TEXT_ONLY_FOR_CONVERTER_1_",
	                                                              {"_TEXT_ONLY_FOR_CONVERTER_2_",
	                                                               "_TEXT_ONLY_FOR_CONVERTER_30_"},
	                                                              "_TEXT_ONLY_FOR_ENGINE_3_");
	if (result != eResult::success)
	{
		return result;
	}

	result = virtualMachine.registerRootModuleBase<cRootStart1>("_TEXT_ONLY_FOR_CONVERTER_3_",
	                                                            "_TEXT_ONLY_FOR_CONVERTER_4_",
	                                                            {{"_TEXT_ONLY_FOR_CONVERTER_5_",
	                                                              &cRootStart1::rootSignalExit}},
	                                                            {{"_TEXT_ONLY_FOR_CONVERTER_28_",
	                                                              "_TEXT_ONLY_FOR_CONVERTER_29_",
	                                                              reinterpret_cast<void* (cRootStart1::*)>(&cRootStart1::string)}});
	if (result != eResult::success)
	{
		return result;
	}

	result = virtualMachine.registerLogicModuleBase<cLogicPrint1>("_TEXT_ONLY_FOR_CONVERTER_6_",
	                                                              "_TEXT_ONLY_FOR_CONVERTER_7_",
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_8_",
	                                                                &cLogicPrint1::signalPrint1},
	                                                               {"_TEXT_ONLY_FOR_CONVERTER_9_",
	                                                                &cLogicPrint1::signalPrint2}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_10_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_11_",
	                                                                reinterpret_cast<const void* (cLogicPrint1::*)>(&cLogicPrint1::string1)}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_12_",
	                                                                &cLogicPrint1::flow}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_13_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_14_",
	                                                                reinterpret_cast<void* (cLogicPrint1::*)>(&cLogicPrint1::string2)}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_15_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_16_",
	                                                                reinterpret_cast<void* (cLogicPrint1::*)>(&cLogicPrint1::string3)}},
	                                                              "_TEXT_ONLY_FOR_ENGINE_4_");
	if (result != eResult::success)
	{
		return result;
	}

	result = virtualMachine.registerLogicModuleBase<cLogicPrint2>("_TEXT_ONLY_FOR_CONVERTER_17_",
	                                                              "_TEXT_ONLY_FOR_CONVERTER_18_",
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_19_",
	                                                                &cLogicPrint2::signalPrint1},
	                                                               {"_TEXT_ONLY_FOR_CONVERTER_20_",
	                                                                &cLogicPrint2::signalPrint2}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_21_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_22_",
	                                                                reinterpret_cast<const void* (cLogicPrint2::*)>(&cLogicPrint2::string1)}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_23_",
	                                                                &cLogicPrint2::flow}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_24_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_25_",
	                                                                reinterpret_cast<void* (cLogicPrint2::*)>(&cLogicPrint2::string2)}},
	                                                              {{"_TEXT_ONLY_FOR_CONVERTER_26_",
	                                                                "_TEXT_ONLY_FOR_CONVERTER_27_",
	                                                                reinterpret_cast<void* (cLogicPrint2::*)>(&cLogicPrint2::string3)}},
	                                                              "_TEXT_ONLY_FOR_ENGINE_5_");
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

}

#endif // VM_H
