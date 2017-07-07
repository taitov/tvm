#ifndef CTOOLBOXMODULESWIDGET_H
#define CTOOLBOXMODULESWIDGET_H

#include <QtCore/QDir>

#include <QtWidgets/QToolBox>
#include <QtWidgets/QTreeWidgetItem>

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

	void setCustomModulePaths(const std::vector<QString>& paths);

Q_SIGNALS:
	void moduleClicked(QString moduleFullName, QString moduleName);

private:
	cTreeModulesWidget* makeTreeModulesAll(bool addSchemeModules);
	void updateCustomModulesItem(QTreeWidgetItem* customItem);
	void updateCustomModuleDir(QTreeWidgetItem* item, const QDir& dir, const QString& moduleFullName);

private:
	const cVirtualMachine* virtualMachine;
	std::vector<QString> customModulePaths;
};

}

}

#endif // CTOOLBOXMODULESWIDGET_H
