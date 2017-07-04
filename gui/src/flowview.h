#ifndef CFLOWVIEWWIDGET_H
#define CFLOWVIEWWIDGET_H

#include <nodes/ConnectionStyle>
#include <nodes/FlowView>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cFlowSceneWidget;

class cFlowViewWidget : public QtNodes::FlowView
{
public:
	cFlowViewWidget(cFlowSceneWidget* flowScene);

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
};

}

}

#endif // CFLOWVIEWWIDGET_H
