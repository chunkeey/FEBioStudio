#include "stdafx.h"
#include "FEProject.h"
#include <FEMLib/FEMKernel.h>
#include <FEMLib/FESurfaceLoad.h>
#include <FEMLib/FEBodyLoad.h>
#include <FEMLib/FERigidConstraint.h>
#include <FEMLib/FEMultiMaterial.h>
#include <FEMLib/FEModelConstraint.h>
#include "GGroup.h"
#include "GModel.h"
#include <GeomLib/GObject.h>
#include <string>
using namespace std;

//=================================================================================================
CLogDataSettings::CLogDataSettings()
{
}

void CLogDataSettings::RemoveLogData(int item)
{
	m_log.erase(m_log.begin() + item);
}

void CLogDataSettings::Save(OArchive& ar)
{
	const int N = (int) m_log.size();
	for (int i = 0; i<N; ++i)
	{
		FELogData& v = m_log[i];
		ar.BeginChunk(CID_PRJ_LOGDATA_ITEM);
		{
			ar.WriteChunk(CID_PRJ_LOGDATA_TYPE, v.type);
			ar.WriteChunk(CID_PRJ_LOGDATA_DATA, v.sdata);
			ar.WriteChunk(CID_PRJ_LOGDATA_MID , v.matID);
			ar.WriteChunk(CID_PRJ_LOGDATA_GID , v.groupID);
		}
		ar.EndChunk();
	}
}

void CLogDataSettings::Load(IArchive& ar)
{
	while (IArchive::IO_OK == ar.OpenChunk())
	{
		if (ar.GetChunkID() == CID_PRJ_LOGDATA_ITEM)
		{
			string tmp;
			int ntype;
			int mid = -1, gid = -1;
			while (IArchive::IO_OK == ar.OpenChunk())
			{
				switch (ar.GetChunkID())
				{
				case CID_PRJ_LOGDATA_TYPE: ar.read(ntype); break;
				case CID_PRJ_LOGDATA_DATA: ar.read(tmp); break;
				case CID_PRJ_LOGDATA_MID : ar.read(mid); break;
				case CID_PRJ_LOGDATA_GID : ar.read(gid); break;
				}
				ar.CloseChunk();
			}

			FELogData d;
			d.type = ntype;
			d.sdata = tmp;
			d.matID = mid;
			d.groupID = gid;
			AddLogData(d);
		}
		ar.CloseChunk();
	}
}

//=================================================================================================
//-----------------------------------------------------------------------------
// constructor
FEProject::FEProject(void) : m_plt(*this)
{
	// set the title
	m_title = "untitled";

	// activate all modules
	m_module = MODULE_ALL;

	static bool init = false;
	if (init == false)
	{
		InitModules();
		init = true;
	}
}

//-----------------------------------------------------------------------------
// destructor
FEProject::~FEProject(void)
{
	
}

//-----------------------------------------------------------------------------
void FEProject::Reset()
{
	m_fem.New();
	m_plt.Init();
}

//-----------------------------------------------------------------------------
unsigned int FEProject::GetModule() const
{
	return m_module;
}

//-----------------------------------------------------------------------------
void FEProject::SetModule(unsigned int mod)
{
	m_module = mod;
}

//-----------------------------------------------------------------------------
void FEProject::SetTitle(const std::string& title)
{ 
	m_title = title; 
}

//-----------------------------------------------------------------------------
// save project data to archive
void FEProject::Save(OArchive& ar)
{
	// save the title
	ar.WriteChunk(CID_PRJ_TITLE   , m_title);

	// save the modules flag
	ar.WriteChunk(CID_PRJ_MODULES, m_module);

	// save the model data
	ar.BeginChunk(CID_FEM);
	{
		m_fem.Save(ar);
	}
	ar.EndChunk();

	// save the output data
	int N = m_plt.PlotVariables();
	if (N > 0)
	{
		ar.BeginChunk(CID_PRJ_OUTPUT);
		{
			m_plt.Save(ar);
		}
		ar.EndChunk();
	}

	// save the logfile data
	N = m_log.LogDataSize();
	if (N > 0)
	{
		ar.BeginChunk(CID_PRJ_LOGDATA);
		{
			m_log.Save(ar);
		}
		ar.EndChunk();
	}
}

