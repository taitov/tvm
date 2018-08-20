#include <iostream>

#include <tvm/engine.h>

#include "logic_print1.h"

using namespace nVirtualMachine;

cLogicPrint1::cLogicPrint1(const std::string& value) :
        value(value)
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_17_" << std::endl;
}

tFlowId cLogicPrint1::signalEntryPrint()
{
	std::cout << "_TEXT_ONLY_FOR_ENGINE_18_"
	          << *string1
	          << *string2
	          << *string3
	          << value
	          << std::endl;
	return flow;
}
