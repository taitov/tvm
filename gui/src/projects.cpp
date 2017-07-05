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
	cProjectWidget* projectWidget = createProject();
	addTab(projectWidget, "untitled");
	setCurrentWidget(projectWidget);
}

bool cProjectsWidget::open(const QString& filePath)
{
	cProjectWidget* projectWidget = createProject();
	if (!projectWidget->open(filePath))
	{
		delete projectWidget;
		return false;
	}
	addTab(projectWidget, projectWidget->getProjectName());
	setCurrentWidget(projectWidget);
	return true;
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

bool cProjectsWidget::saveAs()
{
	if (currentWidget())
	{
		cProjectWidget* projectWidget = (cProjectWidget*)currentWidget();
		return projectWidget->saveAs();
	}
	return false;
}

bool cProjectsWidget::saveAllProjects()
{
	for (int project_i = 0; project_i < count(); project_i++)
	{
		cProjectWidget* projectWidget = (cProjectWidget*)widget(project_i);
		setCurrentWidget(projectWidget);
		if (!projectWidget->save())
		{
			return false;
		}
	}
	return true;
}

cProjectWidget* cProjectsWidget::createProject()
{
	cProjectWidget* projectWidget = new cProjectWidget(virtualMachine);

	connect(projectWidget, &cProjectWidget::projectNameChanged, this, [this, projectWidget](QString projectName)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectName);
	});

	return projectWidget;
}
