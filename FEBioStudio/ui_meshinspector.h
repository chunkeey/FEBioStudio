#pragma once
#include "MeshInspector.h"
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QPushButton>
#include "PlotWidget.h"
#include <QGroupBox>
#include <QLineEdit>
#include <MeshLib/FEMesh.h>
#include <GeomLib/GObject.h>

class CMeshInfo : public QGroupBox
{
public:
	CMeshInfo(QWidget* parent = 0) : QGroupBox("Mesh Info", parent)
	{
		QFormLayout* infoLayout = new QFormLayout;
		infoLayout->addRow("Name:", name = new QLabel(""));
		infoLayout->addRow("Nodes:", nodes = new QLabel(""));
		infoLayout->addRow("Faces:", faces = new QLabel(""));
		infoLayout->addRow("Elements:", elems = new QLabel(""));
		setLayout(infoLayout);
	}

	void setMesh(GObject* po)
	{
		FEMesh* pm = (po ? po->GetFEMesh() : 0);
		if (pm)
		{
			name->setText(QString::fromStdString(po->GetName()));
			nodes->setText(QString::number(pm->Nodes()));
			faces->setText(QString::number(pm->Faces()));
			elems->setText(QString::number(pm->Elements()));
		}
		else
		{
			name->setText("---");
			nodes->setText("---");
			faces->setText("---");
			elems->setText("---");
		}
	}

private:
	QLabel*	name;
	QLabel*	nodes;
	QLabel*	faces;
	QLabel*	elems;
};

class CStatsInfo : public QGroupBox
{
public:
	CStatsInfo(QWidget* parent = 0) : QGroupBox("Statistics", parent)
	{
		QFormLayout* statsLayout = new QFormLayout;
		statsLayout->addRow("Min", min = new QLabel);
		statsLayout->addRow("Max", max = new QLabel);
		statsLayout->addRow("Avg", avg = new QLabel);
		setLayout(statsLayout);
	}

	void setRange(double fmin, double fmax, double favg)
	{
		min->setText(QString::number(fmin));
		max->setText(QString::number(fmax));
		avg->setText(QString::number(favg));
	}

private:
	QLabel*	min;
	QLabel*	max;
	QLabel*	avg;
};

class CSelectionInfo : public QGroupBox
{
public:
	CSelectionInfo(QWidget* parent = 0) : QGroupBox("Selection", parent)
	{
		QFormLayout* formLayout = new QFormLayout;
		formLayout->addRow("min:", min = new QLineEdit); min->setValidator(new QDoubleValidator);
		formLayout->addRow("max:", max = new QLineEdit); max->setValidator(new QDoubleValidator);

		QHBoxLayout* selLayout = new QHBoxLayout;
		selLayout->addLayout(formLayout);
		selButton = new QPushButton("Select");
		selButton->setObjectName("select");
		selLayout->addWidget(selButton);

		setLayout(selLayout);
	}

	void setRange(double fmin, double fmax)
	{
		min->setText(QString::number(fmin));
		max->setText(QString::number(fmax));
	}

	void getRange(double& fmin, double& fmax)
	{
		fmin = min->text().toDouble();
		fmax = max->text().toDouble();
	}

public:
	QLineEdit*	min;
	QLineEdit*	max;
	QPushButton* selButton;
};

class Ui::CMeshInspector
{
public:
	enum { MAX_EVAL_FIELDS = 9 };

public:
	CMeshInfo*		info;
	QTableWidget*	table;
	CPlotWidget*	plot;
	QComboBox*		var;
	CStatsInfo*		stats;
	CSelectionInfo*	sel;

public:
	void setupUi(QMainWindow* wnd)
	{
		info = new CMeshInfo;

		table = new QTableWidget;

		table->setColumnCount(2);
		table->setSelectionBehavior(QAbstractItemView::SelectRows);
		table->setSelectionMode(QAbstractItemView::SingleSelection);
		table->horizontalHeader()->setStretchLastSection(true);
		table->setHorizontalHeaderLabels(QStringList()<<"Type"<<"Count");
		table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
		table->verticalHeader()->setDefaultSectionSize(24);
		table->verticalHeader()->hide();
		table->setEditTriggers(QAbstractItemView::NoEditTriggers);

		var = new QComboBox;
		var->setObjectName("var");
		QFormLayout* varForm = new QFormLayout;
		varForm->addRow("Variable:", var);

		QHBoxLayout* topLayout = new QHBoxLayout;
		topLayout->addWidget(info);
		topLayout->addWidget(table);

		plot = new CPlotWidget;
		plot->showLegend(false);
		plot->setViewLocked(true);

		stats = new CStatsInfo;

		sel = new CSelectionInfo;

		QHBoxLayout* bottomLayout = new QHBoxLayout;
		bottomLayout->addWidget(stats);
		bottomLayout->addWidget(sel);

		QWidget* w = new QWidget;

		QVBoxLayout* mainLayout = new QVBoxLayout;
//		mainLayout->setMargin(0);
//		mainLayout->setSpacing(0);
		mainLayout->addLayout(topLayout);
		mainLayout->addLayout(varForm);
		mainLayout->addWidget(plot);
		mainLayout->addLayout(bottomLayout);
		w->setLayout(mainLayout);

		wnd->setCentralWidget(w);

		var->setFocus();

		QMetaObject::connectSlotsByName(wnd);
	}

