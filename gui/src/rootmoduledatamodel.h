#ifndef CROOTMODULEDATAMODEL_H
#define CROOTMODULEDATAMODEL_H

#include <tvm/type.h>

#include <nodes/NodeDataModel>

#include "data.h"

namespace nVirtualMachine
{

namespace nGui
{

class cRootModuleDataModel : public QtNodes::NodeDataModel
{
	using NodeDataModel = QtNodes::NodeDataModel;
	using PortType = QtNodes::PortType;
	using NodeDataType = QtNodes::NodeDataType;
	using PortIndex = QtNodes::PortIndex;
	using NodeData = QtNodes::NodeData;
	using NodeStyle = QtNodes::NodeStyle;

public:
	using tGuiRootSignalExits = std::vector<tSignalExitName>;

	using tGuiRootMemoryExits = std::vector<std::tuple<tMemoryExitName,
	                                                   tMemoryTypeName>>;

public:
	cRootModuleDataModel(const tLibraryName& libraryName,
	                     const tRootModuleName& rootModuleName,
	                     const tGuiRootSignalExits& guiRootSignalExits,
	                     const tGuiRootMemoryExits& guiRootMemoryExits);
	~cRootModuleDataModel();

	QJsonObject save() const override;
	void restore(const QJsonObject& jsonObject) override;

public:
	QString caption() const override;
	QString name() const override;
	std::unique_ptr<NodeDataModel> clone() const override;
	const void* getData() const override;

public:
	size_t uniqueCount() const override;
	unsigned int nPorts(PortType portType) const override;
	NodeDataType dataType(PortType portType,
	                      PortIndex portIndex) const override;

	ConnectionPolicy portOutConnectionPolicy(PortIndex) const override;
	std::shared_ptr<NodeData> outData(PortIndex) override;
	void setInData(std::shared_ptr<NodeData>, int) override;
	QWidget* embeddedWidget() override;

private:
	cData data;
	const tLibraryName libraryName;
	const tRootModuleName rootModuleName;
	const tGuiRootSignalExits guiRootSignalExits;
	const tGuiRootMemoryExits guiRootMemoryExits;
};

}

}

#endif // CROOTMODULEDATAMODEL_H
