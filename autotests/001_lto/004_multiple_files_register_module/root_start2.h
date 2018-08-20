#ifndef ROOT_START2_H
#define ROOT_START2_H

#include <string>

#include <tvm/register.h>

#include "memory_string2.h"

namespace nVirtualMachine
{

class cRootStart2
{
public:
	tvmRegisterRootModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_9_");

		registerModule.insertRootSignalExit("_TEXT_ONLY_FOR_CONVERTER_10_", ///< signal name
		                                    &cRootStart2::rootSignalExit); ///< store signal id

		registerModule.insertRootMemoryExit("_TEXT_ONLY_FOR_CONVERTER_11_", ///< memory exit name
		                                    "_TEXT_ONLY_FOR_CONVERTER_12_", ///< memory module name
		                                    &cRootStart2::memoryExit); ///< store pointer
	}

public:
	cRootStart2(const std::string& value);

public:
	void functionOnlyForEngine(const cEngine& engine) const;
	void functionOnlyForEngineMemory(const std::string& value) const;

protected:
	tRootSignalExitId rootSignalExit;

protected:
	cMemoryString2* memoryExit;

protected:
	std::string value;
};

}

#endif // ROOT_START2_H
