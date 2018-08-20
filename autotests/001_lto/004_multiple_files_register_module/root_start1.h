#ifndef ROOT_START1_H
#define ROOT_START1_H

#include <string>

#include <tvm/register.h>

#include "memory_string1.h"

namespace nVirtualMachine
{

class cRootStart1
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_5_");

		registerModule.insertRootSignalExit("_TEXT_ONLY_FOR_CONVERTER_6_", ///< signal name
		                                    &cRootStart1::rootSignalExit); ///< store signal id

		registerModule.insertRootMemoryExit("_TEXT_ONLY_FOR_CONVERTER_7_", ///< memory exit name
		                                    "_TEXT_ONLY_FOR_CONVERTER_8_", ///< memory module name
		                                    &cRootStart1::memoryExit); ///< store pointer
	}

public:
	cRootStart1(const std::string& value);

public:
	void functionOnlyForEngine(cEngine* engine) const;

public:
	tRootSignalExitId rootSignalExit;

public:
	cMemoryString1* memoryExit;

protected:
	std::string value;
};

}

#endif // ROOT_START1_H
