#ifndef CMODULEDATAMODEL_H
#define CMODULEDATAMODEL_H

#include <tvm/type.h>
#include <tvm/module.h>

#include <nodes/NodeDataModel>

#include "data.h"

namespace nVirtualMachine
{

namespace nGui
{

class cLogicModuleDataModel : public QtNodes::NodeDataModel
{
	using NodeDataModel = QtNodes::NodeDataModel;
	using PortType = QtNodes::PortType;
	using NodeDataType = QtNodes::NodeDataType;
	using PortIndex = QtNodes::PortIndex;
	using NodeData = QtNodes::NodeData;
	using NodeStyle = QtNodes::NodeStyle;

public:
	using tGuiSignalEntries = cModule::tSignalEntries;

	using tGuiMemoryEntries = cModule::tMemoryEntries;

	using tGuiSignalExits = cModule::tSignalExits;

	using tGuiMemoryExits = cModule::tMemoryExits;

public:
	cLogicModuleDataModel(const tLibraryName& libraryName,
	                      const tModuleTypeName& moduleTypeName,
	                      const tModuleName& moduleName,
	                      const tCaptionName& captionName,
	                      const tGuiSignalEntries& guiSignalEntries,
	                      const tGuiMemoryEntries& guiMemoryEntries,
	                      const tGuiSignalExits& guiSignalExits,
	                      const tGuiMemoryExits& guiMemoryExits);
	~cLogicModuleDataModel();

	QJsonObject save() const override;
	void restore(const QJsonObject& jsonObject) override;

public:
	QString caption() const override;
	QString name() const override;
	std::unique_ptr<NodeDataModel> clone() const override;
	const void* getData() const override;

public:
	const bool canConnect(PortType portType,
	                      NodeDataModel* model,
	                      NodeDataType nodeDataType) const override;
	unsigned int nPorts(PortType portType) const override;
	NodeDataType dataType(PortType portType,
	                      PortIndex portIndex) const override;

	ConnectionPolicy portInConnectionPolicy(PortIndex portIndex) const override;
	ConnectionPolicy portOutConnectionPolicy(PortIndex) const override;
	std::shared_ptr<NodeData> outData(PortIndex) override;
	void setInData(std::shared_ptr<NodeData>, int) override;
	QWidget* embeddedWidget() override;

private:
	cData data;
	const tLibraryName libraryName;
	const tModuleTypeName moduleTypeName;
	const tModuleName moduleName;
	const tCaptionName captionName;
	const tGuiSignalEntries guiSignalEntries;
	const tGuiMemoryEntries guiMemoryEntries;
	const tGuiSignalExits guiSignalExits;
	const tGuiMemoryExits guiMemoryExits;
};

}

}

#endif // CMODULEDATAMODEL_H
