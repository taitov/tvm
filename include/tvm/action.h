// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_ACTION_H
#define TVM_ACTION_H

#include "module.h"
#include "signal.h"

namespace nVirtualMachine
{

class cActionModule: public cModule
{
public:
	virtual ~cActionModule()
	{
		thread.stop();
	}

	const tModuleTypeName getModuleTypeName() const override final
	{
		return "action";
	}

protected:
	template<typename TObject>
	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         bool (TObject::* callback)()) = delete;

	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         const tSignalEntryId signalEntryId);

protected: /** exec */
	virtual bool signalEntry(const tSignalEntryId& signalEntryId);
	virtual void run();
	virtual void stop();

	inline bool signalFlow(tSignalExitId signalExitId);

private:
	bool doSignalEntry(const tSignalEntryId& signalEntryId);

protected:
	class cSimpleThread
	{
	public:
		cSimpleThread()
		{
			thread = 0;
		}

		void run(cActionModule* module)
		{
			if (isRunning())
			{
				return;
			}

			this->module = module;

			pthread_attr_t attr;

			if (pthread_attr_init(&attr) != 0)
			{
				return;
			}
			if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0)
			{
				return;
			}

			if (pthread_create(&thread, &attr, &callHelper, this) != 0)
			{
				pthread_attr_destroy(&attr);
				return;
			}

			pthread_attr_destroy(&attr);
		}

		bool isRunning()
		{
			return thread;
		}

		void stop()
		{
			if (isRunning())
			{
				module->stop();
				pthread_cancel(thread);
				thread = 0;
			}
		}

	private:
		static void* callHelper(void* args);

		pthread_t thread;
		cActionModule* module;
	};

	cSimpleThread thread;
};

inline bool cActionModule::signalEntry(const tSignalEntryId& signalEntryId)
{
	return false;
}

inline void cActionModule::run()
{
}

inline void cActionModule::stop()
{
}

inline bool cActionModule::doSignalEntry(const tSignalEntryId& signalEntryId)
{
	return signalEntry(signalEntryId);
}

}

#endif // TVM_ACTION_H
