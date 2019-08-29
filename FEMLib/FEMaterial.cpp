// FEMaterial.cpp: implementation of the FEMaterial class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FEMaterial.h"
#include <MeshTools/FEProject.h>

//////////////////////////////////////////////////////////////////////
// FEIsotropicElastic  - isotropic elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEIsotropicElastic, MODULE_MECH, FE_ISOTROPIC_ELASTIC, FE_MAT_ELASTIC, "isotropic elastic", MaterialFlags::TOPLEVEL);

FEIsotropicElastic::FEIsotropicElastic() : FEMaterial(FE_ISOTROPIC_ELASTIC)
{
	AddScienceParam(1, Param_DENSITY, "density", "density"        );
	AddScienceParam(0, Param_STRESS ,       "E", "Young's modulus");
	AddScienceParam(0, Param_NONE   ,       "v", "Poisson's ratio");
}

//////////////////////////////////////////////////////////////////////
// FEOrthoElastic - orthotropic elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEOrthoElastic, MODULE_MECH, FE_ORTHO_ELASTIC, FE_MAT_ELASTIC, "orthotropic elastic", MaterialFlags::TOPLEVEL);

FEOrthoElastic::FEOrthoElastic() : FEMaterial(FE_ORTHO_ELASTIC)
{
	AddScienceParam(1, Param_DENSITY, "density", "density"    );
	AddScienceParam(0, Param_STRESS ,       "E1", "E1 modulus");
	AddScienceParam(0, Param_STRESS ,       "E2", "E2 modulus");
	AddScienceParam(0, Param_STRESS ,       "E3", "E3 modulus");
	AddScienceParam(0, Param_STRESS ,       "G12", "G12 shear modulus");
	AddScienceParam(0, Param_STRESS ,       "G23", "G23 shear modulus");
	AddScienceParam(0, Param_STRESS ,       "G31", "G31 shear modulus");
	AddScienceParam(0, Param_NONE   ,       "v12", "Poisson's ratio v12");
	AddScienceParam(0, Param_NONE   ,       "v23", "Poisson's ratio v23");
	AddScienceParam(0, Param_NONE   ,       "v31", "Poisson's ratio v31");
}

//////////////////////////////////////////////////////////////////////
// FENeoHookean - neo-hookean elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FENeoHookean, MODULE_MECH, FE_NEO_HOOKEAN, FE_MAT_ELASTIC, "neo-Hookean", MaterialFlags::TOPLEVEL);

FENeoHookean::FENeoHookean() : FEMaterial(FE_NEO_HOOKEAN)
{
	AddScienceParam(1, Param_DENSITY, "density", "density"        )->MakeVariable(true);
	AddScienceParam(0, Param_STRESS ,       "E", "Young's modulus")->MakeVariable(true);
	AddScienceParam(0, Param_NONE   ,       "v", "Poisson's ratio")->MakeVariable(true);
}

//////////////////////////////////////////////////////////////////////
// FEIncompNeoHookean - incompressible neo-hookean elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEIncompNeoHookean, MODULE_MECH, FE_INCOMP_NEO_HOOKEAN, FE_MAT_ELASTIC_UNCOUPLED, "incomp neo-Hookean", MaterialFlags::TOPLEVEL);

FEIncompNeoHookean::FEIncompNeoHookean() : FEMaterial(FE_INCOMP_NEO_HOOKEAN)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS, "G", "Shear modulus");
	AddScienceParam(0, Param_STRESS, "k", "Bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FEPorousNeoHookean - porous neo-hookean elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPorousNeoHookean, MODULE_MECH, FE_POROUS_NEO_HOOKEAN, FE_MAT_ELASTIC, "porous neo-Hookean", MaterialFlags::TOPLEVEL);

FEPorousNeoHookean::FEPorousNeoHookean() : FEMaterial(FE_POROUS_NEO_HOOKEAN)
{
    AddScienceParam(1, Param_DENSITY, "density", "density"        );
    AddScienceParam(0, Param_STRESS ,       "E", "Young's modulus");
    AddScienceParam(0, Param_NONE   ,    "phi0", "solid volume fraction");
}

//////////////////////////////////////////////////////////////////////
// FEGasserOgdenHolzapfelUC - Gasser-Ogden-Holzapfel uncoupled
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEGasserOgdenHolzapfelUC, MODULE_MECH, FE_HOLZAPFEL_UC, FE_MAT_ELASTIC_UNCOUPLED, "Gasser-Ogden-Holzapfel-uncoupled", MaterialFlags::TOPLEVEL);

