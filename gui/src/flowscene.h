#ifndef CFLOWSCENEWIDGET_H
#define CFLOWSCENEWIDGET_H

#include <QtCore/QDir>

#include <nodes/FlowScene>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cFlowSceneWidget : public QtNodes::FlowScene
{
	using DataModelRegistry = QtNodes::DataModelRegistry;

public:
	cFlowSceneWidget(const cVirtualMachine* virtualMachine,
	                 bool addSchemeModules);

	void setCustomModulePaths(const std::vector<QString>& paths);

private:
	std::shared_ptr<QtNodes::DataModelRegistry> makeVirtualMachineDataModelRegistry(bool addSchemeModules);
	void updateCustomModuleDir(std::shared_ptr<QtNodes::DataModelRegistry> dataModelRegistry,
	                           const QDir& dir,
	                           const QString& modulePrefixName);

private:
	const cVirtualMachine* virtualMachine;
	const bool addSchemeModules;
	std::vector<QString> customModulePaths;
};

}

}

#endif // CFLOWSCENEWIDGET_H
