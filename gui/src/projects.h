#ifndef CPROJECTSWIDGET_H
#define CPROJECTSWIDGET_H

#include <QtWidgets/QTabWidget>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cProjectWidget;

class cProjectsWidget : public QTabWidget
{
	Q_OBJECT

public:
	cProjectsWidget(const cVirtualMachine* virtualMachine);

	void newProject();
	bool open(const QString& filePath);
	bool save();
	bool saveAs();
	bool saveAllProjects();
	bool closeAllProjects();
	void undo();
	void redo();

private:
	cProjectWidget* createProject();

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CPROJECTSWIDGET_H
