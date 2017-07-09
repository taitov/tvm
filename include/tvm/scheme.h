// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_SCHEME_H
#define TVM_SCHEME_H

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
	                                    tSchemeName>;

	using tLoadSchemeSignalEntryModules = std::map<tModuleId,
	                                         tSignalExitName>;

	using tLoadSchemeSignalExitModules = std::map<tModuleId,
	                                         tSignalEntryName>;

	using tLoadSchemeMemoryEntryModules = std::map<tModuleId,
	                                               tMemoryExitName>;

	using tLoadSchemeMemoryExitModules = std::map<tModuleId,
	                                              tMemoryEntryName>;

	using tLoadRootSignalFlows = std::map<std::tuple<tLibraryName,
	                                                 tRootModuleName,
	                                                 tSignalExitName>,
	                                      std::tuple<tModuleId,
	                                                 tSignalEntryName>>;

	using tLoadRootMemoryExitFlows = std::map<std::tuple<tLibraryName,
	                                                     tRootModuleName,
	                                                     tMemoryExitName>,
	                                          std::tuple<tModuleId,
	                                                     tMemoryEntryName>>;

	using tLoadSignalFlows = std::map<std::tuple<tModuleId,
	                                             tSignalExitName>,
	                                  std::tuple<tModuleId,
	                                             tSignalEntryName>>;

	using tLoadMemoryFlows = std::vector<std::tuple<tModuleId,
	                                                tMemoryExitName,
	                                                tModuleId,
	                                                tMemoryEntryName>>;

	using tLoadMemoryModuleVariables = std::map<tModuleId,
	                                            std::vector<uint8_t>>;

public:
	cScheme(cVirtualMachine* virtualMachine);
	~cScheme();

	cScheme* clone() const;

	bool read(cStreamIn& stream);

	bool init(cScheme* parentScheme, tModuleId parentModuleId);

private:
	bool findEntryPathModule(const tModuleId entryModuleId,
	                         const tSignalEntryName& signalEntryName,
	                         cModule*& registerModule,
	                         cModule*& clonedModule,
	                         cSignalEntry*& signalEntry) const;

	bool findMemoryEntryPath(const tModuleId entryModuleId,
	                         const tMemoryEntryName& memoryEntryName,
	                         void*& pointer) const;

	bool findMemoryExitPath(const tModuleId moduleId,
	                        const tMemoryExitName& memoryExitName,
	                        void*& pointer) const;

	tModuleId findSchemeSignalEntryModule(const tSignalExitName& signalExitName) const;
	tModuleId findSchemeMemoryEntryModule(const tMemoryExitName& memoryExitName) const;
	tModuleId findSchemeMemoryExitModule(const tMemoryEntryName& memoryEntryName) const;
	bool getMemoryModule(tModuleId fromModuleId, const tMemoryEntryName& memoryEntryName,
	                     tModuleId& toModuleId, tMemoryExitName& memoryExitName) const;
	bool getMemoryModule(tModuleId fromModuleId, const tMemoryExitName& memoryExitName,
	                     tModuleId& toModuleId, tMemoryEntryName& memoryEntryName) const;

private:
	cVirtualMachine* virtualMachine;
	cScheme* parentScheme;
	tModuleId parentModuleId;

private: /** load */
	tLoadMemories loadMemories;
	tLoadModules loadModules;
	tLoadCustomModules loadCustomModules;
	tLoadSchemeSignalEntryModules loadSchemeSignalEntryModules;
	tLoadSchemeSignalExitModules loadSchemeSignalExitModules;
	tLoadSchemeMemoryEntryModules loadSchemeMemoryEntryModules;
	tLoadSchemeMemoryExitModules loadSchemeMemoryExitModules;
	tLoadRootSignalFlows loadRootSignalFlows;
	tLoadRootMemoryExitFlows loadRootMemoryExitFlows;
	tLoadSignalFlows loadSignalFlows;
	tLoadMemoryFlows loadMemoryFlows;
	tLoadMemoryModuleVariables loadMemoryModuleVariables;

private: /** init */
	using tMemories = std::map<tModuleId,
	                           cMemory*>;

	using tModules = std::map<tModuleId,
	                          cModule*>;

	using tCustomModules = std::map<tModuleId,
	                                cScheme*>;

	tMemories memories;
	tModules modules;
	tCustomModules customModules;

