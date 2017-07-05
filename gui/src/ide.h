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

class cIdeWidget : public QWidget
{
	Q_OBJECT

public:
	cIdeWidget(const cVirtualMachine* virtualMachine,
	           const QString& titleName);

private:
	void closeEvent(QCloseEvent* event) override;

private:
	const cVirtualMachine* virtualMachine;
	QString titleName;

	QStackedWidget* stackedWidget;
	QWidget* welcomeWidget;
	cProjectsWidget* projectsWidget;
	QWidget* customsWidget;
	QWidget* debugWidget;

	QAction* saveAction;
	QAction* saveAsAction;
};

}

}

#endif // CIDEWIDGET_H
