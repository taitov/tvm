#ifndef CMEMORYDATAMODEL_H
#define CMEMORYDATAMODEL_H

#include <tvm/type.h>

#include <nodes/NodeDataModel>

namespace nVirtualMachine
{

namespace nGui
{

class cMemoryDataModel : public QtNodes::NodeDataModel
{
	using NodeDataModel = QtNodes::NodeDataModel;
	using PortType = QtNodes::PortType;
	using NodeDataType = QtNodes::NodeDataType;
	using PortIndex = QtNodes::PortIndex;
	using NodeData = QtNodes::NodeData;
	using NodeStyle = QtNodes::NodeStyle;

public:
	cMemoryDataModel(const tMemoryTypeName& memoryTypeName);
	~cMemoryDataModel();

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

	ConnectionPolicy portInConnectionPolicy(PortIndex) const override;
	ConnectionPolicy portOutConnectionPolicy(PortIndex) const override;
	bool portCaptionVisible(PortType, PortIndex) const override;
	std::shared_ptr<NodeData> outData(PortIndex) override;
	void setInData(std::shared_ptr<NodeData>, int) override;
	QWidget* embeddedWidget() override;

private:
	const tMemoryTypeName memoryTypeName;
};

}

}

#endif // CMEMORYDATAMODEL_H
