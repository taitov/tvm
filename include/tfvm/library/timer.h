// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_LIBRARY_TIMER_H
#define TFVM_LIBRARY_TIMER_H

#include <stdlib.h>
#include <unistd.h>

#include <tfvm/library.h>

namespace nVirtualMachine
{

namespace nLibrary
{

class cTimer : public cLibrary
{
public:
	cTimer(unsigned int seconds) :
	        seconds(seconds)
	{
	}

	bool registerLibrary() override
	{
		setLibraryName("timer_" + std::to_string(seconds) + "s");

		if (!registerRootModules(rootTouch))
		{
			return false;
		}

		return true;
	}

	void run() override
	{
		while (!isStopped())
		{
			sleep(seconds);
			rootSignalFlow(rootTouch.signal);
		}
	}

private: /** rootModules */
	class cRootTouch : public cRootModule
	{
	public:
		bool registerModule() override
		{
			setModuleName("touch");

			if (!registerSignalExit("signal", signal))
			{
				return false;
			}

			return true;
		}

		tRootSignalExitId signal;
	};

private:
	unsigned int seconds;

private:
	cRootTouch rootTouch;
};

}

}

#endif // TFVM_LIBRARY_TIMER_H