FEGasserOgdenHolzapfelUC::FEGasserOgdenHolzapfelUC() : FEMaterial(FE_HOLZAPFEL_UC)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddDoubleParam(0, "c", "c");
	AddDoubleParam(0, "k1", "k1");
	AddDoubleParam(0, "k2", "k2");
	AddDoubleParam(0, "kappa", "kappa");
	AddDoubleParam(0, "gamma", "gamma");
	AddScienceParam(0, Param_STRESS, "k", "Bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FEMooneyRivlin - Mooney-Rivlin rubber
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEMooneyRivlin, MODULE_MECH, FE_MOONEY_RIVLIN, FE_MAT_ELASTIC_UNCOUPLED, "Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FEMooneyRivlin::FEMooneyRivlin() : FEMaterial(FE_MOONEY_RIVLIN)
{
	AddScienceParam(1, Param_DENSITY, "density", "density"     );
	AddScienceParam(0, Param_STRESS , "c1"     , "c1"          );
	AddScienceParam(0, Param_STRESS , "c2"     , "c2"          );
	AddScienceParam(0, Param_STRESS , "k"      , "bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FEVerondaWestmann - Veronda-Westmann elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEVerondaWestmann, MODULE_MECH, FE_VERONDA_WESTMANN, FE_MAT_ELASTIC_UNCOUPLED, "Veronda-Westmann", MaterialFlags::TOPLEVEL);

FEVerondaWestmann::FEVerondaWestmann() : FEMaterial(FE_VERONDA_WESTMANN)
{
	AddScienceParam(1, Param_DENSITY, "density", "density"     );
	AddScienceParam(0, Param_STRESS , "c1"     , "c1"          );
	AddScienceParam(0, Param_NONE   , "c2"     , "c2"          );
	AddScienceParam(0, Param_STRESS , "k"      , "bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FEHolmesMow -Holmes-Mow elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEHolmesMow, MODULE_MECH, FE_HOLMES_MOW, FE_MAT_ELASTIC, "Holmes-Mow", MaterialFlags::TOPLEVEL);

FEHolmesMow::FEHolmesMow() : FEMaterial(FE_HOLMES_MOW)
{
	AddScienceParam(1, Param_DENSITY, "density", "Material density");
	AddScienceParam(0, Param_STRESS , "E", "Young's modulus");
	AddScienceParam(0, Param_NONE   , "v", "Poisson's ratio");
	AddScienceParam(0, Param_NONE   , "beta", "beta"        );
}

//////////////////////////////////////////////////////////////////////
// FEArrudaBoyce - Arruda-Boyce elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEArrudaBoyce, MODULE_MECH, FE_ARRUDA_BOYCE, FE_MAT_ELASTIC_UNCOUPLED, "Arruda-Boyce", MaterialFlags::TOPLEVEL);

FEArrudaBoyce::FEArrudaBoyce() : FEMaterial(FE_ARRUDA_BOYCE)
{
	AddScienceParam(1, Param_DENSITY, "density", "Material density");
	AddScienceParam(0, Param_STRESS , "mu", "Initial modulus");
	AddScienceParam(0, Param_NONE   , "N", "links");
	AddScienceParam(0, Param_STRESS , "k", "Bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FECarterHayes - Carter-Hayes elasticity
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FECarterHayes, MODULE_MECH, FE_CARTER_HAYES, FE_MAT_ELASTIC, "Carter-Hayes", MaterialFlags::TOPLEVEL);

FECarterHayes::FECarterHayes() : FEMaterial(FE_CARTER_HAYES)
{
	AddScienceParam(1, Param_DENSITY, "density", "true density");
	AddScienceParam(0, Param_STRESS , "E0", "E0");
	AddScienceParam(1, Param_DENSITY, "rho0", "rho0");
	AddScienceParam(0, Param_NONE   , "gamma", "gamma");
	AddScienceParam(0, Param_NONE   , "v", "Poisson's ratio");
	AddIntParam    (-1, "sbm", "sbm");
}

//////////////////////////////////////////////////////////////////////
// FEPRLig - Poission-Ratio Ligament
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPRLig, MODULE_MECH, FE_PRLIG, FE_MAT_ELASTIC_UNCOUPLED, "PRLig", MaterialFlags::TOPLEVEL);

FEPRLig::FEPRLig() : FEMaterial(FE_PRLIG)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_NONE   , "c1"     , "c1");
	AddScienceParam(1, Param_NONE   , "c2"     , "c2");
	AddScienceParam(0, Param_NONE   , "v0"     , "v0");
	AddScienceParam(0, Param_NONE   , "m"      , "m" );
	AddScienceParam(0, Param_NONE   , "mu"     , "mu");
	AddScienceParam(0, Param_NONE   , "k"      , "k" );
}

//////////////////////////////////////////////////////////////////////
// FEFiberMaterial - material for fibers
//////////////////////////////////////////////////////////////////////

FEFiberMaterial::FEFiberMaterial()
{
	m_naopt = FE_FIBER_LOCAL;
	m_nuser = 0;
	m_n[0] = m_n[1] = 0;
	m_r = vec3d(0,0,0);
	m_a = vec3d(0,0,1);
	m_d = vec3d(1,0,0);
	m_theta = 0.0;
	m_phi = 90.0;
	m_d0 = m_d1 = vec3d(0,0,1);
	m_R0 = 0; m_R1 = 1;
}

void FEFiberMaterial::copy(FEFiberMaterial* pm)
{
	m_naopt = pm->m_naopt;
	m_nuser = pm->m_nuser;
	m_n[0] = pm->m_n[0];
	m_n[1] = pm->m_n[1];
	m_r = pm->m_r;
	m_a = pm->m_a;
	m_d = pm->m_d;
	m_theta = pm->m_theta;
	m_phi = pm->m_phi;
	m_d0 = pm->m_d0;
	m_d1 = pm->m_d1;
	m_R0 = pm->m_R0;
	m_R1 = pm->m_R1;

//	GetParamBlock() = pm->GetParamBlock();
}

void FEFiberMaterial::Save(OArchive &ar)
{
	ar.WriteChunk(MP_AOPT, m_naopt);
	ar.WriteChunk(MP_N, m_n, 2);
	ar.WriteChunk(MP_R, m_r);
	ar.WriteChunk(MP_A, m_a);
	ar.WriteChunk(MP_D, m_d);
	ar.WriteChunk(MP_NUSER, m_nuser);
	ar.WriteChunk(MP_THETA, m_theta);
	ar.WriteChunk(MP_PHI, m_phi);
	ar.WriteChunk(MP_D0, m_d0);
	ar.WriteChunk(MP_D1, m_d1);
	ar.WriteChunk(MP_R0, m_R0);
	ar.WriteChunk(MP_R1, m_R1);
	ar.BeginChunk(MP_PARAMS);
	{
		ParamContainer::Save(ar);
	}
	ar.EndChunk();
}

void FEFiberMaterial::Load(IArchive& ar)
{
	TRACE("FEFiberMaterial::Load");

	while (IArchive::IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case MP_AOPT: ar.read(m_naopt); break;
		case MP_N: ar.read(m_n, 2); break;
		case MP_R: ar.read(m_r); break;
		case MP_A: ar.read(m_a); break;
		case MP_D: ar.read(m_d); break;
		case MP_NUSER: ar.read(m_nuser); break;
		case MP_THETA: ar.read(m_theta); break;
		case MP_PHI: ar.read(m_phi); break;
		case MP_D0: ar.read(m_d0); break;
		case MP_D1: ar.read(m_d1); break;
		case MP_R0: ar.read(m_R0); break;
		case MP_R1: ar.read(m_R1); break;
		case MP_PARAMS: 
			ParamContainer::Load(ar);
			break;
		}
		ar.CloseChunk();
	}
}

//////////////////////////////////////////////////////////////////////
// FETransverselyIsotropic - base class for transversely isotropic
//////////////////////////////////////////////////////////////////////

FETransverselyIsotropic::FETransverselyIsotropic(int ntype) : FEMaterial(ntype)
{
	m_pfiber = 0;
}

FEFiberMaterial* FETransverselyIsotropic::GetFiberMaterial()
{
	return m_pfiber;
}

void FETransverselyIsotropic::SetFiberMaterial(FEFiberMaterial* fiber)
{
	m_pfiber = fiber;
}

vec3d FETransverselyIsotropic::GetFiber(FEElementRef& el)
{
	int naopt = m_pfiber->m_naopt;
	FEFiberMaterial& fiber = *m_pfiber;
	switch (naopt)
	{
	case FE_FIBER_LOCAL:
		{
			FECoreMesh* pm = el.m_pmesh;
			int n[2] = {m_pfiber->m_n[0], m_pfiber->m_n[1]};
			if ((n[0]==0)&&(n[1]==0)) { n[0] = 1; n[1] = 2; }
			vec3d a = pm->Node(el->m_node[ n[0]-1 ]).r;
			vec3d b = pm->Node(el->m_node[ n[1]-1 ]).r;

			b -= a;
			b.Normalize();

			return b;
		}
		break;
	case FE_FIBER_CYLINDRICAL:
		{
			// we'll use the element center as the reference point
			FECoreMesh* pm = el.m_pmesh;
			int n = el->Nodes();
			vec3d c(0,0,0);
			for (int i=0; i<n; ++i) c += pm->NodePosition(el->m_node[i]);
			c /= (double) n;

			// find the vector to the axis
			vec3d r = m_pfiber->m_r;
			vec3d a = m_pfiber->m_a;
			vec3d v = m_pfiber->m_d;
			vec3d b = (c - r) - a*(a*(c - r)); b.Normalize();

			// setup the rotation vector
			vec3d x_unit(vec3d(1,0,0));
			quatd q(x_unit, b);

			// rotate the reference vector
			v.Normalize();
			q.RotateVector(v);

			return v;
		}
		break;
	case FE_FIBER_SPHERICAL:
		{
			FECoreMesh* pm = el.m_pmesh;
			int n = el->Nodes();
			vec3d c(0,0,0);
			for (int i = 0; i<n; ++i) c += pm->NodePosition(el->m_node[i]);
			c /= (double) n;
			c -= m_pfiber->m_r;
			c.Normalize();

			// setup the rotation vector
			vec3d x_unit(vec3d(1,0,0));
			quatd q(x_unit, c);

			vec3d v = m_pfiber->m_d;
			v.Normalize();
			q.RotateVector(v);

			return v;
		}
		break;
	case FE_FIBER_VECTOR:
		{
			return m_pfiber->m_a;
		}
		break;
	case FE_FIBER_USER:
		{
			return el->m_fiber;
		}
		break;
	case FE_FIBER_ANGLES:
		{
			// convert from degress to radians
			const double pi = 4*atan(1.0);
			const double the = fiber.m_theta*pi/180.;
			const double phi = fiber.m_phi*pi/180.;

			// define the first axis (i.e. the fiber vector)
			vec3d a;
			a.x = cos(the)*sin(phi);
			a.y = sin(the)*sin(phi);
			a.z = cos(phi);
			return a;
		}
		break;
	case FE_FIBER_POLAR:
		{
			// we'll use the element center as the reference point
			FECoreMesh* pm = el.m_pmesh;
			int n = el->Nodes();
			vec3d c(0,0,0);
			for (int i = 0; i<n; ++i) c += pm->NodePosition(el->m_node[i]);
			c /= (double) n;

			// find the vector to the axis
			vec3d r = m_pfiber->m_r;
			vec3d a = m_pfiber->m_a;

			vec3d b = (c - r) - a*(a*(c - r));
			double R = b.Length(); b.Normalize();

			double R0 = fiber.m_R0;
			double R1 = fiber.m_R1;
			if (R1 == R0) R1 += 1;
			double w = (R - R0)/(R1 - R0);
			
			vec3d v0 = m_pfiber->m_d0; v0.Normalize();
			vec3d v1 = m_pfiber->m_d1; v1.Normalize();

			quatd Q0(0,vec3d(0,0,1)), Q1(v0,v1);
			quatd Q = quatd::slerp(Q0, Q1, w);
			vec3d v = v0; Q.RotateVector(v);

			// setup the rotation vector
			vec3d x_unit(vec3d(1,0,0));
			quatd q(x_unit, b);

			// rotate the reference vector
			v.Normalize();
			q.RotateVector(v);

			return v;
		}
		break;
	}

	assert(false);
	return vec3d(0,0,0);
}

void FETransverselyIsotropic::copy(FEMaterial *pmat)
{
	FETransverselyIsotropic& m = dynamic_cast<FETransverselyIsotropic&>(*pmat);
	assert(m.Type() == m_ntype);

	FEMaterial::copy(pmat);
	m_pfiber->copy(m.m_pfiber);
}

void FETransverselyIsotropic::Save(OArchive &ar)
{
	ar.BeginChunk(MP_MAT);
	{
		FEMaterial::Save(ar);
	}
	ar.EndChunk();

	ar.BeginChunk(MP_FIBERS);
	{
		m_pfiber->Save(ar);
	}
	ar.EndChunk();
}

void FETransverselyIsotropic::Load(IArchive &ar)
{
	TRACE("FETransverselyIsotropic::Load");

	while (IArchive::IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case MP_MAT: FEMaterial::Load(ar); break;
		case MP_FIBERS: m_pfiber->Load(ar); break;
		default:
			throw ReadError("unknown CID in FETransverselyIsotropic::Load");
		}

		ar.CloseChunk();
	}
}

//////////////////////////////////////////////////////////////////////
// FETransMooneyRivlinOld - transversely isotropic mooney-rivlin (obsolete implementation)
//////////////////////////////////////////////////////////////////////

//REGISTER_MATERIAL(FETransMooneyRivlinOld, MODULE_MECH, FE_TRANS_MOONEY_RIVLIN_OLD, FE_MAT_ELASTIC_UNCOUPLED, "trans iso Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FETransMooneyRivlinOld::Fiber::Fiber()
{
	AddScienceParam(0, Param_STRESS, "c3", "c3");
	AddScienceParam(0, Param_NONE  , "c4", "c4");
	AddScienceParam(0, Param_STRESS, "c5", "c5");
	AddScienceParam(0, Param_NONE  , "lam_max", "lam_max");
	AddDoubleParam(0, "ca0" , "ca0" )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "beta", "beta")->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "L0"  , "L0"  )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "Lr"  , "Lr"  )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "active_contraction", "active_contraction")->SetState(Param_State::Param_READWRITE)->SetLoadCurve();
}

FETransMooneyRivlinOld::FETransMooneyRivlinOld() : FETransverselyIsotropic(FE_TRANS_MOONEY_RIVLIN_OLD)
{
	// define the fiber class
	SetFiberMaterial(new Fiber);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "c1", "c1");
	AddScienceParam(0, Param_STRESS , "c2", "c2");
	AddScienceParam(0, Param_STRESS , "k" , "bulk modulus");
}

//////////////////////////////////////////////////////////////////////
// FETransVerondaWestmann - transversely isotropic veronda-westmann (obsolete)
//////////////////////////////////////////////////////////////////////

//REGISTER_MATERIAL(FETransVerondaWestmannOld, MODULE_MECH, FE_TRANS_VERONDA_WESTMANN_OLD, FE_MAT_ELASTIC_UNCOUPLED, "trans iso Veronda-Westmann", MaterialFlags::TOPLEVEL);

FETransVerondaWestmannOld::Fiber::Fiber()
{
	AddScienceParam(0, Param_STRESS, "c3", "c3");
	AddScienceParam(0, Param_NONE  , "c4", "c4");
	AddScienceParam(0, Param_STRESS, "c5", "c5");
	AddScienceParam(0, Param_NONE  , "lam_max", "lam_max");
	AddDoubleParam(0, "ca0" , "ca0" )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "beta", "beta")->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "L0"  , "L0"  )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "Lr"  , "Lr"  )->SetState(Param_State::Param_READWRITE);
	AddDoubleParam(0, "active_contraction", "active_contraction")->SetState(Param_State::Param_READWRITE)->SetLoadCurve();
}

FETransVerondaWestmannOld::FETransVerondaWestmannOld() : FETransverselyIsotropic(FE_TRANS_VERONDA_WESTMANN_OLD)
{
	// define the fiber class
	SetFiberMaterial(new Fiber);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "c1", "c1");
	AddScienceParam(0, Param_NONE   , "c2", "c2");
	AddScienceParam(0, Param_STRESS , "k" , "bulk modulus");
}


//////////////////////////////////////////////////////////////////////
// FEActiveContraction - Active contraction material
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEActiveContraction, MODULE_MECH, FE_MAT_ACTIVE_CONTRACTION, FE_MAT_ACTIVE_CONTRACTION_CLASS, "active_contraction", 0);

FEActiveContraction::FEActiveContraction() : FEMaterial(FE_MAT_ACTIVE_CONTRACTION)
{
	AddDoubleParam(0, "ascl", "scale")->SetLoadCurve();
	AddDoubleParam(0, "ca0");
	AddDoubleParam(0, "beta");
	AddDoubleParam(0, "l0");
	AddDoubleParam(0, "refl");
}

//////////////////////////////////////////////////////////////////////
// FETransMooneyRivlin - transversely isotropic mooney-rivlin
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FETransMooneyRivlin, MODULE_MECH, FE_TRANS_ISO_MOONEY_RIVLIN, FE_MAT_ELASTIC_UNCOUPLED, "trans iso Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FETransMooneyRivlin::FETransMooneyRivlin() : FETransverselyIsotropic(FE_TRANS_ISO_MOONEY_RIVLIN)
{
	SetFiberMaterial(new FEFiberMaterial);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS, "c1", "c1");
	AddScienceParam(0, Param_STRESS, "c2", "c2");
	AddScienceParam(0, Param_STRESS, "c3", "c3");
	AddScienceParam(0, Param_STRESS, "c4", "c4");
	AddScienceParam(0, Param_STRESS, "c5", "c5");
	AddScienceParam(0, Param_NOUNIT, "lam_max", "lam_max");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");

	AddProperty("active_contraction", FE_MAT_ACTIVE_CONTRACTION_CLASS);
}

void FETransMooneyRivlin::Convert(FETransMooneyRivlinOld* pold)
{
	if (pold == 0) return;

	FETransMooneyRivlinOld::Fiber* oldFiber = dynamic_cast<FETransMooneyRivlinOld::Fiber*>(pold->GetFiberMaterial());

	SetFloatValue(MP_DENSITY, pold->GetFloatValue(FETransMooneyRivlinOld::MP_DENSITY));
	SetFloatValue(MP_C1     , pold->GetFloatValue(FETransMooneyRivlinOld::MP_C1));
	SetFloatValue(MP_C2     , pold->GetFloatValue(FETransMooneyRivlinOld::MP_C2));
	SetFloatValue(MP_K      , pold->GetFloatValue(FETransMooneyRivlinOld::MP_K));
	SetFloatValue(MP_C3     , oldFiber->GetFloatValue(FETransMooneyRivlinOld::Fiber::MP_C3));
	SetFloatValue(MP_C4     , oldFiber->GetFloatValue(FETransMooneyRivlinOld::Fiber::MP_C4));
	SetFloatValue(MP_C5     , oldFiber->GetFloatValue(FETransMooneyRivlinOld::Fiber::MP_C5));
	SetFloatValue(MP_LAM    , oldFiber->GetFloatValue(FETransMooneyRivlinOld::Fiber::MP_LAM));

	GetFiberMaterial()->copy(oldFiber);
}

