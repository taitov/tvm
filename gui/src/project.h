#ifndef CPROJECTWIDGET_H
#define CPROJECTWIDGET_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cFlowViewWidget;
class cFlowSceneWidget;

class cProjectWidget : public QWidget
{
public:
	cProjectWidget(const cVirtualMachine* virtualMachine);

private:
	const cVirtualMachine* virtualMachine;
	QString filePath;

//	cTreeViewModules* treeView;
	cFlowSceneWidget* flowScene;
	cFlowViewWidget* flowView;
};

}

}

#endif // CPROJECTWIDGET_H