//-----------------------------------------------------------------------------
// load project data from archive
void FEProject::Load(IArchive &ar)
{
	TRACE("FEProject::Load");

	while (IArchive::IO_OK == ar.OpenChunk())
	{
		int nid = ar.GetChunkID();
		switch (nid)
		{
		case CID_PRJ_TITLE  : ar.read(m_title); break;
		case CID_PRJ_MODULES: ar.read(m_module); break;
		case CID_FEM        : m_fem.Load(ar); break;
		case CID_PRJ_OUTPUT : m_plt.Load(ar); break;
		case CID_PRJ_LOGDATA: m_log.Load(ar); break;
		}
		ar.CloseChunk();
	}
}

//-----------------------------------------------------------------------------
// Validate the project, that is try to catch problems that FEBio won't like
// returns 0 on success, non-zero on failure
int FEProject::Validate(std::string &szerr)
{
	szerr = "ERRORS:\n";
	GModel& mdl = m_fem.GetModel();

	// --- collect metrics ---
	int nerrs = 0;

	// number of objects
	int nobj = mdl.Objects();
	if (nobj   ==  0) { szerr += "-No geometry is defined for this model.\n"; nerrs++; }

	// parts that don't have a material assigned
	int npnm = 0; 
	for (int i=0; i<mdl.Parts(); ++i)
	{
		GPart* pg = mdl.Part(i);
		if (pg->GetMaterialID() == -1) npnm++;
	}
	if (npnm   >   0) { szerr += "-Not all parts or objects have a material assigned.\n"; nerrs++; }

	// numer of analysis steps
	int nsteps = m_fem.Steps() - 1;
	if (nsteps <=  0) { szerr += "-At least one analysis step is required to run the model.\n"; nerrs++; }

	// number of objects that have no mesh
	int nnomesh = 0;
	for (int i=0; i<mdl.Objects(); ++i)
	{
		FEMesh* pm = mdl.Object(i)->GetFEMesh();
		if (pm == 0) nnomesh++;
	}
	if (nnomesh > 0) { szerr += "-Not all objects have a mesh.\n"; nerrs++; }

	return nerrs;
}

