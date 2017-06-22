// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_LIBRARY_CONSOLE_H
#define TFVM_LIBRARY_CONSOLE_H

#include <vector>

#include <stdlib.h>
#include <unistd.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cConsole : public cLibrary
{
public:
	cConsole()
	{
	}

	bool registerLibrary() override
	{
		setLibraryName("console");

		if (!registerModules(new cLogicPrint(),
		                     new cLogicPrintLine()))
		{
			return false;
		}

		return true;
	}

private: /** modules */
	class cLogicPrint : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicPrint();
		}

		bool registerModule() override
		{
			setModuleName("print");

			if (!registerSignalEntry("signal", &cLogicPrint::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("string", "string", string))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (string)
			{
				std::cout << *string;
				std::cout << std::flush;
			}
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		std::string* string;
	};

	class cLogicPrintLine : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicPrintLine();
		}

		bool registerModule() override
		{
			setModuleName("printLine");

			if (!registerSignalEntry("signal", &cLogicPrintLine::signalEntry))
			{
				return false;
			}

			if (!registerMemoryEntry("string", "string", string))
			{
				return false;
			}

			if (!registerSignalExit("signal", signalExit))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntry()
		{
			if (string)
			{
				std::cout << *string;
			}
			std::cout << std::endl;
			std::cout << std::flush;
			return signalFlow(signalExit);
		}

	private:
		const tSignalExitId signalExit = 1;

	private:
		std::string* string;
	};
};

}

}

#endif // TFVM_LIBRARY_CONSOLE_H
