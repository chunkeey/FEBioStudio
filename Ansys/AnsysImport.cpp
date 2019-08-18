#include "AnsysImport.h"
#include <GeomLib/GMeshObject.h>

AnsysImport::AnsysImport()
{
	m_pprj = 0;
}

AnsysImport::~AnsysImport(void)
{
}

//-----------------------------------------------------------------------------
bool AnsysImport::Load(FEProject& prj, const char* szfile)
{
	m_pprj = &prj;

	m_szline[0] = 0;

	// open the file
	if (Open(szfile, "rt") == false) return errf("Failed opening file %s", szfile);

	// read the first line
	fgets(m_szline, 255, m_fp);

	// parse the file
	while (!feof(m_fp) && !ferror(m_fp))
	{
		if (strstr(m_szline, "NBLOCK"))
		{
			if (read_NBLOCK() == false) return errf("Error while reading NBLOCK");
		}
		else if (strstr(m_szline, "EBLOCK"))
		{
			if (read_EBLOCK() == false) return errf("Error while reading EBLOCK");
		}
		else fgets(m_szline, 255, m_fp);
	}

	// close the file
	Close();

	// build the mesh
	return BuildMesh(prj.GetFEModel());
}

//-----------------------------------------------------------------------------
bool AnsysImport::read_NBLOCK()
{
	// see if the SOLID field is defined
	bool bSolid = ((strstr(m_szline, "SOLID") != 0) || (strstr(m_szline, "solid") != 0));

	// find the number of nodes
	int nodes = 0;
	char* ch = strrchr(m_szline, ',');
	if (ch) nodes = atoi(ch+1);
	if (nodes <= 0) return false;

	// skip format line
	fgets(m_szline, 255, m_fp);

	// start reading nodes
	float x,y,z;
	for (int i=0; i<nodes; ++i)
	{
		NODE n;
		fgets(m_szline, 255, m_fp);
		x = y = z = 0.f;
		if (bSolid)
			  sscanf(m_szline, "%d%*d%*d%g%g%g", &n.nid, &x, &y, &z);
		else
			sscanf(m_szline, "%d%g%g%g", &n.nid, &x, &y, &z);

		n.r = vec3d(x,y,z);
		m_Node.push_back(n);
	}

	return true;
}

