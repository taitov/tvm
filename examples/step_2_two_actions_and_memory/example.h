#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <tvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cExample : public cLibrary
{
public:
	bool registerLibrary() override
	{
		setLibraryName("example");

		if (!registerMemory<std::string>("string", ""))
		{
			return false;
		}

		if (!registerRootSignalExit("run", "signal", rootSignalExitRun))
		{
			return false;
		}

		if (!registerModules(new cLogicPrint(),
		                     new cLogicSet()))
		{
			return false;
		}

		return true;
	}

	void run() override
	{
		rootSignalFlow(rootSignalExitRun);
	}

private:
	tRootSignalExitId rootSignalExitRun;

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

			if (!registerSignalEntry("print", &cLogicPrint::signalEntryPrint))
			{
				return false;
			}

			if (!registerMemoryEntry("string", "string", string))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntryPrint()
		{
			if (string)
			{
				printf("%s\n", string->c_str());
			}
			return true;
		}

	private:
		std::string* string;
	};

	class cLogicSet : public cLogicModule
	{
	public:
		cModule* clone() const override
		{
			return new cLogicSet();
		}

		bool registerModule() override
		{
			setModuleName("set");

			if (!registerSignalEntry("signal", &cLogicSet::signalEntrySignal))
			{
				return false;
			}

			if (!registerSignalExit("done", signalExitDone))
			{
				return false;
			}

			if (!registerMemoryExit("string", "string", string))
			{
				return false;
			}

			return true;
		}

	private: /** signalEntries */
		bool signalEntrySignal()
		{
			if (string)
			{
				*string = "Hello from 'cLogicSet'";
			}
			return signalFlow(signalExitDone);
		}

	private:
		const tSignalExitId signalExitDone = 1;

	private:
		std::string* string;
	};
};

}

}

#endif // EXAMPLE_H
