#pragma once
#include <QDialog>

namespace Ui {
	class CDlgNew;
}

class CMainWindow;

class CDlgNew : public QDialog
{
	Q_OBJECT

public:
	CDlgNew(CMainWindow* parent);

	void accept();

	int getTemplate();

	bool createNew();

	QString getRecentFileName();

	QString getProjectName();

	QString getProjectFolder();

	void setProjectFolder(const QString& projectFolder);

	bool createProjectFolder();

	void showEvent(QShowEvent* ev);

public slots:
	void onProjectFolder();

public:
	Ui::CDlgNew*	ui;
};
