 /***************************************************************************
 *                                 RasMol                                  *
 *                                                                         *
 *                 Molecular Graphics Visualisation Tool                   *
 *                                                                         *
 * Please read the file RASLIC for the license terms which apply to this   *
 * file (Rasmol license).                                                  *
 ***************************************************************************/

/* Authors:
 * Philippe Valadon, copyright (c) 2000-2007
 */



/////////////////////////////////////////////////////////////////////////////
// datadisplay.cpp 
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RasTop.h"
#include "datadisplay.h"


/////////////////////////////////////////////////////////////////////////////
// CData

IMPLEMENT_DYNCREATE(CData, CObject)

CData::CData()
{	int i = 0;
    Regenerate_Flag = 2;

	//Graphics//
	Data_XRange = 100;	//Dummy values
	Data_YRange = 100;
	Data_ZRange = 100;
	Data_DRange = 10000.0;

	for(i=0; i<10; i++ )
		Data_DialValue[i] = 0.0;

	//molecule//
	Data_Info_filename = "";
	Data_Info_moleculename = "";
	Data_Info_classification = "";
	Data_Info_date = "";
	Data_Info_technique = "";
	Data_Info_identcode = "";

	Data_Info_spacegroup = "";
    Data_Info_cellalpha = 0;
    Data_Info_cellbeta = 0;
    Data_Info_cellgamma = 0;
    Data_Info_cella = 0;
    Data_Info_cellb = 0;
    Data_Info_cellc = 0;
    for ( i=0; i<3; i++ ) {
      Data_Info_vecf2o[i] = Data_Info_veco2f[i] = Data_Info_cell[i] = 0.;
      Data_Info_cell[i+3] = 90.;
      for ( int j=0; j<3; j++) {
        Data_Info_matf2o[i][j] = Data_Info_mato2f[i][j] = ((i!=j)?0.:1.);
      }
    }

    Data_Info_bondcount = 0;
    Data_Info_chaincount = 0;
    Data_Info_ssbondcount = -1;
    Data_Info_hbondcount = -1;
	Data_cisbondcount = -1;

    Data_Info_structsource = SourceNone;
    Data_Info_laddercount = -1;
	Data_Info_helixcount = -1;
    Data_Info_turncount = -1;

	Data_Database = NULL;	
	Data_CurMolecule = NULL;
	Data_CurChain = NULL;
	Data_CurGroup = NULL;
	Data_CurAtom = NULL;

	for (i = 0; i<MAXMASK; i++)
	{	Data_UserMask_radius[i] = 0;
		Data_UserMask_mask[i] = "";
		Data_UserMask_flags[i] = 0;
		Data_UserMask_r[i] = 0;
		Data_UserMask_g[i] = 0;
		Data_UserMask_b[i] = 0;
	}

    Data_MainGroupCount = 0;
	Data_HetaGroupCount = 0;
    Data_HetaAtomCount = 0;
    Data_MainAtomCount = 0;
	Data_CisBondCutOff = CIS;  

    Data_MinX = Data_MinY = Data_MinZ = 0;
    Data_MaxX = Data_MaxY = Data_MaxZ = 0;

    Data_HMinMaxFlag = Data_MMinMaxFlag = 0;
    Data_MinMainTemp = Data_MaxMainTemp = 0;
    Data_MinHetaTemp = Data_MaxHetaTemp = 0;
    Data_MinMainRes = Data_MaxMainRes = 0;
    Data_MinHetaRes = Data_MaxHetaRes = 0;
	Data_MinAltl = Data_MaxAltl = 0;
	Data_MinModel = Data_MaxModel = 0;

	Data_MinBondDist = (Long)100*100;
	Data_MaxBondDist = (Long)475*475;
    Data_ElemNo = MINELEM;
    Data_ResNo = MINRES;
    Data_HasHydrogen = False;
    Data_MaskCount = 0;
    Data_NMRModel = 0;
	Data_NullBonds = 0;
//	Data_MarkAtoms = 0;

	Data_HBondChainsFlag = 0;
    Data_CapClean = 0;

	//Abstree//
	Data_SymbolTable = 0;

	//pixutils//
	Data_SplineCount = 0;
	Data_FontPS = 0;
	Data_FontSize = 8;
	Data_FontStroke = 0;

	//Script//
	Data_KinemageFlag = false;

	//Command//
	//Data_DataFileName = "";
	Data_SelectCount = 0;
	//Data_Interactive = true;
	Data_FileDepth = -1;
	Data_IsPaused = false;
	for( i=0;i<STACKSIZE;i++)
	{	Data_LineStack[i] = 0;
		Data_PosStack[i] = 0;
		Data_AcceptData[i] = '\0';
	}

	Data_CalcBondsFlag = true;

	//Multiple//
	Data_MoleculeList = NULL;
	Data_CurMolSet = NULL;
	Data_CenterSet = NULL;
	Data_RSMPathName = "";
	Data_SpaceName = "";
	Data_FileModified = 0;

	//Transfor//
	for (i = 0; i<LastShade; i++)
	{	Data_Shade[i].refcount = 0;
		Data_Shade[i].r = 0;
		Data_Shade[i].g = 0;
		Data_Shade[i].b = 0;
	}
	Data_RotX[0] = 1.0;
	Data_RotX[1] = Data_RotX[2] = 0.0;
	Data_RotY[0] = Data_RotY[2] = 0.0;
	Data_RotY[1] = 0;
	Data_RotZ[0] = Data_RotZ[1] = 0.0;
	Data_RotZ[2] = 1;
	Data_MOffset[0] = Data_MOffset[1] = Data_MOffset[2] = 0.0;
    Data_MatX[0] = Data_MatX[1] = Data_MatX[2] = 0.0;
    Data_MatY[0] = Data_MatY[1] = Data_MatY[2] = 0.0;
    Data_MatZ[0] = Data_MatZ[1] = Data_MatZ[2] = 0.0;
	Data_OrigCX = 0;
	Data_OrigCY = 0;
	Data_OrigCZ = 0;
	Data_CenX = 0;
	Data_CenY = 0;
	Data_CenZ = 0;
	Data_RCenx = 0.0;
	Data_RCeny = 0.0;
	Data_RCenz = 0.0;
	Data_ShiftS = 0;
    Data_LastTX = Data_LastTY = Data_LastTZ = 0.0;
    Data_LastRX = Data_LastRY = Data_LastRZ = 0.0;
    Data_WLastTX = Data_WLastTY = Data_WLastTZ = 0;
    Data_WLastRX = Data_WLastRY = Data_WLastRZ = 0;
    Data_WTransValue[0] = 0.0;
    Data_WTransValue[1] = 0.0;
    Data_WTransValue[2] = 0.0;
    Data_WRotValue[0] = 0.0;
    Data_WRotValue[1] = 0.0;
    Data_WRotValue[2] = 0.0;
	IdentityMatrix3(Data_WAxes);
	Data_Zoom = 0.0;
	Data_Wo[0] = Data_Wo[1] = Data_Wo[2] = 0.0; 

	Data_ScaleCount = 0;
    for (i=0; i<AltlDepth; i++)
      Data_AltlColours[i] = 0;

	Data_FakeSpecular = false;
	Data_SpecPower = 8;
	Data_ShadePower = 50;
	Data_BackR = Data_BackG = Data_BackB = 0;
	Data_DotR = Data_DotG = Data_DotB = 0;
	Data_LabR = Data_LabG = Data_LabB = 255;
	Data_AxeR =	Data_AxeG =	Data_AxeB = 255;
    Data_BoxCol = 0;
	Data_UseLabelCol = False;
	Data_UseBackFade = False;
	Data_Ambient = DefaultAmbient;
	Data_UseDotCol = DotColNone;
	Data_DotCol = 0;
	Data_LightX = 1;
	Data_LightY = 1;
	Data_LightZ = 2;

	Data_Scale = 0.0;
	Data_MaxZoom = 0.0;
	Data_DScale = 0.0;
	Data_IScale = 0.0;
	Data_SideLen = 0;
	Data_Offset = 0;
	Data_WorldRadius = 0;
	Data_WorldSize = 1;
	Data_LocalRadius = 0;
	Data_XOffset = 0;
	Data_YOffset = 0;
	Data_ZOffset = 0;
	Data_ZoomRange = 0;

	Data_Hydrogens = true;
	Data_HetaGroups = true;	
	Data_DrawAtoms = false;
	Data_DrawBonds = false;
	Data_DrawRibbon = false;
	Data_DrawStars = false;
	Data_ZoneBoth = true;

	//Render//
	Data_UseDepthCue = false;
    Data_DepthCueMode = 1;
	Data_UseStereo = false;
	Data_StereoView = ViewLeft;
	Data_UseShadow = false;
	Data_DisplayMode = 0;
	Data_UseSlabPlane = false;
	Data_UseDepthPlane = false;
	Data_WClip[0] = -999999.;
	Data_WClip[1] = -999999.;
	Data_RefreshState = True;
	Data_SlabMode = SlabClose;
	Data_SSBondMode = false;
	Data_HBondMode = false;
	Data_StereoAngle = 6.0;
    Data_RotMode = RotMol;
	Data_DrawBoundBox = false;
	Data_DrawAxes = false;
	Data_DrawWorldAxes = false;
	Data_DrawDoubleBonds = false;
	Data_DrawUnitCell = false;
	Data_IVoxRatio = 1;
	Data_VoxelsClean = false;
	Data_RotMode = 0;

	//Repres//
	Data_DotPtr = 0;
	Data_MonitList = 0;
	Data_LabelList = 0;
	Data_CartoonHeight = 100;
	Data_SolventDots = false;
	Data_ProbeRadius = 0;
	Data_DotDensity = 100;
	Data_DrawMonitDistance = true;
	Data_DrawBetaArrows = true;

	//Outfile//
	Data_UseTransparent = false;
	Data_UseOutLine = false;

	//brotate//
	Data_BondSelected = NULL;
	Data_RotBondList = NULL;
	Data_BondOrderList = NULL;
	Data_BSrcAtom = NULL;
	Data_BDstAtom = NULL;
	Data_BAxis[0] = 0.0;
	Data_BAxis[1] = 0.0;
	Data_BAxis[2] = 0.0;
	Data_BRotValue = 0;
	Data_BLastRot = -999999.;

}

