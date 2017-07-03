// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_ROOT_H
#define TVM_ROOT_H

#include "type.h"

namespace nVirtualMachine
{

class cLibrary;

class cRootModule
{
	friend class cLibrary;

public:
	using tSignalExits = std::map<tSignalExitName,
	                              tSignalExitId>;

	using tMemoryExits = std::map<tMemoryExitName,
	                              std::tuple<tMemoryTypeName,
	                                         tMemoryExitId>>;

public:
	cRootModule();
	~cRootModule();

	const tRootModuleName& getModuleName() const;
	const tModuleTypeName getModuleTypeName() const;

protected:
	void setModuleName(const tRootModuleName& rootModuleName);

	bool registerSignalExit(const tSignalExitName& signalExitName,
	                        tRootSignalExitId& rootSignalExitId);

	bool registerMemoryExit(const tMemoryExitName& memoryExitName,
	                        const tMemoryTypeName& memoryTypeName,
	                        tRootMemoryExitId& rootMemoryExitId);

private:
	bool doRegisterModule(cLibrary* library);
	virtual bool registerModule() = 0;

private:
	cLibrary* library;

	tRootModuleName rootModuleName;
};

inline cRootModule::cRootModule()
{
}

inline cRootModule::~cRootModule()
{
}

inline const tRootModuleName& cRootModule::getModuleName() const
{
	return rootModuleName;
}

inline const tModuleTypeName cRootModule::getModuleTypeName() const
{
	return "root";
}

inline void cRootModule::setModuleName(const tRootModuleName& rootModuleName)
{
	this->rootModuleName = rootModuleName;
}

inline bool cRootModule::doRegisterModule(cLibrary* library)
{
	this->library = library;
	return registerModule();
}

}

#endif // TVM_ROOT_H
