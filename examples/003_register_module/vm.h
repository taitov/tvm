#ifndef VM_H
#define VM_H

#include <tvm/register.h>

#include "vm1.h"
#include "vm2.h"

namespace nVirtualMachine
{

eResult registerModules(cRegister& virtualMachine)
{
	eResult result;

	/// first method
	result = registerModules1(virtualMachine);
	if (result != eResult::success)
	{
		return result;
	}

	/// second method
	result = registerModules2(virtualMachine);
	if (result != eResult::success)
	{
		return result;
	}

	return result;
}

}

#endif // VM_H
