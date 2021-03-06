/*This file is part of the FEBio Studio source code and is licensed under the MIT license
listed below.

See Copyright-FEBio-Studio.txt for details.

Copyright (c) 2020 University of Utah, The Trustees of Columbia University in 
the City of New York, and others.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

#pragma once
#include "FEMesher.h"
#include <MeshLib/FESurfaceMesh.h>
#include <GeomLib/GItem.h>
#include <vector>
using namespace std;

class GObject;

#ifdef TETLIBRARY
class tetgenio;
#endif

//-----------------------------------------------------------------------------
// The PLC class describes a piece-wise linear complex and is used as an
// intermediate structure beteen the GPLCObject and the TetGen data-structures
//
class PLC
{
public:
	class NODE
	{
	public:
		vec3d	r;
		int		nid;
	};

	class EDGE
	{
	public:
		EDGE() {}
		EDGE(const EDGE& e) { node = e.node; nid = e.nid; }
		void operator = (const EDGE& e) { node = e.node; nid = e.nid; }

		void AddNode(int n) { node.push_back(n); }
		int FindNode(int nid);

		int Nodes() { return (int) node.size(); }

	public:
		vector<int>	node;
		int			nid;
	};

	class FACE
	{
	public:
		FACE() {}
		FACE(const FACE& f) { node = f.node; nid = f.nid; }
		void operator = (const FACE& f) { node = f.node; nid = f.nid; }

		int Nodes() { return (int) node.size(); }

	public:
		vector<int>	node;
		int			nid;
	};

public:
	PLC();
	bool Build(GObject* po, double h);

	int AddNode(vec3d r, int nid = -1);
	int FindNode(int nid);

	int Nodes() { return (int) m_Node.size(); }
	NODE& Node(int i) { return m_Node[i]; }

	int Faces() { return (int) m_Face.size(); }
	FACE& Face(int i) { return m_Face[i]; }

	int Edges() { return (int) m_Edge.size(); }
	EDGE& Edge(int i) { return m_Edge[i]; }

protected:
	bool BuildNodes();
	bool BuildEdges();
	bool BuildFaces();

protected:
	bool BuildFaceQuad   (GFace& fs);
	bool BuildFacePolygon(GFace& fs);
	bool BuildFaceExtrude(GFace& fs);
	bool BuildFaceRevolve(GFace& fs);

protected:
	vector<NODE>	m_Node;
	vector<EDGE>	m_Edge;
	vector<FACE>	m_Face;

	GObject*	m_po;	//!< object we are building a PLC for
	double		m_h;	//!< element size
};

//-----------------------------------------------------------------------------
//! This class implements an interface to the TetGen mesher
class FETetGenMesher : public FEMesher
{
public:
	enum { ELSIZE, QUALITY, ELTYPE, SPLIT_FACES, HOLE, HOLE_COORD };

public:
	FETetGenMesher(){}
	FETetGenMesher(GObject* po);

	// build the mesh
	FEMesh*	BuildMesh() override;

	double ElementSize();

	bool SplitFaces();

	int ElementType();

	double ElementQuality();

public:
	// Generate a volume mesh from a surface mesh
	FEMesh* CreateMesh(FESurfaceMesh* surfaceMesh);

protected:
	GObject*	m_po;	// TODO: move this to base class

#ifdef TETLIBRARY
public:
	bool build_plc(FESurfaceMesh* pm, tetgenio& in);
protected:
	bool build_tetgen_in(tetgenio& in);
	bool build_tetgen_in_remesh(tetgenio& io);
	FEMesh* build_tet_mesh(tetgenio& out);
	FEMesh* build_tet10_mesh(FEMesh* pm);
	FEMesh* build_tet15_mesh(FEMesh* pm);
#endif
};
