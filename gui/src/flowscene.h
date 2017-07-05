#ifndef CFLOWSCENEWIDGET_H
#define CFLOWSCENEWIDGET_H

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
	cFlowSceneWidget(const cVirtualMachine* virtualMachine);

private:
	const cVirtualMachine* virtualMachine;
};

}

}

#endif // CFLOWSCENEWIDGET_H