//-----------------------------------------------------------------------------
bool AnsysImport::read_EBLOCK()
{
	// find the number of nodes
	int elems = 0;
	char* ch = strrchr(m_szline, ',');
	if (ch) elems = atoi(ch + 1);
	if (elems <= 0) return false;

	// read format line
	fgets(m_szline, 255, m_fp);
	// process
	ch = m_szline;
	if (*ch != '(') return false;
	ch++;
	char* ch2 = ch;
	while (isdigit(*ch2)) ch2++;
	*ch2 = 0;
	int n0 = atoi(ch); 
	ch = ch2+1; ch2 = ch;
	while (isdigit(*ch2)) ch2++;
	*ch2 = 0;
	int n1 = atoi(ch);

	// start reading elements
	for (int i=0; i<elems; ++i)
	{
		ELEM e;
		int* n = e.n;
		fgets(m_szline, 255, m_fp);
		int np = sscanf(m_szline, "%*d%*d%*d%*d%*d%*d%*d%*d%d%*d%d%d%d%d%d%d%d%d%d", &e.nn, &e.eid, n,n+1,n+2,n+3,n+4,n+5,n+6,n+7);
		if ((e.nn == 4) || (e.nn == 8)) m_Elem.push_back(e);
		if (e.nn == 10)
		{
			fgets(m_szline, 255, m_fp);
			sscanf(m_szline, "%d%d", n+8, n+9);
			m_Elem.push_back(e);
		}
		else if (e.nn == 20)
		{
			fgets(m_szline, 255, m_fp);
			sscanf(m_szline, "%d%d%d%d%d%d%d%d%d%d%d%d", n+8, n+9, n+10, n+11, n+12, n+13, n+14, n+15, n+16, n+17, n+18, n+19);
			m_Elem.push_back(e);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
bool AnsysImport::BuildMesh(FEModel &fem)
{
	int i;

	// calculate the mesh size
	int nodes = m_Node.size();
	int elems = m_Elem.size();

	if (nodes == 0) return errf("FATAL ERROR: No nodal data defined in file.");
	if (elems == 0) return errf("FATAL ERROR: No element data defined in file.");

	// create a new mesh
	FEMesh* pm = new FEMesh();
	pm->Create(nodes, elems);

	// create nodes
	list<NODE>::iterator in = m_Node.begin();
	FENode* pn = pm->NodePtr();
	int imin = 0, imax = 0;
	for (i=0; i<nodes; ++i, ++pn, ++in)
	{
		in->n = i;
		pn->r = in->r;

		if (i==0 || (in->nid < imin)) imin = in->nid;
		if (i==0 || (in->nid > imax)) imax = in->nid;
	}

	// create node lookup table
	int nsize = imax - imin + 1;
	vector<int> NLT;
	NLT.resize(nsize);
	for (i=0; i<nsize; ++i) NLT[i] = -1;
	in = m_Node.begin();
	for (i=0; i<nodes; ++i, ++in) NLT[ in->nid - imin] = i;

	// get pointer to elements
	FEElement* pe = pm->ElementPtr();

	in = m_Node.begin();
	list<ELEM>::iterator ih = m_Elem.begin();
	for (i=0; i<elems; ++i, ++pe, ++ih)
	{
		pe->m_gid = 0;
		ih->tag = i;
		int* n = ih->n;
		if (ih->nn == 8)
		{
			if ((n[7] == n[6]) && (n[7]==n[5]) && (n[7]==n[4]) && (n[3]==n[2]))
			{
				pe->SetType(FE_TET4);
				pe->m_node[0] = NLT[ n[0] - imin]; if (pe->m_node[0] < 0) return false;
				pe->m_node[1] = NLT[ n[1] - imin]; if (pe->m_node[1] < 0) return false;
				pe->m_node[2] = NLT[ n[2] - imin]; if (pe->m_node[2] < 0) return false;
				pe->m_node[3] = NLT[ n[4] - imin]; if (pe->m_node[3] < 0) return false;
			}
			else if ((n[7] == n[6]) && (n[7] == n[5]) && (n[7] == n[4]))
			{
				pe->SetType(FE_PYRA5);
				pe->m_node[0] = NLT[n[0] - imin]; if (pe->m_node[0] < 0) return false;
				pe->m_node[1] = NLT[n[1] - imin]; if (pe->m_node[1] < 0) return false;
				pe->m_node[2] = NLT[n[2] - imin]; if (pe->m_node[2] < 0) return false;
				pe->m_node[3] = NLT[n[3] - imin]; if (pe->m_node[3] < 0) return false;
				pe->m_node[4] = NLT[n[4] - imin]; if (pe->m_node[4] < 0) return false;
			}
			else if ((n[7] == n[6]) && (n[7] == n[5]))
			{
				pe->SetType(FE_PENTA6);
				pe->m_node[0] = NLT[ n[0] - imin]; if (pe->m_node[0] < 0) return false;
				pe->m_node[1] = NLT[ n[1] - imin]; if (pe->m_node[1] < 0) return false;
				pe->m_node[2] = NLT[ n[2] - imin]; if (pe->m_node[2] < 0) return false;
				pe->m_node[3] = NLT[ n[3] - imin]; if (pe->m_node[3] < 0) return false;
				pe->m_node[4] = NLT[ n[4] - imin]; if (pe->m_node[4] < 0) return false;
				pe->m_node[5] = NLT[ n[5] - imin]; if (pe->m_node[5] < 0) return false;
			}
			else if ((n[2]==n[3]) && (n[7]==n[6]))
			{
				pe->SetType(FE_PENTA6);
				pe->m_node[0] = NLT[n[0] - imin]; if (pe->m_node[0] < 0) return false;
				pe->m_node[1] = NLT[n[1] - imin]; if (pe->m_node[1] < 0) return false;
				pe->m_node[2] = NLT[n[2] - imin]; if (pe->m_node[2] < 0) return false;
				pe->m_node[3] = NLT[n[4] - imin]; if (pe->m_node[3] < 0) return false;
				pe->m_node[4] = NLT[n[5] - imin]; if (pe->m_node[4] < 0) return false;
				pe->m_node[5] = NLT[n[6] - imin]; if (pe->m_node[5] < 0) return false;
			}
			else 
			{
				pe->SetType(FE_HEX8);
				pe->m_node[0] = NLT[ n[0] - imin]; if (pe->m_node[0] < 0) return false;
				pe->m_node[1] = NLT[ n[1] - imin]; if (pe->m_node[1] < 0) return false;
				pe->m_node[2] = NLT[ n[2] - imin]; if (pe->m_node[2] < 0) return false;
				pe->m_node[3] = NLT[ n[3] - imin]; if (pe->m_node[3] < 0) return false;
				pe->m_node[4] = NLT[ n[4] - imin]; if (pe->m_node[4] < 0) return false;
				pe->m_node[5] = NLT[ n[5] - imin]; if (pe->m_node[5] < 0) return false;
				pe->m_node[6] = NLT[ n[6] - imin]; if (pe->m_node[6] < 0) return false;
				pe->m_node[7] = NLT[ n[7] - imin]; if (pe->m_node[7] < 0) return false;
			}
		}
		else if (ih->nn == 4)
		{
			if (n[3] == n[2]) pe->SetType(FE_TRI3); else pe->SetType(FE_QUAD4);
			pe->m_node[0] = NLT[ n[0] - imin]; if (pe->m_node[0] < 0) return false;
			pe->m_node[1] = NLT[ n[1] - imin]; if (pe->m_node[1] < 0) return false;
			pe->m_node[2] = NLT[ n[2] - imin]; if (pe->m_node[2] < 0) return false;
			pe->m_node[3] = NLT[ n[3] - imin]; if (pe->m_node[3] < 0) return false;

			pe->m_h[0] = 0;
			pe->m_h[1] = 0;
			pe->m_h[2] = 0;
			pe->m_h[3] = 0;
		}
		else if (ih->nn == 10)
		{
			pe->SetType(FE_TET10);
			pe->m_node[0] = NLT[n[0] - imin]; if (pe->m_node[0] < 0) return false;
			pe->m_node[1] = NLT[n[1] - imin]; if (pe->m_node[1] < 0) return false;
			pe->m_node[2] = NLT[n[2] - imin]; if (pe->m_node[2] < 0) return false;
			pe->m_node[3] = NLT[n[3] - imin]; if (pe->m_node[3] < 0) return false;
			pe->m_node[4] = NLT[n[4] - imin]; if (pe->m_node[4] < 0) return false;
			pe->m_node[5] = NLT[n[5] - imin]; if (pe->m_node[5] < 0) return false;
			pe->m_node[6] = NLT[n[6] - imin]; if (pe->m_node[6] < 0) return false;
			pe->m_node[7] = NLT[n[7] - imin]; if (pe->m_node[7] < 0) return false;
			pe->m_node[8] = NLT[n[8] - imin]; if (pe->m_node[8] < 0) return false;
			pe->m_node[9] = NLT[n[9] - imin]; if (pe->m_node[9] < 0) return false;
		}
		else if (ih->nn == 20)
		{
			pe->SetType(FE_HEX20);
			pe->m_node[0] = NLT[n[0] - imin]; if (pe->m_node[0] < 0) return false;
			pe->m_node[1] = NLT[n[1] - imin]; if (pe->m_node[1] < 0) return false;
			pe->m_node[2] = NLT[n[2] - imin]; if (pe->m_node[2] < 0) return false;
			pe->m_node[3] = NLT[n[3] - imin]; if (pe->m_node[3] < 0) return false;
			pe->m_node[4] = NLT[n[4] - imin]; if (pe->m_node[4] < 0) return false;
			pe->m_node[5] = NLT[n[5] - imin]; if (pe->m_node[5] < 0) return false;
			pe->m_node[6] = NLT[n[6] - imin]; if (pe->m_node[6] < 0) return false;
			pe->m_node[7] = NLT[n[7] - imin]; if (pe->m_node[7] < 0) return false;
			pe->m_node[8] = NLT[n[8] - imin]; if (pe->m_node[8] < 0) return false;
			pe->m_node[9] = NLT[n[9] - imin]; if (pe->m_node[9] < 0) return false;
			pe->m_node[10] = NLT[n[10] - imin]; if (pe->m_node[10] < 0) return false;
			pe->m_node[11] = NLT[n[11] - imin]; if (pe->m_node[11] < 0) return false;
			pe->m_node[12] = NLT[n[12] - imin]; if (pe->m_node[12] < 0) return false;
			pe->m_node[13] = NLT[n[13] - imin]; if (pe->m_node[13] < 0) return false;
			pe->m_node[14] = NLT[n[14] - imin]; if (pe->m_node[14] < 0) return false;
			pe->m_node[15] = NLT[n[15] - imin]; if (pe->m_node[15] < 0) return false;
			pe->m_node[16] = NLT[n[16] - imin]; if (pe->m_node[16] < 0) return false;
			pe->m_node[17] = NLT[n[17] - imin]; if (pe->m_node[17] < 0) return false;
			pe->m_node[18] = NLT[n[18] - imin]; if (pe->m_node[18] < 0) return false;
			pe->m_node[19] = NLT[n[19] - imin]; if (pe->m_node[19] < 0) return false;
		}
	}

	// update the mesh
	pm->RemoveIsolatedNodes();
	pm->RebuildMesh();
	GObject* po = new GMeshObject(pm);

	// clean up
	m_Node.clear();
	m_Elem.clear();

	// if we get here we are good to go!
	char szname[256];
	FileTitle(szname);
	po->SetName(szname);
	fem.GetModel().AddObject(po);

	// we're good!
	return true;
}