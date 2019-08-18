#include "stdafx.h"
#include "FEMeshValuator.h"
#include <MeshLib/MeshMetrics.h>
#include <MeshLib/triangulate.h>

//-----------------------------------------------------------------------------
// constructor
FEMeshValuator::FEMeshValuator(FEMesh& mesh) : m_mesh(mesh)
{
}

//-----------------------------------------------------------------------------
// evaluate the particular data field
void FEMeshValuator::Evaluate(int nfield)
{
	// evaluate mesh
	int n = 0;
	int NE = m_mesh.Elements();
	for (int i = 0; i<NE; ++i)
	{
		FEElement& el = m_mesh.Element(i);
		if (el.IsVisible()) 
		{
			double val = EvaluateElement(i, nfield);
			m_mesh.SetElementValue(i, val);
			m_mesh.SetElementDataTag(i, 1);
		}
		else m_mesh.SetElementDataTag(i, 0);
	}

	// update stats
	m_mesh.UpdateValueRange();
}

//-----------------------------------------------------------------------------
// Evaluate element data
double FEMeshValuator::EvaluateElement(int n, int nfield, int* err)
{
	if (err) *err = 0;
	double val = 0, sum = 0;
	const FEElement& el = m_mesh.Element(n);
	switch (nfield)
	{
	case 0: // element volume
		val = FEMeshMetrics::ElementVolume(m_mesh, el);
		break;
	case 1: // jacobian
		if (el.IsShell()) val = FEMeshMetrics::ShellJacobian(m_mesh, el, 1);
		else val = FEMeshMetrics::SolidJacobian(m_mesh, el);
		break;
	case 2: // shell thickness
		if (el.IsShell())
		{
			int n = el.Nodes();
			val = el.m_h[0];
			for (int i = 1; i<n; ++i) if (el.m_h[i] < val) val = el.m_h[i];
		}
		break;
	case 3: // shell area
		if (el.IsShell()) val = FEMeshMetrics::ShellArea(m_mesh, el);
		break;
	case 4: // element quality
		val = FEMeshMetrics::TetQuality(m_mesh, el);
		break;
	case 5: // min dihedral angle
		val = FEMeshMetrics::TetMinDihedralAngle(m_mesh, el);
		break;
	case 6: // max dihedral angle
		val = FEMeshMetrics::TetMaxDihedralAngle(m_mesh, el);
		break;
	case 7:
		val = FEMeshMetrics::TriQuality(m_mesh, el);
		break;
	case 8:
		val = FEMeshMetrics::Tet10MidsideNodeOffset(m_mesh, el);
		break;
	default:
		nfield -= 9;
		if ((nfield >= 0) && (nfield < m_mesh.DataFields()))
		{
			FEMeshData* data = m_mesh.GetMeshData(nfield);
			switch (data->GetDataClass())
			{
			case FEMeshData::NODE_DATA:
				{
					FENodeData& nodeData = dynamic_cast<FENodeData&>(*data);
					int ne = el.Nodes();
					val = 0.0;
					for (int i = 0; i<ne; ++i) val += nodeData.get(el.m_node[i]);
					val /= (double)ne;
				}
				break;
			case FEMeshData::SURFACE_DATA:
				// TODO: Not sure what to do here. 
				break;
			case FEMeshData::PART_DATA:
				{
					FEElementData& elemData = dynamic_cast<FEElementData&>(*data);
					val = elemData.get(n);
					if (err) *err = (elemData.GetTag(n) == 0 ? 1 : 0);
				}
				break;
			}
		}
	}

	return val;
}