//////////////////////////////////////////////////////////////////////
// FETransVerondaWestmann - transversely isotropic Veronda-Westmann
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FETransVerondaWestmann, MODULE_MECH, FE_TRANS_ISO_VERONDA_WESTMANN, FE_MAT_ELASTIC_UNCOUPLED, "trans iso Veronda-Westmann", MaterialFlags::TOPLEVEL);

FETransVerondaWestmann::FETransVerondaWestmann() : FETransverselyIsotropic(FE_TRANS_ISO_VERONDA_WESTMANN)
{
	SetFiberMaterial(new FEFiberMaterial);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS, "c1", "c1");
	AddScienceParam(0, Param_STRESS, "c2", "c2");
	AddScienceParam(0, Param_STRESS, "c3", "c3");
	AddScienceParam(0, Param_STRESS, "c4", "c4");
	AddScienceParam(0, Param_STRESS, "c5", "c5");
	AddScienceParam(0, Param_NOUNIT, "lam_max", "lam_max");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");

	AddProperty("active_contraction", FE_MAT_ACTIVE_CONTRACTION_CLASS);
}

void FETransVerondaWestmann::Convert(FETransVerondaWestmannOld* pold)
{
	if (pold == 0) return;

	FETransVerondaWestmannOld::Fiber* oldFiber = dynamic_cast<FETransVerondaWestmannOld::Fiber*>(pold->GetFiberMaterial());

	SetFloatValue(MP_DENSITY, pold->GetFloatValue(FETransVerondaWestmannOld::MP_DENSITY));
	SetFloatValue(MP_C1     , pold->GetFloatValue(FETransVerondaWestmannOld::MP_C1));
	SetFloatValue(MP_C2     , pold->GetFloatValue(FETransVerondaWestmannOld::MP_C2));
	SetFloatValue(MP_K      , pold->GetFloatValue(FETransVerondaWestmannOld::MP_K));
	SetFloatValue(MP_C3     , oldFiber->GetFloatValue(FETransVerondaWestmannOld::Fiber::MP_C3));
	SetFloatValue(MP_C4     , oldFiber->GetFloatValue(FETransVerondaWestmannOld::Fiber::MP_C4));
	SetFloatValue(MP_C5     , oldFiber->GetFloatValue(FETransVerondaWestmannOld::Fiber::MP_C5));
	SetFloatValue(MP_LAM    , oldFiber->GetFloatValue(FETransVerondaWestmannOld::Fiber::MP_LAM));

	GetFiberMaterial()->copy(oldFiber);
}

//=============================================================================
// Coupled Trans-iso Mooney-Rivlin (Obsolete implementation)
//=============================================================================

//REGISTER_MATERIAL(FECoupledTransIsoMooneyRivlinOld, MODULE_MECH, FE_COUPLED_TRANS_ISO_MR, FE_MAT_ELASTIC, "coupled trans-iso Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FECoupledTransIsoMooneyRivlinOld::FECoupledTransIsoMooneyRivlinOld() : FEMaterial(FE_COUPLED_TRANS_ISO_MR)
{
	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "c1", "c1");
	AddScienceParam(0, Param_STRESS , "c2", "c2");
	AddScienceParam(0, Param_STRESS , "c3", "c3");
	AddScienceParam(0, Param_NONE   , "c4", "c4");
	AddScienceParam(0, Param_STRESS , "c5", "c5");
	AddScienceParam(0, Param_STRESS , "k" , "bulk modulus");
	AddScienceParam(0, Param_NONE   , "lambda", "lambda");
}

//=============================================================================
// Coupled Trans-iso Mooney-Rivlin
//=============================================================================

REGISTER_MATERIAL(FECoupledTransIsoMooneyRivlin, MODULE_MECH, FE_COUPLED_TRANS_ISO_MR, FE_MAT_ELASTIC, "coupled trans-iso Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FECoupledTransIsoMooneyRivlin::FECoupledTransIsoMooneyRivlin() : FETransverselyIsotropic(FE_COUPLED_TRANS_ISO_MR)
{
	SetFiberMaterial(new FEFiberMaterial);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS, "c1", "c1");
	AddScienceParam(0, Param_STRESS, "c2", "c2");
	AddScienceParam(0, Param_STRESS, "c3", "c3");
	AddScienceParam(0, Param_NONE, "c4", "c4");
	AddScienceParam(0, Param_STRESS, "c5", "c5");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
	AddScienceParam(0, Param_NONE, "lambda", "lambda");
}

void FECoupledTransIsoMooneyRivlin::Convert(FECoupledTransIsoMooneyRivlinOld* pold)
{
	if (pold == 0) return;

	SetFloatValue(MP_DENSITY, pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_DENSITY));
	SetFloatValue(MP_C1     , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_C1));
	SetFloatValue(MP_C2     , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_C2));
	SetFloatValue(MP_K      , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_K));
	SetFloatValue(MP_C3     , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_C3));
	SetFloatValue(MP_C4     , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_C4));
	SetFloatValue(MP_C5     , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_C5));
	SetFloatValue(MP_LAMBDA , pold->GetFloatValue(FECoupledTransIsoMooneyRivlin::MP_LAMBDA));
}

//=============================================================================
// FEMooneyRivlinVonMisesFibers
//=============================================================================

REGISTER_MATERIAL(FEMooneyRivlinVonMisesFibers, MODULE_MECH, FE_MAT_MR_VON_MISES_FIBERS, FE_MAT_ELASTIC_UNCOUPLED, "Mooney-Rivlin von Mises Fibers", MaterialFlags::TOPLEVEL);

FEMooneyRivlinVonMisesFibers::FEMooneyRivlinVonMisesFibers() : FEMaterial(FE_MAT_MR_VON_MISES_FIBERS)
{
	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density");
	AddScienceParam(0, Param_STRESS, "c1");
	AddScienceParam(0, Param_STRESS, "c2");
	AddScienceParam(0, Param_STRESS, "c3");
	AddScienceParam(0, Param_NONE, "c4");
	AddScienceParam(0, Param_STRESS, "c5");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
	AddScienceParam(0, Param_NONE, "lam_max");
	AddDoubleParam(0, "kf");
	AddDoubleParam(0, "vmc");
	AddDoubleParam(0, "var_n");
	AddDoubleParam(0, "tp");
	AddIntParam(0, "gipt");
}

//=============================================================================
// FE2DTransIsoMooneyRivlin
//=============================================================================

REGISTER_MATERIAL(FE2DTransIsoMooneyRivlin, MODULE_MECH, FE_MAT_2D_TRANS_ISO_MR, FE_MAT_ELASTIC_UNCOUPLED, "2D trans iso Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FE2DTransIsoMooneyRivlin::FE2DTransIsoMooneyRivlin() : FETransverselyIsotropic(FE_MAT_2D_TRANS_ISO_MR)
{
	SetFiberMaterial(new FEFiberMaterial);

	// define material parameters
	AddScienceParam(1, Param_DENSITY, "density");
	AddScienceParam(0, Param_STRESS, "c1");
	AddScienceParam(0, Param_STRESS, "c2");
	AddScienceParam(0, Param_STRESS, "c3");
	AddScienceParam(0, Param_NONE, "c4");
	AddScienceParam(0, Param_STRESS, "c5");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
	AddScienceParam(0, Param_NONE, "lam_max");
}

//////////////////////////////////////////////////////////////////////
// FERigidMaterial - rigid body material
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FERigidMaterial, MODULE_MECH, FE_RIGID_MATERIAL, FE_MAT_RIGID, "Rigid body", MaterialFlags::TOPLEVEL);

FERigidMaterial::FERigidMaterial() : FEMaterial(FE_RIGID_MATERIAL)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "E", "Young's modulus");
	AddScienceParam(0, Param_NONE   , "v", "Poisson's ratio");
	AddBoolParam  (false, "Auto-COM", "Auto-COM");
	AddVecParam   (vec3d(0,0,0), "rc", "Center of mass");

	m_pid = -1;
}

void FERigidMaterial::Save(OArchive& ar)
{
	ar.WriteChunk(CID_MAT_RIGID_PID, m_pid);
	ar.BeginChunk(CID_MAT_PARAMS);
	{
		ParamContainer::Save(ar);
	}
	ar.EndChunk();
}

void FERigidMaterial::Load(IArchive &ar)
{
	TRACE("FERigidMaterial::Load");

	while (IArchive::IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case CID_MAT_RIGID_PID: ar.read(m_pid); break;
		case CID_MAT_PARAMS: ParamContainer::Load(ar); break;
		}
		ar.CloseChunk();
	}
}

void FERigidMaterial::SetAutoCOM(bool b)
{
	SetBoolValue(MP_COM, b);
}

void FERigidMaterial::SetCenterOfMass(const vec3d& r)
{
	SetVecValue(MP_RC, r);
}

void FERigidMaterial::copy(FEMaterial* pmat)
{
	FEMaterial::copy(pmat);
	m_pid = (dynamic_cast<FERigidMaterial*>(pmat))->m_pid;
}

////////////////////////////////////////////////////////////////////////
// FETCNonlinearOrthotropic - Tension-Compression Nonlinear Orthotropic
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FETCNonlinearOrthotropic, MODULE_MECH, FE_TCNL_ORTHO, FE_MAT_ELASTIC_UNCOUPLED, "TC nonlinear orthotropic", MaterialFlags::TOPLEVEL);

FETCNonlinearOrthotropic::FETCNonlinearOrthotropic() : FEMaterial(FE_TCNL_ORTHO)
{
	AddScienceParam(1, Param_DENSITY, "density");
	AddScienceParam(0, Param_STRESS , "c1");
	AddScienceParam(0, Param_STRESS , "c2");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");

	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi", "ksi");
}

////////////////////////////////////////////////////////////////////////
// FEFungOrthotropic - Fung Orthotropic
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEFungOrthotropic, MODULE_MECH, FE_FUNG_ORTHO, FE_MAT_ELASTIC_UNCOUPLED, "Fung orthotropic", MaterialFlags::TOPLEVEL);

FEFungOrthotropic::FEFungOrthotropic() : FEMaterial(FE_FUNG_ORTHO)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "E1", "E1");
	AddScienceParam(0, Param_STRESS , "E2", "E2");
	AddScienceParam(0, Param_STRESS , "E3", "E3");
	AddScienceParam(0, Param_STRESS , "G12", "G12");
	AddScienceParam(0, Param_STRESS , "G23", "G23");
	AddScienceParam(0, Param_STRESS , "G31", "G31");
	AddScienceParam(0, Param_NONE   , "v12", "v12");
	AddScienceParam(0, Param_NONE   , "v23", "v23");
	AddScienceParam(0, Param_NONE   , "v31", "v31");
	AddScienceParam(0, Param_STRESS , "c", "c");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");
}

////////////////////////////////////////////////////////////////////////
// FEFungOrthotropic - Fung Orthotropic
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEFungOrthoCompressible, MODULE_MECH, FE_FUNG_ORTHO_COUPLED, FE_MAT_ELASTIC_UNCOUPLED, "Fung-ortho-compressible", MaterialFlags::TOPLEVEL);

