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

#include "stdafx.h"
#include "FEMultiBlockMesh.h"
#include <MeshLib/FEMesh.h>

void MBBlock::SetNodes(int n1,int n2,int n3,int n4,int n5,int n6,int n7,int n8)
{
	m_node[0] = n1;
	m_node[1] = n2;
	m_node[2] = n3;
	m_node[3] = n4;
	m_node[4] = n5;
	m_node[5] = n6;
	m_node[6] = n7;
	m_node[7] = n8;
}

void MBBlock::SetZoning(double gx, double gy, double gz, bool bx, bool by, bool bz)
{
	m_gx = gx;
	m_gy = gy;
	m_gz = gz;
	m_bx = bx;
	m_by = by;
	m_bz = bz;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

FEMultiBlockMesh::FEMultiBlockMesh()
{
}

//-----------------------------------------------------------------------------

FEMultiBlockMesh::~FEMultiBlockMesh(void)
{
}

//-----------------------------------------------------------------------------
// build the FE mesh
//
FEMesh* FEMultiBlockMesh::BuildMesh()
{
	// create a new mesh
	FEMesh* pm = new FEMesh();

	// build the mesh
	BuildNodes(pm);
	BuildElements(pm);
	BuildFaces(pm);
	BuildEdges(pm);

	// update the mesh
	pm->BuildMesh();

	return pm;
}

//-----------------------------------------------------------------------------
// build the FE nodes
//
void FEMultiBlockMesh::BuildNodes(FEMesh *pm)
{
	int i, j, k, l;

	int NB = m_MBlock.size();
	int NF = m_MBFace.size();
	int NE = m_MBEdge.size();
	int NN = m_MBNode.size();

	// now, let's count the nodes
	// I think the correct formula is:
	// (NX+1)*(NY+1)*(NZ+1)*blocks - (NX+1)*(NY+1)*internal faces + (NX+1)*internal edges - internal nodes
	int nodes = 0;
	for (i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		nodes += (B.m_nx+1)*(B.m_ny+1)*(B.m_nz+1);
	}
	for (i=0; i<NF; ++i)
	{
		MBFace& F = m_MBFace[i];
		if (!F.m_bext) nodes -= (F.m_nx+1)*(F.m_ny+1);
	}
	for (i=0; i<NE; ++i)
	{
		MBEdge& E = m_MBEdge[i];
		if (!E.m_bext) nodes += (E.m_nx + 1);
	}
	for (i=0; i<NN; ++i)
	{
		MBNode& N = m_MBNode[i];
		if (!N.m_bext) nodes -= 1;
	}

	// create storage
	pm->Create(nodes, 0);

	// A. create the nodes
	// A.1. add all MB nodes
	nodes = 0;
	FENode* pn = pm->NodePtr();
	for (i=0; i<NN; ++i, ++pn)
	{
		pn->r = m_MBNode[i].m_r;
		m_MBNode[i].m_ntag = nodes++;
		pn->m_gid = m_MBNode[i].m_gid;
	}

	// A.2. add all edge nodes
	vec3d r1, r2, r3, r4, r5, r6, r7, r8;
	double r, s, t, gr, gs, gt, fr, fs, ft, dr, ds, dt;
	double N1, N2, N3, N4, N5, N6, N7, N8;
	for (i=0; i<NE; ++i)
	{
		MBEdge& e = m_MBEdge[i];
		r1 = m_MBNode[e.m_node[0]].m_r;
		r2 = m_MBNode[e.m_node[1]].m_r;
		e.m_ntag = nodes;

		fr = e.m_gx;
		gr = 1;
		if (e.m_bx)
		{
			gr = 2; if (e.m_nx%2) gr += fr;
			for (j=0; j<e.m_nx/2-1; ++j) gr = fr*gr+2;
			gr = 1 / gr;
		}
		else 
		{
			for (j=0; j<e.m_nx-1; ++j) gr = fr*gr+1; 
			gr = 1 / gr;
		}

		dr = gr;
		r = 0;
		for (j=0; j<e.m_nx; ++j)
		{
			if (j>0)
			{
				N1 = 1-r;
				N2 = r;
				pn->r = r1*N1 + r2*N2;
				++pn;
				++nodes;
			}

			r += dr;
			dr *= fr;
			if (e.m_bx && (j==e.m_nx/2-1))
			{
				if (e.m_nx%2 == 0) dr /= fr;
				fr = 1.0/fr;
			}
		}
	}

	// A.3. add all face nodes
	for (i=0; i<NF; ++i)
	{
		MBFace& f = m_MBFace[i];
		r1 = m_MBNode[f.m_node[0]].m_r;
		r2 = m_MBNode[f.m_node[1]].m_r;
		r3 = m_MBNode[f.m_node[2]].m_r;
		r4 = m_MBNode[f.m_node[3]].m_r;
		f.m_ntag = nodes;

		fr = f.m_gx;
		gr = 1;
		if (f.m_bx)
		{
			gr = 2; if (f.m_nx%2) gr += fr;
			for (j=0; j<f.m_nx/2-1; ++j) gr = fr*gr+2;
			gr = 1 / gr;
		}
		else 
		{
			for (j=0; j<f.m_nx-1; ++j) gr = fr*gr+1; 
			gr = 1 / gr;
		}

		fs = f.m_gy;
		gs = 1;
		if (f.m_by)
		{
			gs = 2; if (f.m_ny%2) gs += fs;
			for (j=0; j<f.m_ny/2-1; ++j) gs = fs*gs+2;
			gs = 1 / gs;
		}
		else 
		{
			for (j=0; j<f.m_ny-1; ++j) gs = fs*gs+1; 
			gs = 1 / gs;
		}

		ds = gs;
		s = 0;
		for (j=0; j<f.m_ny; ++j)
		{
			if (j>0)
			{
				dr = gr;
				r = 0;
				for (k=0; k<f.m_nx; ++k)
				{
					if (k>0)
					{
						N1 = (1-r)*(1-s);
						N2 = r*(1-s);
						N3 = r*s;
						N4 = (1-r)*s;
						pn->r = r1*N1 + r2*N2 + r3*N3 + r4*N4;
						++pn;
						++nodes;
					}
					r += dr;
					dr *= fr;
					if (f.m_bx && (k==f.m_nx/2-1))
					{
						if (f.m_nx%2 == 0) dr /= fr;
						fr = 1.0/fr;
					}
				}
				if (f.m_bx) fr = 1.0/fr;
			}

			s += ds;
			ds *= fs;
			if (f.m_by && (j==f.m_ny/2-1))
			{
				if (f.m_ny%2 == 0) ds /= fs;
				fs = 1.0/fs;
			}
		}
	}

	// A.4. add all block nodes
	for (i=0; i<NB; ++i)
	{
		MBBlock& b = m_MBlock[i];
		r1 = m_MBNode[b.m_node[0]].m_r;
		r2 = m_MBNode[b.m_node[1]].m_r;
		r3 = m_MBNode[b.m_node[2]].m_r;
		r4 = m_MBNode[b.m_node[3]].m_r;
		r5 = m_MBNode[b.m_node[4]].m_r;
		r6 = m_MBNode[b.m_node[5]].m_r;
		r7 = m_MBNode[b.m_node[6]].m_r;
		r8 = m_MBNode[b.m_node[7]].m_r;
		b.m_ntag = nodes;

		fr = b.m_gx;
		gr = 1;
		if (b.m_bx)
		{
			gr = 2; if (b.m_nx%2) gr += fr;
			for (j=0; j<b.m_nx/2-1; ++j) gr = fr*gr+2;
			gr = 1 / gr;
		}
		else 
		{
			for (j=0; j<b.m_nx-1; ++j) gr = fr*gr+1; 
			gr = 1 / gr;
		}

		fs = b.m_gy;
		gs = 1;
		if (b.m_by)
		{
			gs = 2; if (b.m_ny%2) gs += fs;
			for (j=0; j<b.m_ny/2-1; ++j) gs = fs*gs+2;
			gs = 1 / gs;
		}
		else 
		{
			for (j=0; j<b.m_ny-1; ++j) gs = fs*gs+1; 
			gs = 1 / gs;
		}

		ft = b.m_gz;
		gt = 1;
		if (b.m_bz)
		{
			gt = 2; if (b.m_nz%2) gt += ft;
			for (j=0; j<b.m_nz/2-1; ++j) gt = ft*gt+2;
			gt = 1 / gt;
		}
		else 
		{
			for (j=0; j<b.m_nz-1; ++j) gt = ft*gt+1; 
			gt = 1 / gt;
		}

		dt = gt;
		t = 0;
		for (j=0; j<b.m_nz; ++j)
		{
			if (j>0)
			{
				ds = gs;
				s = 0;
				for (k=0; k<b.m_ny; ++k)
				{
					if (k>0)
					{
						dr = gr;
						r = 0;
						for (l=0; l<b.m_nx; ++l)
						{
							if (r>0)
							{
								N1 = (1-r)*(1-s)*(1-t);
								N2 = r*(1-s)*(1-t);
								N3 = r*s*(1-t);
								N4 = (1-r)*s*(1-t);
								N5 = (1-r)*(1-s)*t;
								N6 = r*(1-s)*t;
								N7 = r*s*t;
								N8 = (1-r)*s*t;
								pn->r = r1*N1 + r2*N2 + r3*N3 + r4*N4 + r5*N5 + r6*N6 + r7*N7 + r8*N8;
								++pn;
								++nodes;
							}

							r += dr;
							dr *= fr;
							if (b.m_bx && (l==b.m_nx/2-1))
							{
								if (b.m_nx%2 == 0) dr /= fr;
								fr = 1.0/fr;
							}
						}
						if (b.m_bx) fr = 1.0/fr;
					}

					s += ds;
					ds *= fs;
					if (b.m_by && (k==b.m_ny/2-1))
					{
						if (b.m_ny%2 == 0) ds /= fs;
						fs = 1.0/fs;
					}
				}
				if (b.m_by) fs = 1.0/fs;
			}

			t += dt;
			dt *= ft;
			if (b.m_bz && (j==b.m_nz/2-1))
			{
				if (b.m_nz%2 == 0) dt /= ft;
				ft = 1.0/ft;
			}
		}
	}

	// make sure we have the right number of nodes
	assert(nodes == pm->Nodes());
}

//-----------------------------------------------------------------------------
// build the FE elements
//
void FEMultiBlockMesh::BuildElements(FEMesh* pm)
{
	int i, j, k, l;

	int NB = m_MBlock.size();

	// figure out how many elements we have
	int elems = 0;
	for (i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		elems += B.m_nx*B.m_ny*B.m_nz;
	}

	// allocate elements
	pm->Create(0, elems);

	// create the elements
	int eid = 0;
	for (i=0; i<NB; ++i)
	{
		MBBlock& b = m_MBlock[i];
		for (j=0; j<b.m_nz; ++j)
		{
			for (k=0; k<b.m_ny; ++k)
			{
				for (l=0; l<b.m_nx; ++l)
				{
					FEElement_* pe = pm->ElementPtr(eid++);

					pe->m_node[0] = GetBlockNodeIndex(b, l  , k  , j);
					pe->m_node[1] = GetBlockNodeIndex(b, l+1, k  , j);
					pe->m_node[2] = GetBlockNodeIndex(b, l+1, k+1, j);
					pe->m_node[3] = GetBlockNodeIndex(b, l  , k+1, j);
					pe->m_node[4] = GetBlockNodeIndex(b, l  , k  , j+1);
					pe->m_node[5] = GetBlockNodeIndex(b, l+1, k  , j+1);
					pe->m_node[6] = GetBlockNodeIndex(b, l+1, k+1, j+1);
					pe->m_node[7] = GetBlockNodeIndex(b, l  , k+1, j+1);
					pe->SetType(FE_HEX8);
					pe->m_gid = b.m_gid;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Build the FE faces
//
void FEMultiBlockMesh::BuildFaces(FEMesh* pm)
{
	int i, j, k;

	// count faces
	int faces = 0;
	int n1, n2;
	for (i=0; i<(int) m_MBFace.size(); ++i)
	{
		MBFace& f = m_MBFace[i];
		if (f.m_gid >= 0) faces += f.m_nx*f.m_ny;
	}

	// allocate faces
	pm->Create(0,0, faces);

	// build the faces
	FEFace* pf = pm->FacePtr();
	int m = 0;
	for (k=0; k<(int) m_MBFace.size(); ++k)
	{
		MBFace& f = m_MBFace[k];
		if (f.m_gid >= 0)
		{
			n1 = f.m_block[0];
			n2 = f.m_block[1];

			for (i=0; i<f.m_nx; ++i)
			{
				for (j=0; j<f.m_ny; ++j, ++pf)
				{
					pf->SetType(FE_FACE_QUAD4);
					pf->m_gid = pf->m_sid = f.m_gid;
					pf->n[0] = GetFaceNodeIndex(f, i  , j  );
					pf->n[1] = GetFaceNodeIndex(f, i+1, j  );
					pf->n[2] = GetFaceNodeIndex(f, i+1, j+1);
					pf->n[3] = GetFaceNodeIndex(f, i  , j+1);
				}
			}
			++m;
		}
	}
}

//-----------------------------------------------------------------------------
// Build the FE edges
//
void FEMultiBlockMesh::BuildEdges(FEMesh* pm)
{
	int i, k;

	// count edges
	int edges = 0;
	for (i=0; i<(int) m_MBEdge.size(); ++i)
	{
		MBEdge& e = m_MBEdge[i];
		if (e.m_gid >= 0) edges += e.m_nx;
	}

	// allocate faces
	pm->Create(0,0,0,edges);

	// build the edges
	FEEdge* pe = pm->EdgePtr();
	for (k=0; k<(int) m_MBEdge.size(); ++k)
	{
		MBEdge& e = m_MBEdge[k];
		if (e.m_gid >= 0)
		{
			for (i=0; i<e.m_nx; ++i, ++pe)
			{
				pe->m_gid = e.m_gid;
				pe->SetType(FE_EDGE2);
				pe->n[0] = GetEdgeNodeIndex(e, i);
				pe->n[1] = GetEdgeNodeIndex(e, i+1);
			}
		}
	}
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::BuildMBEdges()
{
	int i, j, n, n1, n2;
	int NF = m_MBFace.size();
	m_MBEdge.clear();
	m_MBEdge.reserve(4*NF);
	int NE = 0;
	for (i=0; i<NF; ++i)
	{
		MBFace& f = m_MBFace[i];
		for (j=0; j<4; ++j)
		{
			n1 = f.m_node[j];
			n2 = f.m_node[(j+1)%4];
			n = FindEdge(n1, n2);

			if (n >= 0)
			{
				MBEdge& e = m_MBEdge[n];
				f.m_edge[j] = n;
				if (f.IsExternal())
				{
					if (e.m_face[0] == -1) e.m_face[0] = i; 
					else e.m_face[1] = i;
				}
			}
			else
			{
				MBEdge e;
				e.m_node[0] = n1;
				e.m_node[1] = n2;
				e.m_face[0] = e.m_face[1] = -1;

				if (f.IsExternal()) e.m_face[0] = i;

				switch (j)
				{
				case 0:
					e.m_nx = f.m_nx;
					e.m_gx = f.m_gx;
					e.m_bx = f.m_bx;
					break;
				case 1:
					e.m_nx = f.m_ny;
					e.m_gx = f.m_gy;
					e.m_bx = f.m_by;
					break;
				case 2:
					e.m_nx = f.m_nx;
					e.m_gx = (f.m_bx?f.m_gx:1/f.m_gx);
					e.m_bx = f.m_bx;
					break;
				case 3:
					e.m_nx = f.m_ny;
					e.m_gx = (f.m_by?f.m_gy:1/f.m_gy);
					e.m_bx = f.m_by;
					break;
				}
				m_MBEdge.push_back(e);
				f.m_edge[j] = NE++;
			}
		}
	}

	NE = m_MBEdge.size();

	// set the external flag of the edges
	for (i=0; i<NE; ++i) m_MBEdge[i].m_bext = false;
	for (i=0; i<NF; ++i)
	{
		MBFace& f = m_MBFace[i];
		if (f.m_bext)
		{
			for (j=0; j<4; ++j) m_MBEdge[ f.m_edge[j] ].m_bext = true;
		}
	}
}

//-----------------------------------------------------------------------------
// This function builds all the faces of the multi-block
void FEMultiBlockMesh::BuildMBFaces()
{
	// clear all faces
	m_MBFace.clear();

	// get the number of blocks
	int NB = m_MBlock.size();

	// reset all block tags
	for (int i=0; i<NB; ++i) m_MBlock[i].m_ntag = 0;

	// count faces
	int faces = 0;
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		for (int j=0; j<6; ++j)
		{
			if (B.m_Nbr[j] == -1) ++faces;
			else 
			{
				MBBlock& B2 = m_MBlock[ B.m_Nbr[j] ];
				if (B2.m_ntag == 0) ++faces;
			}
		}
		B.m_ntag = 1;
	}

	// create face array
	m_MBFace.resize(faces);

	// reset all block tags
	for (int i=0; i<NB; ++i) m_MBlock[i].m_ntag = 0;

	// fill face array
	faces = 0;
	MBFace f;
	int n1, n2;
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		n1 = i;
		for (int j=0; j<6; ++j)
		{
			bool badd = false;
			n2 = -1;
			if (B.m_Nbr[j] == -1) badd = true;
			else 
			{
				MBBlock& B2 = m_MBlock[ B.m_Nbr[j] ];
				if (B2.m_ntag == 0) { badd = true; n2 = B.m_Nbr[j]; }
				else
				{
					for (int k=0; k<6; ++k) 
					{
						if (B2.m_Nbr[k] == i)
						{
							B.m_face[j] = B2.m_face[k];
							break;
						}
					}
				}
			}

			if (badd)
			{
				f = BuildBlockFace(B, j);
				B.m_face[j] = faces;
				f.m_block[0] = n1;
				f.m_block[1] = n2;
				m_MBFace[faces++] = f;
			}
		}
		B.m_ntag = 1;
	}
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::FindFaceNeighbours()
{
	int i, j, k, l;

	int NF = (int) m_MBFace.size();

	// build the node-face table
	vector< vector<int> > NFT;
	BuildNodeFaceTable(NFT);

	// reset all face neighbours
	for (i=0; i<NF; ++i)
	{
		MBFace& F = m_MBFace[i];
		for (j=0; j<4; ++j) F.m_nbr[j] = -1;
	}

	// find the face's neighbours
	int nf, n1, n2;
	for (i=0; i<NF; ++i)
	{
		MBFace& F = m_MBFace[i];
		if (F.IsExternal())
		{
			for (j=0; j<4; ++j)
			{
				if (F.m_nbr[j] == -1)
				{
					// pick a node
					n1 = F.m_node[j];
					n2 = F.m_node[(j+1)%4];
					for (k=0; k<(int) NFT[n1].size(); ++k)
					{
						nf = NFT[n1][k];
						if (nf != i)
						{
							MBFace& F2 = m_MBFace[nf];
							if (F2.IsExternal())
							{
								l = FindEdgeIndex(F2, n1, n2);
								if (l != -1)
								{
									F.m_nbr[j] = nf;
									F2.m_nbr[l] = i;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// This function finds the neighbors for all blocks.
// It is called from UpdateMB().
// At this point it is assumed that the nodes and the blocks are defined.
void FEMultiBlockMesh::FindBlockNeighbours()
{
	// get the number of nodes and blocks
	int NN = m_MBNode.size();
	int NB = m_MBlock.size();

	// build the node-block table
	// This table stores for each ndoe the list of blocks that the node connects to
	vector< vector<int> > NBT;
	BuildNodeBlockTable(NBT);

	// reset all block's neighbours
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		for (int j=0; j<6; ++j) B.m_Nbr[j] = -1;
	}

	// Now we have to find all the block's neighbours.
	// We do this by looping over all the faces for each block
	// and finding the block that has the same face.
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		for (int j=0; j<6; ++j)
		{
			if (B.m_Nbr[j] == -1)
			{
				// get the block's face
				MBFace face = BuildBlockFace(B, j);

				// pick a node on this face
				int inode = face.m_node[0];

				// loop over all the blocks that connect to this node
				for (int k=0; k<(int) NBT[inode].size(); ++k)
				{
					// get the next block index
					int nb = NBT[inode][k];

					// make sure it is not this block
					if (nb != i)
					{
						// get the other block
						MBBlock& B2 = m_MBlock[nb];

						// find the face index of the matching face
						int l = FindFaceIndex(m_MBlock[nb], face);

						// if we find it, assign the neighbors
						if (l != -1)
						{
							B.m_Nbr[j] = nb;
							B2.m_Nbr[l] = i;
							break;
						}
					}
				}
			}
		}
	}

	// set the external flag for the blocks
	// A block is external if it has an open neighbor
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		B.m_bext = false;
		for (int j=0; j<6; ++j)
		{
			if (B.m_Nbr[j] == -1)
			{
				B.m_bext = true;
				break;
			}
		}
	}

	// set the external flag for the nodes
	for (int i=0; i<NN; ++i) m_MBNode[i].m_bext = true;
	for (int i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		if (B.m_bext == false)
		{
			for (int j=0; j<8; ++j) m_MBNode[ B.m_node[j] ].m_bext = false;
		}
	}
}

//-----------------------------------------------------------------------------

MBFace FEMultiBlockMesh::BuildBlockFace(MBBlock& B, int j)
{
	MBFace f;
	int* n = f.m_node;
	int* bn = B.m_node;
	switch (j)
	{
	case 0: 
		n[0] = bn[0]; n[1] = bn[1]; n[2] = bn[5]; n[3] = bn[4]; 
		f.m_nx = B.m_nx; f.m_ny = B.m_nz;
		f.m_gx = B.m_gx; f.m_gy = B.m_gz;
		f.m_bx = B.m_bx; f.m_by = B.m_bz;
		break;
	case 1: 
		n[0] = bn[1]; n[1] = bn[2]; n[2] = bn[6]; n[3] = bn[5]; 
		f.m_nx = B.m_ny; f.m_ny = B.m_nz;
		f.m_gx = B.m_gy; f.m_gy = B.m_gz;
		f.m_bx = B.m_by; f.m_by = B.m_bz;
		break;
	case 2: 
		n[0] = bn[2]; n[1] = bn[3]; n[2] = bn[7]; n[3] = bn[6]; 
		f.m_nx = B.m_nx; f.m_ny = B.m_nz;
		f.m_gx = (B.m_bx?B.m_gx:1/B.m_gx); f.m_gy = B.m_gz;
		f.m_bx = B.m_bx; f.m_by = B.m_bz;
		break;
	case 3: 
		n[0] = bn[3]; n[1] = bn[0]; n[2] = bn[4]; n[3] = bn[7]; 
		f.m_nx = B.m_ny; f.m_ny = B.m_nz;
		f.m_gx = (B.m_by?B.m_gy:1/B.m_gy); f.m_gy = B.m_gz;
		f.m_bx = B.m_by; f.m_by = B.m_bz;
		break;
	case 4: 
		n[0] = bn[3]; n[1] = bn[2]; n[2] = bn[1]; n[3] = bn[0]; 
		f.m_nx = B.m_nx; f.m_ny = B.m_ny;
		f.m_gx = B.m_gx; f.m_gy = (B.m_by?B.m_gy:1/B.m_gy);
		f.m_bx = B.m_bx; f.m_by = B.m_by;
		break;
	case 5: 
		n[0] = bn[4]; n[1] = bn[5]; n[2] = bn[6]; n[3] = bn[7]; 
		f.m_nx = B.m_nx; f.m_ny = B.m_ny;
		f.m_gx = B.m_gx; f.m_gy = B.m_gy;
		f.m_bx = B.m_bx; f.m_by = B.m_by;
		break;
	default:
		assert(false);
	}
	f.m_bext = (B.m_Nbr[j] == -1? true : false);
	return f;
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::FindEdgeIndex(MBFace& F, int n1, int n2)
{
	int m1, m2;
	for (int j=0; j<4; ++j)
	{
		m1 = F.m_node[j];
		m2 = F.m_node[(j+1)%4];
		if (((n1==m1) && (n2==m2))||((n1==m2)&& (n2==m1))) return j;
	}
	return -1;
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::FindFaceIndex(MBBlock &B, MBFace &face)
{
	int i;
	MBFace f2;
	for (i=0; i<6; ++i)
	{
		f2 = BuildBlockFace(B, i);
		if (f2 == face) return i;
	}
	return -1;
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::BuildNodeFaceTable(vector< vector<int> >& NFT)
{
	int i, j, n;

	int NN = m_MBNode.size();
	int NF = m_MBFace.size();

	// calculate node valences
	for (i=0; i<NN; ++i) m_MBNode[i].m_ntag = 0;

	for (i=0; i<NF; ++i)
	{
		MBFace& F = m_MBFace[i];
		for (j=0; j<4; ++j) m_MBNode[ F.m_node[j] ].m_ntag++;
	}

	// create the node-block array
	NFT.resize(NN);
	for (i=0; i<NN; ++i) 
	{
		assert(m_MBNode[i].m_ntag);
		NFT[i].resize( m_MBNode[i].m_ntag );
		m_MBNode[i].m_ntag = 0;
	}

	// fill the node-block array
	for (i=0; i<NF; ++i)
	{
		MBFace& F = m_MBFace[i];
		for (j=0; j<4; ++j)
		{
			n = F.m_node[j];
			MBNode& node = m_MBNode[n];
			NFT[n][node.m_ntag++] = i;
		}
	}
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::BuildNodeBlockTable(vector<vector<int> > &NBT)
{
	int i, j, n;

	int NN = m_MBNode.size();
	int NB = m_MBlock.size();

	// calculate node valences
	for (i=0; i<NN; ++i) m_MBNode[i].m_ntag = 0;

	for (i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		for (j=0; j<8; ++j) m_MBNode[ B.m_node[j] ].m_ntag++;
	}

	// create the node-block array
	NBT.resize(NN);
	for (i=0; i<NN; ++i) 
	{
		assert(m_MBNode[i].m_ntag);
		NBT[i].resize( m_MBNode[i].m_ntag );
		m_MBNode[i].m_ntag = 0;
	}

	// fill the node-block array
	for (i=0; i<NB; ++i)
	{
		MBBlock& B = m_MBlock[i];
		for (j=0; j<8; ++j)
		{
			n = B.m_node[j];
			MBNode& node = m_MBNode[n];
			NBT[n][node.m_ntag++] = i;
		}
	}
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::FindEdge(int n1, int n2)
{
	MBEdge e;
	e.m_node[0] = n1;
	e.m_node[1] = n2;

	int NE = m_MBEdge.size();

	for (int i=0; i<NE; ++i)
	{
		MBEdge& e2 = m_MBEdge[i];
		if (e2 == e) return i;
	}

	return -1;
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::GetBlockNodeIndex(MBBlock &b, int i, int j, int k)
{
	if (i==0) 
	{
		return GetBlockFaceNodeIndex(b, 3, b.m_ny-j, k);
	}
	else if (i==b.m_nx)
	{
		return GetBlockFaceNodeIndex(b, 1, j, k);
	}
	else if (j==0)
	{
		return GetBlockFaceNodeIndex(b, 0, i, k);
	}
	else if (j==b.m_ny)
	{
		return GetBlockFaceNodeIndex(b, 2, b.m_nx-i, k);
	}
	else if (k==0)
	{
		return GetBlockFaceNodeIndex(b, 4, i, b.m_ny-j);
	}
	else if (k==b.m_nz)
	{
		return GetBlockFaceNodeIndex(b, 5, i, j);
	}
	else
	{
		return b.m_ntag + (i-1) + (j-1)*(b.m_nx-1) + (k-1)*(b.m_nx-1)*(b.m_ny-1);
	}
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::GetBlockFaceNodeIndex(MBBlock &b, int nf, int i, int j)
{
	// get the face
	MBFace& f = m_MBFace[b.m_face[nf]];

	// find the transformation
	MBFace bf = BuildBlockFace(b, nf);

	int l = -1;
	if      (bf.m_node[0] == f.m_node[0]) l = 0;
	else if (bf.m_node[0] == f.m_node[1]) l = 1;
	else if (bf.m_node[0] == f.m_node[2]) l = 2;
	else if (bf.m_node[0] == f.m_node[3]) l = 3;

	int m = 0;
	if (bf.m_node[1] == f.m_node[(l+1)%4]) m = 1;
	else if (bf.m_node[1] == f.m_node[(l+3)%4]) m = -1;

	assert((l!=-1) && (m!=0));

	// get the face node index
	switch (l)
	{
	case 0:
		if (m==1) return GetFaceNodeIndex(f, i, j);
		else return GetFaceNodeIndex(f, j, i);
		break;
	case 1:
		if (m==1) return GetFaceNodeIndex(f, j, bf.m_nx-i);
		else return GetFaceNodeIndex(f, bf.m_nx-i, j);
		break;
	case 2:
		if (m==1) return GetFaceNodeIndex(f, bf.m_nx-i, bf.m_ny-j);
		else return GetFaceNodeIndex(f, bf.m_ny-j, bf.m_nx-i);
		break;
	case 3:
		if (m==1) return GetFaceNodeIndex(f, bf.m_ny-j, i);
		else return GetFaceNodeIndex(f, i, bf.m_ny-j);
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::GetFaceNodeIndex(MBFace& f, int i, int j)
{
	if (i==0) 
	{
		return GetFaceEdgeNodeIndex(f, 3, f.m_ny-j);
	}
	else if (i==f.m_nx)
	{
		return GetFaceEdgeNodeIndex(f, 1, j);
	}
	else if (j==0)
	{
		return GetFaceEdgeNodeIndex(f, 0, i);
	}
	else if (j==f.m_ny)
	{
		return GetFaceEdgeNodeIndex(f, 2, f.m_nx-i);
	}
	else
	{
		return f.m_ntag + (i-1) + (j-1)*(f.m_nx-1);
	}
}

//-----------------------------------------------------------------------------


int FEMultiBlockMesh::GetFaceEdgeNodeIndex(MBFace& f, int ne, int i)
{
	// get the edge
	MBEdge& e = m_MBEdge[ f.m_edge[ne] ];

	// next, we need to see if we need to flip the edge or not
	if (e.m_node[0] == f.m_node[ne])
	{
		// don't flip the edge
		return GetEdgeNodeIndex(e, i);
	}
	else if (e.m_node[1] == f.m_node[ne])
	{
		// do flip the edge
		return GetEdgeNodeIndex(e, e.m_nx-i);
	}
	assert(false);
	return -1;
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::SetBlockFaceID(MBBlock& b, int n0, int n1, int n2, int n3, int n4, int n5)
{
	m_MBFace[b.m_face[0]].m_gid = n0;
	m_MBFace[b.m_face[1]].m_gid = n1;
	m_MBFace[b.m_face[2]].m_gid = n2;
	m_MBFace[b.m_face[3]].m_gid = n3;
	m_MBFace[b.m_face[4]].m_gid = n4;
	m_MBFace[b.m_face[5]].m_gid = n5;
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::SetFaceEdgeID(MBFace& f, int n0, int n1, int n2, int n3)
{
	m_MBEdge[f.m_edge[0]].m_gid = n0;
	m_MBEdge[f.m_edge[1]].m_gid = n1;
	m_MBEdge[f.m_edge[2]].m_gid = n2;
	m_MBEdge[f.m_edge[3]].m_gid = n3;
}

//-----------------------------------------------------------------------------

int FEMultiBlockMesh::GetEdgeNodeIndex(MBEdge& e, int i)
{
	if (i==0) 
	{
		return m_MBNode[e.m_node[0]].m_ntag;
	}
	else if (i==e.m_nx)
	{
		return m_MBNode[e.m_node[1]].m_ntag;
	}
	else
	{
		return e.m_ntag + (i-1);
	}
}

//-----------------------------------------------------------------------------

MBFace& FEMultiBlockMesh::GetBlockFace(int nb, int nf)
{
	return m_MBFace[ m_MBlock[nb].m_face[nf] ];
}

//-----------------------------------------------------------------------------

MBEdge& FEMultiBlockMesh::GetFaceEdge(MBFace& f, int n)
{
	return m_MBEdge[ f.m_edge[n] ];
}

//-----------------------------------------------------------------------------

void FEMultiBlockMesh::UpdateMB()
{
	FindBlockNeighbours();
	BuildMBFaces();
	FindFaceNeighbours();
	BuildMBEdges();
}
