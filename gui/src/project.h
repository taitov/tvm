#ifndef CPROJECTWIDGET_H
#define CPROJECTWIDGET_H

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QSplitter>

#include <nodes/NodeDataModel>

#include "document.h"

namespace nVirtualMachine
{

class cVirtualMachine;

namespace nGui
{

class cFlowViewWidget;
class cFlowSceneWidget;
class cToolBoxModulesWidget;

class cProjectWidget : public cDocumentWidget
{
	Q_OBJECT

public:
	cProjectWidget(const cVirtualMachine* virtualMachine,
	               bool addSchemeModules,
	               const std::vector<QString>& customModulePaths);

	bool save() override;
	bool saveAs() override;
	bool open(const QString& filePath) override;

	QString getDocumentName() override;

	bool hasChanges() override;

	void undo() override;
	void redo() override;

	QString getFilePath() override;

public:
	virtual void setCustomModulePaths(const std::vector<QString>& customModulePaths);

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
	void openCustomModule(QString filePath);

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

protected:
	const cVirtualMachine* virtualMachine;
	QString filePath;

	cToolBoxModulesWidget* toolBox;
	cFlowSceneWidget* flowScene;
	cFlowViewWidget* flowView;
	bool flagHasChanges;

	std::vector<cAction> actions;
	size_t actionPosition;

	std::map<QUuid, QPointF> prevPositions;

	std::vector<QString> customModulePaths;
};

}

}

#endif // CPROJECTWIDGET_H