void FEProject::InitModules()
{
	// register material categories
	FEMaterialFactory::AddCategory("elastic"           , MODULE_MECH     , FE_MAT_ELASTIC);
	FEMaterialFactory::AddCategory("uncoupled elastic" , MODULE_MECH     , FE_MAT_ELASTIC_UNCOUPLED);
	FEMaterialFactory::AddCategory("multiphasic"       , MODULE_BIPHASIC , FE_MAT_MULTIPHASIC);
	FEMaterialFactory::AddCategory("heat transfer"     , MODULE_HEAT     , FE_MAT_HEAT_TRANSFER);
	FEMaterialFactory::AddCategory("fluid"             , MODULE_FLUID    , FE_MAT_FLUID);
    FEMaterialFactory::AddCategory("fluid-FSI"         , MODULE_FLUID_FSI, FE_MAT_FLUID_FSI);
	FEMaterialFactory::AddCategory("reaction-diffusion", MODULE_REACTION_DIFFUSION	, FE_MAT_REACTION_DIFFUSION);
	FEMaterialFactory::AddCategory("other"             , MODULE_MECH				, FE_MAT_RIGID);

	// --- MECH MODULE ---
	REGISTER_FE_CLASS(FENonLinearMechanics         , MODULE_MECH, FE_ANALYSIS         , FE_STEP_MECHANICS               , "Structural Mechanics");
	REGISTER_FE_CLASS(FEFixedDisplacement          , MODULE_MECH, FE_ESSENTIAL_BC     , FE_FIXED_DISPLACEMENT           , "Fixed displacement");
	REGISTER_FE_CLASS(FEFixedShellDisplacement     , MODULE_MECH, FE_ESSENTIAL_BC     , FE_FIXED_SHELL_DISPLACEMENT     , "Fixed shell displacement");
	REGISTER_FE_CLASS(FEFixedRotation              , MODULE_MECH, FE_ESSENTIAL_BC     , FE_FIXED_ROTATION               , "Fixed shell rotation");
	REGISTER_FE_CLASS(FEPrescribedDisplacement     , MODULE_MECH, FE_ESSENTIAL_BC     , FE_PRESCRIBED_DISPLACEMENT      , "Prescribed displacement");
	REGISTER_FE_CLASS(FEPrescribedShellDisplacement, MODULE_MECH, FE_ESSENTIAL_BC     , FE_PRESCRIBED_SHELL_DISPLACEMENT, "Prescribed shell displacement");
	REGISTER_FE_CLASS(FEPrescribedRotation         , MODULE_MECH, FE_ESSENTIAL_BC     , FE_PRESCRIBED_ROTATION          , "Prescribed shell rotation");
	REGISTER_FE_CLASS(FEPressureLoad               , MODULE_MECH, FE_SURFACE_LOAD     , FE_PRESSURE_LOAD                , "Pressure");
	REGISTER_FE_CLASS(FESurfaceTraction            , MODULE_MECH, FE_SURFACE_LOAD     , FE_SURFACE_TRACTION             , "Surface traction");
	REGISTER_FE_CLASS(FENodalVelocities            , MODULE_MECH, FE_INITIAL_CONDITION, FE_NODAL_VELOCITIES             , "Velocity");
	REGISTER_FE_CLASS(FENodalShellVelocities       , MODULE_MECH, FE_INITIAL_CONDITION, FE_NODAL_SHELL_VELOCITIES       , "Shell velocity");
	REGISTER_FE_CLASS(FERigidInterface             , MODULE_MECH, FE_INTERFACE        , FE_RIGID_INTERFACE              , "Rigid");
	REGISTER_FE_CLASS(FESlidingWithGapsInterface   , MODULE_MECH, FE_INTERFACE        , FE_SLIDING_WITH_GAPS            , "Sliding node-on-facet");
	REGISTER_FE_CLASS(FEFacetOnFacetInterface      , MODULE_MECH, FE_INTERFACE        , FE_FACET_ON_FACET_SLIDING       , "Sliding facet-on-facet");
    REGISTER_FE_CLASS(FETensionCompressionInterface, MODULE_MECH, FE_INTERFACE        , FE_TENSCOMP_INTERFACE           , "Sliding elastic");
	REGISTER_FE_CLASS(FETiedInterface              , MODULE_MECH, FE_INTERFACE        , FE_TIED_INTERFACE               , "Tied node-on-facet");
	REGISTER_FE_CLASS(FEF2FTiedInterface           , MODULE_MECH, FE_INTERFACE        , FE_FACET_ON_FACET_TIED          , "Tied facet-on-facet");
    REGISTER_FE_CLASS(FETiedElasticInterface       , MODULE_MECH, FE_INTERFACE        , FE_TIED_ELASTIC_INTERFACE       , "Tied elastic");
	REGISTER_FE_CLASS(FEStickyInterface            , MODULE_MECH, FE_INTERFACE        , FE_STICKY_INTERFACE             , "Sticky");
	REGISTER_FE_CLASS(FEPeriodicBoundary           , MODULE_MECH, FE_INTERFACE        , FE_PERIODIC_BOUNDARY            , "Periodic boundary");
	REGISTER_FE_CLASS(FERigidWallInterface         , MODULE_MECH, FE_INTERFACE        , FE_RIGID_WALL                   , "Rigid wall");
	REGISTER_FE_CLASS(FERigidSphereInterface       , MODULE_MECH, FE_INTERFACE        , FE_RIGID_SPHERE_CONTACT         , "Rigid sphere");
	REGISTER_FE_CLASS(FERigidJoint                 , MODULE_MECH, FE_INTERFACE        , FE_RIGID_JOINT                  , "Rigid joint");
	REGISTER_FE_CLASS(FEBodyForce                  , MODULE_MECH, FE_BODY_LOAD        , FE_BODY_FORCE                   , "Body force");

	REGISTER_FE_CLASS(FERigidFixed			, MODULE_MECH, FE_RIGID_CONSTRAINT, FE_RIGID_FIXED				, "Fixed rigid displacement/rotation");
	REGISTER_FE_CLASS(FERigidDisplacement	, MODULE_MECH, FE_RIGID_CONSTRAINT, FE_RIGID_DISPLACEMENT		, "Prescribed rigid displacement/rotation");
	REGISTER_FE_CLASS(FERigidForce			, MODULE_MECH, FE_RIGID_CONSTRAINT, FE_RIGID_FORCE				, "Prescribed rigid force");
	REGISTER_FE_CLASS(FERigidVelocity		, MODULE_MECH, FE_RIGID_CONSTRAINT, FE_RIGID_INIT_VELOCITY		, "Initial rigid velocity");
	REGISTER_FE_CLASS(FERigidAngularVelocity, MODULE_MECH, FE_RIGID_CONSTRAINT, FE_RIGID_INIT_ANG_VELOCITY	, "Initial rigid angular velocity");

	REGISTER_FE_CLASS(FERigidSphericalJoint		, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_SPHERICAL_JOINT	, "Spherical joint");
	REGISTER_FE_CLASS(FERigidRevoluteJoint		, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_REVOLUTE_JOINT		, "Revolute joint" );
	REGISTER_FE_CLASS(FERigidPrismaticJoint		, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_PRISMATIC_JOINT	, "Prismatic joint");
	REGISTER_FE_CLASS(FERigidCylindricalJoint	, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_CYLINDRICAL_JOINT	, "Cylindrical joint");
	REGISTER_FE_CLASS(FERigidPlanarJoint		, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_PLANAR_JOINT		, "Planar joint");
    REGISTER_FE_CLASS(FERigidLock               , MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_RIGID_LOCK         , "Rigid lock");
	REGISTER_FE_CLASS(FERigidSpring				, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_SPRING				, "Spring");
	REGISTER_FE_CLASS(FERigidDamper				, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_DAMPER				, "Damper");
	REGISTER_FE_CLASS(FERigidAngularDamper		, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_ANGULAR_DAMPER		, "Angular damper");
	REGISTER_FE_CLASS(FERigidContractileForce	, MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_CONTRACTILE_FORCE	, "Contractile force");
	REGISTER_FE_CLASS(FEGenericRigidJoint       , MODULE_MECH, FE_RIGID_CONNECTOR, FE_RC_GENERIC_JOINT      , "Generic joint");

	REGISTER_FE_CLASS(FESymmetryPlane    , MODULE_MECH, FE_CONSTRAINT, FE_SYMMETRY_PLANE   , "symmetry plane");
	REGISTER_FE_CLASS(FEVolumeConstraint , MODULE_MECH, FE_CONSTRAINT, FE_VOLUME_CONSTRAINT, "volume constraint");
	REGISTER_FE_CLASS(FEWarpingConstraint, MODULE_MECH, FE_CONSTRAINT, FE_WARP_CONSTRAINT  , "warp-image");

	// --- HEAT MODULE ---
	REGISTER_FE_CLASS(FEHeatTransfer         , MODULE_HEAT, FE_ANALYSIS         , FE_STEP_HEAT_TRANSFER    , "Heat Transfer");
	REGISTER_FE_CLASS(FEFixedTemperature     , MODULE_HEAT, FE_ESSENTIAL_BC     , FE_FIXED_TEMPERATURE     , "Zero temperature");
	REGISTER_FE_CLASS(FEPrescribedTemperature, MODULE_HEAT, FE_ESSENTIAL_BC     , FE_PRESCRIBED_TEMPERATURE, "Prescribed temperature");
	REGISTER_FE_CLASS(FEHeatFlux             , MODULE_HEAT, FE_SURFACE_LOAD     , FE_HEAT_FLUX             , "Heat flux");
	REGISTER_FE_CLASS(FEConvectiveHeatFlux   , MODULE_HEAT, FE_SURFACE_LOAD     , FE_CONV_HEAT_FLUX        , "Convective heat flux");
	REGISTER_FE_CLASS(FEInitTemperature      , MODULE_HEAT, FE_INITIAL_CONDITION, FE_INIT_TEMPERATURE      , "Temperature");
	REGISTER_FE_CLASS(FEHeatSource           , MODULE_HEAT, FE_BODY_LOAD        , FE_HEAT_SOURCE           , "Heat source");
	REGISTER_FE_CLASS(FEGapHeatFluxInterface , MODULE_HEAT, FE_INTERFACE        , FE_GAPHEATFLUX_INTERFACE , "Gap heat flux");

	// --- BIPHASIC MODULE ---
	REGISTER_FE_CLASS(FENonLinearBiphasic      , MODULE_BIPHASIC, FE_ANALYSIS         , FE_STEP_BIPHASIC            , "Biphasic");
	REGISTER_FE_CLASS(FEFixedFluidPressure     , MODULE_BIPHASIC, FE_ESSENTIAL_BC     , FE_FIXED_FLUID_PRESSURE     , "Zero fluid pressure");
	REGISTER_FE_CLASS(FEPrescribedFluidPressure, MODULE_BIPHASIC, FE_ESSENTIAL_BC     , FE_PRESCRIBED_FLUID_PRESSURE, "Prescribed fluid pressure");
	REGISTER_FE_CLASS(FEFluidFlux              , MODULE_BIPHASIC, FE_SURFACE_LOAD     , FE_FLUID_FLUX               , "Fluid flux");
	REGISTER_FE_CLASS(FEBPNormalTraction       , MODULE_BIPHASIC, FE_SURFACE_LOAD     , FE_BP_NORMAL_TRACTION       , "Mixture normal traction");
	REGISTER_FE_CLASS(FEInitFluidPressure      , MODULE_BIPHASIC, FE_INITIAL_CONDITION, FE_INIT_FLUID_PRESSURE      , "Fluid pressure");
    REGISTER_FE_CLASS(FEInitShellFluidPressure , MODULE_BIPHASIC, FE_INITIAL_CONDITION, FE_INIT_SHELL_FLUID_PRESSURE, "Shell fluid pressure");
    REGISTER_FE_CLASS(FEPoroContact            , MODULE_BIPHASIC, FE_INTERFACE        , FE_PORO_INTERFACE           , "Biphasic contact");
    REGISTER_FE_CLASS(FETiedBiphasicInterface  , MODULE_BIPHASIC, FE_INTERFACE        , FE_TIEDBIPHASIC_INTERFACE   , "Tied biphasic contact");

	// --- SOLUTES MODULE ---
	REGISTER_FE_CLASS(FEFixedConcentration      , MODULE_SOLUTES, FE_ESSENTIAL_BC     , FE_FIXED_CONCENTRATION      , "Zero concentration");
	REGISTER_FE_CLASS(FEPrescribedConcentration , MODULE_SOLUTES, FE_ESSENTIAL_BC     , FE_PRESCRIBED_CONCENTRATION , "Prescribed concentration");
	REGISTER_FE_CLASS(FEInitConcentration       , MODULE_SOLUTES, FE_INITIAL_CONDITION, FE_INIT_CONCENTRATION       , "Concentration");
    REGISTER_FE_CLASS(FEInitShellConcentration  , MODULE_SOLUTES, FE_INITIAL_CONDITION, FE_INIT_SHELL_CONCENTRATION , "Shell concentration");

	// --- MULTIPHASIC MODULE ---
	REGISTER_FE_CLASS(FEBiphasicSolutes         , MODULE_MULTIPHASIC, FE_ANALYSIS         , FE_STEP_BIPHASIC_SOLUTE     , "Biphasic-solute");
	REGISTER_FE_CLASS(FEMultiphasicAnalysis     , MODULE_MULTIPHASIC, FE_ANALYSIS         , FE_STEP_MULTIPHASIC         , "Multiphasic");
	REGISTER_FE_CLASS(FEPoroSoluteContact       , MODULE_MULTIPHASIC, FE_INTERFACE        , FE_PORO_SOLUTE_INTERFACE    , "Biphasic-solute contact");
	REGISTER_FE_CLASS(FEMultiphasicContact      , MODULE_MULTIPHASIC, FE_INTERFACE        , FE_MULTIPHASIC_INTERFACE    , "Multiphasic contact");
	REGISTER_FE_CLASS(FETiedMultiphasicInterface, MODULE_MULTIPHASIC, FE_INTERFACE        , FE_TIEDMULTIPHASIC_INTERFACE, "Tied multiphasic contact");
	REGISTER_FE_CLASS(FESoluteFlux              , MODULE_MULTIPHASIC, FE_SURFACE_LOAD     , FE_SOLUTE_FLUX              , "Solute flux");

#ifdef _DEBUG
	REGISTER_FE_CLASS(FESBMPointSource, MODULE_MULTIPHASIC, FE_BODY_LOAD, FE_SBM_POINT_SOURCE, "SBM point source");
#endif

	// --- FLUID MODULE ---
	REGISTER_FE_CLASS(FEFluidAnalysis           , MODULE_FLUID, FE_ANALYSIS    , FE_STEP_FLUID                  , "Fluid Mechanics");
    REGISTER_FE_CLASS(FEFixedFluidVelocity      , MODULE_FLUID, FE_ESSENTIAL_BC, FE_FIXED_FLUID_VELOCITY        , "Zero fluid velocity");
    REGISTER_FE_CLASS(FEPrescribedFluidVelocity , MODULE_FLUID, FE_ESSENTIAL_BC, FE_PRESCRIBED_FLUID_VELOCITY   , "Prescribed fluid velocity");
	REGISTER_FE_CLASS(FEFixedFluidDilatation    , MODULE_FLUID, FE_ESSENTIAL_BC, FE_FIXED_DILATATION            , "Zero fluid dilatation");
	REGISTER_FE_CLASS(FEPrescribedFluidDilatation,MODULE_FLUID, FE_ESSENTIAL_BC, FE_PRESCRIBED_DILATATION       , "Prescribed fluid dilatation");
	REGISTER_FE_CLASS(FEFluidTraction           , MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_TRACTION              , "Fluid viscous traction");
    REGISTER_FE_CLASS(FEFluidVelocity           , MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_VELOCITY              , "Fluid velocity");
    REGISTER_FE_CLASS(FEFluidNormalVelocity     , MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_NORMAL_VELOCITY       , "Fluid normal velocity");
    REGISTER_FE_CLASS(FEFluidRotationalVelocity , MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_ROTATIONAL_VELOCITY   , "Fluid rotational velocity");
    REGISTER_FE_CLASS(FEFluidFlowResistance     , MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_FLOW_RESISTANCE       , "Fluid flow resistance");
    REGISTER_FE_CLASS(FEFluidBackflowStabilization, MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_BACKFLOW_STABIL     , "Fluid back flow stabilization");
    REGISTER_FE_CLASS(FEFluidTangentialStabilization, MODULE_FLUID, FE_SURFACE_LOAD, FE_FLUID_TANGENTIAL_STABIL , "Fluid tangential stabilization");

	REGISTER_FE_CLASS(FENormalFlowSurface       , MODULE_FLUID, FE_CONSTRAINT  , FE_NORMAL_FLUID_FLOW           , "Normal flow constraint");
    REGISTER_FE_CLASS(FEFrictionlessFluidWall   , MODULE_FLUID, FE_CONSTRAINT  , FE_FRICTIONLESS_FLUID_WALL     , "Frictionless fluid wall");

    // --- FLUID-FSI MODULE ---
    REGISTER_FE_CLASS(FEFluidFSIAnalysis        , MODULE_FLUID_FSI, FE_ANALYSIS    , FE_STEP_FLUID_FSI   , "Fluid-FSI Mechanics");
    REGISTER_FE_CLASS(FEFSITraction             , MODULE_FLUID_FSI, FE_SURFACE_LOAD, FE_FSI_TRACTION     , "FSI Interface Traction");

	// --- REACTION-DIFFUSION MODULE ---
	REGISTER_FE_CLASS(FEReactionDiffusionAnalysis, MODULE_REACTION_DIFFUSION, FE_ANALYSIS, FE_STEP_REACTION_DIFFUSION, "Reaction-Diffusion");
	REGISTER_FE_CLASS(FEConcentrationFlux        , MODULE_REACTION_DIFFUSION, FE_SURFACE_LOAD, FE_CONCENTRATION_FLUX, "Concentration Flux");
}

