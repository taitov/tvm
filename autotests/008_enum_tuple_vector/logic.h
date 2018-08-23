#ifndef LOGIC_H
#define LOGIC_H

#include <iostream>

#include <tvm/register.h>

#include "memory.h"

namespace tvm
{

std::string checkString;

class cLogicPrintLine
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "printLine";

		signalEntries.insert("signal",
		                     &cLogicPrintLine::signalEntryPrint);

		memoryEntries.insert("line",
		                     "line",
		                     &cLogicPrintLine::line);
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (line)
		{
			checkString += std::to_string(std::get<0>(*line).value) + ": " + std::get<1>(*line) + "\n";
		}

		return flowNowhere;
	}

protected:
	const cMemoryLine* line;
};

class cLogicPrintLines
{
public:
	tvmRegisterLogicModule()
	{
		moduleName = "printLines";

		signalEntries.insert("signal",
		                     &cLogicPrintLines::signalEntryPrint);

		memoryEntries.insert("lines",
		                     "lines",
		                     &cLogicPrintLines::lines);

		memoryEntries.insert("printType",
		                     "printType",
		                     &cLogicPrintLines::printType);
	}

protected:
	tFlowId signalEntryPrint()
	{
		if (lines)
		{
			cMemoryEnumPrintType printType = cMemoryEnumPrintType::oneLine;
			if (this->printType)
			{
				printType = *(this->printType);
			}

			for (const cMemoryLine& line : *lines)
			{
				checkString += std::to_string(std::get<0>(line).value) + ": " + std::get<1>(line);
				if (printType == cMemoryEnumPrintType::multipleLine)
				{
					checkString += "\n";
				}
			}

			if (printType == cMemoryEnumPrintType::oneLineWithEndLine)
			{
				checkString += "\n";
			}
		}

		return flowNowhere;
	}

protected:
	const cMemoryVectorLines* lines;
	const cMemoryEnumPrintType* printType;
};

//class cLogicLineSetId
//{
//public:
//	tvmRegisterLogicModule()
//	{
//		moduleName = "lineSetId";

//		signalEntries.insert("signal",
//		                     &cLogicLineSetId::signalEntrySetId);

//		memoryEntries.insert("lines",
//		                     "lines",
//		                     &cLogicLineSetId::lines);
//	}

//protected:
//	tFlowId signalEntrySetId()
//	{
//		if (lines)
//		{
//			cMemoryEnumPrintType printType = cMemoryEnumPrintType::oneLine;
//			if (this->printType)
//			{
//				printType = *(this->printType);
//			}

//			for (const cMemoryLine& line : *lines)
//			{
//				checkString += std::to_string(std::get<0>(line).value) + ": " + std::get<1>(line);
//				if (printType == cMemoryEnumPrintType::multipleLine)
//				{
//					checkString += "\n";
//				}
//			}

//			if (printType == cMemoryEnumPrintType::oneLineWithEndLine)
//			{
//				checkString += "\n";
//			}
//		}

//		return flowNowhere;
//	}

//protected:
//	const cMemoryInteger* lines;
//};

}

#endif // LOGIC_H
