#include <tvm/vm.h>
#include <tvm/gui/ide.h>

#include "virtualmachine.h"

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	QApplication app(argc, argv);

	cVirtualMachine virtualMachine;

	if (!registerVirtualMachine(virtualMachine,
	                            argc,
	                            argv,
	                            envp))
	{
		return 1;
	}

	nGui::cIdeWidget ide(&virtualMachine, "step_5_rawsocket_library");

	return app.exec();
}
