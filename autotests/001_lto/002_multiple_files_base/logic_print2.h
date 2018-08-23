#ifndef LOGIC_PRINT2_H
#define LOGIC_PRINT2_H

#include <string>

#include <tvm/common.h>

namespace tvm
{

class cLogicPrint2
{
public:
	cLogicPrint2(const std::string& string);

	tFlowId signalPrint1();
	tFlowId signalPrint2();

public:
	const std::string* string1;
	std::string* string2;
	std::string* string3;

public:
	tFlowId flow;

protected:
	std::string string4;
};

}

#endif // LOGIC_PRINT2_H
