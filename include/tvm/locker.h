// Copyright © 2018, Timur Aitov. Contacts: timonbl4@gmail.com. All rights reserved

#pragma once

#include <shared_mutex>

namespace nVirtualMachine
{

class cLocker
{
public:
	void lock() const;
	void unlock() const;

protected:
	mutable std::shared_mutex mutex;
};

//

inline void cLocker::lock() const
{
	mutex.lock();
}

inline void cLocker::unlock() const
{
	mutex.unlock();
}

}
