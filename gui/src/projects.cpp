#include "projects.h"
#include "project.h"

using namespace nVirtualMachine::nGui;

cProjectsWidget::cProjectsWidget(const cVirtualMachine* virtualMachine) :
        virtualMachine(virtualMachine)
{
	setMovable(true);
	setTabsClosable(true);
}

void cProjectsWidget::newProject()
{
	cProjectWidget* projectWidget = new cProjectWidget(virtualMachine);
	addTab(projectWidget, "untitled");
	setCurrentWidget(projectWidget);
}
