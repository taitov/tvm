#include <iostream>

#include <tvm/engine.h>

#include "logic_print2.h"

using namespace nVirtualMachine;

cLogicPrint2::cLogicPrint2(const std::string& value) :
        value(value)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_19_" << std::endl;
}

tFlowId cLogicPrint2::signalEntryPrint()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_20_"
	          << *string1
	          << *string2
	          << *string3
	          << value
	          << std::endl;
	return flow;
}
