#include <tvm/vm.h>
#include <tvm/library/base.h>
#include <tvm/library/console.h>
#include <tvm/gui/ide.h>

using namespace nVirtualMachine;

int main(int argc, char** argv, char** envp)
{
	QApplication app(argc, argv);

	cVirtualMachine virtualMachine;

	if (!virtualMachine.registerLibraries(new nLibrary::cBase(),
	                                      new nLibrary::cConsole()))
	{
		return 1;
	}

	nGui::cIdeWidget ide(&virtualMachine, "step_1_helloworld");

	return app.exec();
}
