// Copyright © 2017, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#ifndef TVM_LOGIC_H
#define TVM_LOGIC_H

#include <functional>

#include "module.h"
#include "signal.h"

namespace nVirtualMachine
{

template<class ...>
using void_t = void;

class cLogicModule: public cModule
{
public:
	const tModuleTypeName getModuleTypeName() const override final
	{
		return "logic";
	}
};

class cLogicNull : public cLogicModule
{
public:
	cLogicNull(...)
	{
	}

	cModule* clone() const override
	{
		return nullptr;
	}

	bool registerModule()
	{
		setModuleName(":null");
		return true;
	}
};

template<typename TType>
class cLogicCopy : public cLogicModule
{
public:
	cLogicCopy(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicCopy(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("copy");
		setCaptionName("copy");

		if (!registerSignalEntry("signal", &cLogicCopy::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitSignal))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			*to = *from;
		}
		return signalFlow(signalExitSignal);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitSignal = 1;

private:
	TType* from;
	TType* to;
};

template<typename TType>
class cLogicVectorPushBack : public cLogicModule
{
public:
	cLogicVectorPushBack(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorPushBack(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("pushBack");
		setCaptionName("pushBack");

		if (!registerSignalEntry("signal", &cLogicVectorPushBack::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitSignal))
		{
			return false;
		}

		if (!registerMemoryExit("vector<" + memoryTypeName.value + ">", "vector<" + memoryTypeName.value + ">", to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			to->push_back(*from);
		}
		return signalFlow(signalExitSignal);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitSignal = 1;

private:
	TType* from;
	std::vector<TType>* to;
};

template<typename TType>
class cLogicVectorPopBack : public cLogicModule
{
public:
	cLogicVectorPopBack(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorPopBack(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("popBack");
		setCaptionName("popBack");

		if (!registerSignalEntry("signal", &cLogicVectorPopBack::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("vector<" + memoryTypeName.value + ">", "vector<" + memoryTypeName.value + ">", from))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("empty", signalExitEmpty))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from)
		{
			if (!from->size())
			{
				return signalFlow(signalExitEmpty);
			}
			if (to)
			{
				*to = from->back();
			}
			from->pop_back();
		}
		return signalFlow(signalExitDone);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitEmpty = 2;

private:
	std::vector<TType>* from;
	TType* to;
};

template<typename TType, typename TIntegerType>
class cLogicVectorGet : public cLogicModule
{
	using tVector = std::vector<TType>;

public:
	cLogicVectorGet(const tMemoryTypeName& memoryTypeName,
	                const tMemoryTypeName& memoryIntegerTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryIntegerTypeName(memoryIntegerTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorGet(memoryTypeName,
		                           memoryIntegerTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameVector = "vector<" + memoryTypeName.value + ">";

		setModuleName("get");
		setCaptionName("get");

		if (!registerSignalEntry("signal", &cLogicVectorGet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameVector.value, memoryTypeNameVector, vector))
		{
			return false;
		}

		if (!registerMemoryEntry("index", memoryIntegerTypeName, index))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (vector && index)
		{
			if ((std::size_t)(*index) >= vector->size())
			{
				return signalFlow(signalExitFail);
			}

			if (value)
			{
				*value = (*vector)[*index];
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryIntegerTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	tVector* vector;
	TIntegerType* index;
	TType* value;
};

template<typename TType>
class cLogicVectorGetRandomItem : public cLogicModule
{
	using tVector = std::vector<TType>;

public:
	cLogicVectorGetRandomItem(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorGetRandomItem(memoryTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameVector = "vector<" + memoryTypeName.value + ">";

		setModuleName("getRandomItem");
		setCaptionName("getRandomItem");

		if (!registerSignalEntry("signal", &cLogicVectorGetRandomItem::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameVector.value, memoryTypeNameVector, vector))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (vector && vector->size())
		{
			std::size_t index = rand() % vector->size();

			if (value)
			{
				*value = (*vector)[index];
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	tVector* vector;
	TType* value;
};

template<typename TType, typename TIntegerType>
class cLogicVectorSet : public cLogicModule
{
	using tVector = std::vector<TType>;

public:
	cLogicVectorSet(const tMemoryTypeName& memoryTypeName,
	                const tMemoryTypeName& memoryIntegerTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryIntegerTypeName(memoryIntegerTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorSet(memoryTypeName,
		                           memoryIntegerTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameVector = "vector<" + memoryTypeName.value + ">";

		setModuleName("set");
		setCaptionName("set");

		if (!registerSignalEntry("signal", &cLogicVectorSet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, value))
		{
			return false;
		}

		if (!registerMemoryEntry("index", memoryIntegerTypeName, index))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameVector.value, memoryTypeNameVector, vector))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (vector && index)
		{
			if ((std::size_t)(*index) >= vector->size())
			{
				return signalFlow(signalExitFail);
			}

			if (value)
			{
				(*vector)[*index] = *value;
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryIntegerTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	TType* value;
	TIntegerType* index;
	tVector* vector;
};

template<typename TType, typename TIntegerType>
class cLogicSize : public cLogicModule
{
public:
	cLogicSize(const tModuleName& moduleName,
	           const tMemoryTypeName& memoryTypeName,
	           const tMemoryTypeName& memoryIntegerTypeName) :
	        moduleName(moduleName),
	        memoryTypeName(memoryTypeName),
	        memoryIntegerTypeName(memoryIntegerTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicSize(moduleName,
		                      memoryTypeName,
		                      memoryIntegerTypeName);
	}

	bool registerModule() override
	{
		setModuleName(moduleName);
		setCaptionName(moduleName.value);

		if (!registerSignalEntry("signal", &cLogicSize::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitDone))
		{
			return false;
		}

		if (!registerMemoryExit(memoryIntegerTypeName.value, memoryIntegerTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			*to = from->size();
		}
		return signalFlow(signalExitDone);
	}

private:
	const tModuleName moduleName;
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryIntegerTypeName;

private:
	const tSignalExitId signalExitDone = 1;

private:
	TType* from;
	TIntegerType* to;
};

template<typename TType>
class cLogicVectorForEach : public cLogicModule
{
	using tVector = std::vector<TType>;
	using tVectorIterator = typename std::vector<TType>::const_iterator;

public:
	cLogicVectorForEach(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicVectorForEach(memoryTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameVector = "vector<" + memoryTypeName.value + ">";

		setModuleName("forEach");
		setCaptionName("forEach");

		if (!registerSignalEntry("begin", &cLogicVectorForEach::signalEntryBegin))
		{
			return false;
		}

		if (!registerSignalEntry("continue", &cLogicVectorForEach::signalEntryContinue))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameVector.value, memoryTypeNameVector, vector))
		{
			return false;
		}

		if (!registerSignalExit("iteration", signalExitIteration))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntryBegin()
	{
		if (!vector)
		{
			return signalFlow(signalExitDone);
		}

		iter = vector->begin();

		return iteration();
	}

	bool signalEntryContinue()
	{
		if (!vector)
		{
			return signalFlow(signalExitDone);
		}

		return iteration();
	}

private:
	bool iteration()
	{
		if (iter != vector->end())
		{
			if (value)
			{
				*value = *iter;
			}

			++iter;
			return signalFlow(signalExitIteration);
		}
		return signalFlow(signalExitDone);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitIteration = 1;
	const tSignalExitId signalExitDone = 2;

private:
	tVector* vector;
	TType* value;

private:
	tVectorIterator iter;
};

template<typename TType, typename TBooleanType>
class cLogicIsEmpty : public cLogicModule
{
public:
	cLogicIsEmpty(const tMemoryTypeName& memoryTypeName,
	                    const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIsEmpty(memoryTypeName,
		                         memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("isEmpty");
		setCaptionName("isEmpty");

		if (!registerSignalEntry("signal", &cLogicIsEmpty::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from)
		{
			if (to)
			{
				*to = from->empty();
			}

			if (from->empty())
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* from;
	TBooleanType* to;
};

template<typename TType, typename TBooleanType>
class cLogicIfEqual : public cLogicModule
{
public:
	cLogicIfEqual(const tMemoryTypeName& memoryTypeName,
	              const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIfEqual(memoryTypeName,
		                         memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("ifEqual");
		setCaptionName("ifEqual");

		if (!registerSignalEntry("signal", &cLogicIfEqual::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second)
		{
			TBooleanType value = ((*first) == (*second));

			if (to)
			{
				*to = value;
			}

			if (value)
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* first;
	TType* second;
	TBooleanType* to;
};

template<typename TType, typename TBooleanType>
class cLogicIfGreater : public cLogicModule
{
public:
	cLogicIfGreater(const tMemoryTypeName& memoryTypeName,
	                const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIfGreater(memoryTypeName,
		                           memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("ifGreater");
		setCaptionName("ifGreater");

		if (!registerSignalEntry("signal", &cLogicIfGreater::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second)
		{
			TBooleanType value = ((*first) > (*second));

			if (to)
			{
				*to = value;
			}

			if (value)
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* first;
	TType* second;
	TBooleanType* to;
};

template<typename TType, typename TBooleanType>
class cLogicIfLess : public cLogicModule
{
public:
	cLogicIfLess(const tMemoryTypeName& memoryTypeName,
	             const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIfLess(memoryTypeName,
		                        memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("ifLess");
		setCaptionName("ifLess");

		if (!registerSignalEntry("signal", &cLogicIfLess::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second)
		{
			TBooleanType value = ((*first) < (*second));

			if (to)
			{
				*to = value;
			}

			if (value)
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* first;
	TType* second;
	TBooleanType* to;
};

template<typename TType, typename TBooleanType>
class cLogicIfGreaterOrEqual : public cLogicModule
{
public:
	cLogicIfGreaterOrEqual(const tMemoryTypeName& memoryTypeName,
	                       const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIfGreaterOrEqual(memoryTypeName,
		                                  memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("ifGreaterOrEqual");
		setCaptionName("ifGreaterOrEqual");

		if (!registerSignalEntry("signal", &cLogicIfGreaterOrEqual::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second)
		{
			TBooleanType value = ((*first) >= (*second));

			if (to)
			{
				*to = value;
			}

			if (value)
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* first;
	TType* second;
	TBooleanType* to;
};

template<typename TType, typename TBooleanType>
class cLogicIfLessOrEqual : public cLogicModule
{
public:
	cLogicIfLessOrEqual(const tMemoryTypeName& memoryTypeName,
	                    const tMemoryTypeName& memoryBooleanTypeName) :
	        memoryTypeName(memoryTypeName),
	        memoryBooleanTypeName(memoryBooleanTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIfLessOrEqual(memoryTypeName,
		                               memoryBooleanTypeName);
	}

	bool registerModule() override
	{
		setModuleName("ifLessOrEqual");
		setCaptionName("ifLessOrEqual");

		if (!registerSignalEntry("signal", &cLogicIfLessOrEqual::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		if (!registerMemoryExit(memoryBooleanTypeName.value, memoryBooleanTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second)
		{
			TBooleanType value = ((*first) <= (*second));

			if (to)
			{
				*to = value;
			}

			if (value)
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryBooleanTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* first;
	TType* second;
	TBooleanType* to;
};

template<typename TType>
class cLogicSetClear : public cLogicModule
{
public:
	cLogicSetClear(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicSetClear(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("setClear");
		setCaptionName("setClear");

		if (!registerSignalEntry("signal", &cLogicSetClear::signalEntry))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitDone))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName.value, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (to)
		{
			to->clear();
		}
		return signalFlow(signalExitDone);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitDone = 1;

private:
	TType* to;
};

template<typename TKeyType,
         typename TValueType>
class cLogicMapInsertOrUpdate : public cLogicModule
{
	using tMap = std::map<TKeyType, TValueType>;

public:
	cLogicMapInsertOrUpdate(const tMemoryTypeName& memoryKeyTypeName,
	                        const tMemoryTypeName& memoryValueTypeName) :
	        memoryKeyTypeName(memoryKeyTypeName),
	        memoryValueTypeName(memoryValueTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMapInsertOrUpdate(memoryKeyTypeName,
		                                   memoryValueTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		setModuleName("insertOrUpdate");
		setCaptionName("insertOrUpdate");

		if (!registerSignalEntry("signal", &cLogicMapInsertOrUpdate::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("key", memoryKeyTypeName, key))
		{
			return false;
		}

		if (!registerMemoryEntry("value", memoryValueTypeName, value))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameMap.value, memoryTypeNameMap, map))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (map && key && value)
		{
			(*map)[*key] = *value;
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryKeyTypeName;
	const tMemoryTypeName memoryValueTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TKeyType* key;
	TValueType* value;
	tMap* map;
};

template<typename TKeyType,
         typename TValueType>
class cLogicMapFind : public cLogicModule
{
	using tMap = std::map<TKeyType, TValueType>;

public:
	cLogicMapFind(const tMemoryTypeName& memoryKeyTypeName,
	              const tMemoryTypeName& memoryValueTypeName) :
	        memoryKeyTypeName(memoryKeyTypeName),
	        memoryValueTypeName(memoryValueTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMapFind(memoryKeyTypeName,
		                         memoryValueTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		setModuleName("find");
		setCaptionName("find");

		if (!registerSignalEntry("signal", &cLogicMapFind::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameMap.value, memoryTypeNameMap, map))
		{
			return false;
		}

		if (!registerMemoryEntry("key", memoryKeyTypeName, key))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryValueTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (!map || !key)
		{
			return signalFlow(signalExitFail);
		}

		if (map->find(*key) != map->end())
		{
			if (value)
			{
				*value = (*map)[*key];
			}

			return signalFlow(signalExitDone);
		}

		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryKeyTypeName;
	const tMemoryTypeName memoryValueTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	tMap* map;
	TKeyType* key;
	TValueType* value;
};

template<typename TKeyType,
         typename TValueType>
class cLogicMapSelectOrCreate : public cLogicModule
{
	using tMap = std::map<TKeyType, TValueType>;

public:
	cLogicMapSelectOrCreate(const tMemoryTypeName& memoryKeyTypeName,
	                        const tMemoryTypeName& memoryValueTypeName) :
	        memoryKeyTypeName(memoryKeyTypeName),
	        memoryValueTypeName(memoryValueTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMapSelectOrCreate(memoryKeyTypeName,
		                                   memoryValueTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		setModuleName("selectOrCreate");
		setCaptionName("selectOrCreate");

		if (!registerSignalEntry("signal", &cLogicMapSelectOrCreate::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameMap.value, memoryTypeNameMap, map))
		{
			return false;
		}

		if (!registerMemoryEntry("key", memoryKeyTypeName, key))
		{
			return false;
		}

		if (!registerMemoryEntry("newValue", memoryValueTypeName, inValue))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryValueTypeName, outValue))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (!map || !key || !inValue || !outValue)
		{
			return signalFlow(signalExit);
		}

		if (map->find(*key) == map->end())
		{
			(*map)[*key] = *inValue;
		}

		*outValue = (*map)[*key];

		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryKeyTypeName;
	const tMemoryTypeName memoryValueTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	tMap* map;
	TKeyType* key;
	TValueType* inValue;
	TValueType* outValue;
};

template<typename TKeyType,
         typename TValueType>
class cLogicMapGet : public cLogicModule
{
	using tMap = std::map<TKeyType, TValueType>;

public:
	cLogicMapGet(const tMemoryTypeName& memoryKeyTypeName,
	             const tMemoryTypeName& memoryValueTypeName) :
	        memoryKeyTypeName(memoryKeyTypeName),
	        memoryValueTypeName(memoryValueTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMapGet(memoryKeyTypeName,
		                        memoryValueTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		setModuleName("get");
		setCaptionName("get");

		if (!registerSignalEntry("signal", &cLogicMapGet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameMap.value, memoryTypeNameMap, map))
		{
			return false;
		}

		if (!registerMemoryEntry("key", memoryKeyTypeName, key))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryValueTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (map && key)
		{
			if (map->find(*key) == map->end())
			{
				return signalFlow(signalExitFail);
			}

			if (value)
			{
				*value = (*map)[*key];
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryKeyTypeName;
	const tMemoryTypeName memoryValueTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	tMap* map;
	TKeyType* key;
	TValueType* value;
};

template<typename TKeyType,
         typename TValueType>
class cLogicMapForEach : public cLogicModule
{
	using tMap = std::map<TKeyType, TValueType>;
	using tMapIterator = typename std::map<TKeyType, TValueType>::const_iterator;

public:
	cLogicMapForEach(const tMemoryTypeName& memoryKeyTypeName,
	                 const tMemoryTypeName& memoryValueTypeName) :
	        memoryKeyTypeName(memoryKeyTypeName),
	        memoryValueTypeName(memoryValueTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMapForEach(memoryKeyTypeName,
		                            memoryValueTypeName);
	}

	bool registerModule() override
	{
		tMemoryTypeName memoryTypeNameMap = "map<" + memoryKeyTypeName.value + "," + memoryValueTypeName.value + ">";

		setModuleName("forEach");
		setCaptionName("forEach");

		if (!registerSignalEntry("begin", &cLogicMapForEach::signalEntryBegin))
		{
			return false;
		}

		if (!registerSignalEntry("continue", &cLogicMapForEach::signalEntryContinue))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameMap.value, memoryTypeNameMap, map))
		{
			return false;
		}

		if (!registerSignalExit("iteration", signalExitIteration))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerMemoryExit("key", memoryKeyTypeName, key))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryValueTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntryBegin()
	{
		if (!map)
		{
			return signalFlow(signalExitDone);
		}

		iter = map->begin();

		return iteration();
	}

	bool signalEntryContinue()
	{
		if (!map)
		{
			return signalFlow(signalExitDone);
		}

		return iteration();
	}

private:
	bool iteration()
	{
		if (iter != map->end())
		{
			if (key)
			{
				*key = iter->first;
			}

			if (value)
			{
				*value = iter->second;
			}

			++iter;
			return signalFlow(signalExitIteration);
		}
		return signalFlow(signalExitDone);
	}

private:
	const tMemoryTypeName memoryKeyTypeName;
	const tMemoryTypeName memoryValueTypeName;

private:
	const tSignalExitId signalExitIteration = 1;
	const tSignalExitId signalExitDone = 2;

private:
	tMap* map;
	TKeyType* key;
	TValueType* value;

private:
	tMapIterator iter;
};

template<typename TType>
class cLogicAppend : public cLogicModule
{
public:
	cLogicAppend(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicAppend(memoryTypeName);
	}

	bool registerModule() override
	{
		setDeprecated();

		setModuleName("append");
		setCaptionName("append");

		if (!registerSignalEntry("signal", &cLogicAppend::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitSignal))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			(*to) += (*from);
		}
		return signalFlow(signalExitSignal);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitSignal = 1;

private:
	TType* from;
	TType* to;
};

template<typename TType, typename = void>
class cLogicIncrement : public cLogicNull
{
public:
	cLogicIncrement(...)
	{
	}
};

template<typename TType>
class cLogicIncrement<TType,
                      void_t<decltype(++std::declval<TType&>())>>: public cLogicModule
{
public:
	cLogicIncrement(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIncrement(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("increment");
		setCaptionName("increment");

		if (!registerSignalEntry("signal", &cLogicIncrement::signalEntry))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitSignal))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (to)
		{
			++(*to);
		}
		return signalFlow(signalExitSignal);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitSignal = 1;

private:
	TType* to;
};

template<typename TType, typename = void>
class cLogicDecrement : public cLogicNull
{
public:
	cLogicDecrement(...)
	{
	}
};

template<typename TType>
class cLogicDecrement<TType,
                      void_t<decltype(--std::declval<TType&>())>>: public cLogicModule
{
public:
	cLogicDecrement(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicDecrement(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("decrement");
		setCaptionName("decrement");

		if (!registerSignalEntry("signal", &cLogicDecrement::signalEntry))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExitSignal))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (to)
		{
			--(*to);
		}
		return signalFlow(signalExitSignal);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitSignal = 1;

private:
	TType* to;
};

template<typename TType>
class cLogicIf : public cLogicModule
{
public:
	cLogicIf(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicIf(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("if");
		setCaptionName("if");

		if (!registerSignalEntry("signal", &cLogicIf::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, from))
		{
			return false;
		}

		if (!registerSignalExit("true", signalExitTrue))
		{
			return false;
		}

		if (!registerSignalExit("false", signalExitFalse))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from)
		{
			if (!!(*from))
			{
				return signalFlow(signalExitTrue);
			}
		}
		return signalFlow(signalExitFalse);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitTrue = 1;
	const tSignalExitId signalExitFalse = 2;

private:
	TType* from;
};

template<typename TFromType,
         typename TToType>
class cLogicConvert : public cLogicModule
{
public:
	using tFunction = std::function<void(TFromType*, TToType*)>;

public:
	cLogicConvert(const tModuleName& moduleName,
	              const tMemoryTypeName& memoryTypeNameFrom,
	              const tMemoryTypeName& memoryTypeNameTo,
	              tFunction convertFunction) :
	        moduleName(moduleName),
	        memoryTypeNameFrom(memoryTypeNameFrom),
	        memoryTypeNameTo(memoryTypeNameTo),
	        convertFunction(convertFunction)
	{
	}

	cModule* clone() const override
	{
		return new cLogicConvert(moduleName,
		                         memoryTypeNameFrom,
		                         memoryTypeNameTo,
		                         convertFunction);
	}

	bool registerModule() override
	{
		setModuleName(moduleName);
		setCaptionName("convert");
		setCaptionTypeName("convert");

		if (!registerSignalEntry("signal", &cLogicConvert::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameFrom.value, memoryTypeNameFrom, from))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameTo.value, memoryTypeNameTo, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			convertFunction(from, to);
		}
		return signalFlow(signalExit);
	}

private:
	const tModuleName moduleName;
	const tMemoryTypeName memoryTypeNameFrom;
	const tMemoryTypeName memoryTypeNameTo;
	tFunction convertFunction;

private:
	const tSignalExitId signalExit = 1;

private:
	TFromType* from;
	TToType* to;
};

template<typename TFromType,
         typename TToType>
class cLogicConvertBool : public cLogicModule
{
public:
	using tFunction = std::function<bool(TFromType*, TToType*)>;

public:
	cLogicConvertBool(const tModuleName& moduleName,
	                  const tMemoryTypeName& memoryTypeNameFrom,
	                  const tMemoryTypeName& memoryTypeNameTo,
	                  tFunction convertFunction) :
	        moduleName(moduleName),
	        memoryTypeNameFrom(memoryTypeNameFrom),
	        memoryTypeNameTo(memoryTypeNameTo),
	        convertFunction(convertFunction)
	{
	}

	cModule* clone() const override
	{
		return new cLogicConvertBool(moduleName,
		                             memoryTypeNameFrom,
		                             memoryTypeNameTo,
		                             convertFunction);
	}

	bool registerModule() override
	{
		setModuleName(moduleName);
		setCaptionName("convert");
		setCaptionTypeName("convert");

		if (!registerSignalEntry("signal", &cLogicConvertBool::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameFrom.value, memoryTypeNameFrom, from))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameTo.value, memoryTypeNameTo, to))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (from && to)
		{
			if (convertFunction(from, to))
			{
				return signalFlow(signalExitDone);
			}
		}
		return signalFlow(signalExitFail);
	}

private:
	const tModuleName moduleName;
	const tMemoryTypeName memoryTypeNameFrom;
	const tMemoryTypeName memoryTypeNameTo;
	tFunction convertFunction;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	TFromType* from;
	TToType* to;
};

template<typename ... TTypes>
class cLogicTupleGet : public cLogicModule
{
public:
	using tTuple = std::tuple<TTypes ...>;

public:
	cLogicTupleGet(const tMemoryTypeName& memoryTypeNameTuple,
	               const std::vector<tMemoryName>& memoryNames,
	               const std::vector<tMemoryTypeName>& memoryTypeNames) :
	        memoryTypeNameTuple(memoryTypeNameTuple),
	        memoryNames(memoryNames),
	        memoryTypeNames(memoryTypeNames)
	{
	}

	cModule* clone() const override
	{
		return new cLogicTupleGet(memoryTypeNameTuple,
		                          memoryNames,
		                          memoryTypeNames);
	}

	bool registerModule() override
	{
		if (sizeof...(TTypes) != memoryNames.size())
		{
			return false;
		}

		if (sizeof...(TTypes) != memoryTypeNames.size())
		{
			return false;
		}

		setModuleName("get");
		setCaptionName("get");

		if (!registerSignalEntry("signal", &cLogicTupleGet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameTuple.value, memoryTypeNameTuple, tuple))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExitTuple<0, TTypes ...>())
		{
			return false;
		}

		return true;
	}

private:
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), bool>::type
	registerMemoryExitTuple()
	{
		return true;
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), bool>::type
	registerMemoryExitTuple()
	{
		if (!registerMemoryExit(memoryNames[TTupleIndex].value,
		                        memoryTypeNames[TTupleIndex],
		                        std::get<TTupleIndex>(values)))
		{
			return false;
		}
		return registerMemoryExitTuple<TTupleIndex + 1, TArgs ...>();
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	getTuple()
	{
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), void>::type
	getTuple()
	{
		auto* pointer = std::get<TTupleIndex>(values);
		if (pointer)
		{
			*pointer = std::get<TTupleIndex>(*tuple);
		}
		getTuple<TTupleIndex + 1, TArgs ...>();
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (tuple)
		{
			getTuple<0, TTypes ...>();
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeNameTuple;
	const std::vector<tMemoryName> memoryNames;
	const std::vector<tMemoryTypeName> memoryTypeNames;

private:
	const tSignalExitId signalExit = 1;

private:
	tTuple* tuple;
	std::tuple<TTypes* ...> values;
};

template<typename ... TTypes>
class cLogicTupleSet : public cLogicModule
{
public:
	using tTuple = std::tuple<TTypes ...>;

public:
	cLogicTupleSet(const tMemoryTypeName& memoryTypeNameTuple,
	               const std::vector<tMemoryName>& memoryNames,
	               const std::vector<tMemoryTypeName>& memoryTypeNames) :
	        memoryTypeNameTuple(memoryTypeNameTuple),
	        memoryNames(memoryNames),
	        memoryTypeNames(memoryTypeNames)
	{
	}

	cModule* clone() const override
	{
		return new cLogicTupleSet(memoryTypeNameTuple,
		                          memoryNames,
		                          memoryTypeNames);
	}

	bool registerModule() override
	{
		if (sizeof...(TTypes) != memoryNames.size())
		{
			return false;
		}

		if (sizeof...(TTypes) != memoryTypeNames.size())
		{
			return false;
		}

		setModuleName("set");
		setCaptionName("set");

		if (!registerSignalEntry("signal", &cLogicTupleSet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntryTuple<0, TTypes ...>())
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameTuple.value, memoryTypeNameTuple, tuple))
		{
			return false;
		}

		return true;
	}

private:
	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), bool>::type
	registerMemoryEntryTuple()
	{
		return true;
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), bool>::type
	registerMemoryEntryTuple()
	{
		if (!registerMemoryEntry(memoryNames[TTupleIndex].value,
		                         memoryTypeNames[TTupleIndex],
		                         std::get<TTupleIndex>(values)))
		{
			return false;
		}
		return registerMemoryEntryTuple<TTupleIndex + 1, TArgs ...>();
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex == sizeof...(TArgs), void>::type
	setTuple()
	{
	}

	template<size_t TTupleIndex, typename ... TArgs>
	inline typename std::enable_if<TTupleIndex < sizeof...(TArgs), void>::type
	setTuple()
	{
		auto* pointer = std::get<TTupleIndex>(values);
		if (pointer)
		{
			std::get<TTupleIndex>(*tuple) = *pointer;
		}
		setTuple<TTupleIndex + 1, TArgs ...>();
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (tuple)
		{
			setTuple<0, TTypes ...>();
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeNameTuple;
	const std::vector<tMemoryName> memoryNames;
	const std::vector<tMemoryTypeName> memoryTypeNames;

private:
	const tSignalExitId signalExit = 1;

private:
	tTuple* tuple;
	std::tuple<TTypes* ...> values;
};

template<typename TType, typename = void>
class cLogicAddition : public cLogicNull
{
public:
	cLogicAddition(...)
	{
	}
};

template<typename TType>
class cLogicAddition<TType,
                     void_t<decltype(std::declval<TType>() + std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicAddition(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicAddition(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("addition");
		setCaptionName("addition");

		if (!registerSignalEntry("signal", &cLogicAddition::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) + (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicSubtraction : public cLogicNull
{
public:
	cLogicSubtraction(...)
	{
	}
};

template<typename TType>
class cLogicSubtraction<TType,
                        void_t<decltype(std::declval<TType>() - std::declval<TType>())>>: public cLogicModule
{
public:
	cLogicSubtraction(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicSubtraction(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("subtraction");
		setCaptionName("subtraction");

		if (!registerSignalEntry("signal", &cLogicSubtraction::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) - (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicMultiplication : public cLogicNull
{
public:
	cLogicMultiplication(...)
	{
	}
};

template<typename TType>
class cLogicMultiplication<TType,
                void_t<decltype(std::declval<TType>() * std::declval<TType>())>>: public cLogicModule
{
public:
	cLogicMultiplication(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicMultiplication(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("multiplication");
		setCaptionName("multiplication");

		if (!registerSignalEntry("signal", &cLogicMultiplication::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) * (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicDivision : public cLogicNull
{
public:
	cLogicDivision(...)
	{
	}
};

template<typename TType>
class cLogicDivision<TType,
                     void_t<decltype(std::declval<TType>() / std::declval<TType>())>>: public cLogicModule
{
public:
	cLogicDivision(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicDivision(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("division");
		setCaptionName("division");

		if (!registerSignalEntry("signal", &cLogicDivision::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) / (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicModulo : public cLogicNull
{
public:
	cLogicModulo(...)
	{
	}
};

template<typename TType>
class cLogicModulo<TType,
                   void_t<decltype(std::declval<TType>() % std::declval<TType>())>>: public cLogicModule
{
public:
	cLogicModulo(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicModulo(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("modulo");
		setCaptionName("modulo");

		if (!registerSignalEntry("signal", &cLogicModulo::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) % (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseNot : public cLogicNull
{
public:
	cLogicBitwiseNot(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseNot<TType,
                       void_t<decltype(~std::declval<TType>())>>: public cLogicModule
{
public:
	cLogicBitwiseNot(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseNot(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseNot");
		setCaptionName("bitwiseNot");

		if (!registerSignalEntry("signal", &cLogicBitwiseNot::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("value", memoryTypeName, value))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (value && result)
		{
			*result = ~(*value);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* value;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseAnd : public cLogicNull
{
public:
	cLogicBitwiseAnd(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseAnd<TType,
                       void_t<decltype(std::declval<TType>() & std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicBitwiseAnd(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseAnd(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseAnd");
		setCaptionName("bitwiseAnd");

		if (!registerSignalEntry("signal", &cLogicBitwiseAnd::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) & (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseOr : public cLogicNull
{
public:
	cLogicBitwiseOr(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseOr<TType,
                      void_t<decltype(std::declval<TType>() | std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicBitwiseOr(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseOr(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseOr");
		setCaptionName("bitwiseOr");

		if (!registerSignalEntry("signal", &cLogicBitwiseOr::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) | (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseXor : public cLogicNull
{
public:
	cLogicBitwiseXor(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseXor<TType,
                       void_t<decltype(std::declval<TType>() ^ std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicBitwiseXor(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseXor(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseXor");
		setCaptionName("bitwiseXor");

		if (!registerSignalEntry("signal", &cLogicBitwiseXor::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) ^ (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseLeftShift : public cLogicNull
{
public:
	cLogicBitwiseLeftShift(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseLeftShift<TType,
                             void_t<decltype(std::declval<TType>() << std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicBitwiseLeftShift(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseLeftShift(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseLeftShift");
		setCaptionName("bitwiseLeftShift");

		if (!registerSignalEntry("signal", &cLogicBitwiseLeftShift::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) << (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType, typename = void>
class cLogicBitwiseRightShift : public cLogicNull
{
public:
	cLogicBitwiseRightShift(...)
	{
	}
};

template<typename TType>
class cLogicBitwiseRightShift<TType,
                              void_t<decltype(std::declval<TType>() >> std::declval<TType>())>> : public cLogicModule
{
public:
	cLogicBitwiseRightShift(const tMemoryTypeName& memoryTypeName) :
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicBitwiseRightShift(memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("bitwiseRightShift");
		setCaptionName("bitwiseRightShift");

		if (!registerSignalEntry("signal", &cLogicBitwiseRightShift::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry("first", memoryTypeName, first))
		{
			return false;
		}

		if (!registerMemoryEntry("second", memoryTypeName, second))
		{
			return false;
		}

		if (!registerSignalExit("signal", signalExit))
		{
			return false;
		}

		if (!registerMemoryExit("result", memoryTypeName, result))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (first && second && result)
		{
			*result = (*first) >> (*second);
		}
		return signalFlow(signalExit);
	}

private:
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExit = 1;

private:
	TType* first;
	TType* second;
	TType* result;
};

template<typename TType,
         std::size_t TSize,
         typename TIntegerType>
class cLogicArrayGet : public cLogicModule
{
	using tArray = std::array<TType,
	                          TSize>;

public:
	cLogicArrayGet(const tMemoryTypeName& memoryTypeNameArray,
	               const tMemoryTypeName& memoryTypeName,
	               const tMemoryTypeName& memoryIntegerTypeName) :
	        memoryTypeNameArray(memoryTypeNameArray),
	        memoryTypeName(memoryTypeName),
	        memoryIntegerTypeName(memoryIntegerTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicArrayGet(memoryTypeNameArray,
		                          memoryTypeName,
		                          memoryIntegerTypeName);
	}

	bool registerModule() override
	{
		setModuleName("get");
		setCaptionName("get");

		if (!registerSignalEntry("signal", &cLogicArrayGet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameArray.value, memoryTypeNameArray, array))
		{
			return false;
		}

		if (!registerMemoryEntry("index", memoryIntegerTypeName, index))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeName.value, memoryTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (array && index)
		{
			if ((std::size_t)(*index) >= array->size())
			{
				return signalFlow(signalExitFail);
			}

			if (value)
			{
				*value = (*array)[*index];
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryTypeNameArray;
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryIntegerTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	tArray* array;
	TIntegerType* index;
	TType* value;
};

template<typename TType,
         std::size_t TSize,
         typename TIntegerType>
class cLogicArraySet : public cLogicModule
{
	using tArray = std::array<TType,
	                          TSize>;

public:
	cLogicArraySet(const tMemoryTypeName& memoryTypeNameArray,
	               const tMemoryTypeName& memoryTypeName,
	               const tMemoryTypeName& memoryIntegerTypeName) :
	        memoryTypeNameArray(memoryTypeNameArray),
	        memoryTypeName(memoryTypeName),
	        memoryIntegerTypeName(memoryIntegerTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicArraySet(memoryTypeNameArray,
		                          memoryTypeName,
		                          memoryIntegerTypeName);
	}

	bool registerModule() override
	{
		setModuleName("set");
		setCaptionName("set");

		if (!registerSignalEntry("signal", &cLogicArraySet::signalEntry))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeName.value, memoryTypeName, value))
		{
			return false;
		}

		if (!registerMemoryEntry("index", memoryIntegerTypeName, index))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerSignalExit("fail", signalExitFail))
		{
			return false;
		}

		if (!registerMemoryExit(memoryTypeNameArray.value, memoryTypeNameArray, array))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntry()
	{
		if (array && index)
		{
			if ((std::size_t)(*index) >= array->size())
			{
				return signalFlow(signalExitFail);
			}

			if (value)
			{
				(*array)[*index] = *value;
			}

			return signalFlow(signalExitDone);
		}
		return signalFlow(signalExitFail);
	}

private:
	const tMemoryTypeName memoryTypeNameArray;
	const tMemoryTypeName memoryTypeName;
	const tMemoryTypeName memoryIntegerTypeName;

private:
	const tSignalExitId signalExitDone = 1;
	const tSignalExitId signalExitFail = 2;

private:
	TType* value;
	TIntegerType* index;
	tArray* array;
};

template<typename TType,
         std::size_t TSize>
class cLogicArrayForEach : public cLogicModule
{
	using tArray = std::array<TType, TSize>;
	using tArrayIterator = typename tArray::const_iterator;

public:
	cLogicArrayForEach(const tMemoryTypeName& memoryTypeNameArray,
	                   const tMemoryTypeName& memoryTypeName) :
	        memoryTypeNameArray(memoryTypeNameArray),
	        memoryTypeName(memoryTypeName)
	{
	}

	cModule* clone() const override
	{
		return new cLogicArrayForEach(memoryTypeNameArray,
		                              memoryTypeName);
	}

	bool registerModule() override
	{
		setModuleName("forEach");
		setCaptionName("forEach");

		if (!registerSignalEntry("begin", &cLogicArrayForEach::signalEntryBegin))
		{
			return false;
		}

		if (!registerSignalEntry("continue", &cLogicArrayForEach::signalEntryContinue))
		{
			return false;
		}

		if (!registerMemoryEntry(memoryTypeNameArray.value, memoryTypeNameArray, array))
		{
			return false;
		}

		if (!registerSignalExit("iteration", signalExitIteration))
		{
			return false;
		}

		if (!registerSignalExit("done", signalExitDone))
		{
			return false;
		}

		if (!registerMemoryExit("value", memoryTypeName, value))
		{
			return false;
		}

		return true;
	}

private: /** signalEntries */
	bool signalEntryBegin()
	{
		if (!array)
		{
			return signalFlow(signalExitDone);
		}

		iter = array->begin();

		return iteration();
	}

	bool signalEntryContinue()
	{
		if (!array)
		{
			return signalFlow(signalExitDone);
		}

		return iteration();
	}

private:
	bool iteration()
	{
		if (iter != array->end())
		{
			if (value)
			{
				*value = *iter;
			}

			++iter;
			return signalFlow(signalExitIteration);
		}
		return signalFlow(signalExitDone);
	}

private:
	const tMemoryTypeName memoryTypeNameArray;
	const tMemoryTypeName memoryTypeName;

private:
	const tSignalExitId signalExitIteration = 1;
	const tSignalExitId signalExitDone = 2;

private:
	tArray* array;
	TType* value;

private:
	tArrayIterator iter;
};

}

#endif // TVM_LOGIC_H