FEFungOrthoCompressible::FEFungOrthoCompressible() : FEMaterial(FE_FUNG_ORTHO_COUPLED)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "E1", "E1");
	AddScienceParam(0, Param_STRESS , "E2", "E2");
	AddScienceParam(0, Param_STRESS , "E3", "E3");
	AddScienceParam(0, Param_STRESS , "G12", "G12");
	AddScienceParam(0, Param_STRESS , "G23", "G23");
	AddScienceParam(0, Param_STRESS , "G31", "G31");
	AddScienceParam(0, Param_NONE   , "v12", "v12");
	AddScienceParam(0, Param_NONE   , "v23", "v23");
	AddScienceParam(0, Param_NONE   , "v31", "v31");
	AddScienceParam(0, Param_STRESS , "c", "c");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");
}

////////////////////////////////////////////////////////////////////////
// FELinearOrthotropic - Linear Orthotropic
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FELinearOrthotropic, MODULE_MECH, FE_LINEAR_ORTHO, FE_MAT_ELASTIC, "linear orthotropic", MaterialFlags::TOPLEVEL);

FELinearOrthotropic::FELinearOrthotropic() : FEMaterial(FE_LINEAR_ORTHO)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "E1", "E1");
	AddScienceParam(0, Param_STRESS , "E2", "E2");
	AddScienceParam(0, Param_STRESS , "E3", "E3");
	AddScienceParam(0, Param_STRESS , "G12", "G12");
	AddScienceParam(0, Param_STRESS , "G23", "G23");
	AddScienceParam(0, Param_STRESS , "G31", "G31");
	AddScienceParam(0, Param_NONE   , "v12", "v12");
	AddScienceParam(0, Param_NONE   , "v23", "v23");
	AddScienceParam(0, Param_NONE   , "v31", "v31");
}

////////////////////////////////////////////////////////////////////////
// FEMuscleMaterial - Silvia Blemker's muscle material
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEMuscleMaterial, MODULE_MECH, FE_MUSCLE_MATERIAL, FE_MAT_ELASTIC_UNCOUPLED, "muscle material", MaterialFlags::TOPLEVEL);

FEMuscleMaterial::FEMuscleMaterial() : FETransverselyIsotropic(FE_MUSCLE_MATERIAL)
{
	SetFiberMaterial(new FEFiberMaterial);

	AddScienceParam(1, Param_DENSITY, "density");
	AddScienceParam(0, Param_STRESS , "g1");
	AddScienceParam(0, Param_STRESS , "g2");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");
	AddDoubleParam(0, "p1");
	AddDoubleParam(0, "p2");
	AddDoubleParam(0, "Lofl");
	AddDoubleParam(0, "lam_max");
	AddDoubleParam(0, "smax");
}

////////////////////////////////////////////////////////////////////////
// FETendonMaterial - Silvia Blemker's tendon material
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FETendonMaterial, MODULE_MECH, FE_TENDON_MATERIAL, FE_MAT_ELASTIC_UNCOUPLED, "tendon material", MaterialFlags::TOPLEVEL);

FETendonMaterial::FETendonMaterial() : FETransverselyIsotropic(FE_TENDON_MATERIAL)
{
	SetFiberMaterial(new FEFiberMaterial);

	AddScienceParam(1, Param_DENSITY, "density");
	AddScienceParam(0, Param_STRESS , "g1");
	AddScienceParam(0, Param_STRESS , "g2");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");
	AddDoubleParam(0, "l1");
	AddDoubleParam(0, "l2");
	AddDoubleParam(0, "lam_max");
}

////////////////////////////////////////////////////////////////////////
// Ogden material
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEOgdenMaterial, MODULE_MECH, FE_OGDEN_MATERIAL, FE_MAT_ELASTIC_UNCOUPLED, "Ogden", MaterialFlags::TOPLEVEL);

FEOgdenMaterial::FEOgdenMaterial() : FEMaterial(FE_OGDEN_MATERIAL)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "k", "bulk modulus");
	AddScienceParam(0, Param_STRESS , "c1", "c1");
	AddScienceParam(0, Param_STRESS , "c2", "c2");
	AddScienceParam(0, Param_STRESS , "c3", "c3");
	AddScienceParam(0, Param_STRESS , "c4", "c4");
	AddScienceParam(0, Param_STRESS , "c5", "c5");
	AddScienceParam(0, Param_STRESS , "c6", "c6");
	AddScienceParam(1, Param_NONE   , "m1", "m1");
	AddScienceParam(1, Param_NONE   , "m2", "m2");
	AddScienceParam(1, Param_NONE   , "m3", "m3");
	AddScienceParam(1, Param_NONE   , "m4", "m4");
	AddScienceParam(1, Param_NONE   , "m5", "m5");
	AddScienceParam(1, Param_NONE   , "m6", "m6");
}

////////////////////////////////////////////////////////////////////////
// Ogden material
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEOgdenUnconstrained, MODULE_MECH, FE_OGDEN_UNCONSTRAINED, FE_MAT_ELASTIC, "Ogden unconstrained", MaterialFlags::TOPLEVEL);

FEOgdenUnconstrained::FEOgdenUnconstrained() : FEMaterial(FE_OGDEN_UNCONSTRAINED)
{
	AddScienceParam(1, Param_DENSITY, "density", "density");
	AddScienceParam(0, Param_STRESS , "cp", "cp");
	AddScienceParam(0, Param_STRESS , "c1", "c1");
	AddScienceParam(0, Param_STRESS , "c2", "c2");
	AddScienceParam(0, Param_STRESS , "c3", "c3");
	AddScienceParam(0, Param_STRESS , "c4", "c4");
	AddScienceParam(0, Param_STRESS , "c5", "c5");
	AddScienceParam(0, Param_STRESS , "c6", "c6");
	AddScienceParam(1, Param_NONE   , "m1", "m1");
	AddScienceParam(1, Param_NONE   , "m2", "m2");
	AddScienceParam(1, Param_NONE   , "m3", "m3");
	AddScienceParam(1, Param_NONE   , "m4", "m4");
	AddScienceParam(1, Param_NONE   , "m5", "m5");
	AddScienceParam(1, Param_NONE   , "m6", "m6");
}

//////////////////////////////////////////////////////////////////////
// FEEFDMooneyRivlin - ellipsoidal fiber distribution model with MR base
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEEFDMooneyRivlin, MODULE_MECH, FE_EFD_MOONEY_RIVLIN, FE_MAT_ELASTIC_UNCOUPLED, "EFD Mooney-Rivlin", MaterialFlags::TOPLEVEL);

FEEFDMooneyRivlin::FEEFDMooneyRivlin() : FEMaterial(FE_EFD_MOONEY_RIVLIN)
{
	AddScienceParam(0, Param_STRESS, "c1", "c1");
	AddScienceParam(0, Param_STRESS, "c2", "c2");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi", "ksi");
}

//////////////////////////////////////////////////////////////////////
// FEEFDNeoHookean - ellipsoidal fiber distribution model with MR base
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEEFDNeoHookean, MODULE_MECH, FE_EFD_NEO_HOOKEAN, FE_MAT_ELASTIC, "EFD neo-Hookean", MaterialFlags::TOPLEVEL);

FEEFDNeoHookean::FEEFDNeoHookean() : FEMaterial(FE_EFD_NEO_HOOKEAN)
{
	AddScienceParam(0, Param_STRESS, "E", "Young's modulus");
	AddScienceParam(0, Param_NONE  , "v", "Poisson's ratio");
	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi", "ksi"  );
}

//////////////////////////////////////////////////////////////////////
// FEEFDDonnan - ellipsoidal fiber distribution model with Donnan base
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEEFDDonnan, MODULE_MECH, FE_EFD_DONNAN, FE_MAT_ELASTIC, "EFD Donnan equilibrium", MaterialFlags::TOPLEVEL);

FEEFDDonnan::FEEFDDonnan() : FEMaterial(FE_EFD_DONNAN)
{
	AddDoubleParam(0, "phiw0", "phiw0");
	AddDoubleParam(0, "cF0", "cF0");
	AddDoubleParam(0, "bosm", "bosm");
    AddDoubleParam(1, "Phi", "Phi");
	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi", "ksi");
}

//////////////////////////////////////////////////////////////////////
// FEEFDDonnan - ellipsoidal fiber distribution model with Donnan base
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEEFDVerondaWestmann, MODULE_MECH, FE_EFD_VERONDA_WESTMANN, FE_MAT_ELASTIC_UNCOUPLED, "EFD Veronda-Westmann", MaterialFlags::TOPLEVEL);

FEEFDVerondaWestmann::FEEFDVerondaWestmann() : FEMaterial(FE_EFD_VERONDA_WESTMANN)
{
	AddScienceParam(0, Param_STRESS, "c1", "c1");
	AddScienceParam(0, Param_STRESS, "c2", "c2");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi", "ksi"  );
}

////////////////////////////////////////////////////////////////////////
// FECubicCLE - Conewise Linear Elasticity with cubic symmetry
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FECubicCLE, MODULE_MECH, FE_CLE_CUBIC, FE_MAT_ELASTIC, "cubic CLE", MaterialFlags::TOPLEVEL);

FECubicCLE::FECubicCLE() : FEMaterial(FE_CLE_CUBIC)
{
    AddScienceParam(1, Param_DENSITY, "density", "density");
    AddScienceParam(0, Param_STRESS , "lp1", "lambda_+1");
    AddScienceParam(0, Param_STRESS , "lm1", "lambda_-1");
    AddScienceParam(0, Param_STRESS , "l2" , "lambda_2" );
    AddScienceParam(0, Param_STRESS , "mu" , "mu"       );
}

////////////////////////////////////////////////////////////////////////
// FECubicCLE - Conewise Linear Elasticity with orthotropic symmetry
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEOrthotropicCLE, MODULE_MECH, FE_CLE_ORTHOTROPIC, FE_MAT_ELASTIC, "orthotropic CLE", MaterialFlags::TOPLEVEL);

FEOrthotropicCLE::FEOrthotropicCLE() : FEMaterial(FE_CLE_ORTHOTROPIC)
{
    AddScienceParam(1, Param_DENSITY, "density", "density");
    AddScienceParam(0, Param_STRESS , "lp11", "lambda_+11");
    AddScienceParam(0, Param_STRESS , "lp22", "lambda_+22");
    AddScienceParam(0, Param_STRESS , "lp33", "lambda_+33");
    AddScienceParam(0, Param_STRESS , "lm11", "lambda_-11");
    AddScienceParam(0, Param_STRESS , "lm22", "lambda_-22");
    AddScienceParam(0, Param_STRESS , "lm33", "lambda_-33");
    AddScienceParam(0, Param_STRESS , "l12" , "lambda_12" );
    AddScienceParam(0, Param_STRESS , "l23" , "lambda_23" );
    AddScienceParam(0, Param_STRESS , "l31" , "lambda_31" );
    AddScienceParam(0, Param_STRESS , "mu1" , "mu1"       );
    AddScienceParam(0, Param_STRESS , "mu2" , "mu2"       );
    AddScienceParam(0, Param_STRESS , "mu3" , "mu3"       );
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionUniaxial - Prescribed uniaxial active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionUniaxial, MODULE_MECH, FE_ACTIVE_CONTRACT_UNI, FE_MAT_ELASTIC, "prescribed uniaxial active contraction", 0);

