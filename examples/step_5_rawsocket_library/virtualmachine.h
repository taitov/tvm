#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

#include <tfvm/library/base.h>
#include <tfvm/library/console.h>
#include <tfvm/library/rawsocket.h>
#include <tfvm/library/httpserver.h>

static bool registerVirtualMachine(nVirtualMachine::cVirtualMachine& virtualMachine,
                                   int argc = 0,
                                   char** argv = nullptr,
                                   char** envp = nullptr)
{
	using namespace nVirtualMachine::nLibrary;

	if (!virtualMachine.registerLibraries(new cBase(argc,
	                                                argv,
	                                                envp),
	                                      new cConsole(),
	                                      new cRawSocket({"mgmt0"}),
	                                      new cHttpServer("0.0.0.0", 8080)))
	{
		return false;
	}

	if (!virtualMachine.registerMemoryMap<cRawSocket::tEthernetAddress,
	                                      cRawSocket::tPortId>("ethernetAddress",
	                                                           "portId"))
	{
		return false;
	}

	return true;
}

#endif // VIRTUALMACHINE_H
