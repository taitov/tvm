#include <iostream>

#include <tvm/engine.h>

#include "root_start1.h"

using namespace tvm;

cRootStart1::cRootStart1(const std::string& value) :
        value(value)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_13_" << std::endl;
}

void cRootStart1::functionOnlyForEngine(cEngine* engine) const
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_14_" << std::endl;
	engine->rootSignal(rootSignalExit);
}