CData::~CData()	{}

BOOL CData::SaveData()
{
	int i = 0;
    if(Regenerate_Flag != 1)
		Regenerate_Flag = 1;
	else
		return(false);

	//Graphics//
	Data_ZRange = ZRange;
	Data_DRange = DRange;
	for(i=0; i<10; i++ )
		Data_DialValue[i] = DialValue[i];

	//molecule//
	Data_Info_filename = (LPCSTR )Info.filename;
	Data_Info_moleculename = (LPCSTR )Info.moleculename;
	Data_Info_classification = (LPCSTR )Info.classification;
	Data_Info_date = (LPCSTR )Info.date;
	Data_Info_technique = (LPCSTR )Info.technique;
	Data_Info_identcode = (LPCSTR )Info.identcode;

	Data_Info_spacegroup = (LPCSTR )Info.spacegroup;
    Data_Info_cellalpha = Info.cellalpha;
    Data_Info_cellbeta = Info.cellbeta;
    Data_Info_cellgamma = Info.cellgamma;
    Data_Info_cella = Info.cella;
    Data_Info_cellb = Info.cellb;
    Data_Info_cellc = Info.cellc;
    for ( i=0; i<3; i++ ) 
	{	Data_Info_vecf2o[i] = Info.vecf2o[i];
		Data_Info_veco2f[i] = Info.veco2f[i];
		Data_Info_cell[i] = Info.cell[i];
		Data_Info_cell[i+3] = Info.cell[i+3];
		for ( int j=0; j<3; j++) 
		{	Data_Info_matf2o[i][j] = Info.matf2o[i][j];
			Data_Info_mato2f[i][j] = Info.mato2f[i][j];
		}
	}

    Data_Info_bondcount = Info.bondcount;
    Data_Info_chaincount = Info.chaincount;
    Data_Info_ssbondcount = Info.ssbondcount;
    Data_Info_hbondcount = Info.hbondcount;
	Data_cisbondcount = Info.cisbondcount;

    Data_Info_structsource = Info.structsource;
    Data_Info_laddercount = Info.laddercount;
	Data_Info_helixcount = Info.helixcount;
    Data_Info_turncount = Info.turncount;
	
	Data_Database = Database;
		Database = 0;
	Data_CurMolecule = CurMolecule;
		CurMolecule = NULL;
	Data_CurChain = CurChain;
		CurChain = NULL;
	Data_CurGroup = CurGroup;
		CurGroup = NULL;
	Data_CurAtom = CurAtom;
		CurAtom = NULL;

	for (i = 0; i<MAXMASK; i++)
	{	Data_UserMask_radius[i] = UserMask[i].radius;
		Data_UserMask_mask[i] = (LPCSTR )UserMask[i].mask;
		Data_UserMask_flags[i] = UserMask[i].flags;
		Data_UserMask_r[i] = UserMask[i].r;
		Data_UserMask_g[i] = UserMask[i].g;
		Data_UserMask_b[i] = UserMask[i].b;
	}

	for (i = 0; i<(ResNo - MINRES); i++)
	{	Data_Residue[i][0] = Residue[MINRES + i][0];
		Data_Residue[i][1] = Residue[MINRES + i][1];
		Data_Residue[i][2] = Residue[MINRES + i][2];
		Data_Residue[i][3] = Residue[MINRES + i][3];
	}

	for (i = 0 ; i<(ElemNo - MINELEM); i++)
		for( int j = 0; j<12 ; j++ )
			Data_ElemDesc[i][j] = ElemDesc[MINELEM + i][j];

	Data_MainGroupCount = MainGroupCount;
	Data_HetaGroupCount = HetaGroupCount;
	Data_HetaAtomCount = HetaAtomCount;
	Data_MainAtomCount = MainAtomCount;  
	Data_CisBondCutOff = CisBondCutOff;

	Data_MinX = MinX;
	Data_MinY = MinY;
	Data_MinZ = MinZ;
	Data_MaxX = MaxX;
	Data_MaxY = MaxY;
	Data_MaxZ = MaxZ;

	Data_HMinMaxFlag = HMinMaxFlag;
	Data_MMinMaxFlag = MMinMaxFlag;
	Data_MinMainTemp = MinMainTemp;
	Data_MaxMainTemp = MaxMainTemp;
	Data_MinHetaTemp = MinHetaTemp;
	Data_MaxHetaTemp = MaxHetaTemp;
	Data_MinMainRes = MinMainRes;
	Data_MaxMainRes = MaxMainRes;
	Data_MinHetaRes = MinHetaRes;
	Data_MaxHetaRes = MaxHetaRes;
	Data_MinAltl = MinAltl;
	Data_MaxAltl = MaxAltl;

	Data_MinModel = MinModel;
	Data_MaxModel = MaxModel;

	Data_MinBondDist = MinBondDist;
	Data_MaxBondDist = MaxBondDist;
	Data_ElemNo = ElemNo;
	Data_ResNo = ResNo;
	Data_HasHydrogen = HasHydrogen;
	Data_MaskCount = MaskCount;
	Data_NMRModel = NMRModel;
	Data_NullBonds = NullBonds;
//	Data_MarkAtoms = MarkAtoms;

	Data_HBondChainsFlag = HBondChainsFlag;
    Data_CapClean = CapClean;

	//abstree//
	Data_SymbolTable = SymbolTable;
		SymbolTable = 0;

	//pixutils//
	Data_SplineCount = SplineCount;
	Data_FontPS = FontPS;
	Data_FontSize = FontSize;
	Data_FontStroke = FontStroke;

	//Script//
	Data_KinemageFlag = KinemageFlag;

	//Command//
	//Data_DataFileName = (LPCSTR )DataFileName;
	Data_SelectCount = SelectCount;
	//Data_Interactive = Interactive;

	Data_CalcBondsFlag = CalcBondsFlag;

	//Paused scripts
	Data_FileDepth = FileDepth;
	Data_IsPaused = IsPaused;

	if( FileDepth!=-1 )
	{	for( int i=0;i<=FileDepth;i++ )
		{	if( fgetpos(FileStack[i],&Data_PosStack[i]) )
			{	Data_IsPaused = 0;
				Data_FileDepth = -1;
				WriteString(MsgStrs(RTPErrScrData));
				InterruptPauseCommand();
				break;	
			}
			strcpy(Data_NameStack[i],NameStack[i]);	
			Data_LineStack[i] = LineStack[i];
				LineStack[i] = 0;
			Data_AcceptData[i] = AcceptData[i];
				AcceptData[FileDepth] = 'N';
		}
	}

	while( FileDepth>=0 )
    {   fclose(FileStack[FileDepth]);
        free(NameStack[FileDepth]);
        FileDepth--;
    }

	//Multiple//
	Data_MoleculeList = MoleculeList;
		MoleculeList = NULL;
	Data_CurMolSet = CurMolSet;
		CurMolSet = NULL;
	Data_CenterSet = CenterSet;
		CenterSet = NULL;
	Data_RSMPathName = (LPCSTR )RSMPathName;
	Data_SpaceName = (LPCSTR )SpaceName;
	Data_FileModified = FileModified;


	//Transfor//
	for (i = 0; i<LastShade; i++)
		Data_Shade[i] = Shade[i];
	Data_RotX[0] = RotX[0];
	Data_RotX[1] = RotX[1];
	Data_RotX[2] = RotX[2];
	Data_RotY[0] = RotY[0];
	Data_RotY[1] = RotY[1];
	Data_RotY[2] = RotY[2];
	Data_RotZ[0] = RotZ[0];
	Data_RotZ[1] = RotZ[1];
	Data_RotZ[2] = RotZ[2];
	Data_MOffset[0] = MOffset[0];
	Data_MOffset[1] = MOffset[1];
	Data_MOffset[2] = MOffset[2];
    Data_MatX[0] = MatX[0];
    Data_MatX[1] = MatX[1];
    Data_MatX[2] = MatX[2];
    Data_MatY[0] = MatY[0];
    Data_MatY[1] = MatY[1];
    Data_MatY[2] = MatY[2];
    Data_MatZ[0] = MatZ[0];
    Data_MatZ[1] = MatZ[1];
    Data_MatZ[2] = MatZ[2];

	Data_OrigCX = OrigCX;
	Data_OrigCY = OrigCY;
	Data_OrigCZ = OrigCZ;
	Data_CenX = CenX;
	Data_CenY = CenY;
	Data_CenZ = CenZ;
	Data_RCenx = RCenx;
	Data_RCeny = RCeny;
	Data_RCenz = RCenz;
	Data_ShiftS = ShiftS;
    Data_LastTX = LastTX;
	Data_LastTY = LastTY;
	Data_LastTZ = LastTZ;
    Data_LastRX = LastRX;
	Data_LastRY = LastRY;
	Data_LastRZ = LastRZ;
    Data_WLastTX = WLastTX;
	Data_WLastTY = WLastTY;
	Data_WLastTZ = WLastTZ;
    Data_WLastRX = WLastRX;
	Data_WLastRY = WLastRY;
	Data_WLastRZ = WLastRZ;
    for( i=0; i<3; i++ ) Data_WTransValue[i] = WTransValue[i];
    for( i=0; i<3; i++ ) Data_WRotValue[i] = WRotValue[i];
	CopyMatrix3(Data_WAxes,WAxes);
	Data_Zoom = Zoom;
	for( i=0; i<3; i++ ) Data_Wo[i] = Wo[i];

	Data_ScaleCount = ScaleCount;
    for( i=0; i<AltlDepth; i++ )
      Data_AltlColours[i] = AltlColours[i];

	Data_FakeSpecular = FakeSpecular;
	Data_SpecPower = SpecPower;
	Data_ShadePower = ShadePower;
	Data_BackR = BackR;
	Data_BackG = BackG;
	Data_BackB = BackB;
	Data_DotR = DotR;
	Data_DotG = DotG;
	Data_DotB = DotB;
	Data_LabR = LabR;
	Data_LabG = LabG;
	Data_LabB = LabB;
	Data_AxeR =	AxeR;
	Data_AxeG =	AxeG;
	Data_AxeB = AxeB;
    Data_BoxCol = BoxCol;
	Data_UseLabelCol = UseLabelCol;
	Data_UseBackFade = UseBackFade;
	Data_Ambient = Ambient;
	Data_UseDotCol = UseDotCol;
	Data_DotCol = DotCol;
	Data_LightX = LightX;
	Data_LightY = LightY;
	Data_LightZ = LightZ;


	Data_Scale = Scale;
	Data_MaxZoom = MaxZoom;
	Data_DScale = DScale;
	Data_IScale = IScale;
	Data_SideLen = SideLen;
	Data_Offset = Offset;
	Data_WorldRadius = WorldRadius;
	Data_WorldSize = WorldSize;
	Data_LocalRadius = LocalRadius;
	Data_XOffset = XOffset;
	Data_YOffset = YOffset;
	Data_ZOffset = ZOffset;
	Data_ZoomRange = ZoomRange;

	Data_Hydrogens = Hydrogens;
	Data_HetaGroups = HetaGroups;
	Data_DrawAtoms = DrawAtoms;
	Data_DrawBonds = DrawBonds;
	Data_DrawRibbon = DrawRibbon;
	Data_DrawStars = DrawStars;
	Data_ZoneBoth = ZoneBoth;

	//Render//
	Data_UseDepthCue = UseDepthCue;
    Data_DepthCueMode = DepthCueMode;
        SetDepthTable();
	Data_UseStereo = UseStereo;
	Data_StereoView = StereoView;
	Data_UseShadow = UseShadow;
	Data_DisplayMode = DisplayMode;
	Data_UseSlabPlane = UseSlabPlane;
	Data_UseDepthPlane = UseDepthPlane;
	Data_WClip[0] = WClip[0];
	Data_WClip[1] = WClip[1];
	Data_RefreshState = RefreshState;
	Data_SlabMode = SlabMode;
	Data_SSBondMode = SSBondMode;
	Data_HBondMode = HBondMode;
	Data_StereoAngle = StereoAngle;
	Data_RotMode = RotMode;
	Data_DrawBoundBox = DrawBoundBox;
	Data_DrawAxes = DrawAxes;
	Data_DrawWorldAxes = DrawWorldAxes;
	Data_DrawDoubleBonds = DrawDoubleBonds;
	Data_DrawUnitCell = DrawUnitCell;
	Data_IVoxRatio = IVoxRatio;
	Data_VoxelsClean = VoxelsClean;
	Data_RotMode = RotMode;

	//Repres//
	Data_DotPtr = DotPtr;
		DotPtr = 0;
	Data_MonitList = MonitList;
		MonitList = 0;
	Data_LabelList = LabelList;
		LabelList = 0;
	Data_CartoonHeight = CartoonHeight;
	Data_SolventDots = SolventDots;
	Data_ProbeRadius = ProbeRadius;
	Data_DotDensity = DotDensity;
	Data_DotSize = DotSize;
	Data_DrawMonitDistance = DrawMonitDistance;
	Data_DrawBetaArrows = DrawBetaArrows;

	//Outfile//
	Data_UseTransparent = UseTransparent;
	Data_UseOutLine = UseOutLine;

	//brotate//
	Data_BondSelected = BondSelected;
		BondSelected =  (BondRot __far *)NULL;
	Data_RotBondList = RotBondList;
		RotBondList = (BondRot __far *)NULL;
	Data_BondOrderList = BondOrderList;
		BondOrderList = (BondOrder __far *)NULL;
	Data_BSrcAtom = BSrcAtom;
	Data_BDstAtom = BDstAtom;
	Data_BAxis[0] = BAxis[0];
	Data_BAxis[1] = BAxis[1];
	Data_BAxis[2] = BAxis[2];
	Data_BRotValue = BRotValue;
	Data_BLastRot = BLastRot;

	return TRUE;
}

