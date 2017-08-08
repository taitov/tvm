// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_MODULE_H
#define TVM_MODULE_H

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

template<typename TObject>
class cActionSignalEntryObject : public cSignalEntry
{
public:
	using tCallback = void (TObject::*)();

	cActionSignalEntryObject(tCallback callback)
	{
		this->callback = callback;
	}

	bool signalEntry(void* module) override;

private:
	class cSimpleThread
	{
	private:
		struct sArgs
		{
			sArgs(tCallback callback, TObject* module) :
			        callback(callback),
			        module(module)
			{
			}

			tCallback callback;
			TObject* module;
		};

	public:
		cSimpleThread(tCallback callback, void* module)
		{
			pthread_t thread;
			pthread_attr_t attr;

			if (pthread_attr_init(&attr) != 0)
			{
				return;
			}

			sArgs* args = new sArgs(callback, (TObject*)module);

			if (pthread_create(&thread, &attr, &callHelper, args) != 0)
			{
				delete args;
				pthread_attr_destroy(&attr);
				return;
			}

			pthread_attr_destroy(&attr);
		}

	private:
		static void* callHelper(void* pargs)
		{
			sArgs* args = (sArgs*)pargs;
			TObject* object = args->module;
			(object->*(args->callback))();
			delete args;
			return nullptr;
		}
	};

private:
	tCallback callback;
};

class cModule
{
	friend class cLibrary;
	friend class cScheme;
	friend class cVirtualMachine;
	friend class cActionModule;

	template<typename TObject>
	friend class cActionSignalEntryObject;

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
	virtual const tModuleTypeName getModuleTypeName() const = 0;

	const tModuleName& getModuleName() const;
	const tCaptionName& getCaptionName() const;
	const tCaptionTypeName& getCaptionTypeName() const;

	const tSignalEntries& getSignalEntries() const;
	const tMemoryEntries& getMemoryEntries() const;
	const tSignalExits& getSignalExits() const;
	const tMemoryExits& getMemoryExits() const;
	const tVariables& getVariables() const;
	const bool& isDeprecated() const;

	bool setVariables(const std::vector<uint8_t>& buffer);

protected:
	void setModuleName(const tModuleName& moduleName);
	void setCaptionName(const tCaptionName& captionName);
	void setCaptionTypeName(const tCaptionTypeName& captionTypeName);
	void setDeprecated();

	template<typename TObject>
	bool registerSignalEntry(const tSignalEntryName& signalEntryName,
	                         bool (TObject::* callback)());

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
	                      TType& variable);

	template<typename TType, typename TDefaultType>
	bool registerVariable(const tVariableName& variableName,
	                      TType& variable,
	                      const TDefaultType& defaultValue);

private:
	bool doRegisterModule(cVirtualMachine* virtualMachine);
	virtual bool registerModule() = 0;

	bool doInit(cScheme* scheme);
	virtual bool init();

private:
	cVirtualMachine* virtualMachine;

	tModuleName moduleName;
	tCaptionName captionName;
	tCaptionTypeName captionTypeName;
	tSignalEntries signalEntries;
	tMemoryEntries memoryEntries;
	tSignalExits signalExits;
	tMemoryExits memoryExits;
	tVariables variables;
	bool deprecated;

protected: /** exec */
	inline bool signalFlow(tSignalExitId signalExitId);

private: /** exec */
	cScheme* scheme;
};

inline cModule::cModule()
{
	virtualMachine = nullptr;
	deprecated = false;
	scheme = nullptr;
}

inline cModule::~cModule()
{
	for (auto& iter : signalEntries)
	{
		delete std::get<1>(iter.second);
	}
}

inline void cModule::setModuleName(const tModuleName& moduleName)
{
	this->moduleName = moduleName;
}

inline void cModule::setCaptionName(const tCaptionName& captionName)
{
	this->captionName = captionName;
}

inline void cModule::setCaptionTypeName(const tCaptionTypeName& captionTypeName)
{
	this->captionTypeName = captionTypeName;
}

inline void cModule::setDeprecated()
{
	deprecated = true;
}

inline const tModuleName& cModule::getModuleName() const
{
	return moduleName;
}

inline const tCaptionName& cModule::getCaptionName() const
{
	return captionName;
}

inline const tCaptionTypeName& cModule::getCaptionTypeName() const
{
	return captionTypeName;
}

inline const cModule::tSignalEntries& cModule::getSignalEntries() const
{
	return signalEntries;
}

inline const cModule::tMemoryEntries& cModule::getMemoryEntries() const
{
	return memoryEntries;
}

inline const cModule::tSignalExits& cModule::getSignalExits() const
{
	return signalExits;
}

inline const cModule::tMemoryExits& cModule::getMemoryExits() const
{
	return memoryExits;
}

inline const cModule::tVariables& cModule::getVariables() const
{
	return variables;
}

inline const bool& cModule::isDeprecated() const
{
	return deprecated;
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

inline bool cModule::registerSignalExit(const tSignalExitName& signalExitName,
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

inline bool cModule::doRegisterModule(cVirtualMachine* virtualMachine)
{
	this->virtualMachine = virtualMachine;
	return registerModule();
}

inline bool cModule::doInit(cScheme* scheme)
{
	this->scheme = scheme;
	return init();
}

inline bool cModule::init()
{
	return true;
}

template<typename TObject>
bool cModule::registerSignalEntry(const tSignalEntryName& signalEntryName, bool (TObject::* callback)())
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
bool cModule::registerVariable(const tVariableName& variableName, TType& variable)
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
bool cModule::registerVariable(const tVariableName& variableName,
                               TType& variable,
                               const TDefaultType& defaultValue)
{
	variable = defaultValue;
	return registerVariable<TType>(variableName, variable);
}

}

#endif // TVM_MODULE_H
