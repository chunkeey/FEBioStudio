#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "Document.h"
#include <QApplication>
#include <QtCore/QSettings>
#include <QtCore/QDir>
#include <QMessageBox>
#include <PreViewLib/FEObject.h>
#include <PreViewLib/PRVArchive.h>
#include <PreViewIO/FileReader.h>
#include <QtCore/QTimer>
#include <QFileDialog>
#include <FEBio/FEBioImport.h>
#include "DocTemplate.h"
#include "CreatePanel.h"
#include "FileThread.h"
#include "GLHighlighter.h"
#include <PreViewIO/NikeImport.h>
#include <Abaqus/AbaqusImport.h>
#include <QStyleFactory>
#include "DlgImportAbaqus.h"
#include "DlgAddMeshData.h"
#include "GraphWindow.h"
#include "PostDoc.h"
#include "DlgTimeSettings.h"
#include <PostGL/GLModel.h>
#include "DlgWidgetProps.h"

extern GLCOLOR col[];

// create a dark style theme (work in progress)
void darkStyle()
{
	qApp->setStyle(QStyleFactory::create("Fusion"));
	QPalette palette = qApp->palette();
	palette.setColor(QPalette::Window, QColor(53, 53, 53));
	palette.setColor(QPalette::WindowText, Qt::white);
	palette.setColor(QPalette::Base, QColor(30, 30, 30));
	palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
	palette.setColor(QPalette::ToolTipBase, Qt::white);
	palette.setColor(QPalette::ToolTipText, Qt::white);
	palette.setColor(QPalette::Text, Qt::white);
	palette.setColor(QPalette::Button, QColor(53, 53, 53));
	palette.setColor(QPalette::ButtonText, Qt::white);
	palette.setColor(QPalette::BrightText, Qt::red);
	palette.setColor(QPalette::Highlight, QColor(51, 153, 255));
	palette.setColor(QPalette::HighlightedText, Qt::white);
	palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
	palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
	qApp->setPalette(palette);
}

//-----------------------------------------------------------------------------
CMainWindow::CMainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::CMainWindow)
{
	m_doc = 0;

	setDockOptions(dockOptions() | QMainWindow::AllowNestedDocks | QMainWindow::GroupedDragging);

	// setup the GUI
	ui->setupUi(this);

	// create a new document
	m_doc = new CDocument(this);

	// read the settings
	readSettings();

	// activate dark style
	if (ui->m_theme == 1) 
	{
		darkStyle();

		// NOTE: I'm not sure if I can set the dark theme before I can create the document.
		//       Since the bg colors are already set, I need to do this here. Make sure
		//       the values set here coincide with the values from CDocument::NewDocument
		VIEW_SETTINGS& v = m_doc->GetViewSettings();
		v.m_col1 = GLCOLOR(83, 83, 83);
		v.m_nbgstyle = BG_COLOR1;

		GLWidget::set_base_color(GLColor(255, 255, 255));

		// adjust some toolbar buttons
		ui->actionFontBold->setIcon(QIcon(":/icons/font_bold_neg.png"));
		ui->actionFontItalic->setIcon(QIcon(":/icons/font_italic_neg.png"));
	}

	// make sure the file viewer is visible
	ui->showFileViewer();

	// load templates
	TemplateManager::Init();

	UpdateModel();
}

//-----------------------------------------------------------------------------
CMainWindow::~CMainWindow()
{
	// delete document
	delete m_doc;
}

//-----------------------------------------------------------------------------
// get the current theme
int CMainWindow::currentTheme() const
{
	return ui->m_theme;
}

//-----------------------------------------------------------------------------
// set the current theme
void CMainWindow::setCurrentTheme(int n)
{
	ui->m_theme = n;
}

//-----------------------------------------------------------------------------
// get the current post doc
CPostDoc* CMainWindow::GetActiveDocument()
{
	CDocument* doc = GetDocument();
	if (doc == nullptr) return nullptr;

	if (doc->FEBioJobs() == 0) return nullptr;

	int activeDoc = GetActiveView() - 1;
	if (activeDoc >= 0)
	{
		CFEBioJob* job = doc->GetFEBioJob(activeDoc);
		return job->GetPostDoc();
	}
	else return nullptr;
}

//-----------------------------------------------------------------------------
void CMainWindow::UpdateTitle()
{
	// get the file name
	std::string fileName = m_doc->GetDocFileName();
	if (fileName.empty()) fileName = "Untitled";

	std::string wndTitle = fileName + " - FEBio Studio";
	setWindowTitle(QString::fromStdString(wndTitle));
}

//-----------------------------------------------------------------------------
void CMainWindow::ReadFile(const QString& fileName, FileReader* fileReader, bool clearDoc)
{
	if (clearDoc) GetDocument()->Clear();

	m_fileThread = new CFileThread(this, fileReader, false, fileName);
	m_fileThread->start();
	ui->statusBar->showMessage(QString("Reading file %1 ...").arg(fileName));
	ui->fileProgress->setValue(0);
	ui->statusBar->addPermanentWidget(ui->fileProgress);
	ui->fileProgress->show();
	AddLogEntry(QString("Reading file %1 ... ").arg(fileName));
	QTimer::singleShot(100, this, SLOT(checkFileProgress()));
}

//-----------------------------------------------------------------------------
// read a list of files
void CMainWindow::ImportFiles(const QStringList& files)
{
	// set the queue
	m_fileQueue = files;

	// start the process
	ReadNextFileInQueue();

	for (int i=0; i<files.count(); ++i)
		ui->addToRecentGeomFiles(files[i]);
}

