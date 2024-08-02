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
// datadisplay.h : header file
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATADISPLAY_H_INCLUDED_)
#define AFX_DATADISPLAY_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

extern "C"
{
#include "../rasmol/rasmol.h"
#include "../rasmol/prefer.h"
#include "../rasmol/molecule.h"
#include "../rasmol/abstree.h"
#include "../rasmol/command.h"
#include "../rasmol/outfile.h"
#include "../rasmol/pixutils.h"
#include "../rasmol/script.h"
#include "../rasmol/render.h"
#include "../rasmol/transfor.h"
#include "../rasmol/cmndline.h"
#include "../rasmol/infile.h"
#include "../rasmol/repres.h"
#include "../rasmol/multiple.h"
#include "../rasmol/vector.h"
#include "../rasmol/brotate.h"
#include "../rasmol/graphics.h"
#include "../rasmol/langsel.h"
#include "../rasmol/help.h"
}

/////////////////////////////////////////////////////////////////////////////
// CData 

class CData : public CObject
{
public:
	CData();
	DECLARE_DYNCREATE(CData)

// Attributes
	/*Graphics*/
public:	
LPVOID Data_Database;
int Data_XRange;
int Data_YRange;
int Data_ZRange;
double Data_DRange;
int Data_BackR, Data_BackG, Data_BackB;

	/*Multiple*/
LPVOID Data_MoleculeList;
LPVOID Data_CurMolSet;
LPVOID Data_CenterSet;
CString Data_RSMPathName;
CString Data_SpaceName;

protected:
double Data_DialValue[10];
int Data_FileModified;

	/*molecule*/
CString Data_Info_filename;
CString Data_Info_moleculename;
CString Data_Info_classification;
CString Data_Info_date;
CString Data_Info_technique;
CString Data_Info_identcode;

CString Data_Info_spacegroup;
double Data_Info_cellalpha;
double Data_Info_cellbeta;
double Data_Info_cellgamma;
double Data_Info_cella;
double Data_Info_cellb;
double Data_Info_cellc;

double Data_Info_vecf2o[3];
double Data_Info_veco2f[3];
double Data_Info_matf2o[3][3];
double Data_Info_mato2f[3][3];
double Data_Info_cell[6];

Long Data_Info_bondcount;
int Data_Info_chaincount;
int Data_Info_ssbondcount;
int Data_Info_hbondcount;
int Data_cisbondcount;

int Data_Info_structsource;
int Data_Info_laddercount;
int Data_Info_helixcount;
int Data_Info_turncount;

Molecule	*Data_CurMolecule;
Chain		*Data_CurChain;
Group		*Data_CurGroup;
RAtom		*Data_CurAtom;

short Data_UserMask_radius[MAXMASK];
CString  Data_UserMask_mask[MAXMASK];
Byte  Data_UserMask_flags[MAXMASK];
Byte  Data_UserMask_r[MAXMASK];
Byte  Data_UserMask_g[MAXMASK];
Byte  Data_UserMask_b[MAXMASK];

char Data_Residue[MAXRES - MINRES][4];
char Data_ElemDesc[MAXELEM - MINELEM][12];

int Data_MainGroupCount;
int	Data_HetaGroupCount;
Long Data_HetaAtomCount;
Long Data_MainAtomCount;  
int Data_CisBondCutOff;

Long Data_MinX;
Long Data_MinY;
Long Data_MinZ;
Long Data_MaxX;
Long Data_MaxY;
Long Data_MaxZ;

int Data_HMinMaxFlag;
int Data_MMinMaxFlag;
int Data_MinMainTemp;
int Data_MaxMainTemp;
int Data_MinHetaTemp;
int Data_MaxHetaTemp;
int Data_MinMainRes;
int	Data_MaxMainRes;
int Data_MinHetaRes;
int	Data_MaxHetaRes;
int Data_MinAltl;
int Data_MaxAltl;

short Data_MinModel;
short Data_MaxModel;

Long Data_MinBondDist;
Long Data_MaxBondDist;
int Data_ElemNo;
int Data_ResNo;
int Data_HasHydrogen;
int Data_MaskCount;
int Data_NMRModel;
int Data_NullBonds;
//int Data_MarkAtoms;

int Data_HBondChainsFlag;
int Data_CapClean;

	/*Abstree*/
SymEntry *Data_SymbolTable;

	/*pixutils*/
int Data_SplineCount;
int Data_FontPS;
int Data_FontSize;
int Data_FontStroke;

	/*Script*/
int Data_KinemageFlag;

