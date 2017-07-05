#include <QtWidgets/QMessageBox>

#include "projects.h"
#include "project.h"

using namespace nVirtualMachine::nGui;

cProjectsWidget::cProjectsWidget(const cVirtualMachine* virtualMachine) :
        virtualMachine(virtualMachine)
{
	setMovable(true);
	setTabsClosable(true);

	connect(this, &cProjectsWidget::tabCloseRequested, this, [this](int index)
	{
		cProjectWidget* projectWidget = (cProjectWidget*)widget(index);
		if (projectWidget->hasChanges())
		{
			QMessageBox messageBox;
			if (projectWidget->getProjectName().isEmpty())
			{
				messageBox.setText("The project has been modified");
			}
			else
			{
				messageBox.setText("The project '" + projectWidget->getProjectName() + "' has been modified");
			}
			messageBox.setInformativeText("Do you want to save your changes?");
			messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			messageBox.setDefaultButton(QMessageBox::Save);
			int rc = messageBox.exec();
			if (rc == QMessageBox::Save)
			{
				if (!projectWidget->save())
				{
					return;
				}
			}
			else if (rc == QMessageBox::Discard)
			{
			}
			else
			{
				return;
			}
		}
		removeTab(index);
	});
}

void cProjectsWidget::newProject()
{
	cProjectWidget* projectWidget = createProject();
	addTab(projectWidget, projectWidget->getProjectName());
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

bool cProjectsWidget::closeAllProjects()
{
	for (int project_i = 0; project_i < count(); project_i++)
	{
		cProjectWidget* projectWidget = (cProjectWidget*)widget(project_i);
		if (projectWidget->hasChanges())
		{
			setCurrentWidget(projectWidget);
			QMessageBox messageBox;
			if (projectWidget->getProjectName().isEmpty())
			{
				messageBox.setText("The project has been modified");
			}
			else
			{
				messageBox.setText("The project '" + projectWidget->getProjectName() + "' has been modified");
			}
			messageBox.setInformativeText("Do you want to save your changes?");
			messageBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
			messageBox.setDefaultButton(QMessageBox::Save);
			int rc = messageBox.exec();
			if (rc == QMessageBox::Save)
			{
				if (!projectWidget->save())
				{
					return false;
				}
			}
			else if (rc == QMessageBox::Discard)
			{
			}
			else
			{
				return false;
			}
		}
	}

	for (int project_i = 0; project_i < count(); project_i++)
	{
		removeTab(project_i);
	}

	return true;
}

void cProjectsWidget::undo()
{
	if (currentWidget())
	{
		cProjectWidget* projectWidget = (cProjectWidget*)currentWidget();
		return projectWidget->undo();
	}
}

void cProjectsWidget::redo()
{
	if (currentWidget())
	{
		cProjectWidget* projectWidget = (cProjectWidget*)currentWidget();
		return projectWidget->redo();
	}
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

	connect(projectWidget, &cProjectWidget::projectChanged, this, [this, projectWidget](bool flagHasChanges)
	{
		int index = indexOf(projectWidget);
		if (index < 0)
		{
			return;
		}

		setTabText(index, projectWidget->getProjectName() + (flagHasChanges ? "*" : ""));
	});

	return projectWidget;
}