FEPrescribedActiveContractionUniaxial::FEPrescribedActiveContractionUniaxial() : FEMaterial(FE_ACTIVE_CONTRACT_UNI)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
    AddScienceParam(0, Param_ANGLE  , "theta", "theta");
    AddScienceParam(90, Param_ANGLE  , "phi"  , "phi" );
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionTransIso - Prescribed trans iso active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionTransIso, MODULE_MECH, FE_ACTIVE_CONTRACT_TISO, FE_MAT_ELASTIC, "prescribed trans iso active contraction", 0);

FEPrescribedActiveContractionTransIso::FEPrescribedActiveContractionTransIso() : FEMaterial(FE_ACTIVE_CONTRACT_TISO)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
    AddScienceParam(0, Param_ANGLE  , "theta", "theta");
    AddScienceParam(90, Param_ANGLE  , "phi"  , "phi" );
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionIsotropic - Prescribed isotropic active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionIsotropic, MODULE_MECH, FE_ACTIVE_CONTRACT_ISO, FE_MAT_ELASTIC, "prescribed isotropic active contraction", 0);

FEPrescribedActiveContractionIsotropic::FEPrescribedActiveContractionIsotropic() : FEMaterial(FE_ACTIVE_CONTRACT_ISO)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionUniaxialUC - Prescribed uniaxial active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionUniaxialUC, MODULE_MECH, FE_ACTIVE_CONTRACT_UNI_UC, FE_MAT_ELASTIC_UNCOUPLED, "uncoupled prescribed uniaxial active contraction", 0);

FEPrescribedActiveContractionUniaxialUC::FEPrescribedActiveContractionUniaxialUC() : FEMaterial(FE_ACTIVE_CONTRACT_UNI_UC)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
    AddScienceParam(0, Param_ANGLE  , "theta", "theta");
    AddScienceParam(90, Param_ANGLE  , "phi"  , "phi" );
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionTransIsoUC - Prescribed trans iso active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionTransIsoUC, MODULE_MECH, FE_ACTIVE_CONTRACT_TISO_UC, FE_MAT_ELASTIC_UNCOUPLED, "uncoupled prescribed trans iso active contraction", 0);

FEPrescribedActiveContractionTransIsoUC::FEPrescribedActiveContractionTransIsoUC() : FEMaterial(FE_ACTIVE_CONTRACT_TISO_UC)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
    AddScienceParam(0, Param_ANGLE  , "theta", "theta");
    AddScienceParam(90, Param_ANGLE  , "phi"  , "phi" );
}

////////////////////////////////////////////////////////////////////////
// FEPrescribedActiveContractionIsotropicUC - Prescribed isotropic active contraction
////////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEPrescribedActiveContractionIsotropicUC, MODULE_MECH, FE_ACTIVE_CONTRACT_ISO_UC, FE_MAT_ELASTIC_UNCOUPLED, "uncoupled prescribed isotropic active contraction", 0);

FEPrescribedActiveContractionIsotropicUC::FEPrescribedActiveContractionIsotropicUC() : FEMaterial(FE_ACTIVE_CONTRACT_ISO_UC)
{
    AddScienceParam(0, Param_STRESS , "T0", "T0");
}

//////////////////////////////////////////////////////////////////////
// FEIsotropicFourier - Isotropic Fourier
//////////////////////////////////////////////////////////////////////

REGISTER_MATERIAL(FEIsotropicFourier, MODULE_HEAT, FE_ISOTROPIC_FOURIER, FE_MAT_HEAT_TRANSFER, "isotropic Fourier", MaterialFlags::TOPLEVEL);

FEIsotropicFourier::FEIsotropicFourier() : FEMaterial(FE_ISOTROPIC_FOURIER)
{
	AddScienceParam(1, Param_DENSITY     , "density", "density");
	AddScienceParam(0, Param_CONDUCTIVITY, "k"      , "heat conductivity");
	AddScienceParam(0, Param_CAPACITY    , "c"      , "heat capacity");
}

//=============================================================================
// Constant Permeability
//=============================================================================

REGISTER_MATERIAL(FEPermConst, MODULE_BIPHASIC, FE_PERM_CONST, FE_MAT_PERMEABILITY, "perm-const-iso", 0);

FEPermConst::FEPermConst() : FEMaterial(FE_PERM_CONST)
{
	AddScienceParam(0, Param_PERMEABILITY, "perm", "permeability");
}

//=============================================================================
// Holmes-Mow Permeability
//=============================================================================

REGISTER_MATERIAL(FEPermHolmesMow, MODULE_BIPHASIC, FE_PERM_HOLMES_MOW, FE_MAT_PERMEABILITY, "perm-Holmes-Mow", 0);

FEPermHolmesMow::FEPermHolmesMow() : FEMaterial(FE_PERM_HOLMES_MOW)
{
	AddScienceParam(0, Param_PERMEABILITY, "perm" , "permeability");
	AddScienceParam(0, Param_NONE        , "M"    , "M");
	AddScienceParam(0, Param_NONE        , "alpha", "alpha");
}

//=============================================================================
// Ateshian-Weiss isotropic permeability
//=============================================================================

REGISTER_MATERIAL(FEPermAteshianWeissIso, MODULE_BIPHASIC, FE_PERM_REF_ISO, FE_MAT_PERMEABILITY, "perm-ref-iso", 0);

FEPermAteshianWeissIso::FEPermAteshianWeissIso() : FEMaterial(FE_PERM_REF_ISO)
{
	AddScienceParam(0, Param_PERMEABILITY, "perm0", "perm0");
	AddScienceParam(0, Param_PERMEABILITY, "perm1", "perm1");
	AddScienceParam(0, Param_PERMEABILITY, "perm2", "perm2");
	AddScienceParam(0, Param_NONE        , "M"    , "M");
	AddScienceParam(0, Param_NONE        , "alpha", "alpha");
}

//=============================================================================
// Ateshian-Weiss trans-isotropic permeability
//=============================================================================

REGISTER_MATERIAL(FEPermAteshianWeissTransIso, MODULE_BIPHASIC, FE_PERM_REF_TRANS_ISO, FE_MAT_PERMEABILITY, "perm-ref-trans-iso", 0);

FEPermAteshianWeissTransIso::FEPermAteshianWeissTransIso() : FEMaterial(FE_PERM_REF_TRANS_ISO)
{
	AddScienceParam(0, Param_PERMEABILITY, "perm0" , "perm0" );
	AddScienceParam(0, Param_PERMEABILITY, "perm1T", "perm1T");
	AddScienceParam(0, Param_PERMEABILITY, "perm1A", "perm1A");
	AddScienceParam(0, Param_PERMEABILITY, "perm2T", "perm2T");
	AddScienceParam(0, Param_PERMEABILITY, "perm2A", "perm2A");
	AddScienceParam(0, Param_NONE        , "M0"    , "M0"    );
	AddScienceParam(0, Param_NONE        , "MT"    , "MT"    );
	AddScienceParam(0, Param_NONE        , "MA"    , "MA"    );
	AddScienceParam(0, Param_NONE        , "alpha0", "alpha0");
	AddScienceParam(0, Param_NONE        , "alphaA", "alphaA");
	AddScienceParam(0, Param_NONE        , "alphaT", "alphaT");
}

//=============================================================================
// Ateshian-Weiss orthotropic permeability
//=============================================================================

REGISTER_MATERIAL(FEPermAteshianWeissOrtho, MODULE_BIPHASIC, FE_PERM_REF_ORTHO, FE_MAT_PERMEABILITY, "perm-ref-ortho", 0);

FEPermAteshianWeissOrtho::FEPermAteshianWeissOrtho() : FEMaterial(FE_PERM_REF_ORTHO)
{
	AddScienceParam(0, Param_PERMEABILITY, "perm0" , "perm0");
	AddVecParam(vec3d(0,0,0), "perm1" , "perm1");
	AddVecParam(vec3d(0,0,0), "perm2" , "perm2");
	AddScienceParam(0, Param_NONE        , "M0"    , "M0");
	AddScienceParam(0, Param_NONE        , "alpha0", "alpha0");
	AddVecParam(vec3d(0,0,0), "M"     , "M");
	AddVecParam(vec3d(0,0,0), "alpha" , "alpha");
}

//=============================================================================
// constant diffusivity
//=============================================================================

REGISTER_MATERIAL(FEDiffConst, MODULE_BIPHASIC, FE_DIFF_CONST, FE_MAT_DIFFUSIVITY, "diff-const-iso", 0);

FEDiffConst::FEDiffConst() : FEMaterial(FE_DIFF_CONST)
{
	AddScienceParam(0, Param_DIFFUSIVITY, "free_diff", "free diffusivity");
	AddScienceParam(0, Param_DIFFUSIVITY, "diff"     , "diffusivity");
}

//=============================================================================
// orthotropic diffusivity
//=============================================================================

REGISTER_MATERIAL(FEDiffOrtho, MODULE_BIPHASIC, FE_DIFF_CONST_ORTHO, FE_MAT_DIFFUSIVITY, "diff-const-ortho", 0);

FEDiffOrtho::FEDiffOrtho() : FEMaterial(FE_DIFF_CONST_ORTHO)
{
	AddScienceParam(0, Param_DIFFUSIVITY, "free_diff", "free diffusivity");
	AddVecParam(vec3d(0,0,0), "diff", "diffusivity");
}

//=============================================================================
// Ateshian-Weiss isotropic diffusivity
//=============================================================================

REGISTER_MATERIAL(FEDiffAteshianWeissIso, MODULE_BIPHASIC, FE_DIFF_REF_ISO, FE_MAT_DIFFUSIVITY, "diff-ref-iso", 0);

FEDiffAteshianWeissIso::FEDiffAteshianWeissIso() : FEMaterial(FE_DIFF_REF_ISO)
{
	AddScienceParam(0, Param_DIFFUSIVITY, "free_diff", "free diffusivity");
	AddScienceParam(0, Param_DIFFUSIVITY, "diff0"    , "diff0");
	AddScienceParam(0, Param_DIFFUSIVITY, "diff1"    , "diff1");
	AddScienceParam(0, Param_DIFFUSIVITY, "diff2"    , "diff2");
	AddScienceParam(0, Param_NONE       , "M"        , "M"    );
	AddScienceParam(0, Param_NONE       , "alpha"    , "alpha");
}

//=============================================================================
// Albro isotropic diffusivity
//=============================================================================

REGISTER_MATERIAL(FEDiffAlbroIso, MODULE_BIPHASIC, FE_DIFF_ALBRO_ISO, FE_MAT_DIFFUSIVITY, "diff-Albro-iso", 0);

FEDiffAlbroIso::FEDiffAlbroIso() : FEMaterial(FE_DIFF_ALBRO_ISO)
{
	AddScienceParam(0, Param_DIFFUSIVITY, "free_diff", "free diffusivity");
	AddScienceParam(0, Param_NONE       , "cdinv"    , "cdinv");
	AddScienceParam(0, Param_NONE       , "alphad"   , "alphad");
}

//=============================================================================
// constant solubility
//=============================================================================

REGISTER_MATERIAL(FESolubConst, MODULE_BIPHASIC, FE_SOLUB_CONST, FE_MAT_SOLUBILITY, "solub-const", 0);

FESolubConst::FESolubConst() : FEMaterial(FE_SOLUB_CONST)
{
	AddScienceParam(1, Param_NONE, "solub", "solubility");
}

//=============================================================================
// constant osmotic coefficient
//=============================================================================

REGISTER_MATERIAL(FEOsmoConst, MODULE_BIPHASIC, FE_OSMO_CONST, FE_MAT_OSMOTIC_COEFFICIENT, "osm-coef-const", 0);

FEOsmoConst::FEOsmoConst() : FEMaterial(FE_OSMO_CONST)
{
	AddScienceParam(1, Param_NONE, "osmcoef", "osmotic coefficient");
}

//=============================================================================
// SFD compressible
//=============================================================================

