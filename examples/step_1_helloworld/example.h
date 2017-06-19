#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <tfvm/library.h>

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

		if (!registerRootSignalExit("run", "signal", rootSignalExitRun))
		{
			return false;
		}

		if (!registerModules(new cLogicPrint()))
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

			return true;
		}

	private: /** signalEntries */
		bool signalEntryPrint()
		{
			printf("Hello world\n");
			return true;
		}
	};
};

}

}

#endif // EXAMPLE_H
