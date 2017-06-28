// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TFVM_MODULE_H
#define TFVM_MODULE_H

#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>

#include "type.h"
#include "stream.h"

namespace nVirtualMachine
{

class cVirtualMachine;
class cLibrary;
class cScheme;

class cSignalEntry
{
public:
	virtual ~cSignalEntry()
	{
	}

	virtual bool signalEntry(void* module) = 0;
};

template<typename TObject>
class cSignalEntryObject : public cSignalEntry
{
public:
	using tCallback = bool (TObject::*)();

	cSignalEntryObject(tCallback callback)
	{
		this->callback = callback;
	}

	bool signalEntry(void* module) override
	{
		TObject* object = (TObject*)module;
		return (object->*callback)();
	}

private:
	tCallback callback;
};

class cModule
{
	friend class cLibrary;
	friend class cScheme;
	friend class cVirtualMachine;

public:
	using tSignalEntries = std::map<tSignalEntryName,
	                                std::tuple<tSignalEntryId,
	                                           cSignalEntry*>>;

	using tMemoryEntries = std::map<tMemoryEntryName,
	                                std::tuple<tMemoryTypeName,
	                                           std::ptrdiff_t>>;

	using tSignalExits = std::map<tSignalExitName,
	                              tSignalExitId>;

	using tMemoryExits = std::map<tMemoryExitName,
	                              std::tuple<tMemoryTypeName,
	                                         std::ptrdiff_t>>;

	using tVariables = std::map<tVariableName,
	                            std::tuple<std::type_index,
	                                       void*>>;

public:
	cModule();
	virtual ~cModule();

	virtual cModule* clone() const = 0;

	const tModuleName& getModuleName() const;
	const tCaptionName& getCaptionName() const;
	virtual const tModuleTypeName getModuleTypeName() const = 0;

	const tSignalEntries& getSignalEntries() const;
	const tMemoryEntries& getMemoryEntries() const;
	const tSignalExits& getSignalExits() const;
	const tMemoryExits& getMemoryExits() const;
	const tVariables& getVariables() const;

	bool setVariables(const std::vector<uint8_t>& buffer);

protected:
	void setModuleName(const tModuleName& moduleName);
	void setCaptionName(const tCaptionName& captionName);

	template<typename TObject>
	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         bool (TObject::* callback)())
	{
		auto key = signalEntryName;
		if (signalEntries.find(key) != signalEntries.end())
		{
			return false;
		}

		tSignalEntryId signalEntryId = signalEntries.size() + 1;
		signalEntries[key] = std::make_tuple(signalEntryId,
		                                     new cSignalEntryObject<TObject>(callback));
		return true;
	}

	template<typename TType>
	bool registerMemoryEntry(const tMemoryEntryName& memoryEntryName,
	                         const tMemoryTypeName& memoryTypeName,
	                         TType*& memory);

	bool registerSignalExit(const tSignalExitName& signalExitName,
	                        const tSignalExitId signalExitId);

	template<typename TType>
	bool registerMemoryExit(const tMemoryExitName& memoryExitName,
	                        const tMemoryTypeName& memoryTypeName,
	                        TType*& memory);

	template<typename TType>
	bool registerVariable(const tVariableName& variableName,
	                      TType& variable)
	{
		if (variables.find(variableName) != variables.end())
		{
			return false;
		}

		variables.insert(std::make_pair(variableName,
		                                std::make_tuple(std::type_index(typeid(TType)),
		                                                &variable)));
		return true;
	}

	template<typename TType, typename TDefaultType>
	bool registerVariable(const tVariableName& variableName,
	                      TType& variable,
	                      const TDefaultType& defaultValue)
	{
		variable = defaultValue;
		return registerVariable<TType>(variableName, variable);
	}

protected: /** exec */
	inline bool signalFlow(tSignalExitId signalExitId);

private:
	bool doRegisterModule(cVirtualMachine* virtualMachine);
	virtual bool registerModule() = 0;

	bool doInit(cScheme* scheme);
	virtual bool init();

private:
	cVirtualMachine* virtualMachine;

	tModuleName moduleName;
	tCaptionName captionName;
	tSignalEntries signalEntries;
	tMemoryEntries memoryEntries;
	tSignalExits signalExits;
	tMemoryExits memoryExits;
	tVariables variables;

private: /** exec */
	cScheme* scheme;
};

cModule::cModule()
{
	virtualMachine = nullptr;
	scheme = nullptr;
}

cModule::~cModule()
{
	for (auto& iter : signalEntries)
	{
		delete std::get<1>(iter.second);
	}
}

void cModule::setModuleName(const tModuleName& moduleName)
{
	this->moduleName = moduleName;
}

void cModule::setCaptionName(const tCaptionName& captionName)
{
	this->captionName = captionName;
}

const tModuleName& cModule::getModuleName() const
{
	return moduleName;
}

const tCaptionName& cModule::getCaptionName() const
{
	return captionName;
}

const cModule::tSignalEntries& cModule::getSignalEntries() const
{
	return signalEntries;
}

const cModule::tMemoryEntries& cModule::getMemoryEntries() const
{
	return memoryEntries;
}

const cModule::tSignalExits& cModule::getSignalExits() const
{
	return signalExits;
}

const cModule::tMemoryExits& cModule::getMemoryExits() const
{
	return memoryExits;
}

const cModule::tVariables& cModule::getVariables() const
{
	return variables;
}

template<typename TType>
bool cModule::registerMemoryEntry(const tMemoryEntryName& memoryEntryName,
                                  const tMemoryTypeName& memoryTypeName,
                                  TType*& memory)
{
	auto key = memoryEntryName;
	if (memoryEntries.find(key) != memoryEntries.end())
	{
		return false;
	}

	memoryEntries[key] = std::make_tuple(memoryTypeName,
	                                     (std::ptrdiff_t)&memory - (std::ptrdiff_t)this);
	return true;
}

bool cModule::registerSignalExit(const tSignalExitName& signalExitName,
                                 const tSignalExitId signalExitId)
{
	auto key = signalExitName;
	if (signalExits.find(key) != signalExits.end())
	{
		return false;
	}

	for (const auto& iter : signalExits)
	{
		if (iter.second == signalExitId)
		{
			return false;
		}
	}

	signalExits[key] = signalExitId;
	return true;
}

template<typename TType>
bool cModule::registerMemoryExit(const tMemoryExitName& memoryExitName,
                                 const tMemoryTypeName& memoryTypeName,
                                 TType*& memory)
{
	auto key = memoryExitName;
	if (memoryExits.find(key) != memoryExits.end())
	{
		return false;
	}

	memoryExits[key] = std::make_tuple(memoryTypeName,
	                                   (std::ptrdiff_t)&memory - (std::ptrdiff_t)this);
	return true;
}

bool cModule::doRegisterModule(cVirtualMachine* virtualMachine)
{
	this->virtualMachine = virtualMachine;
	return registerModule();
}

bool cModule::doInit(cScheme* scheme)
{
	this->scheme = scheme;
	return init();
}

bool cModule::init()
{
	return true;
}

}

#endif // TFVM_MODULE_H