REGISTER_MATERIAL(FESFDCoupled, MODULE_MECH, FE_SFD_COUPLED, FE_MAT_ELASTIC, "spherical fiber distribution", 0);

FESFDCoupled::FESFDCoupled() : FEMaterial(FE_SFD_COUPLED)
{
	AddScienceParam(0, Param_NONE        , "alpha", "alpha");
	AddScienceParam(0, Param_NONE        , "beta", "beta");
	AddScienceParam(0, Param_NONE        , "ksi" , "ksi" );
}

//=============================================================================
// SFD SBM
//=============================================================================

REGISTER_MATERIAL(FESFDSBM, MODULE_MECH, FE_SFD_SBM, FE_MAT_ELASTIC, "spherical fiber distribution sbm", 0);

FESFDSBM::FESFDSBM() : FEMaterial(FE_SFD_SBM)
{
	AddScienceParam(0, Param_NONE        , "alpha", "alpha" );
	AddScienceParam(0, Param_NONE        , "beta", "beta"   );
	AddScienceParam(0, Param_NONE        , "ksi0" , "ksi0"  );
	AddScienceParam(1, Param_NONE        , "rho0" , "rho0"  );
	AddScienceParam(0, Param_NONE        , "gamma" , "gamma");
	AddIntParam    (-1                   , "sbm"   , "sbm"  );
}

//=============================================================================
// EFD Coupled
//=============================================================================

REGISTER_MATERIAL(FEEFDCoupled, MODULE_MECH, FE_EFD_COUPLED, FE_MAT_ELASTIC, "ellipsoidal fiber distribution", 0);

FEEFDCoupled::FEEFDCoupled() : FEMaterial(FE_EFD_COUPLED)
{
	AddVecParam(vec3d(0,0,0), "beta", "beta");
	AddVecParam(vec3d(0,0,0), "ksi" , "ksi" );
}

//=============================================================================
// EFD Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEEFDUncoupled, MODULE_MECH, FE_EFD_UNCOUPLED, FE_MAT_ELASTIC_UNCOUPLED, "EFD uncoupled", 0);

FEEFDUncoupled::FEEFDUncoupled() : FEMaterial(FE_EFD_UNCOUPLED)
{
	AddVecParam(vec3d(0,0,0), "beta" , "beta");
	AddVecParam(vec3d(0,0,0), "ksi" , "ksi");
	AddScienceParam(0, Param_STRESS, "k", "bulk modulus");
}

//=============================================================================
// Fiber-Exp-Pow
//=============================================================================

REGISTER_MATERIAL(FEFiberExpPow, MODULE_MECH, FE_FIBEREXPPOW_COUPLED, FE_MAT_ELASTIC, "fiber-exp-pow", 0);

FEFiberExpPow::FEFiberExpPow() : FEMaterial(FE_FIBEREXPPOW_COUPLED)
{
	AddDoubleParam(0, "alpha", "alpha");
	AddDoubleParam(0, "beta" , "beta" );
	AddDoubleParam(0, "ksi"  , "ksi"  );
	AddDoubleParam(0, "theta", "theta");
	AddDoubleParam(0, "phi"  , "phi"  );
}

//=============================================================================
// Fiber-Exp-Linear
//=============================================================================

REGISTER_MATERIAL(FEFiberExpLinear, MODULE_MECH, FE_FIBEREXPLIN_COUPLED, FE_MAT_ELASTIC, "fiber-exp-linear", 0);

FEFiberExpLinear::FEFiberExpLinear() : FEMaterial(FE_FIBEREXPLIN_COUPLED)
{
	AddDoubleParam(0.0, "c3", "c3");
	AddDoubleParam(0.0, "c4", "c4");
	AddDoubleParam(0.0, "c5", "c5");
	AddDoubleParam(0.0, "lambda", "lambda");
}

//=============================================================================
// Fiber-Exp-Pow Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEFiberExpPowUncoupled, MODULE_MECH, FE_FIBEREXPPOW_UNCOUPLED, FE_MAT_ELASTIC_UNCOUPLED, "fiber-exp-pow-uncoupled", 0);

FEFiberExpPowUncoupled::FEFiberExpPowUncoupled() : FEMaterial(FE_FIBEREXPPOW_UNCOUPLED)
{
	AddDoubleParam(0, "alpha", "alpha");
	AddDoubleParam(0, "beta" , "beta" );
	AddDoubleParam(0, "ksi"  , "ksi"  );
    AddDoubleParam(0, "k", "bulk modulus");
	AddDoubleParam(0, "theta", "theta");
	AddDoubleParam(0, "phi"  , "phi"  );
}

//=============================================================================
// Fiber-Pow-Linear
//=============================================================================

REGISTER_MATERIAL(FEFiberPowLin, MODULE_MECH, FE_FIBERPOWLIN_COUPLED, FE_MAT_ELASTIC, "fiber-pow-linear", 0);

FEFiberPowLin::FEFiberPowLin() : FEMaterial(FE_FIBERPOWLIN_COUPLED)
{
    AddDoubleParam(0, "E", "E");
    AddDoubleParam(2, "beta" , "beta");
    AddDoubleParam(1, "lam0"  , "lam0");
    AddDoubleParam(0, "theta", "theta");
    AddDoubleParam(0, "phi"  , "phi"  );
}

//=============================================================================
// Fiber-Exp-Pow Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEFiberPowLinUncoupled, MODULE_MECH, FE_FIBERPOWLIN_UNCOUPLED, FE_MAT_ELASTIC_UNCOUPLED, "fiber-pow-linear-uncoupled", 0);

FEFiberPowLinUncoupled::FEFiberPowLinUncoupled() : FEMaterial(FE_FIBERPOWLIN_UNCOUPLED)
{
    AddDoubleParam(0, "E", "E");
    AddDoubleParam(2, "beta" , "beta");
    AddDoubleParam(1, "lam0"  , "lam0");
    AddDoubleParam(0, "k", "bulk modulus");
    AddDoubleParam(0, "theta", "theta");
    AddDoubleParam(0, "phi"  , "phi"  );
}

//=============================================================================
// Donnan swelling
//=============================================================================

REGISTER_MATERIAL(FEDonnanSwelling, MODULE_MECH, FE_DONNAN_SWELLING, FE_MAT_ELASTIC, "Donnan equilibrium", 0);

FEDonnanSwelling::FEDonnanSwelling() : FEMaterial(FE_DONNAN_SWELLING)
{
	AddDoubleParam(0, "phiw0", "phiw0");
	AddDoubleParam(0, "cF0", "cF0");
	AddDoubleParam(0, "bosm", "bosm");
    AddDoubleParam(0, "Phi", "Phi");
}

//=============================================================================
// Perfect Osmometer
//=============================================================================

REGISTER_MATERIAL(FEPerfectOsmometer, MODULE_MECH, FE_PERFECT_OSMOMETER, FE_MAT_ELASTIC, "perfect osmometer", 0);

FEPerfectOsmometer::FEPerfectOsmometer() : FEMaterial(FE_PERFECT_OSMOMETER)
{
	AddDoubleParam(0, "phiw0", "phiw0");
	AddDoubleParam(0, "iosm", "iosm");
	AddDoubleParam(0, "bosm", "bosm");
}

//=============================================================================
// Cell Growth
//=============================================================================

REGISTER_MATERIAL(FECellGrowth, MODULE_MECH, FE_CELL_GROWTH, FE_MAT_ELASTIC, "cell growth", 0);

FECellGrowth::FECellGrowth() : FEMaterial(FE_CELL_GROWTH)
{
	AddDoubleParam(0, "phir", "phir");
	AddDoubleParam(0, "cr", "cr");
	AddDoubleParam(0, "ce", "ce");
}

//=============================================================================
// Osmotic pressure using virial coefficients
//=============================================================================

REGISTER_MATERIAL(FEOsmoticVirial, MODULE_MECH, FE_OSMOTIC_VIRIAL, FE_MAT_ELASTIC, "osmotic virial expansion", 0);

FEOsmoticVirial::FEOsmoticVirial() : FEMaterial(FE_OSMOTIC_VIRIAL)
{
    AddDoubleParam(0, "phiw0", "phiw0");
    AddDoubleParam(0, "cr", "cr");
    AddDoubleParam(0, "c1", "c1");
    AddDoubleParam(0, "c2", "c2");
    AddDoubleParam(0, "c3", "c3");
}

//=============================================================================
// Constant reaction rate
//=============================================================================

REGISTER_MATERIAL(FEReactionRateConst, MODULE_REACTIONS, FE_REACTION_RATE_CONST, FE_MAT_REACTION_RATE, "constant reaction rate", 0);

FEReactionRateConst::FEReactionRateConst() : FEMaterial(FE_REACTION_RATE_CONST)
{
	AddDoubleParam(0, "k", "k");
}

double FEReactionRateConst::GetRateConstant() { return GetParam(0).GetFloatValue(); }

void FEReactionRateConst::SetRateConstant(double K) { SetFloatValue(0, K); }

//=============================================================================
// Huiskes reaction rate
//=============================================================================

REGISTER_MATERIAL(FEReactionRateHuiskes, MODULE_REACTIONS, FE_REACTION_RATE_HUISKES, FE_MAT_REACTION_RATE, "Huiskes reaction rate", 0);

FEReactionRateHuiskes::FEReactionRateHuiskes() : FEMaterial(FE_REACTION_RATE_HUISKES)
{
	AddDoubleParam(0, "B", "B");
	AddDoubleParam(0, "psi0", "psi0");
}

//=============================================================================
// CFD Fiber-Exponential-Power-Law
//=============================================================================

REGISTER_MATERIAL(FECFDFiberExpPow, MODULE_MECH, FE_FIBER_EXP_POW, FE_MAT_CFD_FIBER, "fiber-exponential-power-law", 0);

FECFDFiberExpPow::FECFDFiberExpPow() : FEMaterial(FE_FIBER_EXP_POW)
{
    AddDoubleParam(0, "alpha", "alpha");
    AddDoubleParam(0, "beta" , "beta" );
    AddDoubleParam(0, "ksi"  , "ksi"  );
    AddDoubleParam(0, "mu"   , "mu"   );
}

//=============================================================================
// CFD Fiber-neo-Hookean
//=============================================================================

REGISTER_MATERIAL(FECFDFiberNH, MODULE_MECH, FE_FIBER_NH, FE_MAT_CFD_FIBER, "fiber-NH", 0);

FECFDFiberNH::FECFDFiberNH() : FEMaterial(FE_FIBER_NH)
{
    AddDoubleParam(0, "mu"   , "mu");
}

//=============================================================================
// CFD Fiber-Exponential-Power-Law uncoupled
//=============================================================================

REGISTER_MATERIAL(FECFDFiberExpPowUC, MODULE_MECH, FE_FIBER_EXP_POW_UC, FE_MAT_CFD_FIBER_UC, "fiber-exponential-power-law-uncoupled", 0);

FECFDFiberExpPowUC::FECFDFiberExpPowUC() : FEMaterial(FE_FIBER_EXP_POW_UC)
{
    AddDoubleParam(0, "alpha", "alpha");
    AddDoubleParam(0, "beta" , "beta" );
    AddDoubleParam(0, "ksi"  , "ksi"  );
    AddDoubleParam(0, "mu"   , "mu"   );
    AddDoubleParam(0, "k", "bulk modulus");
}

//=============================================================================
// CFD Fiber-neo-Hookean uncoupled
//=============================================================================

REGISTER_MATERIAL(FECFDFiberNHUC, MODULE_MECH, FE_FIBER_NH_UC, FE_MAT_CFD_FIBER_UC, "fiber-NH-uncoupled", 0);