	/*Command*/
//CString Data_DataFileName;
Long Data_SelectCount;
//int	Data_Interactive;

int Data_CalcBondsFlag;

	/*Paused scripts*/
int Data_FileDepth;
int Data_IsPaused;
char Data_NameStack[STACKSIZE][1024];
int Data_LineStack[STACKSIZE];
fpos_t Data_PosStack[STACKSIZE];
char Data_AcceptData[STACKSIZE];

	/*Transfor*/
ShadeDesc Data_Shade[LastShade];
double Data_RotX[3];
double Data_RotY[3];
double Data_RotZ[3];
double Data_MOffset[3];
Long Data_CenX;
Long Data_CenY;
Long Data_CenZ;
double Data_RCenx;
double Data_RCeny;
double Data_RCenz;
Long Data_ShiftS;
double Data_LastTX;
double Data_LastTY;
double Data_LastTZ;
double Data_LastRX;
double Data_LastRY;
double Data_LastRZ;
double Data_WLastTX, Data_WLastTY, Data_WLastTZ;
double Data_WLastRX, Data_WLastRY, Data_WLastRZ;
double Data_WTransValue[3];
double Data_WRotValue[3];      
double Data_WAxes[3][3];
double Data_Zoom;
double Data_Wo[3];

double Data_MatX[3];
double Data_MatY[3];
double Data_MatZ[3];
double Data_InvX[3];
double Data_InvY[3];
double Data_InvZ[3];
Long Data_OrigCX;
Long Data_OrigCY;
Long Data_OrigCZ;

int Data_ScaleCount;
int Data_AltlColours[AltlDepth];

int Data_FakeSpecular ;
int Data_SpecPower;
int Data_ShadePower;
int Data_DotR,Data_DotG,Data_DotB;
int Data_LabR, Data_LabG, Data_LabB;
int Data_AxeR, Data_AxeG, Data_AxeB;
int Data_BoxCol;
int Data_UseLabelCol;
int Data_UseBackFade;
double Data_Ambient;
int Data_UseDotCol;
int Data_DotCol;
int Data_LightX;
int Data_LightY;
int Data_LightZ;

double Data_Scale;
double Data_MaxZoom;
double Data_DScale;
double Data_IScale;
Long Data_SideLen;
Long Data_Offset;
Card Data_WorldRadius;
Card Data_WorldSize;
Card Data_LocalRadius;
int Data_XOffset;
int Data_YOffset;
int Data_ZOffset;
int Data_UseScreenClip;
int Data_ZoomRange;

int Data_Hydrogens;
int Data_HetaGroups;
int Data_DrawAtoms;
int Data_MaxAtomRadius;
int Data_DrawBonds;
int Data_MaxBondRadius;
int Data_DrawRibbon;
int Data_DrawStars;
int Data_ZoneBoth;

	/*Render*/
int Data_UseDepthCue;
int Data_DepthCueMode;
int Data_UseStereo;
int Data_StereoView;
int Data_UseShadow;
int Data_DisplayMode;
int Data_UseClipping;
int Data_UseSlabPlane;
int Data_UseDepthPlane;
double Data_WClip[2];
int Data_RefreshState;
int Data_SlabMode;
int Data_SlabValue;
int Data_SlabInten;
int Data_SliceValue;
int Data_ImageRadius;
int Data_ImageSize;
int Data_SSBondMode;
int Data_HBondMode;
double Data_StereoAngle;
int Data_RotMode;
int Data_DrawBoundBox;
int Data_DrawAxes;
int Data_DrawWorldAxes;
int Data_DrawDoubleBonds;
int Data_DrawUnitCell;
double Data_IVoxRatio;
int Data_VoxelsClean;

	/*Repres*/
DotStruct *Data_DotPtr;
Monitor *Data_MonitList;
Label *Data_LabelList;
int Data_CartoonHeight;
int Data_SolventDots;
int Data_ProbeRadius;
int Data_DotDensity;
int Data_DotSize;
int Data_DrawMonitDistance;
int Data_DrawBetaArrows;

	/*Outfile*/
int Data_UseTransparent;
int Data_UseOutLine;

	/*brotate*/
BondRot *Data_BondSelected;
BondRot *Data_RotBondList;
BondOrder *Data_BondOrderList;
RAtom  *Data_BSrcAtom;
RAtom  *Data_BDstAtom;
double Data_BAxis[3];
double Data_BRotValue, Data_BLastRot;

// Operations
public:
	BOOL RegenerateData();
	BOOL SaveData();
	virtual ~CData();

protected:
	int Regenerate_Flag;
	void CopyString(char *des, CString src);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_DATADISPLAY_H_INCLUDED_)
