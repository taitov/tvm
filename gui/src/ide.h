#ifndef CIDEWIDGET_H
#define CIDEWIDGET_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QLabel>

#include <QtGui/QCloseEvent>

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cProjectsWidget;
class cCustomModulesWidget;
class cDocumentWidget;

class cIdeWidget : public QWidget
{
	Q_OBJECT

public:
	cIdeWidget(const cVirtualMachine* virtualMachine,
	           const QString& virtualMachineName);

private:
	void closeEvent(QCloseEvent* event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
	const cVirtualMachine* virtualMachine;
	QString virtualMachineName;

	QStackedWidget* stackedWidget;
	QWidget* welcomeWidget;
	cProjectsWidget* projectsWidget;
	cCustomModulesWidget* customsWidget;
	QWidget* debugWidget;
	QAction* welcomeAction;
	QAction* projectAction;
	QAction* customsAction;
	QAction* debugAction;

	QAction* saveAction;
	QAction* saveAsAction;

	std::vector<QString> customModulePaths;

private:
	template<typename ... TArgs>
	void addAction(const TArgs& ... args)
	{
		actions.resize(actionPosition + 1);
		actions[actionPosition] = cAction(args ...);
		actionPosition = actions.size();
	}

	class cAction
	{
	public:
		enum eType
		{
			none,
			projectChanged,
			customModuleChanged,
		};

	public:
		cAction();
		cAction(eType type,
		        QWidget* widget);

	public:
		eType type;
		QWidget* widget;
	};

	std::vector<cAction> actions;
	size_t actionPosition;
	bool doAction;
};

}

}

#endif // CIDEWIDGET_H
