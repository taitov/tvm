#ifndef LOGIC_PRINT1_H
#define LOGIC_PRINT1_H

#include <string>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cLogicPrint1
{
public:
	cLogicPrint1(const std::string& value);

public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_13_");
		registerModule.setCaptionName("_TEXT_ONLY_FOR_CONVERTER_14_");

		registerModule.insertSignalEntry("_TEXT_ONLY_FOR_CONVERTER_15_",
		                                 &cLogicPrint1::signalEntryPrint);

		registerModule.insertMemoryEntry("_TEXT_ONLY_FOR_CONVERTER_16_",
		                                 "_TEXT_ONLY_FOR_CONVERTER_17_",
		                                 &cLogicPrint1::string1);

		registerModule.insertSignalExit("_TEXT_ONLY_FOR_CONVERTER_18_",
		                                &cLogicPrint1::flow);

		registerModule.insertMemoryExit("_TEXT_ONLY_FOR_CONVERTER_19_",
		                                "_TEXT_ONLY_FOR_CONVERTER_20_",
		                                &cLogicPrint1::string2);

		registerModule.insertMemoryEntryExit("_TEXT_ONLY_FOR_CONVERTER_21_",
		                                     "_TEXT_ONLY_FOR_CONVERTER_22_",
		                                     &cLogicPrint1::string3);
	}

protected:
	tFlowId signalEntryPrint();

protected:
	const std::string* string1;
	std::string* string2;
	std::string* string3;

protected:
	tFlowId flow;

protected:
	std::string value;
};

}

#endif // LOGIC_PRINT1_H
