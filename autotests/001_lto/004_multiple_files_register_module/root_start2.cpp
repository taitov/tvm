#include <iostream>

#include <tvm/engine.h>

#include "root_start2.h"

using namespace nVirtualMachine;

cRootStart2::cRootStart2(const std::string& value) :
        value(value)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_15_" << std::endl;
}

void cRootStart2::functionOnlyForEngine(const cEngine& engine) const
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_16_" << std::endl;
	engine.rootSignal(rootSignalExit);
}

void cRootStart2::functionOnlyForEngineMemory(const std::string& value) const
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_23_" << std::endl;
	*(std::string*)memoryExit = value;
}