FECFDFiberNHUC::FECFDFiberNHUC() : FEMaterial(FE_FIBER_NH_UC)
{
    AddDoubleParam(0, "mu"   , "mu"   );
    AddDoubleParam(0, "k", "bulk modulus");
}

//=============================================================================
// FDD Spherical
//=============================================================================

REGISTER_MATERIAL(FEFDDSpherical, MODULE_MECH, FE_DSTRB_SFD, FE_MAT_CFD_DIST, "spherical", 0);

FEFDDSpherical::FEFDDSpherical() : FEMaterial(FE_DSTRB_SFD)
{
}

//=============================================================================
// FDD Ellipsoidal
//=============================================================================

REGISTER_MATERIAL(FEFDDEllipsoidal, MODULE_MECH, FE_DSTRB_EFD, FE_MAT_CFD_DIST, "ellipsoidal", 0);

FEFDDEllipsoidal::FEFDDEllipsoidal() : FEMaterial(FE_DSTRB_EFD)
{
    AddVecParam(vec3d(1,1,1), "spa" , "spa");
}

//=============================================================================
// FDD von Mises 3d
//=============================================================================

REGISTER_MATERIAL(FEFDDvonMises3d, MODULE_MECH, FE_DSTRB_VM3, FE_MAT_CFD_DIST, "von-Mises-3d", 0);

FEFDDvonMises3d::FEFDDvonMises3d() : FEMaterial(FE_DSTRB_VM3)
{
    AddDoubleParam(0, "b"   , "b");
}

//=============================================================================
// FDD Circular
//=============================================================================

REGISTER_MATERIAL(FEFDDCircular, MODULE_MECH, FE_DSTRB_CFD, FE_MAT_CFD_DIST, "circular", 0);

FEFDDCircular::FEFDDCircular() : FEMaterial(FE_DSTRB_CFD)
{
}

//=============================================================================
// FDD Elliptical
//=============================================================================

REGISTER_MATERIAL(FEFDDElliptical, MODULE_MECH, FE_DSTRB_PFD, FE_MAT_CFD_DIST, "elliptical", 0);

FEFDDElliptical::FEFDDElliptical() : FEMaterial(FE_DSTRB_PFD)
{
    AddDoubleParam(0, "spa1"   , "spa1");
    AddDoubleParam(0, "spa2"   , "spa2");
}

//=============================================================================
// FDD von Mises 2d
//=============================================================================

REGISTER_MATERIAL(FEFDDvonMises2d, MODULE_MECH, FE_DSTRB_VM2, FE_MAT_CFD_DIST, "von-Mises-2d", 0);

FEFDDvonMises2d::FEFDDvonMises2d() : FEMaterial(FE_DSTRB_VM2)
{
    AddDoubleParam(0, "b"   , "b");
}

//=============================================================================
// Scheme Gauss-Kronrod Trapezoidal
//=============================================================================

REGISTER_MATERIAL(FESchemeGKT, MODULE_MECH, FE_SCHM_GKT, FE_MAT_CFD_SCHEME, "fibers-3d-gkt", 0);

FESchemeGKT::FESchemeGKT() : FEMaterial(FE_SCHM_GKT)
{
    AddIntParam(11, "nph"   , "nph");// choose from 7, 11, 15, 19, 23, or 27
    AddIntParam(31, "nth"   , "nth");// enter odd value >= 3
}

//=============================================================================
// Scheme Finite Element Integration
//=============================================================================

REGISTER_MATERIAL(FESchemeFEI, MODULE_MECH, FE_SCHM_FEI, FE_MAT_CFD_SCHEME, "fibers-3d-fei", 0);

FESchemeFEI::FESchemeFEI() : FEMaterial(FE_SCHM_FEI)
{
    AddIntParam(11, "resolution"   , "resolution");// choose from 20, 34, 60, 74, 196, 210, 396, 410, ..., 1596, 1610, 1796
}

//=============================================================================
// Scheme Trapezoidal 2d
//=============================================================================

REGISTER_MATERIAL(FESchemeT2d, MODULE_MECH, FE_SCHM_T2D, FE_MAT_CFD_SCHEME, "fibers-2d-trapezoidal", 0);

FESchemeT2d::FESchemeT2d() : FEMaterial(FE_SCHM_T2D)
{
    AddIntParam(31, "nth"   , "nth");// odd and >= 3
}

//=============================================================================
// Scheme Gauss-Kronrod Trapezoidal uncoupled
//=============================================================================

REGISTER_MATERIAL(FESchemeGKTUC, MODULE_MECH, FE_SCHM_GKT_UC, FE_MAT_CFD_SCHEME_UC, "fibers-3d-gkt-uncoupled", 0);

FESchemeGKTUC::FESchemeGKTUC() : FEMaterial(FE_SCHM_GKT_UC)
{
    AddIntParam(11, "nph"   , "nph");// choose from 7, 11, 15, 19, 23, or 27
    AddIntParam(31, "nth"   , "nth");//  enter odd value >= 3
}

//=============================================================================
// Scheme Finite Element Integration uncoupled
//=============================================================================

REGISTER_MATERIAL(FESchemeFEIUC, MODULE_MECH, FE_SCHM_FEI_UC, FE_MAT_CFD_SCHEME_UC, "fibers-3d-fei-uncoupled", 0);

FESchemeFEIUC::FESchemeFEIUC() : FEMaterial(FE_SCHM_FEI_UC)
{
    AddIntParam(11, "resolution"   , "resolution"); // choose from 20, 34, 60, 74, 196, 210, 396, 410, ..., 1596, 1610, 1796
}

//=============================================================================
// Scheme Trapezoidal 2d uncoupled
//=============================================================================

REGISTER_MATERIAL(FESchemeT2dUC, MODULE_MECH, FE_SCHM_T2D_UC, FE_MAT_CFD_SCHEME_UC, "fibers-2d-trapezoidal-uncoupled", 0);

FESchemeT2dUC::FESchemeT2dUC() : FEMaterial(FE_SCHM_T2D_UC)
{
    AddIntParam(31, "nth"   , "nth"); // nth (odd and >= 3)
}

//=============================================================================
// CDF Simo
//=============================================================================

REGISTER_MATERIAL(FECDFSimo, MODULE_MECH, FE_CDF_SIMO, FE_MAT_DAMAGE, "CDF Simo", 0);

FECDFSimo::FECDFSimo() : FEMaterial(FE_CDF_SIMO)
{
    AddDoubleParam(0, "a" , "a"); // a must be ≥ 0
    AddDoubleParam(0, "b" , "b");
}

//=============================================================================
// CDF Log Normal
//=============================================================================

REGISTER_MATERIAL(FECDFLogNormal, MODULE_MECH, FE_CDF_LOG_NORMAL, FE_MAT_DAMAGE, "CDF log-normal", 0);

FECDFLogNormal::FECDFLogNormal() : FEMaterial(FE_CDF_LOG_NORMAL)
{
    AddDoubleParam(0, "mu" , "mu"); // mu must be > 0
    AddDoubleParam(0, "sigma" , "sigma"); // sigma must be > 0
    AddDoubleParam(1, "Dmax" , "Dmax"); // Maximum allowable damage (0 ≤ Dmax ≤ 1)
}

//=============================================================================
// CDF Weibull
//=============================================================================

REGISTER_MATERIAL(FECDFWeibull, MODULE_MECH, FE_CDF_WEIBULL, FE_MAT_DAMAGE, "CDF Weibull", 0);

FECDFWeibull::FECDFWeibull() : FEMaterial(FE_CDF_WEIBULL)
{
    AddDoubleParam(0, "alpha" , "alpha"); // alpha must be ≥ 0
    AddDoubleParam(0, "mu" , "mu"); // mu must be > 0
    AddDoubleParam(1, "Dmax" , "Dmax"); // Maximum allowable damage (0 ≤ Dmax ≤ 1)
}

//=============================================================================
// CDF Step
//=============================================================================

REGISTER_MATERIAL(FECDFStep, MODULE_MECH, FE_CDF_STEP, FE_MAT_DAMAGE, "CDF step", 0);

FECDFStep::FECDFStep() : FEMaterial(FE_CDF_STEP)
{
    AddDoubleParam(0, "mu" , "mu" ); //  mu must be > 0
    AddDoubleParam(1, "Dmax" , "Dmax"); // Maximum allowable damage (0 ≤ Dmax ≤ 1)
}

//=============================================================================
// CDF Quintic
//=============================================================================

REGISTER_MATERIAL(FECDFQuintic, MODULE_MECH, FE_CDF_QUINTIC, FE_MAT_DAMAGE, "CDF quintic", 0);

FECDFQuintic::FECDFQuintic() : FEMaterial(FE_CDF_QUINTIC)
{
    AddDoubleParam(0, "mumin" , "mumin"); // mumin must be > 0
    AddDoubleParam(0, "mumax" , "mumax"); // mumax must be > mumin
    AddDoubleParam(1, "Dmax" , "Dmax" ); // Maximum allowable damage (0 ≤ Dmax ≤ 1)
}

//=============================================================================
// DC Simo
//=============================================================================

REGISTER_MATERIAL(FEDCSimo, MODULE_MECH, FE_DC_SIMO, FE_MAT_DAMAGE_CRITERION, "DC Simo", 0);

FEDCSimo::FEDCSimo() : FEMaterial(FE_DC_SIMO)
{
}

//=============================================================================
// DC Strain Energy Density
//=============================================================================

REGISTER_MATERIAL(FEDCStrainEnergyDensity, MODULE_MECH, FE_DC_SED, FE_MAT_DAMAGE_CRITERION, "DC strain energy density", 0);

FEDCStrainEnergyDensity::FEDCStrainEnergyDensity() : FEMaterial(FE_DC_SED)
{
}

//=============================================================================
// DC Specific Strain Energy
//=============================================================================

REGISTER_MATERIAL(FEDCSpecificStrainEnergy, MODULE_MECH, FE_DC_SSE, FE_MAT_DAMAGE_CRITERION, "DC specific strain energy", 0);

FEDCSpecificStrainEnergy::FEDCSpecificStrainEnergy() : FEMaterial(FE_DC_SSE)
{
}

//=============================================================================
// DC von Mises Stress
//=============================================================================

REGISTER_MATERIAL(FEDCvonMisesStress, MODULE_MECH, FE_DC_VMS, FE_MAT_DAMAGE_CRITERION, "DC von Mises stress", 0);

FEDCvonMisesStress::FEDCvonMisesStress() : FEMaterial(FE_DC_VMS)
{
}

//=============================================================================
// DC Maximum Shear Stress
//=============================================================================

REGISTER_MATERIAL(FEDCMaxShearStress, MODULE_MECH, FE_DC_MSS, FE_MAT_DAMAGE_CRITERION, "DC max shear stress", 0);

FEDCMaxShearStress::FEDCMaxShearStress() : FEMaterial(FE_DC_MSS)
{
}

//=============================================================================
// DC Maximum Normal Stress
//=============================================================================

REGISTER_MATERIAL(FEDCMaxNormalStress, MODULE_MECH, FE_DC_MNS, FE_MAT_DAMAGE_CRITERION, "DC max normal stress", 0);

FEDCMaxNormalStress::FEDCMaxNormalStress() : FEMaterial(FE_DC_MNS)
{
}

//=============================================================================
// DC Maximum Normal Lagrange Strain
//=============================================================================

REGISTER_MATERIAL(FEDCMaxNormalLagrangeStrain, MODULE_MECH, FE_DC_MNLE, FE_MAT_DAMAGE_CRITERION, "DC max normal Lagrange strain", 0);

FEDCMaxNormalLagrangeStrain::FEDCMaxNormalLagrangeStrain() : FEMaterial(FE_DC_MNLE)
{
}

