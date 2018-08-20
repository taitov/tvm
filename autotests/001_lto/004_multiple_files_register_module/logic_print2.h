#ifndef LOGIC_PRINT2_H
#define LOGIC_PRINT2_H

#include <string>

#include <tvm/register.h>

namespace nVirtualMachine
{

class cLogicPrint2
{
public:
	cLogicPrint2(const std::string& value);

public:
	tvmRegisterLogicModule()
	{
		registerModule.setModuleName("_TEXT_ONLY_FOR_CONVERTER_23_");
		registerModule.setCaptionName("_TEXT_ONLY_FOR_CONVERTER_24_");

		registerModule.insertSignalEntry("_TEXT_ONLY_FOR_CONVERTER_25_",
		                                 &cLogicPrint2::signalEntryPrint);

		registerModule.insertMemoryEntry("_TEXT_ONLY_FOR_CONVERTER_26_",
		                                 "_TEXT_ONLY_FOR_CONVERTER_27_",
		                                 &cLogicPrint2::string1);

		registerModule.insertSignalExit("_TEXT_ONLY_FOR_CONVERTER_28_",
		                                &cLogicPrint2::flow);

		registerModule.insertMemoryExit("_TEXT_ONLY_FOR_CONVERTER_29_",
		                                "_TEXT_ONLY_FOR_CONVERTER_30_",
		                                &cLogicPrint2::string2);

		registerModule.insertMemoryEntryExit("_TEXT_ONLY_FOR_CONVERTER_31_",
		                                     "_TEXT_ONLY_FOR_CONVERTER_32_",
		                                     &cLogicPrint2::string3);
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

#endif // LOGIC_PRINT2_H
