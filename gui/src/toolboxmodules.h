#ifndef CTOOLBOXMODULESWIDGET_H
#define CTOOLBOXMODULESWIDGET_H

#include <QtWidgets/QToolBox>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cTreeModulesWidget;

class cToolBoxModulesWidget : public QToolBox
{
	Q_OBJECT

public:
	cToolBoxModulesWidget(const cVirtualMachine* virtualMachine,
	                      bool addSchemeModules);

Q_SIGNALS:
	void moduleClicked(QString moduleFullName, QString moduleName);

private:
	cTreeModulesWidget* makeTreeModulesAll(bool addSchemeModules);

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CTOOLBOXMODULESWIDGET_H
