// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#if !defined (TVM_ENGINE) && !defined (TVM_CONVERTER)
#error "include 'engine' or 'converter'"
#endif

#ifdef TVM_ENGINE
#define TVM_ENGINE_UNUSED TVM_UNUSED
#else
#define TVM_ENGINE_UNUSED
#endif

#ifdef TVM_CONVERTER
#define TVM_CONVERTER_UNUSED TVM_UNUSED
#else
#define TVM_CONVERTER_UNUSED
#endif

#include <vector>
#include <string>
#include <tuple>
#include <map>
#include <fstream>

#include "result.h"
#include "type.h"
#include "memory.h"
#include "root.h"
#include "logic.h"
#include "common.h"
#include "stream.h"
#include "locker.h"

namespace nVirtualMachine
{

#define tvmRegisterMemoryModule() \
	static void tvmRegisterModule(cRegisterMemoryModule<auto>& registerModule)

#define tvmRegisterRootModule() \
	static void tvmRegisterModule(cRegisterRootModule<auto>& registerModule)

#define tvmRegisterLogicModule() \
	static void tvmRegisterModule(cRegisterLogicModule<auto>& registerModule)

template<typename TType>
class cEngineValueHelper
{
public:
	cEngineValueHelper(const TType& value);

#ifdef TVM_ENGINE
	const TType& getValue() const;

protected:
	const TType value;
#endif
};

//

class cConverterStringHelper
{
public:
	cConverterStringHelper();
	cConverterStringHelper(const char* string);

#ifdef TVM_CONVERTER
public:
	bool operator<(const cConverterStringHelper& second) const;

	bool contains(const char symbol) const;
	const std::string& getString() const;
	std::string& getString();

protected:
	std::string string;
#endif
};

//

class cMemoryValueType
{
public:
	cMemoryValueType();

	cMemoryValueType(const cConverterStringHelper& valueType); ///< @todo: enum

public:
	void valueSetType(const cConverterStringHelper& valueType);

	void vectorSetMemoryModule(const cConverterStringHelper& memoryModuleName);

	void tupleInsertMemoryModule(const cConverterStringHelper& valueName,
	                             const cConverterStringHelper& memoryModuleName);

	template<typename TEnumType>
	void enumInsertValue(const cConverterStringHelper& valueName,
	                     TEnumType value);

#ifdef TVM_CONVERTER
public:
	bool isValue() const;
	const auto& valueGetType() const;

	bool isVector() const;
	const auto& vectorGetMemoryModule() const;

	bool isTuple() const;
	const auto& tupleGetMemoryModules() const;

	bool isEnum() const;
	size_t enumGetSize() const;
	const auto& enumGetValues() const;

protected:
	std::string type; ///< @todo: enum

	/// value
	std::string valueType;

	/// vector
	std::string vectorMemoryModule;

	/// tuple
	std::vector<std::tuple<std::string,
	                       std::string>> tupleMemoryModules;

	/// enum
	size_t enumSize;
	std::vector<std::tuple<std::string,
	                       size_t>> enumValues;
#endif
};

//

class cEngine;
class cConverter;

//

class cRegister
{
public:
	template<typename TType>
	using tSignalEntry = std::tuple<tSignalEntryName,
	                                cEngineValueHelper<tFlowId (TType::*)()>>;

	template<typename TType>
	using tMemoryEntry = std::tuple<tMemoryEntryName,
	                                tModuleName,
	                                cEngineValueHelper<const void* (TType::*)>>; ///< @todo: bool lock for global

	template<typename TType>
	using tSignalExit = std::tuple<tSignalExitName,
	                               cEngineValueHelper<tFlowId (TType::*)>>;

	template<typename TType>
	using tMemoryExit = std::tuple<tMemoryExitName,
	                               tModuleName,
	                               cEngineValueHelper<void* (TType::*)>>; ///< @todo: bool lock for global

	template<typename TType>
	using tMemoryEntryExit = std::tuple<tMemoryEntryExitName,
	                                    tModuleName,
	                                    cEngineValueHelper<void* (TType::*)>>; ///< @todo: bool lock for global

	template<typename TType>
	using tRootSignalExit = std::tuple<tSignalExitName,
	                                   cEngineValueHelper<tRootSignalExitId (TType::*)>>;

	template<typename TType>
	using tRootMemoryExit = std::tuple<tMemoryExitName,
	                                   tModuleName,
	                                   cEngineValueHelper<void* (TType::*)>>;

#ifdef TVM_CONVERTER
	using tConverterMemoryModule = std::tuple<tModuleName,
	                                          cMemoryValueType>;

	using tConverterRootModule = std::tuple<tModuleName,
	                                        tCaptionName,
	                                        std::vector<std::tuple<tSignalExitName,
	                                                               tRootSignalExitId>>,
	                                        std::vector<std::tuple<tMemoryExitName,
	                                                               tModuleName>>>;