//-----------------------------------------------------------------------------
void CMainWindow::ReadNextFileInQueue()
{
	// make sure we have a file
	if (m_fileQueue.isEmpty()) return;

	// get the next file name
	QString fileName = m_fileQueue.at(0);

	// create a file reader
	FileReader* fileReader = CreateFileReader(fileName);

	// make sure we have one
	if (fileReader)
	{
		ReadFile(fileName, fileReader, false);
	}
	else
	{
		QMessageBox::critical(this, "Read file", QString("Cannot read file\n{0}").arg(fileName));
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::OpenDocument(const QString& fileName)
{
	m_fileQueue.clear();

	// check to see if the document is modified or not
	if (m_doc->IsModified())
	{
		QString msg("The project was modified since the last save.\nDo you want to save the project ?");
		int n = QMessageBox::question(this, "FEBio Studio", msg, QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
		if (n == QMessageBox::Yes)
		{
			on_actionSave_triggered();
		}
		else if (n == QMessageBox::Cancel) return;
	}

	// start reading the file
	ReadFile(fileName, 0, true);

	// add file to recent list
	ui->addToRecentFiles(fileName);
}

//-----------------------------------------------------------------------------
//! Open a plot file
void CMainWindow::OpenPlotFile(const QString& fileName)
{
	CDocument* doc = GetDocument();
	std::string sfile = fileName.toStdString();
	if (doc->LoadPlotFile(sfile) == false)
	{
		QMessageBox::critical(this, "FEBio Studio", "Failed loading plot file.");
	}
	else
	{
		UpdateModel();
		UpdatePostPanel();
		UpdatePostToolbar();

		CFEBioJob* job = doc->GetFEBioJob(doc->FEBioJobs() - 1);
		ui->modelViewer->Select(job);
		SetActivePostDoc(job->GetPostDoc());
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::OpenFEModel(const QString& fileName)
{
	m_fileQueue.clear();

	// create a file reader
	FileReader* reader = 0;
	QString ext = QFileInfo(fileName).suffix();
	if      (ext.compare("feb", Qt::CaseInsensitive) == 0) reader = new FEBioImport();
	else if (ext.compare("n"  , Qt::CaseInsensitive) == 0) reader = new FENIKEImport();
	else if (ext.compare("inp", Qt::CaseInsensitive) == 0) 
	{
		AbaqusImport* abaqusReader = new AbaqusImport();

		CDlgImportAbaqus dlg(abaqusReader, this);
		if (dlg.exec() == 0)
		{
			return;
		}

		abaqusReader->m_breadPhysics = true;
		reader = abaqusReader;
	}
	else
	{
		QMessageBox::critical(this, "Read File", QString("Cannot reade file\n%0").arg(fileName));
		return;
	}

	/*	// remove quotes from the filename
	char* ch = strchr((char*)szfile, '"');
	if (ch) szfile++;
	ch = strrchr((char*)szfile, '"');
	if (ch) *ch = 0;
	*/

	// start reading the file
	ReadFile(fileName, reader, true);

	// add file to recent list
	ui->addToRecentFEFiles(fileName);
}

//-----------------------------------------------------------------------------
void CMainWindow::on_recentFiles_triggered(QAction* action)
{
	QString fileName = action->text();
	OpenDocument(fileName);
}

//-----------------------------------------------------------------------------
void CMainWindow::on_recentFEFiles_triggered(QAction* action)
{
	QString fileName = action->text();
	OpenFEModel(fileName);
}

//-----------------------------------------------------------------------------
void CMainWindow::on_recentGeomFiles_triggered(QAction* action)
{
	QString fileName = action->text();
	ImportFiles(QStringList(fileName));
}

//-----------------------------------------------------------------------------
void CMainWindow::checkFileProgress()
{
	if (m_fileThread)
	{
		float f = m_fileThread->getFileProgress();
		int n = (int)(100.f*f);
		ui->fileProgress->setValue(n);
		if (f < 1.0f) QTimer::singleShot(100, this, SLOT(checkFileProgress()));
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::finishedReadingFile(bool success, const QString& errorString)
{
	m_fileThread = 0;
	ui->statusBar->clearMessage();
	ui->statusBar->removeWidget(ui->fileProgress);

	if (success == false)
	{
		if (m_fileQueue.isEmpty())
		{
			QString err = QString("Failed reading file :\n%1").arg(errorString);
			QMessageBox::critical(this, "FEBio Studio", err);
		}

		QString err = QString("FAILED:\n%1").arg(errorString);
		AddLogEntry(err);
		return;
	}
	else if (errorString.isEmpty() == false)
	{
		if (m_fileQueue.isEmpty())
		{
			QMessageBox::information(this, "FEBio Studio", errorString);
		}
		AddLogEntry("success!\n");
		AddLogEntry("Warnings were generated:\n");
		AddLogEntry(errorString);
	}
	else 
	{
		AddLogEntry("success!\n");
	}

	// remove the last file that was read
	if (m_fileQueue.isEmpty() == false) m_fileQueue.removeAt(0);

	if (m_fileQueue.isEmpty() == false)
	{
		ReadNextFileInQueue();
	}
	else
	{
		UpdateTitle();
		Reset();
		UpdateModel();
		UpdateToolbar();
		Update();
		if (ui->modelViewer) ui->modelViewer->Show();

		// If the main window is not active, this will alert the user that the file has been read. 
		QApplication::alert(this, 0);
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::Update(QWidget* psend, bool breset)
{
	static bool bmodified = !m_doc->IsModified();

	if (bmodified != m_doc->IsModified())
	{
		UpdateTitle();
		bmodified = m_doc->IsModified();
	}

	if (breset)
	{
		//	m_pGLView->OnZoomExtents(0,0);
		UpdateModel();
	}

	// redraw the GL view
	RedrawGL();

	// update the GL control bar
	UpdateGLControlBar();

	// Update the command windows
	if (ui->buildPanel->isVisible() && (psend != ui->buildPanel)) ui->buildPanel->Update();

//	if (m_pCurveEdit->visible() && (m_pCurveEdit != psend)) m_pCurveEdit->Update();
	if (ui->meshWnd && ui->meshWnd->isVisible()) ui->meshWnd->Update();
}

//-----------------------------------------------------------------------------
CGLView* CMainWindow::GetGLView()
{
	return ui->glview;
}

//-----------------------------------------------------------------------------
CBuildPanel* CMainWindow::GetBuildPanel()
{
	return ui->buildPanel;
}

//-----------------------------------------------------------------------------
CCreatePanel* CMainWindow::GetCreatePanel()
{
	return ui->buildPanel->CreatePanel();
}

//-----------------------------------------------------------------------------
//! This function resets the GL View. It is called when creating a new file new
//! (CWnd::OnFileNew) or when opening a file (CWnd::OnFileOpen). 
//! \sa CGLView::Reset
void CMainWindow::Reset()
{
	ui->glview->Reset();
	ui->glview->ZoomExtents(false);
}

bool CMainWindow::maybeSave()
{
	if (GetDocument()->IsModified())
	{
		QMessageBox::StandardButton b = QMessageBox::question(this, "", "The project was changed. Do you want to save it?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		if (b == QMessageBox::Cancel) return false;

		if (b==QMessageBox::Yes)
		{
			on_actionSave_triggered();
		}
		return true;
	}
	else return true;
}

void CMainWindow::ReportSelection()
{
	CDocument* doc = GetDocument();
	FESelection* sel = doc->GetCurrentSelection();
	if ((sel == 0) || (sel->Size() == 0)) 
	{
		ClearStatusMessage();
		return;
	}

	GetCreatePanel()->setInput(sel);
	int N = sel->Size();
	QString msg;
	switch (sel->Type())
	{
	case SELECT_OBJECTS:
	{
		GObjectSelection& s = dynamic_cast<GObjectSelection&>(*sel);
		if (N == 1)
		{
			GObject* po = s.Object(0);
			msg = QString("Object \"%1\" selected (Id = %2)").arg(QString::fromStdString(po->GetName())).arg(po->GetID());
		}
		else msg = QString("%1 Objects selected").arg(N);
	}
	break;
	case SELECT_PARTS:
	{
		if (N == 1)
		{
			GPartSelection& ps = dynamic_cast<GPartSelection&>(*sel);
			GPartSelection::Iterator it(&ps);
			msg = QString("Part \"%1\" selected (Id = %2)").arg(QString::fromStdString(it->GetName())).arg(it->GetID());
		}
		else msg = QString("%1 Parts selected").arg(N);
	}
	break;
	case SELECT_SURFACES:
	{
		if (N == 1)
		{
			GFaceSelection& fs = dynamic_cast<GFaceSelection&>(*sel);
			GFaceSelection::Iterator it(&fs);
			msg = QString("Surface \"%1\" selected (Id = %2)").arg(QString::fromStdString(it->GetName())).arg(it->GetID());
		}
		else msg = QString("%1 Surfaces selected").arg(N);
	}
	break;
	case SELECT_CURVES:
	{
		GEdgeSelection& es = dynamic_cast<GEdgeSelection&>(*sel);
		if (N == 1)
		{
			GEdgeSelection::Iterator it(&es);
			msg = QString("Curve \"%1\" selected (Id = %2)").arg(QString::fromStdString(it->GetName())).arg(it->GetID());
		}
		else msg = QString("%1 Curves selected").arg(N);
	}
	break;
	case SELECT_NODES:
	{
		if (N == 1)
		{
			GNodeSelection& ns = dynamic_cast<GNodeSelection&>(*sel);
			GNodeSelection::Iterator it(&ns);
			msg = QString("Node \"%1\" selected (Id = %2)").arg(QString::fromStdString(it->GetName())).arg(it->GetID());
		}
		else msg = QString("%1 Nodes selected").arg(N);
	}
	break;
	case SELECT_DISCRETE_OBJECT:
	{
		if (N == 1)
		{
			msg = QString("1 discrete object selected");
		}
		else msg = QString("%1 discrete objects selected").arg(N);
	}
	break;
	case SELECT_FE_ELEMENTS:
	{
		msg = QString("%1 elements selected").arg(N);
	}
	break;
	case SELECT_FE_FACES:
	{
		msg = QString("%1 faces selected").arg(N);
	}
	break;
	case SELECT_FE_EDGES:
	{
		msg = QString("%1 edges selected").arg(N);
	}
	break;
	case SELECT_FE_NODES:
	{
		msg = QString("%1 nodes selected").arg(N);
	}
	break;
	}
	SetStatusMessage(msg);

	FEElementSelection* es = dynamic_cast<FEElementSelection*>(sel);
	if (es)
	{
		if (es->Size() == 1)
		{
			FEElement* el = es->Element(0);
			AddLogEntry("1 element selected:\n");

			switch (el->GetType())
			{
			case FE_HEX8   : AddLogEntry("  Type = HEX8"   ); break;
			case FE_TET4   : AddLogEntry("  Type = TET4"   ); break;
			case FE_TET5   : AddLogEntry("  Type = TET5"   ); break;
			case FE_PENTA6 : AddLogEntry("  Type = PENTA6" ); break;
			case FE_QUAD4  : AddLogEntry("  Type = QUAD4"  ); break;
			case FE_TRI3   : AddLogEntry("  Type = TRI3"   ); break;
			case FE_BEAM2  : AddLogEntry("  Type = BEAM2"  ); break;
			case FE_HEX20  : AddLogEntry("  Type = HEX20"  ); break;
			case FE_QUAD8  : AddLogEntry("  Type = QUAD8"  ); break;
			case FE_BEAM3  : AddLogEntry("  Type = BEAM3"  ); break;
			case FE_TET10  : AddLogEntry("  Type = TET10"  ); break;
			case FE_TRI6   : AddLogEntry("  Type = TRI6"   ); break;
			case FE_TET15  : AddLogEntry("  Type = TET15"  ); break;
			case FE_HEX27  : AddLogEntry("  Type = HEX27"  ); break;
			case FE_TRI7   : AddLogEntry("  Type = TRI7"   ); break;
			case FE_QUAD9  : AddLogEntry("  Type = QUAD9"  ); break;
			case FE_PENTA15: AddLogEntry("  Type = PENTA15"); break;
			case FE_PYRA5  : AddLogEntry("  Type = PYRA5"  ); break;
			case FE_TET20  : AddLogEntry("  Type = TET20"  ); break;
			case FE_TRI10  : AddLogEntry("  Type = TRI10"  ); break;
			}
			AddLogEntry("\n");

			AddLogEntry("  nodes: ");
			int n = el->Nodes();
			for (int i=0; i<n; ++i)
			{
				AddLogEntry(QString::number(el->m_node[i]));
				if (i < n - 1) AddLogEntry(", ");
				else AddLogEntry("\n");
			}

			AddLogEntry("  neighbors: ");
			n = 0;
			if (el->IsSolid()) n = el->Faces();
			else if (el->IsShell()) n = el->Edges();

			for (int i=0; i<n; ++i)
			{
				AddLogEntry(QString::number(el->m_nbr[i]));
				if (i < n - 1) AddLogEntry(", ");
				else AddLogEntry("\n");
			}
		}
	}

	FEFaceSelection* fs = dynamic_cast<FEFaceSelection*>(sel);
	if (fs)
	{
		if (fs->Size() == 1)
		{
			FEFaceSelection::Iterator it = fs->begin();
			FEFace* pf = it;
			AddLogEntry("1 face selected:\n");
			switch (pf->Type())
			{
			case FE_FACE_TRI3 : AddLogEntry("  Type = TRI3"); break;
			case FE_FACE_QUAD4: AddLogEntry("  Type = QUAD4"); break;
			case FE_FACE_TRI6 : AddLogEntry("  Type = TRI6"); break;
			case FE_FACE_TRI7 : AddLogEntry("  Type = TRI7"); break;
			case FE_FACE_QUAD8: AddLogEntry("  Type = QUAD8"); break;
			case FE_FACE_QUAD9: AddLogEntry("  Type = QUAD9"); break;
			case FE_FACE_TRI10: AddLogEntry("  Type = TRI10"); break;
			}
			AddLogEntry("\n");

			AddLogEntry("  neighbors: ");
			int n = pf->Edges();
			for (int i = 0; i<n; ++i)
			{
				AddLogEntry(QString::number(pf->m_nbr[i]));
				if (i < n - 1) AddLogEntry(", ");
				else AddLogEntry("\n");
			}
		}
	}
}

void CMainWindow::closeEvent(QCloseEvent* ev)
{
	if (maybeSave())
	{
		writeSettings();
		ev->accept();
	}
	else
		ev->ignore();
}

void CMainWindow::keyPressEvent(QKeyEvent* ev)
{
	if (ev->key() == Qt::Key_Escape)
	{
		// give the build panels a chance to react first
		if (ui->buildPanel->OnEscapeEvent()) return;

		// if the build panel didn't process it, clear selection
		FESelection* ps = m_doc->GetCurrentSelection();
		if ((ps == 0) || (ps->Size() == 0))
		{
			if (m_doc->GetItemMode() != ITEM_MESH) m_doc->SetItemMode(ITEM_MESH);
			else ui->SetSelectionMode(SELECT_OBJECT);
			Update();
			UpdateUI();
		}
		else on_actionClearSelection_triggered();
		ev->accept();
		GLHighlighter::ClearHighlights();
		GLHighlighter::setTracking(false);
	}
	else if ((ev->key() == Qt::Key_1) && (ev->modifiers() && Qt::CTRL)) 
	{
		ui->showFileViewer();
		ev->accept();
	}
	else if ((ev->key() == Qt::Key_2) && (ev->modifiers() && Qt::CTRL))
	{
		ui->showModelViewer();
		ev->accept();
	}
	else if ((ev->key() == Qt::Key_3) && (ev->modifiers() && Qt::CTRL))
	{
		ui->showBuildPanel();
		ev->accept();
	}
}

void CMainWindow::SetCurrentFolder(const QString& folder)
{
	ui->currentPath = folder;
}

void CMainWindow::writeSettings()
{
	QSettings settings("MRLSoftware", "FEBio Studio");
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", saveGeometry());
	settings.setValue("state", saveState());
	settings.setValue("theme", ui->m_theme);
	settings.endGroup();

	settings.beginGroup("FolderSettings");
	settings.setValue("currentPath", ui->currentPath);

	settings.setValue("febioPaths", ui->m_febio_path);
	settings.setValue("febioInfo", ui->m_febio_info);

	QStringList folders = ui->fileViewer->FolderList();
	if (folders.isEmpty() == false) settings.setValue("folders", folders);

	if (ui->m_recentFiles.isEmpty() == false) settings.setValue("recentFiles", ui->m_recentFiles);
	if (ui->m_recentFEFiles.isEmpty() == false) settings.setValue("recentFEFiles", ui->m_recentFEFiles);
	if (ui->m_recentGeomFiles.isEmpty() == false) settings.setValue("recentGeomFiles", ui->m_recentGeomFiles);

	settings.endGroup();
}

void CMainWindow::readSettings()
{
	QSettings settings("MRLSoftware", "FEBio Studio");
	settings.beginGroup("MainWindow");
	restoreGeometry(settings.value("geometry").toByteArray());
	restoreState(settings.value("state").toByteArray());
	ui->m_theme = settings.value("theme", 0).toInt();
	settings.endGroup();

	settings.beginGroup("FolderSettings");
	ui->currentPath = settings.value("currentPath", QDir::homePath()).toString();

	ui->m_febio_path = settings.value("febioPaths", ui->m_febio_path).toStringList();
	ui->m_febio_info = settings.value("febioInfo", ui->m_febio_info).toStringList();

	QStringList folders = settings.value("folders").toStringList();
	if (folders.isEmpty() == false)
	{
		ui->fileViewer->SetFolderList(folders);
	}

	ui->fileViewer->setCurrentPath(ui->currentPath);

	QStringList recentFiles = settings.value("recentFiles").toStringList(); ui->setRecentFiles(recentFiles);
	QStringList recentFEFiles = settings.value("recentFEFiles").toStringList(); ui->setRecentFEFiles(recentFEFiles);
	QStringList recentGeomFiles = settings.value("recentGeomFiles").toStringList(); ui->setRecentGeomFiles(recentGeomFiles);

	settings.endGroup();
}


//-----------------------------------------------------------------------------
void CMainWindow::UpdateUI()
{
/*	m_pToolbar->Update();
	m_pCmdWnd->Update();
	if (m_pCurveEdit->visible()) m_pCurveEdit->Update();
*/	
	ui->glc->Update();
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::UpdateToolbar()
{
	if (m_doc->IsValid() == false) return;

	VIEW_SETTINGS& view = m_doc->GetViewSettings();
	if (view.m_bfiber != ui->actionShowFibers->isChecked()) ui->actionShowFibers->trigger();
	if (view.m_bmesh  != ui->actionShowMeshLines->isChecked()) ui->actionShowMeshLines->trigger();
	if (view.m_bgrid  != ui->actionShowGrid->isChecked()) ui->actionShowGrid->trigger();
}

//-----------------------------------------------------------------------------
void CMainWindow::OpenInCurveEditor(FEObject* po)
{
//	OnToolsCurveEditor(0, 0);
//	m_pCurveEdit->Select(po);
}

//-----------------------------------------------------------------------------
void CMainWindow::ShowInModelViewer(FEObject* po)
{
	ui->modelViewer->Select(po);
	ui->modelViewer->UpdateObject(po);
}

//-----------------------------------------------------------------------------
//! set the selection mode
void CMainWindow::SetSelectionMode(int nselect)
{
	ui->SetSelectionMode(nselect);
}

//-----------------------------------------------------------------------------
//! Updates the model editor and selects object po.
//! \param po pointer to object that will be selected in the model editor
void CMainWindow::UpdateModel(FEObject* po, bool bupdate)
{
	if (ui->modelViewer)
	{
		if (bupdate)
		{
			ui->modelViewer->Update();
			if (po)
			{
//				ui->showModelViewer();
				ui->modelViewer->Select(po);
			}
		}
		else ui->modelViewer->UpdateObject(po);
	}
}

//-----------------------------------------------------------------------------
//! Updates the GLView control bar
void CMainWindow::UpdateGLControlBar()
{
	ui->glc->Update();
}

//-----------------------------------------------------------------------------
//! Update the post tool bar
void CMainWindow::UpdatePostToolbar()
{
	CPostDoc* doc = GetActiveDocument();
	if ((doc == nullptr) || (doc->IsValid() == false))
	{
		ui->postToolBar->setDisabled(true);
		return;
	}

	Post::FEModel* pfem = doc->GetFEModel();
	ui->selectData->BuildMenu(pfem, Post::DATA_SCALAR);

	Post::CGLModel* mdl = doc->GetGLModel();
	if (mdl == 0) ui->postToolBar->setDisabled(true);
	else
	{
		int ntime = mdl->currentTimeIndex() + 1;

		Post::FEModel* fem = mdl->GetFEModel();
		int states = fem->GetStates();
		QString suff = QString("/%1").arg(states);
		ui->pspin->setSuffix(suff);
		ui->pspin->setRange(1, states);
		ui->pspin->setValue(ntime);
		ui->postToolBar->setEnabled(true);
	}
}

//-----------------------------------------------------------------------------
//! set the post doc that will be rendered in the GL view
void CMainWindow::SetActivePostDoc(CPostDoc* postDoc)
{
	if (postDoc == nullptr) 
		SetActiveView(0);
	else
	{
		int views = Views();
		CDocument* doc = GetDocument();
		for (int i = 0; i < doc->FEBioJobs(); ++i)
		{
			CFEBioJob* job = doc->GetFEBioJob(i);
			if (job->GetPostDoc() == postDoc)
			{
				int activeView = i + 1;
				if (activeView >= views)
					AddView(job->GetName());
				else
					SetActiveView(activeView);

				break;
			}
		}
	}
}

//-----------------------------------------------------------------
int CMainWindow::Views()
{
	return ui->glc->views();
}

//-----------------------------------------------------------------
void CMainWindow::SetActiveView(int n)
{
	ui->glc->setActiveView(n);
	ui->glview->UpdateWidgets(false);
	RedrawGL();
}

//-----------------------------------------------------------------
void CMainWindow::AddView(const std::string& viewName, bool makeActive)
{
	ui->glc->addView(viewName, makeActive);
	ui->glview->ZoomExtents(false);
	ui->glview->UpdateWidgets(false);
}

//-----------------------------------------------------------------------------
int CMainWindow::GetActiveView()
{
	return ui->glc->getActiveView();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_glbar_currentViewChanged(int n)
{
	UpdatePostPanel();
	UpdatePostToolbar();
	RedrawGL();
}

//-----------------------------------------------------------------------------
//! Update the post panel
void CMainWindow::UpdatePostPanel()
{
	ui->postPanel->Update(true);
}

//-----------------------------------------------------------------------------
void CMainWindow::RedrawGL()
{
	ui->glview->repaint();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectObjects_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_OBJECT); 
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectParts_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_PART);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectSurfaces_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_FACE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectCurves_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_EDGE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectNodes_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_NODE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionSelectDiscrete_toggled(bool b)
{
	if (b) m_doc->SetSelectionMode(SELECT_DISCRETE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_selectRect_toggled(bool b)
{
	if (b) m_doc->SetSelectionStyle(REGION_SELECT_BOX);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_selectCircle_toggled(bool b)
{
	if (b) m_doc->SetSelectionStyle(REGION_SELECT_CIRCLE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_selectFree_toggled(bool b)
{
	if (b) m_doc->SetSelectionStyle(REGION_SELECT_FREE);
	Update();
}

//-----------------------------------------------------------------------------
void CMainWindow::onTimer()
{
	if (ui->m_isAnimating == false) return;

	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();

	int N = doc->GetFEModel()->GetStates();
	int N0 = time.m_start;
	int N1 = time.m_end;

	int nstep = doc->GetActiveState();

	if (time.m_bfix)
	{
/*		float f0 = doc->GetTimeValue(N0);
		float f1 = doc->GetTimeValue(N1);

		float ftime = doc->GetTimeValue();

		if (time.m_mode == MODE_FORWARD)
		{
			ftime += time.m_dt;
			if (ftime > f1)
			{
				if (time.m_bloop) ftime = f0;
				else { ftime = f1; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_REVERSE)
		{
			ftime -= time.m_dt;
			if (ftime < f0)
			{
				if (time.m_bloop) ftime = f1;
				else { ftime = f0; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_CYLCE)
		{
			ftime += time.m_dt*time.m_inc;
			if (ftime > f1)
			{
				time.m_inc = -1;
				ftime = f1;
				if (time.m_bloop == false) StopAnimation();
			}
			else if (ftime < f0)
			{
				time.m_inc = 1;
				ftime = f0;
				if (time.m_bloop == false) StopAnimation();
			}
		}

		SetCurrentTimeValue(ftime);
*/	}
	else
	{
		if (time.m_mode == MODE_FORWARD)
		{
			nstep++;
			if (nstep > N1)
			{
				if (time.m_bloop) nstep = N0;
				else { nstep = N1; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_REVERSE)
		{
			nstep--;
			if (nstep < N0)
			{
				if (time.m_bloop) nstep = N1;
				else { nstep = N0; StopAnimation(); }
			}
		}
		else if (time.m_mode == MODE_CYLCE)
		{
			nstep += time.m_inc;
			if (nstep > N1)
			{
				time.m_inc = -1;
				nstep = N1;
				if (time.m_bloop == false) StopAnimation();
			}
			else if (nstep < N0)
			{
				time.m_inc = 1;
				nstep = N0;
				if (time.m_bloop == false) StopAnimation();
			}
		}
		doc->SetActiveState(nstep);
		ui->pspin->setValue(nstep+1);
	}

	RedrawGL();

	// TODO: Should I start the event before or after the view is redrawn?
	if (ui->m_isAnimating)
	{
		if (doc == nullptr) return;
		if (doc->IsValid())
		{
			TIMESETTINGS& time = doc->GetTimeSettings();
			double fps = time.m_fps;
			if (fps < 1.0) fps = 1.0;
			double msec_per_frame = 1000.0 / fps;
			QTimer::singleShot(msec_per_frame, this, SLOT(onTimer()));
		}
	}
}

void CMainWindow::on_selectData_currentValueChanged(int index)
{
//	if (index == -1)
//		ui->actionColorMap->setDisabled(true);
//	else
	{
//		if (ui->actionColorMap->isEnabled() == false)
//			ui->actionColorMap->setEnabled(true);

		int nfield = ui->selectData->currentValue();
		CPostDoc* doc = GetActiveDocument();
		if (doc == nullptr) return;
		doc->SetDataField(nfield);

		// turn on the colormap
		if (ui->actionColorMap->isChecked() == false)
		{
			ui->actionColorMap->toggle();
		}

		ui->glview->UpdateWidgets(false);
		RedrawGL();
	}

//	UpdateGraphs(false);

//	if (ui->modelViewer->isVisible()) ui->modelViewer->Update(false);
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionPlay_toggled(bool bchecked)
{
	CPostDoc* doc = GetActiveDocument();
	if (doc && doc->IsValid())
	{
		if (bchecked)
		{
			TIMESETTINGS& time = doc->GetTimeSettings();
			double fps = time.m_fps;
			if (fps < 1.0) fps = 1.0;
			double msec_per_frame = 1000.0 / fps;

			ui->m_isAnimating = true;
			QTimer::singleShot(msec_per_frame, this, SLOT(onTimer()));
		}
		else ui->m_isAnimating = false;
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionFirst_triggered()
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	doc->SetActiveState(time.m_start);
	ui->pspin->setValue(time.m_start+1);
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionPrev_triggered()
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	int nstep = doc->GetActiveState();
	nstep--;
	if (nstep < time.m_start) nstep = time.m_start;
	doc->SetActiveState(nstep);
	ui->pspin->setValue(nstep+1);
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionNext_triggered()
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	int nstep = doc->GetActiveState();
	nstep++;
	if (nstep > time.m_end) nstep = time.m_end;
	doc->SetActiveState(nstep);
	ui->pspin->setValue(nstep+1);
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionLast_triggered()
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	TIMESETTINGS& time = doc->GetTimeSettings();
	doc->SetActiveState(time.m_end);
	ui->pspin->setValue(time.m_end+1);
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionTimeSettings_triggered()
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;

	CDlgTimeSettings dlg(doc, this);
	if (dlg.exec())
	{
		TIMESETTINGS& time = doc->GetTimeSettings();
//		ui->timePanel->SetRange(time.m_start, time.m_end);

		int ntime = doc->GetActiveState();
		if ((ntime < time.m_start) || (ntime > time.m_end))
		{
			if (ntime < time.m_start) ntime = time.m_start;
			if (ntime > time.m_end) ntime = time.m_end;
		}
		doc->SetActiveState(ntime);
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::on_actionColorMap_toggled(bool bchecked)
{
	CPostDoc* doc = GetActiveDocument();
	if (doc == nullptr) return;
	doc->ActivateColormap(bchecked);
	RedrawGL();
}

//-----------------------------------------------------------------------------
void CMainWindow::StopAnimation()
{
	ui->stopAnimation();
}

//-----------------------------------------------------------------------------
// add to the log 
void CMainWindow::AddLogEntry(const QString& txt)
{
	ui->logPanel->AddText(txt);
}

//-----------------------------------------------------------------------------
// add to the output window
void CMainWindow::AddOutputEntry(const QString& txt)
{
	ui->logPanel->AddText(txt, 1);
}

//-----------------------------------------------------------------------------
// clear the log
void CMainWindow::ClearLog()
{
	ui->logPanel->ClearLog();
}

//-----------------------------------------------------------------------------
// clear the output window
void CMainWindow::ClearOutput()
{
	ui->logPanel->ClearOutput();
}

//-----------------------------------------------------------------------------
void CMainWindow::on_glview_pointPicked(const vec3d& r)
{
	GetCreatePanel()->setInput(r);
}

//-----------------------------------------------------------------------------
void CMainWindow::on_glview_selectionChanged()
{
}

//-----------------------------------------------------------------------------
void CMainWindow::SetStatusMessage(const QString& message)
{
	ui->statusBar->showMessage(message);
}

//-----------------------------------------------------------------------------
void CMainWindow::ClearStatusMessage()
{
	ui->statusBar->clearMessage();
}

//-----------------------------------------------------------------------------
void CMainWindow::BuildContextMenu(QMenu& menu)
{
	menu.addAction(ui->actionZoomSelect);
	menu.addAction(ui->actionShowGrid);
	menu.addAction(ui->actionShowMeshLines);
	menu.addAction(ui->actionShowEdgeLines);
	menu.addAction(ui->actionOrtho);
	menu.addAction(ui->actionShowNormals);
	menu.addAction(ui->actionShowFibers);
	menu.addSeparator();
	menu.addAction(ui->actionFront);
	menu.addAction(ui->actionBack);
	menu.addAction(ui->actionLeft);
	menu.addAction(ui->actionRight);
	menu.addAction(ui->actionTop);
	menu.addAction(ui->actionBottom);
	menu.addSeparator();
	menu.addAction(ui->actionOptions);
}

//-----------------------------------------------------------------------------
// Update the physics menu based on active modules
void CMainWindow::UpdatePhysicsUi()
{
	FEProject& prj = GetDocument()->GetProject();
	int module = prj.GetModule();

	ui->actionAddRigidConstraint->setVisible(module & MODULE_MECH);
	ui->actionAddRigidConnector->setVisible(module & MODULE_MECH);
	ui->actionSoluteTable->setVisible(module & MODULE_SOLUTES);
	ui->actionSBMTable->setVisible(module & MODULE_SOLUTES);
	ui->actionAddReaction->setVisible(module & MODULE_REACTIONS);
}

//-----------------------------------------------------------------------------
void CMainWindow::onExportAllMaterials()
{
	CDocument* doc = GetDocument();
	FEModel& fem = *doc->GetFEModel();

	vector<GMaterial*> matList;
	for (int i=0; i<fem.Materials(); ++i)
	{
		matList.push_back(fem.GetMaterial(i));
	}

	onExportMaterials(matList);
}

//-----------------------------------------------------------------------------
void CMainWindow::onExportMaterials(const vector<GMaterial*>& matList)
{
	if (matList.size() == 0)
	{
		QMessageBox::information(this, "Export Materials", "This project does not contain any materials");
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, "Export Materials", "", "PreView Materials (*.pvm)");
	if (fileName.isEmpty() == false)
	{
		CDocument* doc = GetDocument();
		if (doc->ExportMaterials(fileName.toStdString(), matList) == false)
		{
			QMessageBox::critical(this, "Export Materials", "Failed exporting materials");
		}
	}	
}

//-----------------------------------------------------------------------------
void CMainWindow::onImportMaterials()
{
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Import Materials", "", "PreView Materials (*.pvm)");
	if (fileNames.isEmpty() == false)
	{
		CDocument* doc = GetDocument();

		for (int i=0; i<fileNames.size(); ++i)
		{
			QString file = fileNames.at(i);
			if (doc->ImportMaterials(file.toStdString()) == false)
			{
				QString msg = QString("Failed importing materials from\n%1").arg(file);
				QMessageBox::critical(this, "Import Materials", msg);
			}
		}

		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllMaterials()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all materials?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllMaterials();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllBC()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all boundary conditions?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllBC();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllLoads()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all loads?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllLoads();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllIC()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all initial conditions?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllIC();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllContact()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all contact interfaces?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllContact();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllConstraints()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all rigid constraints?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllConstraints();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllConnectors()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all rigid connectors?\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllConnectors();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::DeleteAllSteps()
{
	if (QMessageBox::question(this, "PreView2", "Are you sure you want to delete all steps?\nThis will also delete all boundary conditions, etc., associated with the steps.\nThis cannot be undone.", QMessageBox::Ok | QMessageBox::Cancel))
	{
		CDocument* doc = GetDocument();
		FEModel& fem = *doc->GetFEModel();
		fem.DeleteAllSteps();
		UpdateModel();
		RedrawGL();
	}
}

//-----------------------------------------------------------------------------
void CMainWindow::GenerateMap(FEObject* po)
{
	CDlgAddMeshData dlg(po, this);
	if (dlg.exec())
	{
		std::string mapName = dlg.GetMapName();
		std::string paramName = dlg.GetParamName();
		Param_Type paramType = dlg.GetParamType();

		CDocument* doc = GetDocument();
		FEObject* data = doc->CreateDataMap(po, mapName, paramName, paramType);
		if (data == 0)
		{
			QMessageBox::critical(this, "PreView2", "It pains me to inform you that your command could not be executed.");
		}
		else UpdateModel(data);
	}
}

// remove a graph from the list
void CMainWindow::RemoveGraph(::CGraphWindow* graph)
{
	ui->graphList.removeOne(graph);
}

// Add a graph to the list of managed graph windows
void CMainWindow::AddGraph(CGraphWindow* graph)
{
	graph->setWindowTitle(QString("FEBioStudio : Graph%1").arg(ui->graphList.size() + 1));
	ui->graphList.push_back(graph);
}

void CMainWindow::on_fontStyle_currentFontChanged(const QFont& font)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont old_font = pw->get_font();
		std::string s = font.family().toStdString();
		QFont new_font(font.family(), old_font.pointSize());
		new_font.setBold(old_font.bold());
		new_font.setItalic(old_font.italic());
		pw->set_font(new_font);
		RedrawGL();
	}
}

void CMainWindow::on_fontSize_valueChanged(int i)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setPointSize(i);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::on_fontBold_toggled(bool checked)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setBold(checked);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::on_fontItalic_toggled(bool bchecked)
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		font.setItalic(bchecked);
		pw->set_font(font);
		RedrawGL();
	}
}

void CMainWindow::on_actionProperties_triggered()
{
	// get the selected widget
	GLWidget* pglw = GLWidget::get_focus();
	if (pglw == 0) return;

	// edit the properties
	if (dynamic_cast<GLBox*>(pglw))
	{
		CDlgBoxProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLLegendBar*>(pglw))
	{
		CDlgLegendProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLTriad*>(pglw))
	{
		CDlgTriadProps dlg(pglw, this);
		dlg.exec();
	}
	else if (dynamic_cast<GLSafeFrame*>(pglw))
	{
		CDlgCaptureFrameProps dlg(pglw, this);
		dlg.exec();
	}
	else
	{
		QMessageBox::information(this, "Properties", "No properties available");
	}

	UpdateFontToolbar();

	RedrawGL();
}

void CMainWindow::UpdateFontToolbar()
{
	GLWidget* pw = GLWidget::get_focus();
	if (pw)
	{
		QFont font = pw->get_font();
		ui->pFontStyle->setCurrentFont(font);
		ui->pFontSize->setValue(font.pointSize());
		ui->actionFontBold->setChecked(font.bold());
		ui->actionFontItalic->setChecked(font.italic());
		ui->pFontToolBar->setEnabled(true);
	}
	else ui->pFontToolBar->setDisabled(true);
}
