#ifndef CPROJECTWIDGET_H
#define CPROJECTWIDGET_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>

#include <nodes/NodeDataModel>

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

	bool hasChanges();

	void undo();
	void redo();

private:
	bool saveProject(const QString& filePath);
	bool openProject(const QString& filePath);

	template<typename ... TArgs>
	void addAction(const TArgs& ... args)
	{
		actions.resize(actionPosition + 1);
		actions[actionPosition] = cAction(args ...);
		actionPosition = actions.size();
	}

Q_SIGNALS:
	void projectNameChanged(QString projectName);
	void projectChanged(bool flagHasChanges);

private:
	class cAction
	{
	public:
		enum eType
		{
			none,
			nodeCreated,
			nodeDeleted,
			nodeMoved
		};

	public:
		cAction();
		cAction(eType type, const QtNodes::Node& node);
		cAction(eType type, const QtNodes::Node& node,
		        const QPointF& fromPosition, const QPointF& toPosition);

	public:
		eType type;
		QUuid id;
		QString moduleFullName;
		QPointF position;
		QPointF fromPosition;
		QPointF toPosition;
	};

private:
	const cVirtualMachine* virtualMachine;
	QString filePath;

	cToolBoxModulesWidget* toolBox;
	cFlowSceneWidget* flowScene;
	cFlowViewWidget* flowView;
	bool flagHasChanges;

	std::vector<cAction> actions;
	size_t actionPosition;

	std::map<QUuid, QPointF> prevPositions;
};

}

}

#endif // CPROJECTWIDGET_H