	void setMesh(GObject* po)
	{
		const int MAX_ELEM = 20;
		static int ET[] = {
			FE_HEX8, FE_TET4, FE_PENTA6, FE_QUAD4, FE_TRI3, FE_BEAM2, FE_HEX20, FE_QUAD8, FE_BEAM3, FE_TET10, FE_TRI6, FE_TET15, FE_HEX27, FE_TRI7, FE_QUAD9, FE_TET20, FE_TRI10, FE_PYRA5, FE_PENTA15, FE_TET5, 0};

		static const char* EN[] = {
			"HEX8", "TET4", "PENTA6", "QUAD4", "TRI3", "BEAM2", "HEX20", "QUAD8", "LINE3", "TET10", "TRI6", "TET15", "HEX27", "TRI7", "QUAD9", "TET20", "TRI10", "PYRA5", "PENTA15", "TET5", "(unknown)"};

		table->setRowCount(0);

		info->setMesh(po);
		if (po == 0) return;

		FEMesh* pm = po->GetFEMesh();
		if (pm == 0) return;

		int n[MAX_ELEM + 1] = { 0 };
		int NE = pm->Elements();
		for (int i = 0; i<NE; ++i)
		{
			FEElement& e = pm->Element(i);
			switch (e.Type())
			{
			case FE_HEX8   : n[0]++; break;
			case FE_TET4   : n[1]++; break;
			case FE_PENTA6 : n[2]++; break;
			case FE_QUAD4  : n[3]++; break;
			case FE_TRI3   : n[4]++; break;
			case FE_BEAM2  : n[5]++; break;
			case FE_HEX20  : n[6]++; break;
			case FE_QUAD8  : n[7]++; break;
			case FE_BEAM3  : n[8]++; break;
			case FE_TET10  : n[9]++; break;
			case FE_TRI6   : n[10]++; break;
			case FE_TET15  : n[11]++; break;
			case FE_HEX27  : n[12]++; break;
			case FE_TRI7   : n[13]++; break;
			case FE_QUAD9  : n[14]++; break;
			case FE_TET20  : n[15]++; break;
			case FE_TRI10  : n[16]++; break;
			case FE_PYRA5  : n[17]++; break;
			case FE_PENTA15: n[18]++; break;
			case FE_TET5   : n[19]++; break;
			default:
				assert(false);
				n[MAX_ELEM]++; break;
			}
		}

		int m = 0;
		for (int i = 0; i<MAX_ELEM + 1; ++i) if (n[i] != 0) m++;

		// fill the rows
		table->setRowCount(m); m = 0;
		for (int i = 0; i<MAX_ELEM + 1; ++i)
		{
			if (n[i] != 0)
			{
				QTableWidgetItem* item = new QTableWidgetItem(EN[i]);
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				item->setData(Qt::UserRole, ET[i]);
				table->setItem(m, 0, item);
				item = new QTableWidgetItem(QString::number(n[i]));
				item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
				table->setItem(m, 1, item);
				m++;
			}
		}

		// NOTE: If a new field is added, make sure to update the MAX_EVAL_FIELDS enum above.
		QStringList items;
		items << "Element Volume";
		items << "Jacobian";
		items << "Shell thickness";
		items << "Shell area";
		items << "Tet quality";
		items << "Tet minimal dihedral angle";
		items << "Tet maximal dihedral angle";
		items << "Triangle quality";
		items << "Tet10 midside node offset";
		var->clear();
		var->addItems(items);

		// Add data fields
		if (pm->DataFields())
		{
			var->insertSeparator(var->count());

			for (int i = 0; i<pm->DataFields(); ++i)
			{
				FEMeshData& di = *pm->GetMeshData(i);
				var->addItem(QString::fromStdString(di.GetName()));
			}
		}
	}
};