//-------------------------------------------------------------------------------------------------
void FEProject::ActivatePlotVariables(FEAnalysisStep* pstep)
{
	FEModel* pfem = pstep->GetFEModel();

	// add default plot variables for step
	CPlotDataSettings& plt = GetPlotDataSettings();
	switch (pstep->GetType())
	{
	case FE_STEP_MECHANICS:
		plt.FindVariable("displacement")->setActive(true);
		plt.FindVariable("stress")->setActive(true);
		if (pstep->GetSettings().nanalysis == FE_DYNAMIC)
		{
			plt.FindVariable("velocity")->setActive(true);
			plt.FindVariable("acceleration")->setActive(true);
		}
		break;
	case FE_STEP_HEAT_TRANSFER:
		plt.FindVariable("temperature")->setActive(true);
		break;
	case FE_STEP_BIPHASIC:
		plt.FindVariable("displacement")->setActive(true);
		plt.FindVariable("stress")->setActive(true);
		plt.FindVariable("effective fluid pressure")->setActive(true);
		plt.FindVariable("fluid flux")->setActive(true);
		break;
	case FE_STEP_BIPHASIC_SOLUTE:
	case FE_STEP_MULTIPHASIC:
		plt.FindVariable("displacement")->setActive(true);
		plt.FindVariable("stress")->setActive(true);
		plt.FindVariable("fluid flux")->setActive(true);
		plt.FindVariable("effective fluid pressure")->setActive(true);
		plt.FindVariable("effective solute concentration")->setActive(true);
		plt.FindVariable("solute concentration")->setActive(true);
		plt.FindVariable("solute flux")->setActive(true);
/*		for (int i = 0; i<pfem->Solutes(); ++i)
		{
			char sz[256] = { 0 };
			FEPlotVariable* pv = 0;

			sprintf(sz, "effective solute %d concentration", i + 1);
			pv = plt.FindVariable(sz);
			if (pv) pv->setActive(true);

			sprintf(sz, "solute %d concentration", i + 1);
			pv = plt.FindVariable(sz);
			if (pv) pv->setActive(true);

			sprintf(sz, "solute %d flux", i + 1);
			pv = plt.FindVariable(sz);
			if (pv) pv->setActive(true);
		}
*/
		break;
	case FE_STEP_REACTION_DIFFUSION:
		plt.SetAllVariables(false);
		plt.SetAllModuleVariables(MODULE_REACTION_DIFFUSION, true);
		break;
	case FE_STEP_FLUID:
		plt.FindVariable("displacement")->setActive(true);
		plt.FindVariable("fluid pressure", MODULE_FLUID)->setActive(true);
		plt.FindVariable("nodal fluid velocity")->setActive(true);
		plt.FindVariable("fluid stress")->setActive(true);
		plt.FindVariable("fluid velocity")->setActive(true);
		plt.FindVariable("fluid acceleration")->setActive(true);
		plt.FindVariable("fluid vorticity")->setActive(true);
		plt.FindVariable("fluid rate of deformation")->setActive(true);
		plt.FindVariable("fluid stress power density")->setActive(false);
		plt.FindVariable("fluid heat supply density")->setActive(false);
		plt.FindVariable("fluid density")->setActive(false);
		plt.FindVariable("fluid dilatation")->setActive(true);
		plt.FindVariable("fluid volume ratio")->setActive(true);
		plt.FindVariable("fluid surface force")->setActive(false);
		plt.FindVariable("fluid surface traction power")->setActive(false);
		plt.FindVariable("fluid surface energy flux")->setActive(false);
		plt.FindVariable("fluid shear viscosity")->setActive(false);
		plt.FindVariable("fluid mass flow rate")->setActive(false);
		plt.FindVariable("fluid strain energy density")->setActive(false);
		plt.FindVariable("fluid kinetic energy density")->setActive(false);
		plt.FindVariable("fluid energy density")->setActive(false);
		plt.FindVariable("fluid element strain energy")->setActive(false);
		plt.FindVariable("fluid element kinetic energy")->setActive(false);
		plt.FindVariable("fluid element linear momentum")->setActive(false);
		plt.FindVariable("fluid element angular momentum")->setActive(false);
		plt.FindVariable("fluid element center of mass")->setActive(false);
		break;
	case FE_STEP_FLUID_FSI:
		plt.FindVariable("displacement")->setActive(true);
		plt.FindVariable("velocity")->setActive(true);
		plt.FindVariable("acceleration")->setActive(true);
		plt.FindVariable("fluid pressure", MODULE_FLUID)->setActive(true);
		plt.FindVariable("nodal fluid velocity")->setActive(true);
		plt.FindVariable("fluid stress")->setActive(true);
		plt.FindVariable("fluid velocity")->setActive(true);
		plt.FindVariable("fluid acceleration")->setActive(true);
		plt.FindVariable("fluid vorticity")->setActive(true);
		plt.FindVariable("fluid rate of deformation")->setActive(true);
		plt.FindVariable("fluid stress power density")->setActive(false);
		plt.FindVariable("fluid heat supply density")->setActive(false);
		plt.FindVariable("fluid density")->setActive(false);
		plt.FindVariable("fluid dilatation")->setActive(true);
		plt.FindVariable("fluid volume ratio")->setActive(true);
		plt.FindVariable("fluid surface force")->setActive(false);
		plt.FindVariable("fluid surface traction power")->setActive(false);
		plt.FindVariable("fluid surface energy flux")->setActive(false);
		plt.FindVariable("fluid shear viscosity")->setActive(false);
		plt.FindVariable("fluid mass flow rate")->setActive(false);
		plt.FindVariable("fluid strain energy density")->setActive(false);
		plt.FindVariable("fluid kinetic energy density")->setActive(false);
		plt.FindVariable("fluid energy density")->setActive(false);
		plt.FindVariable("fluid element strain energy")->setActive(false);
		plt.FindVariable("fluid element kinetic energy")->setActive(false);
		plt.FindVariable("fluid element linear momentum")->setActive(false);
		plt.FindVariable("fluid element angular momentum")->setActive(false);
		plt.FindVariable("fluid element center of mass")->setActive(false);
		plt.FindVariable("relative fluid velocity")->setActive(true);
		plt.FindVariable("nodal relative fluid velocity")->setActive(true);
		break;
	}
}
