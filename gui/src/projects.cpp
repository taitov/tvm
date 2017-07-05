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

	connect(projectWidget, &cProjectWidget::projectNameChanged, this, [this, projectWidget](QString projectName)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectName);
	});

	setCurrentWidget(projectWidget);
}

bool cProjectsWidget::save()
{
	if (currentWidget())
	{
		cProjectWidget* projectWidget = (cProjectWidget*)currentWidget();
		return projectWidget->save();
	}
	return false;
}

bool cProjectsWidget::saveAllProjects()
{
	for (int project_i = 0; project_i < count(); project_i++)
	{
		cProjectWidget* projectWidget = (cProjectWidget*)widget(project_i);
		if (!projectWidget->save())
		{
			return false;
		}
	}
	return true;
}
