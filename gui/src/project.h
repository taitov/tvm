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
class cToolBoxModulesWidget;

class cProjectWidget : public QWidget
{
	Q_OBJECT

public:
	cProjectWidget(const cVirtualMachine* virtualMachine);

	bool save();
	bool saveAs();
	bool open(const QString& filePath);

	QString getProjectName();

private:
	bool saveProject(const QString& filePath);
	bool openProject(const QString& filePath);

Q_SIGNALS:
	void projectNameChanged(QString projectName);

private:
	const cVirtualMachine* virtualMachine;
	QString filePath;

	cToolBoxModulesWidget* toolBox;
	cFlowSceneWidget* flowScene;
	cFlowViewWidget* flowView;
};

}

}

#endif // CPROJECTWIDGET_H
