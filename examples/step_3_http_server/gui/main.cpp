#include <tvm/vm.h>
#include <tvm/gui/ide.h>

#include "example.h"

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	QApplication app(argc, argv);

	cVirtualMachine virtualMachine;

	if (!virtualMachine.registerLibraries(new nLibrary::cExample()))
	{
		return 1;
	}

	nGui::cIdeWidget ide(&virtualMachine, "step_3_http_server");

	return app.exec();
}
