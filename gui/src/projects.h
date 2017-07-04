#ifndef CPROJECTSWIDGET_H
#define CPROJECTSWIDGET_H

#include <QtWidgets/QTabWidget>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cProjectsWidget : public QTabWidget
{
	Q_OBJECT

public:
	cProjectsWidget(const cVirtualMachine* virtualMachine);

	void newProject();

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CPROJECTSWIDGET_H