	using tConverterLogicModule = std::tuple<tModuleName,
	                                         tCaptionName,
	                                         std::vector<tSignalEntryName>,
	                                         std::vector<std::tuple<tMemoryEntryName,
	                                                                tModuleName>>,
	                                         std::vector<tSignalExitName>,
	                                         std::vector<std::tuple<tMemoryExitName,
	                                                                tModuleName>>,
	                                         std::vector<std::tuple<tMemoryEntryExitName,
	                                                                tModuleName>>>;
#endif

public:
	template<typename TType,
	         typename ... TArgs>
	eResult registerMemoryModuleBase(const tModuleName& moduleName,
	                                 const cMemoryValueType& memoryValueType,
	                                 TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult registerMemoryModule(TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult registerRootModuleBase(const tModuleName& moduleName,
	                               const tCaptionName& captionName,
	                               const std::vector<tRootSignalExit<TType>>& rootSignalExits,
	                               const std::vector<tRootMemoryExit<TType>>& rootMemoryExits,
	                               TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult registerRootModule(TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult registerLogicModuleBase(const tModuleName& moduleName,
	                                const tCaptionName& captionName,
	                                const std::vector<tSignalEntry<TType>>& signalEntries,
	                                const std::vector<tMemoryEntry<TType>>& memoryEntries,
	                                const std::vector<tSignalExit<TType>>& signalExits,
	                                const std::vector<tMemoryExit<TType>>& memoryExits,
	                                const std::vector<tMemoryEntryExit<TType>>& memoryEntryExits,
	                                TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult registerLogicModule(TArgs ... args);

	void unregisterModules();

	eResult getRegisterResult() const;

protected:
	cRegister();
	~cRegister();

	template<typename TType,
	         typename ... TArgs>
	eResult localRegisterMemoryModuleBase(const tModuleName& moduleName,
	                                      const cMemoryValueType& memoryValueType,
	                                      TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult localRegisterRootModuleBase(const tModuleName& moduleName,
	                                    const tCaptionName& captionName,
	                                    const std::vector<tRootSignalExit<TType>>& rootSignalExits,
	                                    const std::vector<tRootMemoryExit<TType>>& rootMemoryExits,
	                                    TArgs ... args);

	template<typename TType,
	         typename ... TArgs>
	eResult localRegisterLogicModuleBase(const tModuleName& moduleName,
	                                     const tCaptionName& captionName,
	                                     const std::vector<tSignalEntry<TType>>& signalEntries,
	                                     const std::vector<tMemoryEntry<TType>>& memoryEntries,
	                                     const std::vector<tSignalExit<TType>>& signalExits,
	                                     const std::vector<tMemoryExit<TType>>& memoryExits,
	                                     const std::vector<tMemoryEntryExit<TType>>& memoryEntryExits,
	                                     TArgs ... args);

protected:
	tRootSignalExitId rootSignalExitsCount;
	eResult registerResult;

#ifdef TVM_ENGINE
protected:
	std::vector<cMemoryModuleManager*> memoryModuleManagers;
	std::vector<cRootModuleManager*> rootModuleManagers;
	std::vector<cLogicModuleManager*> logicModuleManagers;
#endif

#ifdef TVM_CONVERTER
protected:
	std::vector<tConverterMemoryModule> converterMemoryModules;
	std::vector<tConverterRootModule> converterRootModules;
	std::vector<tConverterLogicModule> converterLogicModules;
#endif
};

//

template<typename TType>
class cRegisterMemoryModule
{
public:
	void setModuleName(const tModuleName& moduleName);

	void setMemoryAsString();
	void setMemoryAsInteger8();
	void setMemoryAsInteger16();
	void setMemoryAsInteger32();
	void setMemoryAsInteger64();
	void setMemoryAsUnsignedInteger8();
	void setMemoryAsUnsignedInteger16();
	void setMemoryAsUnsignedInteger32();
	void setMemoryAsUnsignedInteger64();
	void setMemoryAsBoolean();

	void setMemoryAsVector(const cConverterStringHelper& memoryModuleName);

	void setMemoryAsTuple(...);

	template<typename TEnumType>
	void setMemoryAsEnumInsertValue(const cConverterStringHelper& valueName,
	                                TEnumType value)
	{
		this->memoryValueType.enumInsertValue<TEnumType>(valueName, value);
	}

	const tModuleName& getModuleName() const;
	const cMemoryValueType& getMemoryValueType() const;

protected:
	tModuleName moduleName;
	cMemoryValueType memoryValueType;
};

//

template<typename TType>
class cRegisterRootModule
{
public:
	using tRootSignalExitIdPointer = tRootSignalExitId (TType::*);

	template<typename TMemoryType>
	using tRootMemoryExitPointer = TMemoryType* (TType::*);

public:
	void setModuleName(const tModuleName& moduleName);
	void setCaptionName(const tCaptionName& captionName);

	void insertRootSignalExit(const tSignalExitName& signalExitName,
	                          const tRootSignalExitIdPointer& rootSignalExitId);

	template<typename TMemoryType>
	void insertRootMemoryExit(const tMemoryExitName& memoryExitName,
	                          const tModuleName& memoryModuleName,
	                          const tRootMemoryExitPointer<TMemoryType>& memoryPointer)
	{
		rootMemoryExits.emplace_back(memoryExitName,
		                             memoryModuleName,
		                             cEngineValueHelper<void* (TType::*)>(reinterpret_cast<void* (TType::*)>(memoryPointer)));
	}

	const tModuleName& getModuleName() const;
	const tCaptionName& getCaptionName() const;
	const std::vector<cRegister::tRootSignalExit<TType>>& getRootSignalExits() const;
	const std::vector<cRegister::tRootMemoryExit<TType>>& getRootMemoryExits() const;

protected:
	tModuleName moduleName;
	tCaptionName captionName;
	std::vector<cRegister::tRootSignalExit<TType>> rootSignalExits;
	std::vector<cRegister::tRootMemoryExit<TType>> rootMemoryExits;
};

//

template<typename TType>
class cRegisterLogicModule
{
public:
	using tSignalEntryCallback = tFlowId (TType::*)();

	template<typename TMemoryType>
	using tMemoryEntryPointer = const TMemoryType* (TType::*);

	using tSignalExitFlowIdPointer = tFlowId (TType::*);

	template<typename TMemoryType>
	using tMemoryExitPointer = TMemoryType* (TType::*);

	template<typename TMemoryType>
	using tMemoryEntryExitPointer = TMemoryType* (TType::*);

public:
	void setModuleName(const tModuleName& moduleName);
	void setCaptionName(const tCaptionName& value);

	void insertSignalEntry(const tSignalEntryName& signalEntryName,
	                       const tSignalEntryCallback& callback);

	template<typename TMemoryType>
	void insertMemoryEntry(const tMemoryEntryName& memoryEntryName,
	                       const tModuleName& memoryModuleName,
	                       const tMemoryEntryPointer<TMemoryType>& memoryPointer)
	{
		this->memoryEntries.emplace_back(memoryEntryName,
		                                 memoryModuleName,
		                                 cEngineValueHelper<const void* (TType::*)>(reinterpret_cast<const void* (TType::*)>(memoryPointer)));
	}

	void insertSignalExit(const tSignalExitName& signalExitName,
	                      const tSignalExitFlowIdPointer& flowIdPointer);

	template<typename TMemoryType>
	void insertMemoryExit(const tMemoryExitName& memoryExitName,
	                      const tModuleName& memoryModuleName,
	                      const tMemoryExitPointer<TMemoryType>& memoryPointer)
	{
		this->memoryExits.emplace_back(memoryExitName,
		                               memoryModuleName,
		                               cEngineValueHelper<void* (TType::*)>(reinterpret_cast<void* (TType::*)>(memoryPointer)));
	}

	template<typename TMemoryType>
	void insertMemoryEntryExit(const tMemoryEntryExitName& memoryEntryExitName,
	                           const tModuleName& memoryModuleName,
	                           const tMemoryEntryExitPointer<TMemoryType>& memoryPointer)
	{
		this->memoryEntryExits.emplace_back(memoryEntryExitName,
		                                    memoryModuleName,
		                                    cEngineValueHelper<void* (TType::*)>(reinterpret_cast<void* (TType::*)>(memoryPointer)));
	}

	const tModuleName& getModuleName() const;
	const tCaptionName& getCaptionName() const;
	const std::vector<cRegister::tSignalEntry<TType>>& getSignalEntries() const;
	const std::vector<cRegister::tMemoryEntry<TType>>& getMemoryEntries() const;
	const std::vector<cRegister::tSignalExit<TType>>& getSignalExits() const;
	const std::vector<cRegister::tMemoryExit<TType>>& getMemoryExits() const;
	const std::vector<cRegister::tMemoryEntryExit<TType>>& getMemoryEntryExits() const;

protected:
	tModuleName moduleName;
	tCaptionName captionName;
	std::vector<cRegister::tSignalEntry<TType>> signalEntries;
	std::vector<cRegister::tMemoryEntry<TType>> memoryEntries;
	std::vector<cRegister::tSignalExit<TType>> signalExits;
	std::vector<cRegister::tMemoryExit<TType>> memoryExits;
	std::vector<cRegister::tMemoryEntryExit<TType>> memoryEntryExits;
};

//

#ifdef TVM_ENGINE
template<typename TType>
cEngineValueHelper<TType>::cEngineValueHelper(const TType& value) :
        value(value)
{
}
#else
template<typename TType>
cEngineValueHelper<TType>::cEngineValueHelper(TVM_CONVERTER_UNUSED const TType& value)
{
}
#endif

#ifdef TVM_ENGINE
template<typename TType>
const TType& cEngineValueHelper<TType>::getValue() const
{
	return value;
}
#endif

//

inline cConverterStringHelper::cConverterStringHelper()
{
}

#ifdef TVM_CONVERTER
inline cConverterStringHelper::cConverterStringHelper(const char* string) :
        string(string)
{
}

inline bool cConverterStringHelper::operator<(const cConverterStringHelper& second) const
{
	return (this->string < second.string);
}

inline bool cConverterStringHelper::contains(const char symbol) const
{
	return (string.find(symbol) != std::string::npos);
}

inline const std::string& cConverterStringHelper::getString() const
{
	return string;
}

inline std::string& cConverterStringHelper::getString()
{
	return string;
}
#else
inline cConverterStringHelper::cConverterStringHelper(TVM_ENGINE_UNUSED const char* string)
{
}
#endif

//

inline cMemoryValueType::cMemoryValueType()
{
}

inline cMemoryValueType::cMemoryValueType(TVM_ENGINE_UNUSED const cConverterStringHelper& valueType)
{
	valueSetType(valueType);
}

inline void cMemoryValueType::valueSetType(TVM_ENGINE_UNUSED const cConverterStringHelper& valueType)
{
#ifdef TVM_CONVERTER
	type = "value";
	this->valueType = valueType.getString();
#endif
}

inline void cMemoryValueType::vectorSetMemoryModule(TVM_ENGINE_UNUSED const cConverterStringHelper& memoryModuleName)
{
#ifdef TVM_CONVERTER
	type = "vector";
	this->vectorMemoryModule = memoryModuleName.getString();
#endif
}

inline void cMemoryValueType::tupleInsertMemoryModule(TVM_ENGINE_UNUSED const cConverterStringHelper& valueName,
                                                      TVM_ENGINE_UNUSED const cConverterStringHelper& memoryModuleName)
{
#ifdef TVM_CONVERTER
	type = "tuple";
	this->tupleMemoryModules.emplace_back(valueName.getString(),
	                                      memoryModuleName.getString());
#endif
}

template<typename TEnumType>
inline void cMemoryValueType::enumInsertValue(TVM_ENGINE_UNUSED const cConverterStringHelper& valueName,
                                              TVM_ENGINE_UNUSED TEnumType value)
{
#ifdef TVM_CONVERTER
	type = "enum";
	this->enumSize = sizeof(TEnumType);
	this->enumValues.emplace_back(valueName.getString(),
	                              value);
#endif
}

#ifdef TVM_CONVERTER
inline bool cMemoryValueType::isValue() const
{
	return type == "value";
}

inline const auto& cMemoryValueType::valueGetType() const
{
	return valueType;
}

inline bool cMemoryValueType::isVector() const
{
	return type == "vector";
}

inline const auto& cMemoryValueType::vectorGetMemoryModule() const
{
	return vectorMemoryModule;
}

inline bool cMemoryValueType::isTuple() const
{
	return type == "tuple";
}

inline const auto& cMemoryValueType::tupleGetMemoryModules() const
{
	return tupleMemoryModules;
}

inline bool cMemoryValueType::isEnum() const
{
	return type == "enum";
}

inline size_t cMemoryValueType::enumGetSize() const
{
	return enumSize;
}

inline const auto& cMemoryValueType::enumGetValues() const
{
	return enumValues;
}
#endif

//

template<typename TType>
inline void cRegisterMemoryModule<TType>::setModuleName(const tModuleName& moduleName)
{
	this->moduleName = moduleName;
}

template<typename TType>
inline void cRegisterMemoryModule<TType>::setMemoryAsString()
{
	this->memoryValueType.valueSetType("string");
}

template<typename TType>
inline void cRegisterMemoryModule<TType>::setMemoryAsVector(const cConverterStringHelper& memoryModuleName)
{
	this->memoryValueType.vectorSetMemoryModule(memoryModuleName);
}

template<typename TType>
inline const tModuleName& cRegisterMemoryModule<TType>::getModuleName() const
{
	return moduleName;
}

template<typename TType>
inline const cMemoryValueType& cRegisterMemoryModule<TType>::getMemoryValueType() const
{
	return memoryValueType;
}

//

template<typename TType>
inline void cRegisterRootModule<TType>::setModuleName(const tModuleName& moduleName)
{
	this->moduleName = moduleName;
}

template<typename TType>
inline void cRegisterRootModule<TType>::setCaptionName(const tCaptionName& captionName)
{
	this->captionName = captionName;
}

template<typename TType>
inline void cRegisterRootModule<TType>::insertRootSignalExit(const tSignalExitName& signalExitName,
                                                             const cRegisterRootModule<TType>::tRootSignalExitIdPointer& rootSignalExitId)
{
	this->rootSignalExits.emplace_back(signalExitName,
	                                   rootSignalExitId);
}

template<typename TType>
inline const tModuleName& cRegisterRootModule<TType>::getModuleName() const
{
	return moduleName;
}

template<typename TType>
inline const tCaptionName& cRegisterRootModule<TType>::getCaptionName() const
{
	return captionName;
}

template<typename TType>
inline const std::vector<cRegister::tRootSignalExit<TType>>& cRegisterRootModule<TType>::getRootSignalExits() const
{
	return rootSignalExits;
}

template<typename TType>
inline const std::vector<cRegister::tRootMemoryExit<TType>>& cRegisterRootModule<TType>::getRootMemoryExits() const
{
	return rootMemoryExits;
}

//

template<typename TType>
inline void cRegisterLogicModule<TType>::setModuleName(const tModuleName& moduleName)
{
	this->moduleName = moduleName;
}

template<typename TType>
inline void cRegisterLogicModule<TType>::setCaptionName(const tCaptionName& captionName)
{
	this->captionName = captionName;
}

template<typename TType>
inline void cRegisterLogicModule<TType>::insertSignalEntry(const tSignalEntryName& signalEntryName,
                                                           const cRegisterLogicModule<TType>::tSignalEntryCallback& callback)
{
	this->signalEntries.emplace_back(signalEntryName,
	                                 cEngineValueHelper<cRegisterLogicModule<TType>::tSignalEntryCallback>(callback));
}

template<typename TType>
inline void cRegisterLogicModule<TType>::insertSignalExit(const tSignalExitName& signalExitName,
                                                          const cRegisterLogicModule<TType>::tSignalExitFlowIdPointer& flowIdPointer)
{
	this->signalExits.emplace_back(signalExitName,
	                               cEngineValueHelper<cRegisterLogicModule<TType>::tSignalExitFlowIdPointer>(flowIdPointer));
}

template<typename TType>
inline const tModuleName& cRegisterLogicModule<TType>::getModuleName() const
{
	return moduleName;
}

template<typename TType>
inline const tCaptionName& cRegisterLogicModule<TType>::getCaptionName() const
{
	return captionName;
}

template<typename TType>
inline const std::vector<cRegister::tSignalEntry<TType>>& cRegisterLogicModule<TType>::getSignalEntries() const
{
	return signalEntries;
}

template<typename TType>
inline const std::vector<cRegister::tMemoryEntry<TType>>& cRegisterLogicModule<TType>::getMemoryEntries() const
{
	return memoryEntries;
}

template<typename TType>
inline const std::vector<cRegister::tSignalExit<TType>>& cRegisterLogicModule<TType>::getSignalExits() const
{
	return signalExits;
}

template<typename TType>
inline const std::vector<cRegister::tMemoryExit<TType>>& cRegisterLogicModule<TType>::getMemoryExits() const
{
	return memoryExits;
}

template<typename TType>
inline const std::vector<cRegister::tMemoryEntryExit<TType>>& cRegisterLogicModule<TType>::getMemoryEntryExits() const
{
	return memoryEntryExits;
}

//

inline cRegister::cRegister() :
        rootSignalExitsCount(0),
        registerResult(eResult::success)
{
}

inline cRegister::~cRegister()
{
	unregisterModules();
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::registerMemoryModuleBase(const tModuleName& moduleName,
                                            const cMemoryValueType& memoryValueType,
                                            TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	registerResult = localRegisterMemoryModuleBase<TType>(moduleName,
	                                                      memoryValueType,
	                                                      args ...);
	return registerResult;
}

template<typename TType,
         typename ... TArgs>
inline eResult cRegister::registerRootModuleBase(const tModuleName& moduleName,
                                                 const tCaptionName& captionName,
                                                 const std::vector<tRootSignalExit<TType>>& rootSignalExits,
                                                 const std::vector<tRootMemoryExit<TType>>& rootMemoryExits,
                                                 TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	registerResult = localRegisterRootModuleBase<TType>(moduleName,
	                                                    captionName,
	                                                    rootSignalExits,
	                                                    rootMemoryExits,
	                                                    args ...);
	return registerResult;
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::registerLogicModuleBase(const tModuleName& moduleName,
                                           const tCaptionName& captionName,
                                           const std::vector<tSignalEntry<TType>>& signalEntries,
                                           const std::vector<tMemoryEntry<TType>>& memoryEntries,
                                           const std::vector<tSignalExit<TType>>& signalExits,
                                           const std::vector<tMemoryExit<TType>>& memoryExits,
                                           const std::vector<tMemoryEntryExit<TType>>& memoryEntryExits,
                                           TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	registerResult = localRegisterLogicModuleBase(moduleName,
	                                              captionName,
	                                              signalEntries,
	                                              memoryEntries,
	                                              signalExits,
	                                              memoryExits,
	                                              memoryEntryExits,
	                                              args ...);
	return registerResult;
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::localRegisterMemoryModuleBase(TVM_ENGINE_UNUSED const tModuleName& moduleName,
                                                 TVM_ENGINE_UNUSED const cMemoryValueType& memoryValueType,
                                                 TVM_CONVERTER_UNUSED TArgs ... args)
{
	eResult result = eResult::success;

#ifdef TVM_ENGINE
	class cManager : public cMemoryModuleManager
	{
	public:
		cManager(TArgs ... args) :
		        arguments(args ...)
		{
		}

		void* create(const std::vector<uint8_t>& value) final
		{
			TType* module = std::apply([](TArgs ... args){return new TType(args ...);}, arguments);

			cStreamIn stream(value);
			stream.pop(*module);

			/// @todo:
			/// if (stream.isFailed())
			/// {
			/// 	delete module;
			/// 	return nullptr;
			/// }

			return module;
		}

		void destroy(void* pointer) final
		{
			TType* module = (TType*)pointer;
			delete module;
		}

	private:
		std::tuple<TArgs ...> arguments;
	};

	cManager* manager = new cManager(args ...);
	if (!manager)
	{
		TVM_LOG_ERROR("error allocation memory\n");
		return eResult::errorAllocationMemory;
	}

	memoryModuleManagers.emplace_back(manager);
#endif

#ifdef TVM_CONVERTER
	if (moduleName.contains(':'))
	{
		TVM_LOG_ERROR("invalid moduleName: '%s'\n", moduleName.getString().data());
		return eResult::invalidName;
	}

	/// @todo: add more checks

	std::string totalModuleName = ":memory:" + moduleName.getString();
	tConverterMemoryModule converterMemoryModule = {totalModuleName.data(),
	                                                memoryValueType};

	converterMemoryModules.emplace_back(converterMemoryModule);
#endif

	return result;
}

template<typename TType,
         typename ... TArgs>
inline eResult cRegister::localRegisterRootModuleBase(TVM_ENGINE_UNUSED const tModuleName& moduleName,
                                                      TVM_ENGINE_UNUSED const tCaptionName& captionName,
                                                      const std::vector<tRootSignalExit<TType>>& rootSignalExits,
                                                      const std::vector<tRootMemoryExit<TType>>& rootMemoryExits,
                                                      TVM_CONVERTER_UNUSED TArgs ... args)
{
	eResult result = eResult::success;

#ifdef TVM_ENGINE
	class cManager : public cRootModuleManager
	{
	public:
		cManager(tRootSignalExitId rootSignalExitStartId,
		         const std::vector<tRootSignalExit<TType>>& rootSignalExits,
		         const std::vector<tRootMemoryExit<TType>>& rootMemoryExits,
		         TArgs ... args) :
		        module(args ...)
		{
			for (const auto& iter : rootSignalExits)
			{
				tRootSignalExitId* rootSignalExitId = &(module.*(std::get<1>(iter).getValue()));
				*rootSignalExitId = rootSignalExitStartId;
				rootSignalExitStartId++;
			}

			for (const auto& iter : rootMemoryExits)
			{
				rootMemoryExitPointers.emplace_back(std::get<2>(iter).getValue());
			}
		}

		void initRootMemory() final
		{
			for (const auto& iter : rootMemoryExitPointers)
			{
				void** memoryPointer = &(module.*(iter));
				*memoryPointer = nullptr;
			}
		}

		eResult setRootMemoryExit(const tRootMemoryExitId& rootMemoryExitId,
		                          void* memoryModulePointer) final
		{
			if (rootMemoryExitPointers.size() <= rootMemoryExitId)
			{
				TVM_LOG_ERROR("invalid rootMemoryExitId: '%" PRIu32 "'\n", rootMemoryExitId);
				return eResult::invalidRootMemoryExitId;
			}

			void** memoryPointer = &(module.*(rootMemoryExitPointers[rootMemoryExitId]));
			*memoryPointer = memoryModulePointer;

			return eResult::success;
		}

		tUniqueId getRootModuleUniqueId() const final
		{
			return getUniqueId<TType>();
		}

		const void* getRootModule() const final
		{
			return (const void*)&module;
		}

	private:
		TType module;
		std::vector<void* (TType::*)> rootMemoryExitPointers;
	};

	cManager* manager = new cManager(rootSignalExitsCount,
	                                 rootSignalExits,
	                                 rootMemoryExits,
	                                 args ...);
	if (!manager)
	{
		TVM_LOG_ERROR("error allocation memory\n");
		return eResult::errorAllocationMemory;
	}

	const tUniqueId uniqueId = manager->getRootModuleUniqueId();
	for (const auto& iter : rootModuleManagers)
	{
		if (iter->getRootModuleUniqueId() == uniqueId)
		{
			TVM_LOG_ERROR("rootModule with uniqueId '%" PRIu64 "' already exist\n", uniqueId);
			delete manager;
			return eResult::alreadyExist;
		}
	}

	rootSignalExitsCount += rootSignalExits.size();

	rootModuleManagers.emplace_back(manager);
#endif

#ifdef TVM_CONVERTER
	if (moduleName.contains(':'))
	{
		TVM_LOG_ERROR("invalid moduleName: '%s'\n", moduleName.getString().data());
		return eResult::invalidName;
	}

	std::string totalModuleName = ":root:" + moduleName.getString();
	tConverterRootModule converterRootModule = {totalModuleName.data(),
	                                            captionName,
	                                            {},
	                                            {}};

	auto& converterRootSignalExits = std::get<2>(converterRootModule);
	auto& converterRootMemoryExits = std::get<3>(converterRootModule);

	for (const auto& iter : rootSignalExits)
	{
		tRootSignalExitId rootSignalId = rootSignalExitsCount;
		rootSignalExitsCount++;

		if (std::get<0>(iter).contains(':'))
		{
			TVM_LOG_ERROR("invalid signalExitName: '%s'\n", std::get<0>(iter).getString().data());
			return eResult::invalidName;
		}

		std::tuple<tSignalExitName, tRootSignalExitId> converterSignalExit = {std::get<0>(iter),
		                                                                      rootSignalId};
		converterRootSignalExits.emplace_back(converterSignalExit);
	}

	for (const auto& iter : rootMemoryExits)
	{
		const tMemoryExitName& memoryExitName = std::get<0>(iter);
		const tModuleName& memoryModuleName = std::get<1>(iter);

		if (memoryExitName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryExitName: '%s'\n", memoryExitName.getString().data());
			return eResult::invalidName;
		}
		else if (memoryModuleName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryModuleName: '%s'\n", memoryModuleName.getString().data());
			return eResult::invalidName;
		}

		std::string totalMemoryModuleName = ":memory:" + memoryModuleName.getString();
		converterRootMemoryExits.emplace_back(std::make_tuple(memoryExitName,
		                                                      totalMemoryModuleName.data()));
	}

	/// @todo: add more checks

	converterRootModules.emplace_back(converterRootModule);
#endif

	return result;
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::localRegisterLogicModuleBase(TVM_ENGINE_UNUSED const tModuleName& moduleName,
                                                TVM_ENGINE_UNUSED const tCaptionName& captionName,
                                                const std::vector<tSignalEntry<TType>>& signalEntries,
                                                const std::vector<tMemoryEntry<TType>>& memoryEntries,
                                                const std::vector<tSignalExit<TType>>& signalExits,
                                                const std::vector<tMemoryExit<TType>>& memoryExits,
                                                const std::vector<tMemoryEntryExit<TType>>& memoryEntryExits,
                                                TVM_CONVERTER_UNUSED TArgs ... args)
{
	eResult result = eResult::success;

#ifdef TVM_ENGINE
	class cCallback : public cCallbackBase
	{
	public:
		cCallback(tFlowId (TType::*signalEntry)(),
		                TType* object) :
		        signalEntry(signalEntry),
		        object(object)
		{
		}

		tFlowId call() final
		{
			return (object->*signalEntry.getValue())();
		}

	private:
		cEngineValueHelper<tFlowId (TType::*)()> signalEntry;
		TType* object;
	};

	class cManager : public cLogicModuleManager
	{
	public:
		cManager(const std::vector<tSignalEntry<TType>>& signalEntries,
		         const std::vector<tMemoryEntry<TType>>& memoryEntries,
		         const std::vector<tSignalExit<TType>>& signalExits,
		         const std::vector<tMemoryExit<TType>>& memoryExits,
		         const std::vector<tMemoryEntryExit<TType>>& memoryEntryExits,
		         TArgs ... args) :
		        arguments(args ...)
		{
			for (const auto& iter : signalEntries)
			{
				signalEntryCallbacks.emplace_back(std::get<1>(iter).getValue());
			}

			for (const auto& iter : memoryEntries)
			{
				memoryEntryPointers.emplace_back(std::get<2>(iter).getValue());
			}

			for (const auto& iter : signalExits)
			{
				signalExitFlowIds.emplace_back(std::get<1>(iter).getValue());
			}

			for (const auto& iter : memoryExits)
			{
				memoryExitPointers.emplace_back(std::get<2>(iter).getValue());
			}

			for (const auto& iter : memoryEntryExits)
			{
				memoryEntryExitPointers.emplace_back(std::get<2>(iter).getValue());
			}
		}

		void* create() final
		{
			TType* module = std::apply([](TArgs ... args){return new TType(args ...);}, arguments);

			for (const auto& iter : signalExitFlowIds)
			{
				tFlowId* flowId = &(module->*(iter));
				*flowId = flowNowhere;
			}

			for (const auto& iter : memoryEntryPointers)
			{
				const void** memoryPointer = &(module->*(iter));
				*memoryPointer = nullptr;
			}

			for (const auto& iter : memoryExitPointers)
			{
				void** memoryPointer = &(module->*(iter));
				*memoryPointer = nullptr;
			}

			for (const auto& iter : memoryEntryExitPointers)
			{
				void** memoryPointer = &(module->*(iter));
				*memoryPointer = nullptr;
			}

			return module;
		}

		void destroy(void* modulePointer) final
		{
			TType* module = (TType*)modulePointer;
			delete module;
		}

		eResult getCallback(const tSignalEntryId& signalEntryId,
		                    void* modulePointer,
		                    cCallbackBase** callback) final
		{
			if (signalEntryCallbacks.size() <= signalEntryId)
			{
				TVM_LOG_ERROR("invalid signal entry id: '%" PRIu32 "'\n", signalEntryId);
				return eResult::invalidSignalEntryId;
			}

			*callback = new cCallback(signalEntryCallbacks[signalEntryId], (TType*)modulePointer);
			if (*callback == nullptr)
			{
				return eResult::errorAllocationMemory;
			}

			return eResult::success;
		}

		eResult getFlowId(const tSignalExitId& signalExitId,
		                  void* modulePointer,
		                  tFlowId** flowId) final
		{
			if (signalExitFlowIds.size() <= signalExitId)
			{
				TVM_LOG_ERROR("invalid signal exit id: '%" PRIu32 "'\n", signalExitId);
				return eResult::invalidSignalExitId;
			}

			TType* module = (TType*)modulePointer;
			*flowId = &(module->*(signalExitFlowIds[signalExitId]));

			return eResult::success;
		}

		eResult getMemoryEntry(const tMemoryEntryId& memoryEntryId,
		                       void* modulePointer,
		                       const void*** memoryPointer) final
		{
			if (memoryEntryPointers.size() <= memoryEntryId)
			{
				TVM_LOG_ERROR("invalid memory entry id: '%" PRIu32 "'\n", memoryEntryId);
				return eResult::invalidMemoryEntryId;
			}

			TType* module = (TType*)modulePointer;
			*memoryPointer = &(module->*(memoryEntryPointers[memoryEntryId]));

			return eResult::success;
		}

		eResult getMemoryExit(const tMemoryExitId& memoryExitId,
		                      void* modulePointer,
		                      void*** memoryPointer) final
		{
			if (memoryExitPointers.size() <= memoryExitId)
			{
				TVM_LOG_ERROR("invalid memory exit id: '%" PRIu32 "'\n", memoryExitId);
				return eResult::invalidMemoryExitId;
			}

			TType* module = (TType*)modulePointer;
			*memoryPointer = &(module->*(memoryExitPointers[memoryExitId]));

			return eResult::success;
		}

		eResult getMemoryEntryExit(const tMemoryEntryExitId& memoryEntryExitId,
		                           void* modulePointer,
		                           void*** memoryPointer) final
		{
			if (memoryEntryExitPointers.size() <= memoryEntryExitId)
			{
				TVM_LOG_ERROR("invalid memory entryExit id: '%" PRIu32 "'\n", memoryEntryExitId);
				return eResult::invalidMemoryEntryExitId;
			}

			TType* module = (TType*)modulePointer;
			*memoryPointer = &(module->*(memoryEntryExitPointers[memoryEntryExitId]));

			return eResult::success;
		}

	private:
		std::tuple<TArgs ...> arguments;
		std::vector<tFlowId (TType::*)()> signalEntryCallbacks;
		std::vector<const void* (TType::*)> memoryEntryPointers;
		std::vector<tFlowId (TType::*)> signalExitFlowIds;
		std::vector<void* (TType::*)> memoryExitPointers;
		std::vector<void* (TType::*)> memoryEntryExitPointers;
	};

	cManager* manager = new cManager(signalEntries,
	                                 memoryEntries,
	                                 signalExits,
	                                 memoryExits,
	                                 memoryEntryExits,
	                                 args ...);
	if (!manager)
	{
		TVM_LOG_ERROR("error allocation memory\n");
		return eResult::errorAllocationMemory;
	}

	logicModuleManagers.emplace_back(manager);
#endif

#ifdef TVM_CONVERTER
	if (moduleName.contains(':'))
	{
		TVM_LOG_ERROR("invalid moduleName: '%s'\n", moduleName.getString().data());
		return eResult::invalidName;
	}

	std::string totalModuleName = ":logic:" + moduleName.getString();
	tConverterLogicModule converterLogicModule = {totalModuleName.data(),
	                                              captionName,
	                                              {},
	                                              {},
	                                              {},
	                                              {},
	                                              {}};

	auto& converterSignalEntries = std::get<2>(converterLogicModule);
	auto& converterMemoryEntries = std::get<3>(converterLogicModule);
	auto& converterSignalExits = std::get<4>(converterLogicModule);
	auto& converterMemoryExits = std::get<5>(converterLogicModule);
	auto& converterMemoryEntryExits = std::get<6>(converterLogicModule);

	for (const auto& iter : signalEntries)
	{
		const tSignalEntryName& signalEntryName = std::get<0>(iter);

		if (signalEntryName.contains(':'))
		{
			TVM_LOG_ERROR("invalid signalEntryName: '%s'\n", signalEntryName.getString().data());
			return eResult::invalidName;
		}

		converterSignalEntries.emplace_back(signalEntryName);
	}

	for (const auto& iter : memoryEntries)
	{
		const tMemoryEntryName& memoryEntryName = std::get<0>(iter);
		const tModuleName& memoryModuleName = std::get<1>(iter);

		if (memoryEntryName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryEntryName: '%s'\n", memoryEntryName.getString().data());
			return eResult::invalidName;
		}
		else if (memoryModuleName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryModuleName: '%s'\n", memoryModuleName.getString().data());
			return eResult::invalidName;
		}

		std::string totalMemoryModuleName = ":memory:" + memoryModuleName.getString();
		converterMemoryEntries.emplace_back(std::make_tuple(memoryEntryName,
		                                                    totalMemoryModuleName.data()));
	}

	for (const auto& iter : signalExits)
	{
		const tSignalExitName& signalExitName = std::get<0>(iter);

		if (signalExitName.contains(':'))
		{
			TVM_LOG_ERROR("invalid signalExitName: '%s'\n", signalExitName.getString().data());
			return eResult::invalidName;
		}

		converterSignalExits.emplace_back(signalExitName);
	}

	for (const auto& iter : memoryExits)
	{
		const tMemoryExitName& memoryExitName = std::get<0>(iter);
		const tModuleName& memoryModuleName = std::get<1>(iter);

		if (memoryExitName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryExitName: '%s'\n", memoryExitName.getString().data());
			return eResult::invalidName;
		}
		else if (memoryModuleName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryModuleName: '%s'\n", memoryModuleName.getString().data());
			return eResult::invalidName;
		}

		std::string totalMemoryModuleName = ":memory:" + memoryModuleName.getString();
		converterMemoryExits.emplace_back(std::make_tuple(memoryExitName,
		                                                  totalMemoryModuleName.data()));
	}

	for (const auto& iter : memoryEntryExits)
	{
		const tMemoryEntryExitName& memoryEntryExitName = std::get<0>(iter);
		const tModuleName& memoryModuleName = std::get<1>(iter);

		if (memoryEntryExitName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryEntryExitName: '%s'\n", memoryEntryExitName.getString().data());
			return eResult::invalidName;
		}
		else if (memoryModuleName.contains(':'))
		{
			TVM_LOG_ERROR("invalid memoryModuleName: '%s'\n", memoryModuleName.getString().data());
			return eResult::invalidName;
		}

		std::string totalMemoryModuleName = ":memory:" + memoryModuleName.getString();
		converterMemoryEntryExits.emplace_back(std::make_tuple(memoryEntryExitName,
		                                                       totalMemoryModuleName.data()));
	}

	/// @todo: add more checks
	/// @todo: add checks for signal entry, exit, etc

	converterLogicModules.emplace_back(converterLogicModule);
#endif

	return result;
}

inline void cRegister::unregisterModules()
{
#ifdef TVM_ENGINE
	for (auto& iter : memoryModuleManagers)
	{
		delete(iter);
	}
	memoryModuleManagers.clear();

	for (auto& iter : rootModuleManagers)
	{
		delete(iter);
	}
	rootModuleManagers.clear();

	for (auto& iter : logicModuleManagers)
	{
		delete(iter);
	}
	logicModuleManagers.clear();
#endif

#ifdef TVM_CONVERTER
	converterMemoryModules.clear();
	converterRootModules.clear();
	converterLogicModules.clear();
#endif

	rootSignalExitsCount = 0;
	registerResult = eResult::success;
}

inline eResult cRegister::getRegisterResult() const
{
	return registerResult;
}

//

template<typename TType,
         typename ... TArgs>
eResult cRegister::registerMemoryModule(TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	cRegisterMemoryModule<TType> registerModule;
	TType::tvmRegisterModule(registerModule);
	registerResult = localRegisterMemoryModuleBase<TType>(registerModule.getModuleName(),
	                                                      registerModule.getMemoryValueType(),
	                                                      args ...);

	return registerResult;
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::registerRootModule(TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	cRegisterRootModule<TType> registerModule;
	TType::tvmRegisterModule(registerModule);
	registerResult = localRegisterRootModuleBase<TType>(registerModule.getModuleName(),
	                                                    registerModule.getCaptionName(),
	                                                    registerModule.getRootSignalExits(),
	                                                    registerModule.getRootMemoryExits(),
	                                                    args ...);

	return registerResult;
}

template<typename TType,
         typename ... TArgs>
eResult cRegister::registerLogicModule(TArgs ... args)
{
	if (registerResult != eResult::success)
	{
		return registerResult;
	}

	cRegisterLogicModule<TType> registerModule;
	TType::tvmRegisterModule(registerModule);
	registerResult = localRegisterLogicModuleBase<TType>(registerModule.getModuleName(),
	                                                     registerModule.getCaptionName(),
	                                                     registerModule.getSignalEntries(),
	                                                     registerModule.getMemoryEntries(),
	                                                     registerModule.getSignalExits(),
	                                                     registerModule.getMemoryExits(),
	                                                     registerModule.getMemoryEntryExits(),
	                                                     args ...);

	return registerResult;
}

}
