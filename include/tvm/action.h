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
	const tModuleTypeName getModuleTypeName() const override final
	{
		return "action";
	}

protected:
	template<typename TObject>
	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         bool (TObject::* callback)()) = delete;

	template<typename TObject>
	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         void (TObject::* callback)());

protected: /** exec */
	inline bool signalFlow(tSignalExitId signalExitId);
};

}

#endif // TVM_ACTION_H
