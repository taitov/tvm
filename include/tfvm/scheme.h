// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_SCHEME_H
#define TFVM_SCHEME_H

#include <vector>

#include "type.h"
#include "module.h"
#include "stream.h"
#include "vm.h"

namespace nVirtualMachine
{

class cScheme
{
	friend class cVirtualMachine;
	friend class cModule;

public:
	using tLoadMemories = std::map<tModuleId,
	                               tMemoryTypeName>;

	using tLoadModules = std::map<tModuleId,
	                              std::tuple<tLibraryName,
	                                         tModuleName>>;

	using tLoadCustomModules = std::map<tModuleId,
	                                    tCustomModuleName>;

	using tLoadRootSignalFlows = std::map<std::tuple<tLibraryName,
	                                                 tRootModuleName,
	                                                 tSignalExitName>,
	                                      std::tuple<tModuleId,
	                                                 tSignalEntryName>>;

	using tLoadRootMemoryExitFlows = std::map<std::tuple<tLibraryName,
	                                                     tRootModuleName,
	                                                     tMemoryExitName>,
	                                          tModuleId>;

	using tLoadSignalFlows = std::map<std::tuple<tModuleId,
	                                             tSignalExitName>,
	                                  std::tuple<tModuleId,
	                                             tSignalEntryName>>;

	using tLoadMemoryEntryFlows = std::map<std::tuple<tModuleId,
	                                                  tMemoryEntryName>,
	                                       tModuleId>;

	using tLoadMemoryExitFlows = std::map<std::tuple<tModuleId,
	                                                 tMemoryExitName>,
	                                      tModuleId>;

	using tLoadMemoryModuleVariables = std::map<tModuleId,
	                                            std::vector<uint8_t>>;

public:
	cScheme(cVirtualMachine* virtualMachine);
	~cScheme();

	cScheme* clone() const;

	bool read(cStreamIn& stream);

	bool init(cScheme* parentScheme);

private: /** exec */
	inline bool rootSignalFlow(tRootSignalExitId rootSignalExitId);

	template<typename TType>
	inline void rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value)
	{
		auto iter = rootMemoryFlows.find(rootMemoryExitId);
		if (iter != rootMemoryFlows.end())
		{
			*(TType*)iter->second = value;
		}

		if (parentScheme)
		{
			parentScheme->rootSetMemory(rootMemoryExitId, value);
		}
	}

	inline bool signalFlow(cModule* fromModule, tSignalExitId fromSignalExit);

private:
	cVirtualMachine* virtualMachine;
	cScheme* parentScheme;

private: /** load */
	tLoadMemories loadMemories;
	tLoadModules loadModules;
	tLoadRootSignalFlows loadRootSignalFlows;
	tLoadRootMemoryExitFlows loadRootMemoryExitFlows;
	tLoadSignalFlows loadSignalFlows;
	tLoadMemoryEntryFlows loadMemoryEntryFlows;
	tLoadMemoryExitFlows loadMemoryExitFlows;
	tLoadMemoryModuleVariables loadMemoryModuleVariables;

private: /** init */
	using tMemories = std::map<tModuleId,
	                           cMemory*>;

	using tModules = std::map<tModuleId,
	                          cModule*>;

	tMemories memories;
	tModules modules;

private: /** exec */
	using tRootSignalFlows = std::map<tRootSignalExitId,
	                                  std::tuple<cSignalEntry*,
	                                             void*>>;

	using tRootMemoryFlows = std::map<tRootMemoryExitId,
	                                  void*>;

	using tSignalFlows = std::map<std::tuple<cModule*,
	                                         tSignalExitId>,
	                              std::tuple<cSignalEntry*,
	                                         void*>>;

	tRootSignalFlows rootSignalFlows;
	tRootMemoryFlows rootMemoryFlows;
	tSignalFlows signalFlows;
};

cScheme::cScheme(cVirtualMachine* virtualMachine)
{
	this->virtualMachine = virtualMachine;
	parentScheme = nullptr;
}

cScheme::~cScheme()
{
	for (auto& iter : memories)
	{
		delete iter.second;
	}

	for (auto& iter : modules)
	{
		delete iter.second;
	}
}

cScheme* cScheme::clone() const
{
	cScheme* newScheme = new cScheme(virtualMachine);

	newScheme->loadMemories = loadMemories;
	newScheme->loadModules = loadModules;
	newScheme->loadRootSignalFlows = loadRootSignalFlows;
	newScheme->loadRootMemoryExitFlows = loadRootMemoryExitFlows;
	newScheme->loadSignalFlows = loadSignalFlows;
	newScheme->loadMemoryEntryFlows = loadMemoryEntryFlows;
	newScheme->loadMemoryExitFlows = loadMemoryExitFlows;
	newScheme->loadMemoryModuleVariables = loadMemoryModuleVariables;

	return newScheme;
}

bool cScheme::read(cStreamIn& stream)
{
	stream.pop(loadMemories);
	stream.pop(loadModules);
	stream.pop(loadRootSignalFlows);
	stream.pop(loadRootMemoryExitFlows);
	stream.pop(loadSignalFlows);
	stream.pop(loadMemoryEntryFlows);
	stream.pop(loadMemoryExitFlows);
	stream.pop(loadMemoryModuleVariables);

	if (stream.isFailed())
	{
		return false;
	}

	return true;
}

inline bool cScheme::rootSignalFlow(tRootSignalExitId rootSignalExitId)
{
	auto iter = rootSignalFlows.find(rootSignalExitId);
	if (iter == rootSignalFlows.end())
	{
		if (parentScheme)
		{
			return parentScheme->rootSignalFlow(rootSignalExitId);
		}

		return false;
	}

	cSignalEntry* signalEntry = std::get<0>(iter->second);
	if (!signalEntry->signalEntry(std::get<1>(iter->second)))
	{
		if (parentScheme)
		{
			return parentScheme->rootSignalFlow(rootSignalExitId);
		}

		return false;
	}

	return true;
}

inline bool cScheme::signalFlow(cModule* fromModule, tSignalExitId fromSignalExit)
{
	auto iter = signalFlows.find(std::make_tuple(fromModule, fromSignalExit));
	if (iter == signalFlows.end())
	{
		return false;
	}

	cSignalEntry* signalEntry = std::get<0>(iter->second);
	return signalEntry->signalEntry(std::get<1>(iter->second));
}

inline bool cModule::signalFlow(tSignalExitId signalExitId)
{
	return scheme->signalFlow(this, signalExitId);
}

}

#endif // TFVM_SCHEME_H
