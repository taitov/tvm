#include "flowview.h"

using namespace nVirtualMachine::nGui;

cFlowViewWidget::cFlowViewWidget(cFlowSceneWidget* flowScene) :
        FlowView::FlowView((QtNodes::FlowScene*)flowScene)
{
	QtNodes::ConnectionStyle::setConnectionStyle(
	R"(
	  {
	    "ConnectionStyle": {
	      "UseDataDefinedColors": true
	    }
	  }
	  )");

	setSceneRect(-20000, -20000, 40000, 40000);
}

void cFlowViewWidget::contextMenuEvent(QContextMenuEvent *event)
{
}
