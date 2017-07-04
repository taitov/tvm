#include "toolboxmodules.h"
#include "treemodules.h"

using namespace nVirtualMachine::nGui;

cToolBoxModulesWidget::cToolBoxModulesWidget()
{
	addItem(new cTreeModulesWidget(), "All");
}