//=============================================================================
// DC Simo Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCSimoUC, MODULE_MECH, FE_DC_SIMO_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC Simo uncoupled", 0);

FEDCSimoUC::FEDCSimoUC() : FEMaterial(FE_DC_SIMO_UC)
{
}

//=============================================================================
// DC Strain Energy Density Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCStrainEnergyDensityUC, MODULE_MECH, FE_DC_SED_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC strain energy density uncoupled", 0);

FEDCStrainEnergyDensityUC::FEDCStrainEnergyDensityUC() : FEMaterial(FE_DC_SED_UC)
{
}

//=============================================================================
// DC Specific Strain Energy Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCSpecificStrainEnergyUC, MODULE_MECH, FE_DC_SSE_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC specific strain energy uncoupled", 0);

FEDCSpecificStrainEnergyUC::FEDCSpecificStrainEnergyUC() : FEMaterial(FE_DC_SSE_UC)
{
}

//=============================================================================
// DC von Mises Stress Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCvonMisesStressUC, MODULE_MECH, FE_DC_VMS_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC von Mises stress uncoupled", 0);

FEDCvonMisesStressUC::FEDCvonMisesStressUC() : FEMaterial(FE_DC_VMS_UC)
{
}

//=============================================================================
// DC Maximum Shear Stress Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCMaxShearStressUC, MODULE_MECH, FE_DC_MSS_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC max shear stress uncoupled", 0);

FEDCMaxShearStressUC::FEDCMaxShearStressUC() : FEMaterial(FE_DC_MSS_UC)
{
}

//=============================================================================
// DC Maximum Normal Stress Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCMaxNormalStressUC, MODULE_MECH, FE_DC_MNS_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC max normal stress uncoupled", 0);

FEDCMaxNormalStressUC::FEDCMaxNormalStressUC() : FEMaterial(FE_DC_MNS_UC)
{
}

//=============================================================================
// DC Maximum Normal Lagrange Strain Uncoupled
//=============================================================================

REGISTER_MATERIAL(FEDCMaxNormalLagrangeStrainUC, MODULE_MECH, FE_DC_MNLE_UC, FE_MAT_DAMAGE_CRITERION_UC, "DC max normal Lagrange strain uncoupled", 0);

FEDCMaxNormalLagrangeStrainUC::FEDCMaxNormalLagrangeStrainUC() : FEMaterial(FE_DC_MNLE_UC)
{
}

//=============================================================================
// Relaxation Exponential
//=============================================================================

REGISTER_MATERIAL(FERelaxExp, MODULE_MECH, FE_RELAX_EXP, FE_MAT_RV_RELAX, "relaxation-exponential", 0);

FERelaxExp::FERelaxExp() : FEMaterial(FE_RELAX_EXP)
{
    AddDoubleParam(0, "tau"   , "tau"); // characteristic relaxation time
}

//=============================================================================
// Relaxation Exponential Distortion
//=============================================================================

REGISTER_MATERIAL(FERelaxExpDistortion, MODULE_MECH, FE_RELAX_EXP_DIST, FE_MAT_RV_RELAX, "relaxation-exp-distortion", 0);

FERelaxExpDistortion::FERelaxExpDistortion() : FEMaterial(FE_RELAX_EXP_DIST)
{
    AddDoubleParam(0, "tau0"  , "tau0" ); // characteristic relaxation time
    AddDoubleParam(0, "tau1"  , "tau1" );
    AddDoubleParam(0, "alpha" , "alpha");
}

//=============================================================================
// Relaxation Fung
//=============================================================================

REGISTER_MATERIAL(FERelaxFung, MODULE_MECH, FE_RELAX_FUNG, FE_MAT_RV_RELAX, "relaxation-Fung", 0);

FERelaxFung::FERelaxFung() : FEMaterial(FE_RELAX_FUNG)
{
    AddDoubleParam(0, "tau1"   , "tau1"); //  minimum characteristic relaxation time
    AddDoubleParam(0, "tau2"   , "tau2"); // maximum characteristic relaxation time
}

//=============================================================================
// Relaxation Park
//=============================================================================

REGISTER_MATERIAL(FERelaxPark, MODULE_MECH, FE_RELAX_PARK, FE_MAT_RV_RELAX, "relaxation-Park", 0);

FERelaxPark::FERelaxPark() : FEMaterial(FE_RELAX_PARK)
{
    AddDoubleParam(0, "tau"   , "tau" ); // characteristic relaxation time
    AddDoubleParam(0, "beta"  , "beta"); // exponent
}

//=============================================================================
// Relaxation Park Distortion
//=============================================================================

REGISTER_MATERIAL(FERelaxParkDistortion, MODULE_MECH, FE_RELAX_PARK_DIST, FE_MAT_RV_RELAX, "relaxation-Park-distortion", 0);

FERelaxParkDistortion::FERelaxParkDistortion() : FEMaterial(FE_RELAX_PARK_DIST)
{
    AddDoubleParam(0, "tau0"  , "tau0" ); // characteristic relaxation time
    AddDoubleParam(0, "tau1"  , "tau1" );
    AddDoubleParam(0, "beta0" , "beta0"); // exponent
    AddDoubleParam(0, "beta1" , "beta1");
    AddDoubleParam(0, "alpha" , "alpha");
}

//=============================================================================
// Relaxation power
//=============================================================================

REGISTER_MATERIAL(FERelaxPow, MODULE_MECH, FE_RELAX_POW, FE_MAT_RV_RELAX, "relaxation-power", 0);

FERelaxPow::FERelaxPow() : FEMaterial(FE_RELAX_POW)
{
    AddDoubleParam(0, "tau"   , "tau" ); // characteristic relaxation time
    AddDoubleParam(0, "beta"  , "beta"); // exponent
}

//=============================================================================
// Relaxation power distortion
//=============================================================================

REGISTER_MATERIAL(FERelaxPowDistortion, MODULE_MECH, FE_RELAX_POW_DIST, FE_MAT_RV_RELAX, "relaxation-power-distortion", 0);

FERelaxPowDistortion::FERelaxPowDistortion() : FEMaterial(FE_RELAX_POW_DIST)
{
    AddDoubleParam(0, "tau0"  , "tau0" ); // characteristic relaxation time
    AddDoubleParam(0, "tau1"  , "tau1" );
    AddDoubleParam(0, "beta0" , "beta0");
    AddDoubleParam(0, "beta1" , "beta1");
    AddDoubleParam(0, "alpha" , "alpha");
}

//=============================================================================
// Elastic pressure for ideal gas
//=============================================================================

REGISTER_MATERIAL(FEEPIdealGas, MODULE_FLUID, FE_EP_IDEAL_GAS, FE_MAT_FLUID_ELASTIC, "ideal gas", 0);

FEEPIdealGas::FEEPIdealGas() : FEMaterial(FE_EP_IDEAL_GAS)
{
    AddDoubleParam(0, "molar_mass"  , "molar_mass");
}

//=============================================================================
// Elastic pressure for ideal fluid
//=============================================================================

REGISTER_MATERIAL(FEEPIdealFluid, MODULE_FLUID, FE_EP_IDEAL_FLUID, FE_MAT_FLUID_ELASTIC, "ideal fluid", 0);

FEEPIdealFluid::FEEPIdealFluid() : FEMaterial(FE_EP_IDEAL_FLUID)
{
    AddDoubleParam(0, "k"  , "Bulk modulus");
}

//=============================================================================
// Elastic pressure for neo-Hookean fluid
//=============================================================================

REGISTER_MATERIAL(FEEPNeoHookeanFluid, MODULE_FLUID, FE_EP_NEOHOOKEAN_FLUID, FE_MAT_FLUID_ELASTIC, "neo-Hookean fluid", 0);

FEEPNeoHookeanFluid::FEEPNeoHookeanFluid() : FEMaterial(FE_EP_NEOHOOKEAN_FLUID)
{
    AddDoubleParam(0, "k"  , "Bulk modulus");
}

//=============================================================================
// Viscous Newtonian fluid
//=============================================================================

REGISTER_MATERIAL(FEVFNewtonian, MODULE_FLUID, FE_VF_NEWTONIAN, FE_MAT_FLUID_VISCOSITY, "Newtonian fluid", 0);

FEVFNewtonian::FEVFNewtonian() : FEMaterial(FE_VF_NEWTONIAN)
{
    AddDoubleParam(0, "mu"  , "shear viscosity");
    AddDoubleParam(0, "kappa", "bulk viscosity");
}

//=============================================================================
// Viscous Carreau fluid
//=============================================================================

REGISTER_MATERIAL(FEVFCarreau, MODULE_FLUID, FE_VF_CARREAU, FE_MAT_FLUID_VISCOSITY, "Carreau", 0);

FEVFCarreau::FEVFCarreau() : FEMaterial(FE_VF_CARREAU)
{
    AddDoubleParam(0, "mu0" , "mu0"); // viscosity at zero shear rate
    AddDoubleParam(0, "mui" , "mui"); // viscosity at infinite shear rate
    AddDoubleParam(0, "lambda" , "relaxation time"  );
    AddDoubleParam(0, "n" , "power index"  );
}

//=============================================================================
// Viscous Carreau-Yasuda fluid
//=============================================================================

REGISTER_MATERIAL(FEVFCarreauYasuda, MODULE_FLUID, FE_VF_CARREAU_YASUDA, FE_MAT_FLUID_VISCOSITY, "Carreau-Yasuda", 0);

FEVFCarreauYasuda::FEVFCarreauYasuda() : FEMaterial(FE_VF_CARREAU_YASUDA)
{
    AddDoubleParam(0, "mu0" , "viscosity at zero shear rate"  );
    AddDoubleParam(0, "mui" , "viscosity at infinite shear rate"  );
    AddDoubleParam(0, "lambda" , "relaxation time"  );
    AddDoubleParam(0, "n" , "power index"  );
    AddDoubleParam(0, "a" , "power denominator"  );
}

//=============================================================================
// Viscous Powell-Eyring fluid
//=============================================================================

REGISTER_MATERIAL(FEVFPowellEyring, MODULE_FLUID, FE_VF_POWELL_EYRING, FE_MAT_FLUID_VISCOSITY, "Powell-Eyring", 0);

FEVFPowellEyring::FEVFPowellEyring() : FEMaterial(FE_VF_POWELL_EYRING)
{
    AddDoubleParam(0, "mu0" , "viscosity at zero shear rate"  );
    AddDoubleParam(0, "mui" , "viscosity at infinite shear rate"  );
    AddDoubleParam(0, "lambda" , "relaxation time"  );
}

//=============================================================================
// Viscous Cross fluid
//=============================================================================

REGISTER_MATERIAL(FEVFCross, MODULE_FLUID, FE_VF_CROSS, FE_MAT_FLUID_VISCOSITY, "Cross", 0);

FEVFCross::FEVFCross() : FEMaterial(FE_VF_CROSS)
{
    AddDoubleParam(0, "mu0" , "viscosity at zero shear rate"  );
    AddDoubleParam(0, "mui" , "viscosity at infinite shear rate"  );
    AddDoubleParam(0, "lambda" , "relaxation time"  );
    AddDoubleParam(0, "m" , "power"  );
}

//=============================================================================
// Starling solvent supply
//=============================================================================

REGISTER_MATERIAL(FEStarlingSupply, MODULE_MULTIPHASIC, FE_STARLING_SUPPLY, FE_MAT_SOLVENT_SUPPLY, "Starling", 0);

FEStarlingSupply::FEStarlingSupply() : FEMaterial(FE_STARLING_SUPPLY)
{
	AddDoubleParam(0, "kp", "filtration coefficient");
	AddDoubleParam(0, "pv", "external pressure");
}