private: /** exec */
	inline bool rootSignalFlow(tRootSignalExitId rootSignalExitId);

	template<typename TType>
	inline void rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value);

	inline bool signalFlow(cModule* fromModule, tSignalExitId fromSignalExit);

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

inline cScheme::cScheme(cVirtualMachine* virtualMachine)
{
	this->virtualMachine = virtualMachine;
	parentScheme = nullptr;
}

inline cScheme::~cScheme()
{
	for (auto& iter : memories)
	{
		delete iter.second;
	}

	for (auto& iter : modules)
	{
		delete iter.second;
	}

	for (auto& iter : customModules)
	{
		delete iter.second;
	}
}

inline cScheme* cScheme::clone() const
{
	cScheme* newScheme = new cScheme(virtualMachine);

	/** @todo: delete */
	newScheme->loadMemories = loadMemories;
	newScheme->loadModules = loadModules;
	newScheme->loadCustomModules = loadCustomModules;
	newScheme->loadSchemeSignalEntryModules = loadSchemeSignalEntryModules;
	newScheme->loadSchemeSignalExitModules = loadSchemeSignalExitModules;
	newScheme->loadSchemeMemoryEntryModules = loadSchemeMemoryEntryModules;
	newScheme->loadSchemeMemoryExitModules = loadSchemeMemoryExitModules;
	newScheme->loadRootSignalFlows = loadRootSignalFlows;
	newScheme->loadRootMemoryExitFlows = loadRootMemoryExitFlows;
	newScheme->loadSignalFlows = loadSignalFlows;
	newScheme->loadMemoryFlows = loadMemoryFlows;
	newScheme->loadMemoryModuleVariables = loadMemoryModuleVariables;

	return newScheme;
}

inline bool cScheme::read(cStreamIn& stream)
{
	stream.pop(loadMemories);
	stream.pop(loadModules);
	stream.pop(loadCustomModules);
	stream.pop(loadSchemeSignalEntryModules);
	stream.pop(loadSchemeSignalExitModules);
	stream.pop(loadSchemeMemoryEntryModules);
	stream.pop(loadSchemeMemoryExitModules);
	stream.pop(loadRootSignalFlows);
	stream.pop(loadRootMemoryExitFlows);
	stream.pop(loadSignalFlows);
	stream.pop(loadMemoryFlows);
	stream.pop(loadMemoryModuleVariables);

	if (stream.isFailed())
	{
		return false;
	}

	return true;
}

inline tModuleId cScheme::findSchemeSignalEntryModule(const tSignalExitName& signalExitName) const
{
	for (const auto& iter : loadSchemeSignalEntryModules)
	{
		if (iter.second == signalExitName)
		{
			return iter.first;
		}
	}
	return 0;
}

inline tModuleId cScheme::findSchemeMemoryEntryModule(const tMemoryExitName& memoryExitName) const
{
	for (const auto& iter : loadSchemeMemoryEntryModules)
	{
		if (iter.second == memoryExitName)
		{
			return iter.first;
		}
	}
	return 0;
}

inline tModuleId cScheme::findSchemeMemoryExitModule(const tMemoryEntryName& memoryEntryName) const
{
	for (const auto& iter : loadSchemeMemoryExitModules)
	{
		if (iter.second == memoryEntryName)
		{
			return iter.first;
		}
	}
	return 0;
}

inline bool cScheme::getMemoryModule(tModuleId fromModuleId, const tMemoryEntryName& memoryEntryName,
                                     tModuleId& toModuleId, tMemoryExitName& memoryExitName) const
{
	for (const auto& iter : loadMemoryFlows)
	{
		if (std::get<2>(iter) == fromModuleId &&
		    std::get<3>(iter) == memoryEntryName)
		{
			toModuleId = std::get<0>(iter);
			memoryExitName = std::get<1>(iter);
			return true;
		}
	}
	return false;
}

inline bool cScheme::getMemoryModule(tModuleId fromModuleId, const tMemoryExitName& memoryExitName,
                                     tModuleId& toModuleId, tMemoryEntryName& memoryEntryName) const
{
	for (const auto& iter : loadMemoryFlows)
	{
		if (std::get<0>(iter) == fromModuleId &&
		    std::get<1>(iter) == memoryExitName)
		{
			toModuleId = std::get<2>(iter);
			memoryEntryName = std::get<3>(iter);
			return true;
		}
	}
	return false;
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

template<typename TType>
inline void cScheme::rootSetMemory(tRootMemoryExitId rootMemoryExitId, const TType& value)
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

}

#endif // TVM_SCHEME_H
