#include <QtCore/QDir>

#include <QtWidgets/QFileDialog>

#include "ide.h"
#include "projects.h"
#include "custommodules.h"

using namespace nVirtualMachine::nGui;

cIdeWidget::cIdeWidget(const cVirtualMachine* virtualMachine,
                       const QString& titleName) :
        virtualMachine(virtualMachine),
        titleName(titleName)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	{ /** menu */
		auto menuBar = new QMenuBar();

		auto fileMenu = menuBar->addMenu("&File");
		auto newProjectAction = fileMenu->addAction(QIcon::fromTheme("document-new"), "&New Project");
		auto newCustomModuleAction = fileMenu->addAction("&New Custom Module");
		fileMenu->addSeparator();
		auto openAction = fileMenu->addAction(QIcon::fromTheme("document-open"), "&Open");
		fileMenu->addSeparator();
		saveAction = fileMenu->addAction(QIcon::fromTheme("document-save"), "Save");
		saveAction->setEnabled(false);
		saveAsAction = fileMenu->addAction(QIcon::fromTheme("document-save-as"), "Save &As");
		saveAsAction->setEnabled(false);
		auto saveAllAction = fileMenu->addAction("&Save All");
		fileMenu->addSeparator();
		auto quitAction = fileMenu->addAction(QIcon::fromTheme("window-close"), "&Quit");

		auto editMenu = menuBar->addMenu("&Edit");
		auto undoAction = editMenu->addAction(QIcon::fromTheme("edit-undo"), "&Undo");
		auto redoAction = editMenu->addAction(QIcon::fromTheme("edit-redo"), "&Redo");
		editMenu->addSeparator();
		auto cutAction = editMenu->addAction(QIcon::fromTheme("edit-cut"), "Cu&t");
		auto copyAction = editMenu->addAction(QIcon::fromTheme("edit-copy"), "&Copy");
		auto pasteAction = editMenu->addAction(QIcon::fromTheme("edit-paste"), "&Paste");
		editMenu->addSeparator();
		auto settingsAction = editMenu->addAction(QIcon::fromTheme("settings-configure"), "&Settings");

		auto debugMenu = menuBar->addMenu("&Debug");
		auto newConnectionAction = debugMenu->addAction(QIcon::fromTheme("network-connect"), "&New Connection");

		auto helpMenu = menuBar->addMenu("&Help");
		auto aboutAction = helpMenu->addAction(QIcon::fromTheme("help-about"), "&About");

		newProjectAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
		newCustomModuleAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_T));
		openAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
		saveAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
		saveAllAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
		quitAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
		undoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z));
		redoAction->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
		cutAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));
		copyAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
		pasteAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_V));

		connect(newProjectAction, &QAction::triggered,
		        this, [this]
		{
			projectsWidget->newDocument();
			stackedWidget->setCurrentWidget(projectsWidget);
		});

		connect(newCustomModuleAction, &QAction::triggered,
		        this, [this]
		{
			customsWidget->newDocument();
			stackedWidget->setCurrentWidget(customsWidget);
		});

		connect(openAction, &QAction::triggered,
		        this, [this]
		{
			QString filePath = QFileDialog::getOpenFileName(nullptr,
			                                                ("Open"),
			                                                QDir::homePath(), /**< @todo */
			                                                ("All Files (*.tvmproject *.tvmcustom);;TVM Project (*.tvmproject);;TVM Custom Module (*.tvmcustom)"));

			if (filePath.isEmpty())
			{
				return;
			}

			if (filePath.endsWith(".tvmproject", Qt::CaseInsensitive))
			{
				if (projectsWidget->open(filePath))
				{
					stackedWidget->setCurrentWidget(projectsWidget);
				}
				return;
			}
			else if (filePath.endsWith(".tvmcustom", Qt::CaseInsensitive))
			{
				if (customsWidget->open(filePath))
				{
					stackedWidget->setCurrentWidget(customsWidget);
				}
				return;
			}
		});

		connect(saveAction, &QAction::triggered,
		        this, [this]
		{
			QWidget* widget = stackedWidget->currentWidget();
			if (widget == projectsWidget)
			{
				projectsWidget->save();
			}
			else if (widget == customsWidget)
			{
				customsWidget->save();
			}
		});

		connect(saveAsAction, &QAction::triggered,
		        this, [this]
		{
			QWidget* widget = stackedWidget->currentWidget();
			if (widget == projectsWidget)
			{
				projectsWidget->saveAs();
			}
			else if (widget == customsWidget)
			{
				customsWidget->saveAs();
			}
		});

		connect(saveAllAction, &QAction::triggered,
		        this, [this]
		{
			QWidget* current = stackedWidget->currentWidget();

			stackedWidget->setCurrentWidget(projectsWidget);
			projectsWidget->saveAllDocuments();

			stackedWidget->setCurrentWidget(customsWidget);
			customsWidget->saveAllDocuments();

			stackedWidget->setCurrentWidget(current);
		});

		connect(undoAction, &QAction::triggered,
		        this, [this]
		{
			QWidget* widget = stackedWidget->currentWidget();
			if (widget == projectsWidget)
			{
				projectsWidget->undo();
			}
			else if (widget == customsWidget)
			{
				customsWidget->undo();
			}
		});

		connect(redoAction, &QAction::triggered,
		        this, [this]
		{
			QWidget* widget = stackedWidget->currentWidget();
			if (widget == projectsWidget)
			{
				projectsWidget->redo();
			}
			else if (widget == customsWidget)
			{
				customsWidget->redo();
			}
		});

		connect(quitAction, &QAction::triggered,
		        this, &QWidget::close);

		mainLayout->addWidget(menuBar);
	}

	{ /** body */
		QHBoxLayout* bodyLayout = new QHBoxLayout();

		{ /** left panel */
			auto toolBar = new QToolBar();

			toolBar->setOrientation(Qt::Vertical);
			toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			toolBar->setIconSize(QSize(48, 48));
			toolBar->setMovable(false);

			auto welcomeAction = toolBar->addAction(QIcon::fromTheme("network-connect"), "Welcome");
			auto projectAction = toolBar->addAction(QIcon::fromTheme("project-development"), "Projects");
			auto customsAction = toolBar->addAction(QIcon::fromTheme("project-development-new-template"), "Customs");
			auto debugAction = toolBar->addAction(QIcon::fromTheme("run-build"), "Debug");

			QWidget* spacer = new QWidget();
			spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			toolBar->addWidget(spacer);

			connect(welcomeAction, &QAction::triggered,
			        this, [this]
			{
				stackedWidget->setCurrentWidget(welcomeWidget);
			});

			connect(projectAction, &QAction::triggered,
			        this, [this]
			{
				stackedWidget->setCurrentWidget(projectsWidget);
			});

			connect(customsAction, &QAction::triggered,
			        this, [this]
			{
				stackedWidget->setCurrentWidget(customsWidget);
			});

			connect(debugAction, &QAction::triggered,
			        this, [this]
			{
				stackedWidget->setCurrentWidget(debugWidget);
			});

			bodyLayout->addWidget(toolBar);
		}

		{
			stackedWidget = new QStackedWidget();

			{ /** welcome */
				welcomeWidget = new QLabel("@todo");
				stackedWidget->addWidget(welcomeWidget);
			}

			{ /** projects */
				projectsWidget = new cProjectsWidget(virtualMachine);

				connect(projectsWidget, &cProjectsWidget::openCustomModule, this, [this](QString filePath)
				{
					if (customsWidget->open(filePath))
					{
						stackedWidget->setCurrentWidget(customsWidget);
					}
				});

				stackedWidget->addWidget(projectsWidget);
			}

			{ /** customs */
				customsWidget = new cCustomModulesWidget(virtualMachine);
				stackedWidget->addWidget(customsWidget);
			}

			{ /** debug */
				debugWidget = new QLabel("@todo");
				stackedWidget->addWidget(debugWidget);
			}

			connect(stackedWidget, &QStackedWidget::currentChanged, this, [this](int index)
			{
				saveAction->setEnabled(false);
				saveAsAction->setEnabled(false);

				QWidget* widget = stackedWidget->widget(index);
				if (widget == projectsWidget)
				{
					saveAction->setEnabled(true);
					saveAsAction->setEnabled(true);
				}
				else if (widget == customsWidget)
				{
					saveAction->setEnabled(true);
					saveAsAction->setEnabled(true);
				}
			});

			stackedWidget->setCurrentWidget(welcomeWidget);
			bodyLayout->addWidget(stackedWidget);
		}

		mainLayout->addLayout(bodyLayout);
	}

	setWindowTitle(this->titleName);
	showMaximized();
}

void cIdeWidget::closeEvent(QCloseEvent* event)
{
	stackedWidget->setCurrentWidget(projectsWidget);
	if (projectsWidget->closeAllDocuments())
	{
		stackedWidget->setCurrentWidget(customsWidget);
		if (customsWidget->closeAllDocuments())
		{
			event->accept();
			return;
		}
	}

	event->ignore();
}