BOOL CData::RegenerateData()
{
	int i = 0;
    if (Regenerate_Flag == 1)
		Regenerate_Flag = 0;
	else
		return(false);

	//Graphics//
	ZRange = Data_ZRange;
	DRange = Data_DRange;
	for(i=0; i<10; i++ )
		DialValue[i] = Data_DialValue[i];
	ReDrawFlag = 0;

	//molecule//
	CopyString(Info.filename, Data_Info_filename);
	CopyString(Info.moleculename, Data_Info_moleculename);
	CopyString(Info.classification, Data_Info_classification);
	CopyString(Info.date, Data_Info_date);
	CopyString(Info.technique, Data_Info_technique);
	CopyString(Info.identcode, Data_Info_identcode);

	CopyString(Info.spacegroup, Data_Info_spacegroup);
    Info.cellalpha = Data_Info_cellalpha;
    Info.cellbeta = Data_Info_cellbeta;
    Info.cellgamma = Data_Info_cellgamma;
    Info.cella = Data_Info_cella;
    Info.cellb = Data_Info_cellb;
    Info.cellc = Data_Info_cellc;

    Info.bondcount = Data_Info_bondcount;
    Info.chaincount = Data_Info_chaincount;
    Info.ssbondcount = Data_Info_ssbondcount;
    Info.hbondcount = Data_Info_hbondcount;
    for ( i=0; i<3; i++ ) 
	{	Info.vecf2o[i] = Data_Info_vecf2o[i];
		Info.veco2f[i] = Data_Info_veco2f[i];
		Info.cell[i] = Data_Info_cell[i];
		Info.cell[i+3] = Data_Info_cell[i+3];
		for ( int j=0; j<3; j++) 
		{	Info.matf2o[i][j] = Data_Info_matf2o[i][j];
			Info.mato2f[i][j] = Data_Info_mato2f[i][j];
		}
	}

    Info.structsource = Data_Info_structsource;
    Info.laddercount = Data_Info_laddercount;
	Info.helixcount = Data_Info_helixcount;
    Info.turncount = Data_Info_turncount;

	Database = (Molecule *)Data_Database;
		Data_Database = 0;
	CurMolecule = Data_CurMolecule;
		Data_CurMolecule = NULL;
	CurChain = Data_CurChain;
		Data_CurChain = NULL;
	CurGroup = Data_CurGroup;
		Data_CurGroup = NULL;
	CurAtom = Data_CurAtom;
		Data_CurAtom = NULL;

	for (i = 0; i<MAXMASK; i++)
	{	UserMask[i].radius = Data_UserMask_radius[i];
		CopyString(UserMask[i].mask, Data_UserMask_mask[i]);
		UserMask[i].flags = Data_UserMask_flags[i];
		UserMask[i].r = Data_UserMask_r[i];
		UserMask[i].g = Data_UserMask_g[i];
		UserMask[i].b = Data_UserMask_b[i];
	}

	for (i = 0; i<(Data_ResNo - MINRES); i++)
	{	Residue[MINRES + i][0] = Data_Residue[i][0];
		Residue[MINRES + i][1] = Data_Residue[i][1];
		Residue[MINRES + i][2] = Data_Residue[i][2];
		Residue[MINRES + i][3] = Data_Residue[i][3];
	}


	for (i = 0 ; i<(Data_ElemNo - MINELEM); i++)
		for( int j = 0; j<12 ; j++ )
			ElemDesc[MINELEM + i][j] = Data_ElemDesc[i][j];

	MainGroupCount = Data_MainGroupCount;
	HetaGroupCount = Data_HetaGroupCount;
	HetaAtomCount = Data_HetaAtomCount;
	MainAtomCount = Data_MainAtomCount;
	CisBondCutOff = Data_CisBondCutOff; 

	MinX = Data_MinX;
	MinY = Data_MinY;
	MinZ = Data_MinZ;
	MaxX = Data_MaxX;
	MaxY = Data_MaxY;
	MaxZ = Data_MaxZ;

	HMinMaxFlag = Data_HMinMaxFlag;
	MMinMaxFlag = Data_MMinMaxFlag;
	MinMainTemp = Data_MinMainTemp;
	MaxMainTemp = Data_MaxMainTemp;
	MinHetaTemp = Data_MinHetaTemp;
	MaxHetaTemp = Data_MaxHetaTemp;
	MinMainRes = Data_MinMainRes;
	MaxMainRes = Data_MaxMainRes;
	MinHetaRes = Data_MinHetaRes;
	MaxHetaRes = Data_MaxHetaRes;
	MinAltl = Data_MinAltl;
	MaxAltl = Data_MaxAltl;

	MinModel = Data_MinModel;
	MaxModel = Data_MaxModel;

	MinBondDist = Data_MinBondDist;
	MaxBondDist = Data_MaxBondDist;
	ElemNo = Data_ElemNo;
	ResNo = Data_ResNo;
	HasHydrogen = Data_HasHydrogen;
	MaskCount = Data_MaskCount;
	NMRModel = Data_NMRModel;
	NullBonds = Data_NullBonds;
//	MarkAtoms = Data_MarkAtoms;

	HBondChainsFlag = Data_HBondChainsFlag;
    CapClean = Data_CapClean;

	//abstree//
	SymbolTable = Data_SymbolTable;
		Data_SymbolTable = 0;

	//pixutils//
	SplineCount = Data_SplineCount;
	if( Data_FontPS )
		SetFontSize( - Data_FontSize );
	else
		SetFontSize( Data_FontSize );
	SetFontStroke( Data_FontStroke );

	//Script//
	KinemageFlag = Data_KinemageFlag;

	//Command//
	//CopyString(DataFileName, Data_DataFileName);
	SelectCount =Data_SelectCount;
	//Interactive = Data_Interactive;

	CalcBondsFlag = Data_CalcBondsFlag;

	//Paused scripts
	FileDepth = Data_FileDepth;
	IsPaused = Data_IsPaused;

	if( Data_FileDepth!=-1 )
	{	for( int i=0;i<=Data_FileDepth;i++ )
		{	FileStack[i] = fopen(Data_NameStack[i],"r");
			if( FileStack[i]==NULL )
			{	Data_IsPaused = 0;
				Data_FileDepth = -1;
				FileDepth = i - 1;
				WriteString(MsgStrs(RTPErrScrData));
				InterruptPauseCommand();
				break;
			}
			int len = 1;
			char *ptr;
			char *name = Data_NameStack[i];
			for( ptr = name; *ptr; ptr++ )
				len++;
	        ptr = (char*)malloc( len );
	        NameStack[i] = ptr;
	        while( (*ptr++ = *name++) );
			fsetpos( FileStack[i], &Data_PosStack[i] );
			Data_PosStack[i] = 0;

			LineStack[i] = Data_LineStack[i];
				Data_LineStack[i] = 0;
			AcceptData[i] = Data_AcceptData[i];
				Data_AcceptData[FileDepth] = 'N';
		}
	}

	//Multiple//
	MoleculeList = (MolSet *)Data_MoleculeList;
		Data_MoleculeList = NULL;
	CurMolSet = (MolSet *)Data_CurMolSet;
		Data_CurMolSet = NULL;
	CenterSet = (MolSet *)Data_CenterSet;
		Data_CenterSet = NULL;
	CopyString(RSMPathName, Data_RSMPathName);
	CopyString(SpaceName, Data_SpaceName);
	FileModified = Data_FileModified;

	//Transfor
	for (i = 0; i<LastShade; i++)
		Shade[i] = Data_Shade[i];
	RotX[0] = Data_RotX[0];
	RotX[1] = Data_RotX[1];
	RotX[2] = Data_RotX[2];
	RotY[0] = Data_RotY[0];
	RotY[1] = Data_RotY[1];
	RotY[2] = Data_RotY[2];
	RotZ[0] = Data_RotZ[0];
	RotZ[1] = Data_RotZ[1];
	RotZ[2] = Data_RotZ[2];
	MOffset[0] = Data_MOffset[0];
	MOffset[1] = Data_MOffset[1];
	MOffset[2] = Data_MOffset[2];
    MatX[0] = Data_MatX[0];
    MatX[1] = Data_MatX[1];
    MatX[2] = Data_MatX[2];
    MatY[0] = Data_MatY[0];
    MatY[1] = Data_MatY[1];
    MatY[2] = Data_MatY[2];
    MatZ[0] = Data_MatZ[0];
    MatZ[1] = Data_MatZ[1];
    MatZ[2] = Data_MatZ[2];

	OrigCX = Data_OrigCX;
	OrigCY = Data_OrigCY;
	OrigCZ = Data_OrigCZ;
	CenX = Data_CenX;
	CenY = Data_CenY;
	CenZ = Data_CenZ;
	RCenx = Data_RCenx;
	RCeny = Data_RCeny;
	RCenz = Data_RCenz;
	ShiftS = Data_ShiftS;
    LastTX = Data_LastTX;
	LastTY = Data_LastTY;
	LastTZ = Data_LastTZ;
    LastRX = Data_LastRX;
	LastRY = Data_LastRY;
	LastRZ = Data_LastRZ;
    WLastTX = Data_WLastTX;
	WLastTY = Data_WLastTY;
	WLastTZ = Data_WLastTZ;
    WLastRX = Data_WLastRX;
	WLastRY = Data_WLastRY;
	WLastRZ = Data_WLastRZ;
    for(i=0; i<3; i++ ) WTransValue[i] = Data_WTransValue[i];
    for(i=0; i<3; i++ ) WRotValue[i] = Data_WRotValue[i];
	CopyMatrix3(WAxes,Data_WAxes);
	Zoom = Data_Zoom;
	for(i=0; i<3; i++ ) Wo[i] = Data_Wo[i];

	ScaleCount = Data_ScaleCount;
    for (i=0; i<AltlDepth; i++)
      AltlColours[i] = Data_AltlColours[i];

	FakeSpecular = Data_FakeSpecular;
	SpecPower = Data_SpecPower;
	ShadePower = Data_ShadePower;
	BackR = Data_BackR;
	BackG = Data_BackG;
	BackB = Data_BackB;
	DotR = Data_DotR;
	DotG = Data_DotG;
	DotB = Data_DotB;
	LabR = Data_LabR;
	LabG = Data_LabG;
	LabB = Data_LabB;
	AxeR =	Data_AxeR;
	AxeG =	Data_AxeG;
	AxeB = Data_AxeB;
    BoxCol = Data_BoxCol;
	UseLabelCol = Data_UseLabelCol;
	UseBackFade = Data_UseBackFade;
	Ambient = Data_Ambient;
	UseDotCol = Data_UseDotCol;
	DotCol = Data_DotCol;
	LightX = Data_LightX;
	LightY = Data_LightY;
	LightZ = Data_LightZ;

	Scale = Data_Scale;
	MaxZoom = Data_MaxZoom;
	DScale = Data_DScale;
	IScale = Data_IScale;
	SideLen = Data_SideLen;
	Offset = Data_Offset;
	WorldRadius = Data_WorldRadius;
	WorldSize = Data_WorldSize;
	LocalRadius = Data_LocalRadius;
	XOffset = Data_XOffset;
	YOffset = Data_YOffset;
	ZOffset = Data_ZOffset;
	ZoomRange = Data_ZoomRange;

	Hydrogens = Data_Hydrogens;
	HetaGroups = Data_HetaGroups;
	DrawAtoms = Data_DrawAtoms;
	DrawBonds = Data_DrawBonds;
	DrawRibbon = Data_DrawRibbon;
	DrawStars = Data_DrawStars;
	ZoneBoth = Data_ZoneBoth;

	//Render//
	UseDepthCue = Data_UseDepthCue;
    DepthCueMode = Data_DepthCueMode;
        SetDepthTable();
	UseStereo = Data_UseStereo;
	StereoView = Data_StereoView;
	UseShadow = Data_UseShadow;
	DisplayMode = Data_DisplayMode;
	UseSlabPlane = Data_UseSlabPlane;
	UseDepthPlane = Data_UseDepthPlane;
	WClip[0] = Data_WClip[0];
	WClip[1] = Data_WClip[1];
	RefreshState = Data_RefreshState;
	SlabMode = Data_SlabMode;
	SSBondMode = Data_SSBondMode;
	HBondMode = Data_HBondMode;
	StereoAngle = Data_StereoAngle;
	RotMode = Data_RotMode;
	PickCount = 0;
	DrawBoundBox = Data_DrawBoundBox;
	DrawAxes = Data_DrawAxes;
	DrawWorldAxes = Data_DrawWorldAxes;
	DrawDoubleBonds = Data_DrawDoubleBonds;
	DrawUnitCell = Data_DrawUnitCell;
	IVoxRatio = Data_IVoxRatio;
	VoxelsClean = Data_VoxelsClean;
	Data_RotMode = RotMode;
	BuckY = 0;
	ItemX = 0;
	FBufX = 1000000;


	//Repres//
	DotPtr = Data_DotPtr;
		Data_DotPtr = 0;
	MonitList = Data_MonitList;
		Data_MonitList = 0;
	LabelList = Data_LabelList;
		Data_LabelList = 0;
	CartoonHeight = Data_CartoonHeight;
	SolventDots = Data_SolventDots;
	ProbeRadius = Data_ProbeRadius;
	DotDensity = Data_DotDensity;
	DotSize = Data_DotSize;
	DrawMonitDistance = Data_DrawMonitDistance;
	DrawBetaArrows = Data_DrawBetaArrows;

	//Outfile//
	UseTransparent = Data_UseTransparent;
	UseOutLine = Data_UseOutLine;

	//brotate//
	BondSelected = Data_BondSelected;
	RotBondList = Data_RotBondList;
	BondOrderList = Data_BondOrderList;
	BSrcAtom = Data_BSrcAtom;
	BDstAtom = Data_BDstAtom;
	BAxis[0] = Data_BAxis[0];
	BAxis[1] = Data_BAxis[1];
	BAxis[2] = Data_BAxis[2];
	BRotValue = Data_BRotValue;
	BLastRot = Data_BLastRot;

	return true;
}

void CData::CopyString(char *des, CString src)
{	
	char *ptr = (LPTSTR) src.LockBuffer();
	while( *ptr )
		*des++ = *ptr++;
	*des = '\0';
	src.UnlockBuffer();
}
