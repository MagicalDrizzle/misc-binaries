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
 * Naoum Salamé, copyright (C) 2002 (RasTop 2.0.1-VF May 2002)
 */



/////////////////////////////////////////////////////////////////////////////
// rasview.cpp 
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "string.h"
#include "RasTop.h"
class CRasView;	//Forward declaration
#include "mainfrm.h"
#include "rasdoc.h"
#include "pannel.h"
#include "dialog.h"
#include "rasview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ATOI(x)	atoi( ( LPCTSTR) x.Right( x.GetLength() - x.FindOneOf( "1234567890-+" ) ) );	

#define STOPTIMER		GETFRAME \
						pFrame->m_wndCommandBar.KillTimerPannel();
#define STARTTIMER		pFrame->m_wndCommandBar.SetTimerPannel();

static CString Command;

///////////////////////////////////////////////////////////////////////////////////
void RefreshView()
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	pView->RefreshView();
}


/////////////////////////////////////////////////////////////////////////////
// CRasFrame

IMPLEMENT_DYNCREATE(CRasFrame, CMDIChildWnd)

CRasFrame::CRasFrame()	{}

CRasFrame::~CRasFrame()	{}

BEGIN_MESSAGE_MAP(CRasFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CRasFrame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CRasFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style = WS_CHILD | WS_THICKFRAME | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION 
		     | WS_SYSMENU | FWS_ADDTOTITLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | 
			   WS_DLGFRAME;

	/*Maximize the new view if others are maximized*/
	CMDIFrameWnd *pFrame = (CMDIFrameWnd *)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	if( pChild->IsZoomed() )
		cs.style |= WS_MAXIMIZE;

	cs.cx = 350;
	cs.cy = 350;
	return CMDIChildWnd::PreCreateWindow(cs);
}

BOOL CRasFrame::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD 
	dwStyle, const RECT& rect, CMDIFrameWnd* pParentWnd, CCreateContext* pContext) 
{	lpszClassName = AfxRegisterWndClass(NULL, NULL, NULL,
					AfxGetApp()->LoadIcon(IDR_RASVIEW));
	return CMDIChildWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, pContext);
}

/////////////////////////////////////////////////////////////////////////////
// CRasView

IMPLEMENT_DYNCREATE(CRasView, CView)

BEGIN_MESSAGE_MAP(CRasView, CView)
	//{{AFX_MSG_MAP(CRasView)
	ON_COMMAND(ID_COPY_IMAGE, OnCopyImage)
	ON_UPDATE_COMMAND_UI(ID_COPY_IMAGE, OnUpdateCopyImage)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_COMMAND(ID_COLOR_BACKGROUND, OnColorBackground)
	ON_COMMAND(ID_COLOUR_CHAIN, OnColourChain)
	ON_COMMAND(ID_COLOUR_CPK, OnColourCpk)
	ON_COMMAND(ID_COLOUR_GROUP, OnColourGroup)
	ON_COMMAND(ID_COLOUR_SHAPELY, OnColourShapely)
	ON_COMMAND(ID_COLOUR_STRUCTURE, OnColourStructure)
	ON_COMMAND(ID_COLOUR_TEMPERATURE, OnColourTemperature)
	ON_COMMAND(ID_COLOUR_USER, OnColourUser)
	ON_COMMAND(ID_COLOUR_CHARGE, OnColourCharge)
	ON_COMMAND(ID_COLOR_ATOM, OnColorAtom)
	ON_COMMAND(ID_COLOR_BONDS, OnColorBonds)
	ON_COMMAND(ID_COLOR_DOTS, OnColorDots)
	ON_COMMAND(ID_COLOR_HBONDS, OnColorHbonds)
	ON_COMMAND(ID_COLOR_LABELS, OnColorLabels)
	ON_COMMAND(ID_COLOR_SSBONDS, OnColorSsbonds)
	ON_COMMAND(ID_COLOR_DOTS_POTENTIAL, OnColorDotsPotential)
	ON_COMMAND(ID_DOTSON, OnDisplayDotson)
	ON_COMMAND(ID_DOTSOFF, OnDisplayDotsoff)
	ON_COMMAND(ID_SET_DOTS_DENSITY, OnSetDotsDensity)
	ON_COMMAND(ID_DOTS_CONOLLY, OnDotsConolly)
	ON_UPDATE_COMMAND_UI(ID_DOTS_CONOLLY, OnUpdateDotsConolly)
	ON_COMMAND(ID_DOTS_RADIUS, OnDotsRadius)
	ON_COMMAND(ID_DOTS_VDWAALS, OnDotsVdwaals)
	ON_UPDATE_COMMAND_UI(ID_DOTS_VDWAALS, OnUpdateDotsVdwaals)
	ON_COMMAND(ID_DISPLAY_WIREFRAME, OnDisplayWireframe)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_FULL, OnDisplayWireframeFull)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_OFF, OnDisplayWireframeOff)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_WIRE, OnDisplayWireframeWire)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_WIRE, OnUpdateDisplayWireframeWire)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_DASH, OnDisplayWireframeDash)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_DASH, OnUpdateDisplayWireframeDash)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_CYLINDER, OnDisplayWireframeCylinder)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_CYLINDER, OnUpdateDisplayWireframeCylinder)
	ON_COMMAND(ID_DISPLAY_BACKBONE_CYLINDER, OnDisplayBackboneCylinder)
	ON_COMMAND(ID_DISPLAY_BACKBONE_DASH, OnDisplayBackboneDash)
	ON_COMMAND(ID_DISPLAY_BACKBONE_WIRE, OnDisplayBackboneWire)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BACKBONE_CYLINDER, OnUpdateDisplayBackboneCylinder)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BACKBONE_DASH, OnUpdateDisplayBackboneDash)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BACKBONE_WIRE, OnUpdateDisplayBackboneWire)
	ON_COMMAND(ID_DISPLAY_BALLANDSTICK, OnDisplayBallandstick)
	ON_COMMAND(ID_DISPLAY_SPACEFILL, OnDisplaySpacefill)
	ON_COMMAND(ID_DISPLAY_STICKS, OnDisplaySticks)
	ON_COMMAND(ID_SPACEFILL_FIXEDRADIUS, OnSpacefillFixedradius)
	ON_COMMAND(ID_SPACEFILL_TEMPERATURE, OnSpacefillTemperature)
	ON_COMMAND(ID_SPACEFILL_USER, OnSpacefillUser)
	ON_COMMAND(ID_SPACEFILL_VANDERWAALS, OnSpacefillVanderwaals)
	ON_COMMAND(ID_DISPLAY_SPACEFILL_FULL, OnDisplaySpacefillFull)
	ON_COMMAND(ID_DISPLAY_SPACEFILL_OFF, OnDisplaySpacefillOff)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_FIXEDRADIUS, OnUpdateSpacefillFixedradius)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_TEMPERATURE, OnUpdateSpacefillTemperature)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_USER, OnUpdateSpacefillUser)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_VANDERWAALS, OnUpdateSpacefillVanderwaals)
	ON_COMMAND(ID_COLOR_FADE, OnColorFade)
	ON_UPDATE_COMMAND_UI(ID_COLOR_FADE, OnUpdateColorFade)
	ON_COMMAND(ID_VIEW_STEREO, OnViewStereo)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STEREO, OnUpdateViewStereo)
	ON_COMMAND(ID_DISPLAY_HBONDS_CYLINDER, OnDisplayHbondsCylinder)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_CYLINDER, OnUpdateDisplayHbondsCylinder)
	ON_COMMAND(ID_DISPLAY_HBONDS_DASH, OnDisplayHbondsDash)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_DASH, OnUpdateDisplayHbondsDash)
	ON_COMMAND(ID_DISPLAY_HBONDS_OFF, OnDisplayHbondsOff)
	ON_COMMAND(ID_DISPLAY_HBONDS_ON, OnDisplayHbondsOn)
	ON_COMMAND(ID_DISPLAY_SSBONDS_CYLINDER, OnDisplaySsbondsCylinder)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_CYLINDER, OnUpdateDisplaySsbondsCylinder)
	ON_COMMAND(ID_DISPLAY_SSBONDS_DASH, OnDisplaySsbondsDash)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_DASH, OnUpdateDisplaySsbondsDash)
	ON_COMMAND(ID_DISPLAY_SSBONDS_OFF, OnDisplaySsbondsOff)
	ON_COMMAND(ID_DISPLAY_SSBONDS_ON, OnDisplaySsbondsOn)
	ON_COMMAND(ID_DISPLAY_WIREFRAME_ONLY, OnDisplayWireframeOnly)
	ON_COMMAND(ID_DISPLAY_HBONDS_BACKBONE, OnDisplayHbondsBackbone)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_BACKBONE, OnUpdateDisplayHbondsBackbone)
	ON_COMMAND(ID_DISPLAY_HBONDS_SIDECHAIN, OnDisplayHbondsSidechain)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_SIDECHAIN, OnUpdateDisplayHbondsSidechain)
	ON_COMMAND(ID_DISPLAY_SSBONDS_BACKBONE, OnDisplaySsbondsBackbone)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_BACKBONE, OnUpdateDisplaySsbondsBackbone)
	ON_COMMAND(ID_DISPLAY_SSBONDS_SIDECHAIN, OnDisplaySsbondsSidechain)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_SIDECHAIN, OnUpdateDisplaySsbondsSidechain)
	ON_COMMAND(ID_RIBBONS_BACKBONE, OnRibbonsBackbone)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_BACKBONE, OnUpdateRibbonsBackbone)
	ON_COMMAND(ID_RIBBONS_CARTOONS, OnRibbonsCartoons)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_CARTOONS, OnUpdateRibbonsCartoons)
	ON_COMMAND(ID_RIBBONS_COLOR, OnRibbonsColor)
	ON_COMMAND(ID_RIBBONS_COLOR_INSIDE, OnRibbonsColorInside)
	ON_COMMAND(ID_RIBBONS_OFF, OnRibbonsOff)
	ON_COMMAND(ID_RIBBONS_ON, OnRibbonsOn)
	ON_COMMAND(ID_RIBBONS_ONLY, OnRibbonsOnly)
	ON_COMMAND(ID_RIBBONS_RIBBONS, OnRibbonsRibbons)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_RIBBONS, OnUpdateRibbonsRibbons)
	ON_COMMAND(ID_RIBBONS_SIZE, OnRibbonsSize)
	ON_COMMAND(ID_RIBBONS_STRANDS, OnRibbonsStrands)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_STRANDS, OnUpdateRibbonsStrands)
	ON_COMMAND(ID_RIBBONS_TRACE, OnRibbonsTrace)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_TRACE, OnUpdateRibbonsTrace)
	ON_COMMAND(ID_RIBBONS_COLOR_OUTSIDE, OnRibbonsColorOutside)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_SIZE, OnUpdateRibbonsSize)
	ON_COMMAND(ID_SET_STRANDS_NUMBER, OnSetStrandsNumber)
	ON_UPDATE_COMMAND_UI(ID_SET_STRANDS_NUMBER, OnUpdateSetStrandsNumber)
	ON_COMMAND(ID_DISPLAY_TRACE_DOTS, OnDisplayTraceDots)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_TRACE_DOTS, OnUpdateDisplayTraceDots)
	ON_COMMAND(ID_DISPLAY_TRACE_TEMPERATURE, OnDisplayTraceTemperature)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_TRACE_TEMPERATURE, OnUpdateDisplayTraceTemperature)
	ON_COMMAND(ID_SET_CARTOONS_HEIGHT, OnSetCartoonsHeight)
	ON_UPDATE_COMMAND_UI(ID_SET_CARTOONS_HEIGHT, OnUpdateSetCartoonsHeight)
	ON_COMMAND(ID_DISPLAY_RIBBONS_STRUCTURE, OnDisplayRibbonsStructure)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_RIBBONS_STRUCTURE, OnUpdateDisplayRibbonsStructure)
	ON_COMMAND(ID_DISPLAY_CARTOONS_ARROW, OnDisplayCartoonsArrow)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_CARTOONS_ARROW, OnUpdateDisplayCartoonsArrow)
	ON_COMMAND(ID_SET_SLAB_HALF, OnSetSlabHalf)
	ON_UPDATE_COMMAND_UI(ID_SET_SLAB_HALF, OnUpdateSetSlabHalf)
	ON_COMMAND(ID_SET_SLAB_HOLLOW, OnSetSlabHollow)
	ON_UPDATE_COMMAND_UI(ID_SET_SLAB_HOLLOW, OnUpdateSetSlabHollow)
	ON_COMMAND(ID_SET_SLAB_REJECT, OnSetSlabReject)
	ON_UPDATE_COMMAND_UI(ID_SET_SLAB_REJECT, OnUpdateSetSlabReject)
	ON_COMMAND(ID_SET_SLAB_SECTION, OnSetSlabSection)
	ON_UPDATE_COMMAND_UI(ID_SET_SLAB_SECTION, OnUpdateSetSlabSection)
	ON_COMMAND(ID_SET_SLAB_SOLID, OnSetSlabSolid)
	ON_UPDATE_COMMAND_UI(ID_SET_SLAB_SOLID, OnUpdateSetSlabSolid)
	ON_COMMAND(ID_VIEW_SHADOWS, OnViewShadows)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHADOWS, OnUpdateViewShadows)
	ON_COMMAND(ID_DISPLAY_AXES, OnDisplayAxes)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_AXES, OnUpdateDisplayAxes)
	ON_COMMAND(ID_MOLECULE_SEQ, OnMoleculeSeq)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_SEQ, OnUpdateMoleculeSeq)
	ON_COMMAND(ID_MOLECULE_STRUC, OnMoleculeStruc)
	ON_COMMAND(ID_MOLECULE_SYM, OnMoleculeSym)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_SYM, OnUpdateMoleculeSym)
	ON_COMMAND(ID_MOLECULE_INFO, OnMoleculeInfo)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_INFO, OnUpdateMoleculeInfo)
	ON_COMMAND(ID_STATICROTX180, OnStaticrotx180)
	ON_COMMAND(ID_STATICROTX270, OnStaticrotx270)
	ON_COMMAND(ID_STATICROTX90, OnStaticrotx90)
	ON_COMMAND(ID_STATICROTY180, OnStaticroty180)
	ON_COMMAND(ID_STATICROTY270, OnStaticroty270)
	ON_COMMAND(ID_STATICROTY90, OnStaticroty90)
	ON_COMMAND(ID_STATICROTZ180, OnStaticrotz180)
	ON_COMMAND(ID_STATICROTZ270, OnStaticrotz270)
	ON_COMMAND(ID_STATICROTZ90, OnStaticrotz90)
	ON_COMMAND(ID_LABELS_ON, OnLabelsOn)
	ON_COMMAND(ID_LABELS_OFF, OnLabelsOff)
	ON_COMMAND(ID_LABELS_DEFAULT, OnLabelsDefault)
	ON_COMMAND(ID_DISPLAY_BOUNDBOX, OnDisplayBoundbox)
	ON_COMMAND(ID_DISPLAY_UNITCELL, OnDisplayUnitcell)
	ON_COMMAND(ID_MOLECULE_CONNECT, OnMoleculeConnect)
	ON_COMMAND(ID_MOLECULE_RENUMBER, OnMoleculeRenumber)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BOUNDBOX, OnUpdateDisplayBoundbox)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_UNITCELL, OnUpdateDisplayUnitcell)
	ON_COMMAND(ID_COLOR_HBONDS_TYPE, OnColorHbondsType)
	ON_COMMAND(ID_MOLECULE_FILE, OnEditMolecularFile)
	ON_COMMAND(ID_SET_CONNECT, OnSetConnect)
	ON_UPDATE_COMMAND_UI(ID_SET_CONNECT, OnUpdateSetConnect)
	ON_COMMAND(ID_DISTANCE_REMOVE, OnDistanceRemove)
	ON_COMMAND(ID_DISTANCE_SHOW, OnDistanceShow)
	ON_COMMAND(ID_MONITOR_COLOUR, OnMonitorColour)
	ON_UPDATE_COMMAND_UI(ID_DISTANCE_SHOW, OnUpdateDistanceShow)
	ON_COMMAND(ID_LABELS_SETTINGS, OnLabelsSettings)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_CONNECT, OnUpdateMoleculeConnect)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_RENUMBER, OnUpdateMoleculeRenumber)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_STRUC, OnUpdateMoleculeStruc)
	ON_COMMAND(ID_COLOR_AXE, OnColorAxe)
	ON_UPDATE_COMMAND_UI(ID_COLOR_AXE, OnUpdateColorAxe)
	ON_COMMAND(ID_COLOR_BOX, OnColorBox)
	ON_UPDATE_COMMAND_UI(ID_COLOR_BOX, OnUpdateColorBox)
	ON_COMMAND(ID_SET_BONDS, OnSetBonds)
	ON_UPDATE_COMMAND_UI(ID_SET_BONDS, OnUpdateSetBonds)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_CHAIN, OnUpdateColourChain)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_CHARGE, OnUpdateColourCharge)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_CPK, OnUpdateColourCpk)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_GROUP, OnUpdateColourGroup)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_SHAPELY, OnUpdateColourShapely)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_STRUCTURE, OnUpdateColourStructure)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_TEMPERATURE, OnUpdateColourTemperature)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_USER, OnUpdateColourUser)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BALLANDSTICK, OnUpdateDisplayBallandstick)
	ON_UPDATE_COMMAND_UI(ID_DOTSOFF, OnUpdateDotsoff)
	ON_UPDATE_COMMAND_UI(ID_DOTSON, OnUpdateDotson)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_OFF, OnUpdateDisplayHbondsOff)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_HBONDS_ON, OnUpdateDisplayHbondsOn)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SPACEFILL, OnUpdateDisplaySpacefill)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SPACEFILL_FULL, OnUpdateDisplaySpacefillFull)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SPACEFILL_OFF, OnUpdateDisplaySpacefillOff)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_OFF, OnUpdateDisplaySsbondsOff)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SSBONDS_ON, OnUpdateDisplaySsbondsOn)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_STICKS, OnUpdateDisplaySticks)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME, OnUpdateDisplayWireframe)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_FULL, OnUpdateDisplayWireframeFull)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_OFF, OnUpdateDisplayWireframeOff)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WIREFRAME_ONLY, OnUpdateDisplayWireframeOnly)
	ON_UPDATE_COMMAND_UI(ID_DISTANCE_REMOVE, OnUpdateDistanceRemove)
	ON_UPDATE_COMMAND_UI(ID_LABELS_DEFAULT, OnUpdateLabelsDefault)
	ON_UPDATE_COMMAND_UI(ID_LABELS_OFF, OnUpdateLabelsOff)
	ON_UPDATE_COMMAND_UI(ID_LABELS_ON, OnUpdateLabelsOn)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_OFF, OnUpdateRibbonsOff)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_ON, OnUpdateRibbonsOn)
	ON_UPDATE_COMMAND_UI(ID_RIBBONS_ONLY, OnUpdateRibbonsOnly)
	ON_UPDATE_COMMAND_UI(ID_SET_DOTS_DENSITY, OnUpdateSetDotsDensity)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_COMMAND(ID_WINDOW_TILE_VERT, OnWindowTileVert)
	ON_COMMAND(ID_WINDOW_TILE_HORZ, OnWindowTileHorz)
	ON_COMMAND(ID_WINDOW_CASCADE, OnWindowCascade)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_COMMAND(ID_WINDOW_TILE, OnWindowTile)
	ON_COMMAND(ID_SPACEFILL_SPHERE, OnSpacefillSphere)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_SPHERE, OnUpdateSpacefillSphere)
	ON_COMMAND(ID_SPACEFILL_STAR, OnSpacefillStar)
	ON_UPDATE_COMMAND_UI(ID_SPACEFILL_STAR, OnUpdateSpacefillStar)
	ON_COMMAND(ID_COLOUR_ALT, OnColourAlt)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_ALT, OnUpdateColourAlt)
	ON_COMMAND(ID_COLOUR_MODEL, OnColourModel)
	ON_UPDATE_COMMAND_UI(ID_COLOUR_MODEL, OnUpdateColourModel)
	ON_COMMAND(ID_SET_CISANGLE, OnSetCisangle)
	ON_UPDATE_COMMAND_UI(ID_SET_CISANGLE, OnUpdateSetCisangle)
	ON_COMMAND(ID_SET_BONDMODE_NOBONDED, OnSetBondmodeNobonded)
	ON_UPDATE_COMMAND_UI(ID_SET_BONDMODE_NOBONDED, OnUpdateSetBondmodeNobonded)
	ON_COMMAND(ID_SET_BONDMODE_NONE, OnSetBondmodeNone)
	ON_UPDATE_COMMAND_UI(ID_SET_BONDMODE_NONE, OnUpdateSetBondmodeNone)
	ON_COMMAND(ID_SET_BONDMODE_ALL, OnSetBondmodeAll)
	ON_UPDATE_COMMAND_UI(ID_SET_BONDMODE_ALL, OnUpdateSetBondmodeAll)
	ON_COMMAND(ID_MOLECULE_PHIPSI, OnMoleculePhipsi)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_PHIPSI, OnUpdateMoleculePhipsi)
	ON_COMMAND(ID_MOLECULE_RPP, OnMoleculeRpp)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_RPP, OnUpdateMoleculeRpp)
	ON_COMMAND(ID_COPY_POSITION, OnCopyPosition)
	ON_UPDATE_COMMAND_UI(ID_COPY_POSITION, OnUpdateCopyPosition)
	ON_COMMAND(ID_COPY_SELECTION, OnSelectionCopy)
	ON_UPDATE_COMMAND_UI(ID_COPY_SELECTION, OnUpdateSelectionCopy)
	ON_COMMAND(ID_PASTE, OnPaste)
	ON_UPDATE_COMMAND_UI(ID_PASTE, OnUpdatePaste)
	ON_COMMAND(ID_EDIT_RASMOL_COMMAND, OnEditRasmolCommand)
	ON_COMMAND(ID_SELECT_DEFINE, OnSelectDefine)
	ON_UPDATE_COMMAND_UI(ID_SELECT_DEFINE, OnUpdateSelectDefine)
	ON_COMMAND(ID_SELECT_ALL, OnSelectAll)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, OnUpdateSelectAll)
	ON_COMMAND(ID_SELECT_INVERSE, OnSelectInverse)
	ON_UPDATE_COMMAND_UI(ID_SELECT_INVERSE, OnUpdateSelectInverse)
	ON_COMMAND(ID_SELECT_PREVIOUS, OnSelectPrevious)
	ON_UPDATE_COMMAND_UI(ID_SELECT_PREVIOUS, OnUpdateSelectPrevious)
	ON_COMMAND(ID_SHOW_SELECT_ATM, OnShowSelectAtm)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECT_ATM, OnUpdateShowSelectAtm)
	ON_COMMAND(ID_SHOW_SELECT_GRP, OnShowSelectGrp)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECT_GRP, OnUpdateShowSelectGrp)
	ON_COMMAND(ID_SHOW_SELECT_CHN, OnShowSelectChn)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECT_CHN, OnUpdateShowSelectChn)
	ON_COMMAND(ID_SHOW_SELECT_COOR, OnShowSelectCoor)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECT_COOR, OnUpdateShowSelectCoor)
	ON_COMMAND(ID_SELECT_EXPRESSION, OnSelectExpression)
	ON_UPDATE_COMMAND_UI(ID_SELECT_EXPRESSION, OnUpdateSelectExpression)
	ON_COMMAND(ID_SELECT_WITHIN, OnSelectWithin)
	ON_UPDATE_COMMAND_UI(ID_SELECT_WITHIN, OnUpdateSelectWithin)
	ON_COMMAND(ID_SELECT_HYDROGENS, OnSelectHydrogens)
	ON_UPDATE_COMMAND_UI(ID_SELECT_HYDROGENS, OnUpdateSelectHydrogens)
	ON_COMMAND(ID_SELECT_BONDMODE, OnSelectBondmode)
	ON_UPDATE_COMMAND_UI(ID_SELECT_BONDMODE, OnUpdateSelectBondmode)
	ON_COMMAND(ID_SELECT_HETE_ATOMS, OnSelectHeteAtoms)
	ON_UPDATE_COMMAND_UI(ID_SELECT_HETE_ATOMS, OnUpdateSelectHeteAtoms)
	ON_COMMAND(ID_SELECT_MODEL, OnSelectModel)
	ON_UPDATE_COMMAND_UI(ID_SELECT_MODEL, OnUpdateSelectModel)
	ON_COMMAND(ID_RESTRICT, OnRestrict)
	ON_UPDATE_COMMAND_UI(ID_RESTRICT, OnUpdateRestrict)
	ON_COMMAND(ID_SELECT_SHOW, OnSelectShow)
	ON_UPDATE_COMMAND_UI(ID_SELECT_SHOW, OnUpdateSelectShow)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_SET_DOTS_SIZE, OnSetDotsSize)
	ON_UPDATE_COMMAND_UI(ID_SET_DOTS_SIZE, OnUpdateSetDotsSize)
	ON_COMMAND(ID_VIEW_LIGHTSOURCE, OnViewLightsource)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIGHTSOURCE, OnUpdateViewLightsource)
	ON_COMMAND(ID_DISPLAY_SPHERE, OnDisplaySphere)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SPHERE, OnUpdateDisplaySphere)
	ON_COMMAND(ID_DISPLAY_STAR, OnDisplayStar)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_STAR, OnUpdateDisplayStar)
	ON_COMMAND(ID_DISPLAY_NOTHING, OnDisplayNothing)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_NOTHING, OnUpdateDisplayNothing)
	ON_COMMAND(ID_SELECT_CENTRE, OnSelectCentre)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CENTRE, OnUpdateSelectCentre)
	ON_COMMAND(ID_DISPLAY_WORLDAXES, OnDisplayWorldaxes)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_WORLDAXES, OnUpdateDisplayWorldaxes)
	ON_COMMAND(ID_SET_STEREO_ANGLE, OnSetStereoAngle)
	ON_UPDATE_COMMAND_UI(ID_SET_STEREO_ANGLE, OnUpdateSetStereoAngle)
	ON_COMMAND(ID_MOLECULE_SHOW, OnMoleculeShow)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_SHOW, OnUpdateMoleculeShow)
	ON_COMMAND(ID_CENTRE_WORLD, OnCentreWorld)
	ON_UPDATE_COMMAND_UI(ID_CENTRE_WORLD, OnUpdateCentreWorld)
	ON_COMMAND(ID_CENTRE_ORIGIN, OnCentreOrigin)
	ON_UPDATE_COMMAND_UI(ID_CENTRE_ORIGIN, OnUpdateCentreOrigin)
	ON_UPDATE_COMMAND_UI(ID_CENTRE_ORIGINTOCENTRE, OnUpdateOrigintocentre)
	ON_COMMAND(ID_CENTRE_ORIGINTOCENTRE, OnOrigintocentre)
	ON_COMMAND(ID_SLAB_RESET, OnSlabWorldReset)
	ON_UPDATE_COMMAND_UI(ID_SLAB_RESET, OnUpdateSlabWorldReset)
	ON_COMMAND(ID_DEPTH_RESET, OnDepthWorldReset)
	ON_UPDATE_COMMAND_UI(ID_DEPTH_RESET, OnUpdateDepthWorldReset)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_MOLECULE_DELETE, OnMoleculeDelete)
	ON_UPDATE_COMMAND_UI(ID_MOLECULE_DELETE, OnUpdateMoleculeDelete)
	ON_COMMAND(ID_RESET_BOND, OnResetBond)
	ON_UPDATE_COMMAND_UI(ID_RESET_BOND, OnUpdateResetBond)
	ON_COMMAND(ID_WORLD_INFO, OnWorldInfo)
	ON_UPDATE_COMMAND_UI(ID_WORLD_INFO, OnUpdateWorldInfo)
	ON_COMMAND(ID_DEPTHCUEMODE1, OnDepthcuemode1)
	ON_COMMAND(ID_DEPTHCUEMODE2, OnDepthcuemode2)
	ON_COMMAND(ID_DEPTHCUEMODE3, OnDepthcuemode3)
	ON_COMMAND(ID_DEPTHCUEMODE4, OnDepthcuemode4)
	ON_UPDATE_COMMAND_UI(ID_DEPTHCUEMODE1, OnUpdateDepthcuemode1)
	ON_UPDATE_COMMAND_UI(ID_DEPTHCUEMODE2, OnUpdateDepthcuemode2)
	ON_UPDATE_COMMAND_UI(ID_DEPTHCUEMODE3, OnUpdateDepthcuemode3)
	ON_UPDATE_COMMAND_UI(ID_DEPTHCUEMODE4, OnUpdateDepthcuemode4)
	ON_COMMAND(ID_DEPTH_WORLD, OnDepthWorld)
	ON_UPDATE_COMMAND_UI(ID_DEPTH_WORLD, OnUpdateDepthWorld)
	ON_COMMAND(ID_CENTRE_CENTRETOORIGIN, OnCentreCentretoorigin)
	ON_UPDATE_COMMAND_UI(ID_CENTRE_CENTRETOORIGIN, OnUpdateCentreCentretoorigin)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//static declaration for menu variables
CString CRasView::m_PreviousCommand = "";

int CRasView::m_SpacefillRepres = SphereFlag;
CString CRasView::m_SpacefillDisplay = "on";

CString CRasView::m_BondDisplay = "wireframe on\n";
int CRasView::m_BondsRadius = 0;
CString CRasView::m_BondHDisplay = "on";
CString CRasView::m_BondSSDisplay = "on";

CString CRasView::m_RibbonsCommand = "";
CString CRasView::m_RibbonsColor = "none";
int CRasView::RibbonType = TypeRibbons;

CRasView::CRasView()
{	m_drawingtime = 10;
}

CRasView::~CRasView()	{}	

void CRasView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	//Force Rasview cursor display
	if( ::ShowCursor( true ) >= 1 )
		::ShowCursor(false);	
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void CRasView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
    BYTE ks[256];
    GetKeyboardState(ks);
    WORD w;
    UINT scan=0;
    ToAscii(nChar,scan,ks,&w,0);
    char ch = char(w);
    ProcessCharacter(ch);
    ProcessCharacter(0);    // Flush
	if( ::ShowCursor(true)>= 1 )
		::ShowCursor(false);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRasView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	//Force Rasview cursor display
	if( ::ShowCursor(true)>= 1 )
		::ShowCursor(false);
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CRasView::OnKillFocus(CWnd* pNewWnd) 
{	CView::OnKillFocus(pNewWnd);
	if( IsPaused && pNewWnd )
	{	if( !pNewWnd->IsKindOf(RUNTIME_CLASS(CRasView)) )
		{	GETFRAME
			pFrame->m_wndCommandBar.ChangeStatusLight(true);	
		}
	}	
}

void CRasView::OnSetFocus(CWnd* pOldWnd) 
{	CView::OnSetFocus(pOldWnd);		
	if( IsPaused )
	{	GETFRAME
		pFrame->m_wndCommandBar.ChangeStatusLight(true);	
	}	
}


BOOL CRasView::OnEraseBkgnd(CDC* pDC) 
{
	if( !Database )
	    return CView::OnEraseBkgnd(pDC);
    else return true;
}


static void PrintView(CDC* pDC, CRasDoc* pDoc)
{	
    register int cbfactor;

#ifdef EIGHTBIT			
	HPALETTE hOldPal = NULL;
#endif
	HDC hDC;

	GETFRAME

    /* Adjust ClipBoardFactor */
    cbfactor = pDoc->GetMaxCBFactor( );
    if( !cbfactor )
        return;

	BeginWait();

	if( cbfactor!=1 )
	{	XRange *= cbfactor;
		if( int dx = XRange%4 )
			XRange += 4-dx;	
		YRange *= cbfactor;
		Range =  MinFun(XRange,YRange);
		ZRange = Range;
		HRange = YRange>>1;
		WRange = XRange>>1;
		ReDrawFlag |= RFReSize;
	    RefreshState = True;
		ClearImage();
		RefreshScreen();
	}
	
	CRect rcDest;
		
	int cxPage = pDC->GetDeviceCaps(HORZRES);
	int cyPage = pDC->GetDeviceCaps(VERTRES);
	int cxInch = pDC->GetDeviceCaps(LOGPIXELSX);
	int cyInch = pDC->GetDeviceCaps(LOGPIXELSY);
	rcDest.top = rcDest.left = 0;

	//The factor 4 shoud be adjusted to the screen specificity (LOGPIXELSX-Y)
	if( cxPage>(int)((double)4*XRange/cbfactor) )
	{	rcDest.bottom = (int)(((double)4*YRange*cyInch)/(cxInch*cbfactor));
		rcDest.right = (int)((double)4*XRange/cbfactor);
	} else
	{	rcDest.bottom = (int)(((double)YRange*cxPage*cyInch)
			/ ((double)XRange*cxInch));
		rcDest.right = cxPage;
	}
	
	//Center
	int Dx = (int)((cxPage - rcDest.right)/2);
	int Dy = (int)((cyPage - rcDest.bottom)/2);
	double Dxy;
	if( Dy!=0 )
	{	Dxy =  fabs(((double)Dx/(double)cxPage)/((double)Dy/(double)cyPage));
	} else
		Dxy = 1.6E308 ;
	if( (Dx>=0) && (Dy>=0) )
	{	rcDest.left += Dx;
		rcDest.right += Dx;
		rcDest.top += Dy;
		rcDest.bottom += Dy;
	} else if( (Dx<0)&&(Dxy>=1) )
	{	rcDest.bottom = int((double)rcDest.bottom*((double)cxPage/rcDest.right));
		rcDest.right = cxPage;
		Dy = (int)((cyPage - rcDest.bottom)/2);
		rcDest.top += Dy;
		rcDest.bottom += Dy;
	} else if( (Dy<0)&&(Dxy>=1) )
	{	rcDest.right = int((double)rcDest.right*((double)cxPage/rcDest.bottom));
		rcDest.bottom = cyPage;
		Dx = (int)((cxPage - rcDest.right)/2);
		rcDest.left += Dx;
		rcDest.right += Dx;
	} 

	hDC = pDC->m_hDC;
	LPSTR lpRASBits = (LPSTR) ::GlobalLock((HGLOBAL) FBufHandle); 

#ifdef EIGHTBIT			
	if( ColourMap != NULL )
		hOldPal = ::SelectPalette(hDC, ColourMap, TRUE);
	::RealizePalette(hDC);
#endif 
	::SetStretchBltMode(hDC, COLORONCOLOR);
	::StretchDIBits(hDC,
					rcDest.left,
					rcDest.top,
					rcDest.right - rcDest.left,
					rcDest.bottom - rcDest.top,
					0,
					0,
					XRange,
					YRange,
					lpRASBits,
					(LPBITMAPINFO)BitInfo,
					DIB_RGB_COLORS,
					SRCCOPY);
	::GlobalUnlock((HGLOBAL) FBufHandle);
#ifdef EIGHTBIT			
	if( hOldPal != NULL )
		::SelectPalette(hDC, hOldPal, true);
#endif
	
	if( cbfactor!=1 )
	{	XRange = pDoc->m_DataDisplay.Data_XRange;
		YRange = pDoc->m_DataDisplay.Data_YRange;
		Range =  MinFun(XRange,YRange);
		ZRange = Range;
		HRange = YRange>>1;
		WRange = XRange>>1;
		ReDrawFlag |= RFReSize;
	    RefreshState = True;
		ClearImage();
		RefreshScreen();
	}

	EndWait();
}

void CRasView::OnDraw(CDC* pDC)
{
	HDC hDC = pDC->m_hDC;
#ifdef EIGHTBIT			
	HPALETTE hOldPal = NULL;
#endif
	COLORREF BackColRef;
	CBrush Brush;
	CBrush *OldBrush;
	HDC hMemDC;
	CRect rec;

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	GETDOC
	pDoc->FixRange();

	if( pDC->IsPrinting() )
	{	// Preview
		if( pView!=this )
			return;
		if( !Database||!PixMap )
			return;
		PrintView(pDC, GetDocument());
		return;
	}

	/* Drawing active view */
	if( this==pView )	
	{	// Ensure activation state
		if( pDoc->ViewActivationFlag==0 )
			pDoc->ManageDisplay(true);

		if( !Database )
		{	TRACE1("TRACE\tOnDraw active view without molecule\t%s\n",pDoc->GetTitle());
	
#ifdef EIGHTBIT			
			if( ColourMap )
			{   hOldPal = SelectPalette(hDC,ColourMap,false);
				BackColRef = RGB(BackR,BackG,BackB);
				RealizePalette(hDC);
			} else
				BackColRef = RGB(0,0,0);
#else
			BackColRef = RGB(BackR,BackG,BackB);
#endif			
			Brush.CreateSolidBrush(BackColRef);
			rec.SetRect(0,0,XRange,YRange);
			OldBrush = pDC->SelectObject(&Brush);
			pDC->FillRect((LPCRECT) rec, &Brush);
			pDC->SelectObject(OldBrush);
			DeleteObject(Brush);	
	
#ifdef EIGHTBIT			
			if( ColourMap&&hOldPal )
				::SelectPalette(hDC,hOldPal,False);
#endif
		} else 
		{	TRACE("TRACE\tOnDraw active view ");
			if( ReDrawFlag )
			{	TRACE1("with refresh\t%s\n",pDoc->GetTitle());
                pFrame->m_wndCommandBar.ChangeStatusLight(false);
				clock_t DrawStart = clock();
				RefreshScreen();
				clock_t DrawEnd = clock(); 
				if( DrawEnd==-1||DrawStart==-1 )
					m_drawingtime = 25;
				else
					m_drawingtime =  
					(int)(1000*((double)(1+DrawEnd-DrawStart)/CLOCKS_PER_SEC));

				pFrame->m_wndCommandBar.ChangeStatusLight(true);
			} else
                TRACE1("without Refresh\t%s\n",pDoc->GetTitle());

			if( PixMap )
			{	
#ifdef EIGHTBIT			
				hOldPal = ::SelectPalette(hDC, ColourMap, false);
				::RealizePalette(hDC); 
#endif
				::SetStretchBltMode(hDC, COLORONCOLOR);
				hMemDC = CreateCompatibleDC(hDC); 
				::SelectObject(hMemDC,PixMap);
				::BitBlt(hDC,0,0,XRange,YRange,hMemDC,0,0,SRCCOPY);			 
				DeleteDC(hMemDC);
#ifdef EIGHTBIT			
				if( hOldPal!=NULL )
					hOldPal = ::SelectPalette(hDC,hOldPal,true);
#endif
			}
		}
	} else	
	/* Drawing inactive view */
	{	// Ensure activation state
		if( pDoc->ViewActivationFlag==1 )
			pDoc->ManageDisplay(false);

		if( !pDoc->m_DataDisplay.Data_Database )
		{	TRACE1("TRACE\tOnDraw inactive view without molecule\t%s\n",
		            pDoc->GetTitle() );
			
			BackColRef = RGB(pDoc->m_DataDisplay.Data_BackR,
							 pDoc->m_DataDisplay.Data_BackG,
							 pDoc->m_DataDisplay.Data_BackB);
			Brush.CreateSolidBrush(BackColRef);
			rec.SetRect(0,0,XRange,YRange);
			OldBrush = pDC->SelectObject(&Brush);
			pDC->FillRect((LPCRECT) rec, &Brush);
			pDC->SelectObject(OldBrush);
			DeleteObject(Brush);	
		} else
		{	TRACE1( "TRACE\tOnDraw inactive view\t%s\n", pDoc->GetTitle() );

			::SetStretchBltMode(hDC, COLORONCOLOR);
	
			hMemDC = CreateCompatibleDC(hDC); 
			::SelectObject(hMemDC,pDoc->m_PixMap);
			::BitBlt(hDC,0,0,pDoc->m_DataDisplay.Data_XRange,
							 pDoc->m_DataDisplay.Data_YRange,
							 hMemDC,0,0,SRCCOPY);			
			DeleteDC(hMemDC);
		}
	}
	ReDrawFlag = 0;
}

void CRasView::OnWindowTileVert() 
{	
	GETDOC
	pDoc->ManageDisplay(false);

	GETFRAME
	pFrame->MDITile(MDITILE_VERTICAL);
	pFrame->PostMessage( WM_UPDATERASVIEW , 0, 0);
}

void CRasView::OnWindowTileHorz() 
{	
	GETDOC
	pDoc->ManageDisplay(false);

	GETFRAME
	pFrame->MDITile(MDITILE_HORIZONTAL);
	pFrame->PostMessage(WM_UPDATERASVIEW,0,0);
}

void CRasView::OnWindowTile() 
{	GETDOC
	GETFRAME
	int nDoc = 0;	
	CRect rRecFrame;

	//Count documents	
	CDocTemplate* pDocTemp;
	pDocTemp = pDoc->GetDocTemplate( );
	POSITION posdoc = pDocTemp->GetFirstDocPosition( );
	while( posdoc != NULL )
	{	pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		if( pDoc->IsKindOf(RUNTIME_CLASS(CRasDoc)) )
			nDoc++;
	}

	if( nDoc <= 1 )
	{	OnWindowTileVert();
		return;
	}

	rRecFrame = pFrame->GetScreenClientCoor();

	if( nDoc == 2 )
	{	if(  rRecFrame.Width( ) >= rRecFrame.Height( ) )
			OnWindowTileVert();
		else
			OnWindowTileHorz();
		return;
	}

	if( rRecFrame.Height() == 0 )
		return;

	int nCol = 1;
	int nRaw = 1;
	double dRatio = (double) rRecFrame.Width( )/rRecFrame.Height( );
	if(  dRatio >= 1 )
	{	nCol = (int) sqrt((double)nDoc);
		if( nCol*nCol < nDoc )
			nCol++;
		while( nCol*nRaw < nDoc )
			nRaw++;
		//maximize occupancy
		if( abs((nCol + 1)*(nRaw - 1) - nDoc) < (nCol*nRaw - nDoc) 
			&& nRaw != 1 && nCol != 1 )
		{	nCol++;
			nRaw--;
		}
		//correct some discrepency for extreme ratio
		if( (int) (dRatio + 1 ) >= nDoc )
		{	nCol  = nDoc;
			nRaw = 1;
		}
	} else
	{	nRaw = (int) sqrt((double)nDoc);
		if( nRaw*nRaw < nDoc )
			nRaw++;
		while( nCol*nRaw < nDoc )
			nCol++;
		if( abs((nCol - 1)*(nRaw + 1) - nDoc) < (nCol*nRaw - nDoc) 
			&& nRaw != 1 && nCol != 1 )
		{	nCol--;
			nRaw++;
		}
		dRatio = 1/dRatio;
		if( (int)(dRatio + 1) >= nDoc )
		{	nRaw  = nDoc;
			nCol = 1;
		}
	}

	int nSizeX;
	int nSizeY;
	nSizeX = rRecFrame.Width( )/nCol;		
	nSizeY = rRecFrame.Height( )/nRaw;
	if( nSizeX < ::GetSystemMetrics(SM_CXMINIMIZED) )
		nSizeX = ::GetSystemMetrics(SM_CXMINIMIZED);
	if( nSizeY <= ::GetSystemMetrics(SM_CYMINIMIZED) + 10 )
		return;

	pDoc = GetDocument();
	pDoc->ManageDisplay(false);

	nDoc = 0;
	pDocTemp = pDoc->GetDocTemplate( );
	posdoc = pDocTemp->GetFirstDocPosition( );
	while( posdoc != NULL )
	{	pDoc = (CRasDoc *)pDocTemp->GetNextDoc(posdoc);
		if( !pDoc->IsKindOf(RUNTIME_CLASS(CRasDoc)) )
			continue;
		//Get the unique view
		POSITION posview = pDoc->GetFirstViewPosition( );
		CRasView* pView = (CRasView *)pDoc->GetNextView( posview );
		ASSERT( posview == 0 && pView != NULL);
		CMDIChildWnd *pParent = (CMDIChildWnd *) pView->GetParent( );

		if( pParent->IsIconic( ) )
			pParent->MDIRestore( );

		if( !pParent->IsIconic( ) )
		{	pParent->ShowWindow(SW_SHOWNORMAL);
			pParent->MoveWindow( nSizeX*(nDoc%nCol),
							 nSizeY*(nDoc/nCol), 
							 nSizeX,
	 						 nSizeY, 
							 true );
		}

		nDoc++;
	}

	pFrame->PostMessage( WM_UPDATERASVIEW , 0, 0);

}

void CRasView::OnWindowCascade() 
{	
	GETDOC
	pDoc->ManageDisplay(false);

	GETFRAME
	pFrame->MDICascade( );
	pFrame->PostMessage( WM_UPDATERASVIEW, 0, 0);
}

void CRasView::PrintImage( )
{	OnFilePrint();	}

void CRasView::OnFilePrint() 
{	CView::OnFilePrint();	}

void CRasView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(Database != NULL);	}

BOOL CRasView::OnPreparePrinting(CPrintInfo* pInfo)
{	// default preparation
	return DoPreparePrinting(pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// CRasView diagnostics

#ifdef _DEBUG
void CRasView::AssertValid() const	{	CView::AssertValid();	}
void CRasView::Dump(CDumpContext& dc) const	{	CView::Dump(dc);}

CRasDoc* CRasView::GetDocument() // non-debug version is inline
{	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRasDoc)));
	return (CRasDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

BOOL CRasView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, 
	 const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	lpszClassName =	AfxRegisterWndClass(CS_BYTEALIGNCLIENT | CS_DBLCLKS |
					CS_HREDRAW | CS_VREDRAW, NULL, NULL, NULL);
	
	return CWnd::Create(lpszClassName, lpszWindowName, 
		dwStyle, rect, pParentWnd, nID, pContext);
}

void CRasView::RefreshView()
{	Invalidate(true);
	UpdateWindow( );
	GETFRAME
	pFrame->m_wndCommandBar.UpdatePannel();
}

void CRasView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);	

	if( bActivate )
		ASSERT( pActivateView==this );
    else
		ASSERT( pDeactiveView==this );

	GETDOC

    if( !bActivate )
    {   // Test when we can keep this view alive
        if( pDeactiveView->IsKindOf(RUNTIME_CLASS(CRasView)) )
	    {   TRACE1( "TRACE\tOnActivateView\tLiving without Managing display to false\t%s\n", pDoc->GetTitle() );
            return;
        }
    }

	pDoc->ManageDisplay( bActivate );
}

void CRasView::OnSize(UINT nType, int cx, int cy) 
{	int dx;

	GETDOC
	CView::OnSize(nType, cx, cy);
//	TRACE1( "TRACE\tOnSize\t%s\n", pDoc->GetTitle() );

	/*Save screen size*/
	if( dx = cx%4 )
		cx += 4-dx;
	pDoc->m_DataDisplay.Data_XRange = cx;
	pDoc->m_DataDisplay.Data_YRange = cy;

	if( Database )
		ReDrawFlag |= RFReSize;
}


int CRasView::ExecuteCommand(int display, CString str)
{	char *ptr =  (LPTSTR) str.LockBuffer();
	int len = 0;
    char echo[MAXBUFFLEN];
    int i = 0;

	do
	{	while( *ptr != '\n' && *ptr !='\r' )
        {   if( !(*ptr) ) 
                break;
            if( len<MAXBUFFLEN )
                  CurLine[len++] = *ptr++;
            else
                break;
		}

        if( len<MAXBUFFLEN )
        {   CurLine[len] = '\0';
			if( len )
			{	if (GUItoCmdEcho) {
                    for (i=0;i<MAXBUFFLEN;i++) {
                        echo[i] = CurLine[i];
                        if (!echo[i])
                            break;
                    }
                    InvalidateCmndLine();
                    WriteString(echo);
                    WriteString("\n");
                    /* Regenerate CurLine */
			        for (i=0;i<MAXBUFFLEN;i++) {
                        CurLine[i] = echo[i];
                        if (!CurLine[i])
                            break;
                    }
                }

                if( ::ExecuteCommand() )
					RasMolExit();
				if( display==DisplayOn )
					RefreshView();
			    len = 0;
                
			}
        } else 
			WriteString(MsgStrs(RTPScrLong));
		
		if( *ptr ) ptr++;
	}
	while( *ptr );

	str.UnlockBuffer();
	return true;
}

static int GetStatus( int mask )
{
    register int status;
    
    status = 0;                             
    if( mask & MK_LBUTTON ) status |= MMLft;
    if( mask & MK_MBUTTON ) status |= MMMid;
    if( mask & MK_RBUTTON ) status |= MMRgt;
    if( mask & MK_CONTROL ) status |= MMCtl;          
    if( mask & MK_SHIFT )   status |= MMSft;
    return status;
}

void CRasView::OnMouseMove(UINT nFlags, CPoint point) 
{	
	// Cursor Mode
	if( (point.x>0) && (point.y>0) && (point.x<XRange) && (point.y<YRange) )
	{	HCURSOR Cursor;
		if( CursorMode==CURRasmol )
			Cursor = AfxGetApp()->LoadCursor(IDC_RASCURSOR);
		else if( CursorMode==CURPicking )
			Cursor = AfxGetApp()->LoadCursor(IDC_PICKINGCURSOR);
		else if( CursorMode==CURPickplus )
			Cursor = AfxGetApp()->LoadCursor(IDC_PICKINGPLUS);
		else if( CursorMode==CURPickminus )
			Cursor = AfxGetApp()->LoadCursor(IDC_PICKINGMINUS);
		if( Cursor!=::GetCursor() ) 
			::SetCursor(Cursor);
	}
	
	// Remove repeated messages to accuratly reflect mouse moves
	MSG msg;
	/* GetQueueStatus */
	if( ::PeekMessage(&msg, m_hWnd, WM_LBUTTONUP, WM_LBUTTONUP, PM_NOREMOVE) )
		while(::PeekMessage(&msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
	if( ::PeekMessage(&msg, m_hWnd, WM_MBUTTONUP, WM_MBUTTONUP, PM_NOREMOVE) )
		while(::PeekMessage(&msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
	if( ::PeekMessage(&msg, m_hWnd, WM_RBUTTONUP, WM_RBUTTONUP, PM_NOREMOVE) )
		while(::PeekMessage(&msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
	
	GETFRAME
	GETVIEW
	if( (pView==NULL) || (this!=pView) || !Database )
		return;

	ProcessMouseMove(point.x,point.y,GetStatus(nFlags));

	if( ReDrawFlag )
		RefreshView();
}

// Need improvement to count messages...
BOOL CRasView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{	static int totalcount = 0;
	static short z = 120;
	static UINT fl = 0;

	if( zDelta )
	{	z = zDelta;
		fl = nFlags;
	}
	
	if( (nFlags&MK_LBUTTON)||(nFlags&MK_MBUTTON)||(nFlags&MK_RBUTTON) )
	{	totalcount = 0;
		return false;
	}

	// Count wheel move messages
	MSG msg;
	msg.wParam = 0;
	int count = 1;
	if( ::PeekMessage(&msg, m_hWnd, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_NOREMOVE) )
		while(::PeekMessage(&msg, m_hWnd, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_REMOVE))
			count++;
	if( totalcount!=count )
	{	totalcount = count;
		// tric to force cumulated messages in the queue
		while(count--) PostMessage( WM_MOUSEWHEEL,0,0);
		return false;
	}

	GETFRAME
	GETVIEW
	if( (pView==NULL) || (this!=pView) || !Database )
	{	totalcount = 0;
		return false;
	}

	double notchvalue;
	if(fl) 
	{	notchvalue = 0.01;
		z = -z;
	} else
		notchvalue = 0.002;

	double dial = notchvalue*count*count*count*z/WHEEL_DELTA;
	
	ProcessWheelDial(dial,GetStatus(fl));
	totalcount = 0;

	if( ReDrawFlag )
	{	RefreshView();
		return true;
	} else return false;
}

void CRasView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	//Stop spinMode
	GETFRAME
	pFrame->m_wndCommandBar.ResetSpinMode();

	//Stop PickingMode
	::SetPickMode(PickIdent);
	DrawArea = false;
}

void CRasView::OnLButtonDown(UINT nFlags, CPoint point) 
{	OnButtomDown(nFlags, point) ;	}
void CRasView::OnMButtonDown(UINT nFlags, CPoint point) 
{	OnButtomDown(nFlags, point) ;	}
void CRasView::OnRButtonDown(UINT nFlags, CPoint point) 
{	OnButtomDown(nFlags, point) ;	}

void CRasView::OnLButtonUp(UINT nFlags, CPoint point) 
{	OnButtomUp(nFlags, point) ;	}
void CRasView::OnMButtonUp(UINT nFlags, CPoint point) 
{	OnButtomUp(nFlags, point) ;	}
void CRasView::OnRButtonUp(UINT nFlags, CPoint point) 
{	OnButtomUp(nFlags, point) ;	}

void CRasView::OnButtomDown(UINT nFlags, CPoint point)
{	ProcessMouseDown(point.x,point.y,GetStatus(nFlags));
	STOPTIMER
}

void CRasView::OnButtomUp(UINT nFlags, CPoint point)
{	ProcessMouseUp(point.x, point.y,GetStatus(nFlags));
	RefreshView();
	GETFRAME
	STARTTIMER
}


///////////////////////////////////////////////////////////////////////////////////
// Defined Representations in Main Toolbar
///////////////////////////////////////////////////////////////////////////////////

void CRasView::OnDisplaySphere() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Sphere VDW representation\n");
	ExecuteCommand(DisplayOff,"wireframe off\n");
	ExecuteCommand(DisplayOff,"spacefill on\n");	
	ExecuteCommand(DisplayOn," \n");	
}

void CRasView::OnUpdateDisplaySphere(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}

void CRasView::OnDisplayStar() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Star representation\n");
	ExecuteCommand(DisplayOff,"wireframe off\n");
	if( MainAtomCount<256 )
		ExecuteCommand(DisplayOff,"star 80\n");
	else 
		ExecuteCommand(DisplayOff,"star 160\n");
	ExecuteCommand(DisplayOn," \n");	
}

void CRasView::OnUpdateDisplayStar(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}

void CRasView::OnDisplayWireframeOnly() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Wireframe representation\n");
	ExecuteCommand(DisplayOff,"spacefill off\n");	
	ExecuteCommand(DisplayOff,"wireframe on\n");
	ExecuteCommand(DisplayOn," \n");	
}

void CRasView::OnUpdateDisplayWireframeOnly(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}

void CRasView::OnDisplaySticks() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Stick Representation\n");
	ExecuteCommand(DisplayOff,"spacefill off\n");	
	if( MainAtomCount<256 )
		ExecuteCommand(DisplayOff,"wireframe 40\n");
	else 
		ExecuteCommand(DisplayOff,"wireframe 75\n");	
	ExecuteCommand(DisplayOn," \n");	
}

void CRasView::OnUpdateDisplaySticks(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}

void CRasView::OnDisplayBallandstick() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Ball & Stick representation\n");
	ExecuteCommand(DisplayOff,"spacefill 120\n");	
	ExecuteCommand(DisplayOff,"wireframe 40\n");
	ExecuteCommand(DisplayOn," \n");	
}

void CRasView::OnUpdateDisplayBallandstick(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}


void CRasView::OnDisplayNothing() 
{	if( !Database )
	return;
	ExecuteCommand(DisplayOff,"# Remove all representations except surfaces\n");
	ExecuteCommand(DisplayOff,"spacefill off\n");	
	ExecuteCommand(DisplayOff,"ribbon off\n");
	ExecuteCommand(DisplayOff,"wireframe off\n");
	ExecuteCommand(DisplayOff,"backbone off\n");
	ExecuteCommand(DisplayOn," \n");	
	
}

void CRasView::OnUpdateDisplayNothing(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	pCmdUI->SetCheck( false );
}


///////////////////////////////////////////////////////////////////////////////////
// Scripting Menu
///////////////////////////////////////////////////////////////////////////////////

/*======*/
/* Edit */
/*======*/

void CRasView::OnEditRasmolCommand() 
{	CDialogCommand dlg;
	dlg.m_Command = m_PreviousCommand;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	if( dlg.m_Command.IsEmpty() )
	{	STARTTIMER
		return;
	}

	ExecuteCommand( DisplayOff, dlg.m_Command + '\n');
	RefreshView();

	if( dlg.m_Command.Find("script")!=-1 )
	{	m_PreviousCommand = "";
		STARTTIMER
		return;
	}

	m_PreviousCommand = dlg.m_Command;

	if( m_PreviousCommand.Find('\n')!=-1 && m_PreviousCommand.Find('\r')==-1 )
	{	CString temp;
		temp = m_PreviousCommand[0];	//not empty
		for( int i=1;i<m_PreviousCommand.GetLength();i++ )
		{	if( m_PreviousCommand[i]=='\n' && m_PreviousCommand[i-1]!='\r' )
				temp += "\r\n";
			else
				temp += m_PreviousCommand[i];
		}
		m_PreviousCommand = temp;
	}
	STARTTIMER
}


/*======================*/
/* Clipboard operations */
/*======================*/

void CRasView::OnCopyImage() 
{	ExecuteCommand( DisplayOff, "clipboard image\n");	}

void CRasView::OnUpdateCopyImage(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnSelectionCopy() 
{	ExecuteCommand( DisplayOff, "clipboard selected\n");	}

void CRasView::OnUpdateSelectionCopy(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnCopyPosition() 
{	ExecuteCommand( DisplayOff, "clipboard position\n");	}

void CRasView::OnUpdateCopyPosition(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnPaste() 
{	ExecuteCommand( DisplayOff, "clipboard paste\n");	}

void CRasView::OnUpdatePaste(CCmdUI* pCmdUI) 
{	char *ptr;
	HANDLE data;
	BOOL Clip = false;
	char Content[10] = {0};
	int i;

	if( OpenClipboard() )
	{	if( data = GetClipboardData(CF_TEXT) )
		{	ptr = (char*)GlobalLock(data);
			GlobalUnlock(data);
			for( i=0;i<9;i++ )
			{	if( !ptr )
					break;
				Content[i] = *(ptr+i);
			}
			Clip = !(strstr(Content,"#!rasmol")==NULL);
		}				

		CloseClipboard();
	}			

	pCmdUI->Enable(Clip);
}


/*========*/
/* Select */
/*========*/

void CRasView::OnSelectDefine() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_DEF1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_DEF2);
	dlg.m_TexComment = s;
	dlg.m_ValueType = Type_Ident;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	if( dlg.m_ValueTex.IsEmpty() )
	{	STARTTIMER
		return;
	}

	//Check for reentry
	SymEntry  **prev, *ptr;
	int test = 1;

	prev = &SymbolTable;
	while( *prev )
	{   ptr = *prev;
		    test = _stricmp((LPCSTR)(dlg.m_ValueTex),ptr->ident);
		    if( !test ) break;
		    prev = (test<0)? &(ptr->lft) : &(ptr->rgt);
	}
	if( !test )
	{	CDialog Wng(IDD_DIALOG_NEWSET,NULL);
		if( Wng.DoModal()!=IDOK )
		{	STARTTIMER
			return;
		}
	}

	//the dialog box makes character checking
	//Add the new set
	ExecuteCommand( DisplayOff, "define " + dlg.m_ValueTex + " selected\n");

	GETDOC
	pDoc->SetModifiedFlag(true);
	STARTTIMER
}

void CRasView::OnUpdateSelectDefine(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnSelectAll() 
{	if( Database ) ExecuteCommand( DisplayOn, "select all\n"); }

void CRasView::OnUpdateSelectAll(CCmdUI* pCmdUI) 
{	/*pCmdUI->Enable( Database!=NULL );*/	}

void CRasView::OnSelectInverse() 
{	if( Database ) ExecuteCommand( DisplayOn, "select not selected\n");	}

void CRasView::OnUpdateSelectInverse(CCmdUI* pCmdUI) 
{	/*pCmdUI->Enable( Database!=NULL );*/	}

void CRasView::OnSelectPrevious() 
{	if( Database ) ExecuteCommand( DisplayOn , "select previous\n"); }

void CRasView::OnUpdateSelectPrevious(CCmdUI* pCmdUI) 
{	/*pCmdUI->Enable( Database!=NULL );*/	}


/* Centre */

void CRasView::OnSelectCentre() 
{	ExecuteCommand( DisplayOn, "centre selected centre\n");	}

void CRasView::OnUpdateSelectCentre(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnCentreWorld() 
{	ExecuteCommand( DisplayOn, "centre world\n");	}

void CRasView::OnUpdateCentreWorld(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnCentreOrigin() 
{	ExecuteCommand( DisplayOn, "centre origin\n");	}

void CRasView::OnUpdateCentreOrigin(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnOrigintocentre() 
{	ExecuteCommand( DisplayOn, "centre origin centre\n");	}

void CRasView::OnUpdateOrigintocentre(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnCentreCentretoorigin() 
{	ExecuteCommand( DisplayOn, "centre origin translate\n");	}

void CRasView::OnUpdateCentreCentretoorigin(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnShowSelectAtm() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show selected atom\n");
        return;
    }

	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_ATM;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateShowSelectAtm(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnShowSelectGrp() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show selected group\n");
        return;
    }

	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_GRP;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateShowSelectGrp(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnShowSelectChn() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show selected chain\n");
        return;
    }

	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_CHN;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateShowSelectChn(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnShowSelectCoor() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show selected coord\n");
        return;
    }

	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_COOR;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateShowSelectCoor(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnSelectExpression() 
{	static CString Exp = _T("");
	CDialogSelect dlg;
	dlg.m_sExpr = Exp;
	
	if( !Database ) return;

	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	if( dlg.m_sExpr.IsEmpty() )
	{	STARTTIMER
		return;
	}
	pFrame->SetSelection(dlg.m_nCommand, dlg.m_sExpr);
	Exp = dlg.m_sExpr;
	STARTTIMER
}

void CRasView::OnUpdateSelectExpression(CCmdUI* pCmdUI) 
{	/*pCmdUI->Enable( Database!=NULL );*/	}

void CRasView::OnSelectWithin() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_WITH1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_WITH2);
	dlg.m_TexComment = s;
	dlg.m_ValueType = Type_Within;
	
	STOPTIMER
	if(dlg.DoModal()==IDOK )
	ExecuteCommand( DisplayOn,"select within(" + dlg.m_ValueTex + ", selected)\n");
	STARTTIMER
}

void CRasView::OnUpdateSelectWithin(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnSelectHydrogens() 
{	if( Hydrogens )
		ExecuteCommand( DisplayOn, "set hydrogen off\n");
	else 
		ExecuteCommand( DisplayOn, "set hydrogen on\n");
}

void CRasView::OnUpdateSelectHydrogens(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(Hydrogens);	}

void CRasView::OnSelectBondmode() 
{	if( ZoneBoth )
		ExecuteCommand( DisplayOn, "set bondmode or\n");
	else 
		ExecuteCommand( DisplayOn, "set bondmode and\n");
}

void CRasView::OnUpdateSelectBondmode(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(!ZoneBoth);	}

void CRasView::OnSelectHeteAtoms() 
{	if( HetaGroups )
		ExecuteCommand( DisplayOn, "set hetero off\n");
	else 
		ExecuteCommand( DisplayOn, "set hetero on\n");
}

void CRasView::OnUpdateSelectHeteAtoms(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(HetaGroups);	}

void CRasView::OnSelectModel() 
{	ModelInclude = !(BOOL)ModelInclude;	}

void CRasView::OnUpdateSelectModel(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(ModelInclude);	}

void CRasView::OnRestrict() 
{	ExecuteCommand( DisplayOn, "restrict selected\n");	}

void CRasView::OnUpdateRestrict(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(Database!=NULL);	}

void CRasView::OnSelectShow() 
{	if( !Database ) return;
	if( DisplayMode==0 )
		ExecuteCommand( DisplayOn, "set display selected\n");
	else
		ExecuteCommand( DisplayOn, "set display normal\n");
}

void CRasView::OnUpdateSelectShow(CCmdUI* pCmdUI) 
{	if( Database==NULL )
		pCmdUI->SetCheck(false);
	else
		pCmdUI->SetCheck(DisplayMode);
}


/*==========*/
/* Molecule */
/*==========*/

void CRasView::OnMoleculeDelete() 
{	CString s;
    s.LoadString(IDS_LOCAL_DEL);
	if( AfxMessageBox(s, MB_YESNOCANCEL) == IDYES )
	{	ExecuteCommand( DisplayOn, "zap molecule\n");
	}	
}

void CRasView::OnUpdateMoleculeDelete(CCmdUI* pCmdUI) 
{	pCmdUI->Enable((BOOL)Database);	}

void CRasView::OnMoleculeInfo() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show information\n");
        return;
    }

    CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_INFO;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateMoleculeInfo(CCmdUI* pCmdUI) 
{	pCmdUI->Enable((BOOL)Database);	}

void CRasView::OnWorldInfo() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show world\n");
        return;
    }
    
    CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_WORLD;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateWorldInfo(CCmdUI* pCmdUI) 
{	pCmdUI->Enable((BOOL)Database);	}

void CRasView::OnMoleculeSeq()
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show sequence\n");
        return;
    }
    
 	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_SEQ;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateMoleculeSeq(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnMoleculePhipsi() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show phipsi\n");
        return;
    }
    
	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_PHIPSI;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateMoleculePhipsi(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnMoleculeRpp() 
{	
    if( WindowCommandLine ) {
        ExecuteCommand(DisplayOff,"show RPP\n");
        return;
    }
    
	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_RPP;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateMoleculeRpp(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnEditMolecularFile() 
{	//document variables are set in the dialog
	STOPTIMER
	CDialogEditMolecularFile dlg;
	dlg.DoModal(); 
	STARTTIMER
}

void CRasView::OnMoleculeShow() 
{	if( CurMolSet->visible )
		ExecuteCommand(DisplayOn,"molecule hide\n");
	else
		ExecuteCommand(DisplayOn,"molecule show\n");
}

void CRasView::OnUpdateMoleculeShow(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

/*========*/
/* Labels */
/*========*/

void CRasView::OnLabelsOn()		
{	ExecuteCommand(DisplayOn,"label on\n");
} 

void CRasView::OnUpdateLabelsOn(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnLabelsDefault() 
{	DefaultLabels(true);
	ReDrawFlag |= RFRefresh;
	RefreshView();
}

void CRasView::OnUpdateLabelsDefault(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnLabelsOff()
{	ExecuteCommand(DisplayOn,"label off\n");	}

void CRasView::OnUpdateLabelsOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnLabelsSettings() 
{	CDialogLabel dlg;

	dlg.m_nFontSize = FontSize;
	dlg.m_FontStroke = FontStroke;
	if( FontPS==0 )
		dlg.m_FontFS = 0;
	else
		dlg.m_FontFS = 1;

	dlg.m_sLabelFormat = LabelFormat;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}

	int i;
	for( i=0; i<dlg.m_sLabelFormat.GetLength();i++ )
		LabelFormat[i] = dlg.m_sLabelFormat.GetAt(i);
	LabelFormat[i] = '\0';

	if( dlg.m_nFontSize!=FontSize )
	{	Command.Format("set fontsize %d ", dlg.m_nFontSize);
		if( dlg.m_FontFS==1 )
			Command += "PS\n";
		else
			Command += "FS\n";
		ExecuteCommand(DisplayOn, Command);

	} else if( dlg.m_FontFS==!FontPS )
	{	Command.Format("set fontsize %d ", FontSize);
		if( dlg.m_FontFS==0 )
			Command += "FS\n";
		else
			Command += "PS\n";
		ExecuteCommand(DisplayOn, Command);
	}

	if( dlg.m_FontStroke!=FontStroke )
	{	Command.Format("set fontstroke %d\n", dlg.m_FontStroke);
		ExecuteCommand(DisplayOn, Command);
	}
	STARTTIMER
}

void CRasView::OnColorLabels() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	ExecuteCommand(DisplayOn, "color label none\n");
		return;
	}
	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	if( !LabelList )
		DefaultLabels(true);
	Command = "color labels " + Command + "\n";
	ExecuteCommand(DisplayOn, Command);
}


/*==========*/
/* Monitors */
/*==========*/

void CRasView::OnMonitorColour() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "colour monitor none\n");
		return;
	}
	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color monitor " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnDistanceRemove() 
{	ExecuteCommand( DisplayOn, "monitor false\n");	}

void CRasView::OnUpdateDistanceRemove(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDistanceShow() 
{	if(DrawMonitDistance)
		ExecuteCommand( DisplayOn, "set monitor false\n");
	else
		ExecuteCommand( DisplayOn, "set monitor true\n");
}

void CRasView::OnUpdateDistanceShow(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DrawMonitDistance);	}


/*===========*/
/* Crystallo */
/*===========*/

void CRasView::OnMoleculeSym()	
{	CDialogRasMol Dlg;
	Dlg.m_ShowType = DLG_SYM;
	STOPTIMER
	Dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateMoleculeSym(CCmdUI* pCmdUI) 
{	pCmdUI->Enable((BOOL)Database && *Info.spacegroup);	}

void CRasView::OnDisplayUnitcell() 
{	if( !DrawUnitCell )
		ExecuteCommand( DisplayOn, "set unitcell on\n");
	else
		ExecuteCommand( DisplayOn, "set unitcell off\n");
}

void CRasView::OnUpdateDisplayUnitcell(CCmdUI* pCmdUI) 
{	if( (Database != NULL) && *Info.spacegroup )
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck(DrawUnitCell);
	} else
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnDisplayAxes()  
{	if( !DrawAxes )
		ExecuteCommand( DisplayOn, "set axes on\n");
	else
		ExecuteCommand( DisplayOn, "set axes off\n");
}

void CRasView::OnUpdateDisplayAxes(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );
	if( Database != NULL )
		pCmdUI->SetCheck(DrawAxes);	
}

void CRasView::OnDisplayWorldaxes() 
{	if( !Database )
        return;
    if( !DrawWorldAxes )
		ExecuteCommand( DisplayOn, "set axes world on\n");
	else
		ExecuteCommand( DisplayOn, "set axes world off\n");
}

void CRasView::OnUpdateDisplayWorldaxes(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );
	if( Database != NULL )
		pCmdUI->SetCheck(DrawWorldAxes);
    else
		pCmdUI->SetCheck(false);
}

void CRasView::OnDisplayBoundbox() 
{	if( !DrawBoundBox )
		ExecuteCommand( DisplayOn, "set boundbox on\n");
	else
		ExecuteCommand( DisplayOn, "set boundbox off\n");
}

void CRasView::OnUpdateDisplayBoundbox(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );
	if(Database != NULL)
		pCmdUI->SetCheck(DrawBoundBox);
}

void CRasView::OnColorAxe() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
		return;

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);

	Command = "color world axes " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnUpdateColorAxe(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(DrawWorldAxes);	}

void CRasView::OnColorBox() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
		return;

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);

	Command = "color axes " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnUpdateColorBox(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(DrawUnitCell || DrawAxes || DrawBoundBox);	}


/*===========*/
/* Structure */
/*===========*/

void CRasView::OnMoleculeStruc()
{	ExecuteCommand( DisplayOn, "structure\n");	}

void CRasView::OnUpdateMoleculeStruc(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnMoleculeConnect() 
{	ExecuteCommand( DisplayOn, "connect\n");	}

void CRasView::OnUpdateMoleculeConnect(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnMoleculeRenumber() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_NUM1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_NUM2);
	dlg.m_TexComment = s;
	dlg.m_ValueNumber = 1;
	dlg.m_ValueType = Type_Renumber;
	
	STOPTIMER
	if(dlg.DoModal()==IDOK)
		ExecuteCommand( DisplayOn, "renumber " + dlg.m_ValueTex + "\n");
	STARTTIMER
}

void CRasView::OnUpdateMoleculeRenumber(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}


void CRasView::OnSetCisangle() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_CIS1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_CIS2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = CisBondCutOff;
	dlg.m_ValueType = Type_CisAngle;
	
	STOPTIMER
	if(dlg.DoModal()==IDOK)
		ExecuteCommand( DisplayOff, "set cisangle " + dlg.m_ValueTex + "\n");
	STARTTIMER
}

void CRasView::OnUpdateSetCisangle(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnSetConnect() 
{	CalcBondsFlag = !CalcBondsFlag;	}

void CRasView::OnUpdateSetConnect(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(CalcBondsFlag);	}



/*========*/
/* Rotate */
/*========*/

void CRasView::OnStaticrotx90() 
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate x 90\n");
	else
		ExecuteCommand( DisplayOn, "rotate world x 90\n");
}
void CRasView::OnStaticrotx180()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate x 180\n");
	else
		ExecuteCommand( DisplayOn, "rotate world x 180\n");
}
void CRasView::OnStaticrotx270()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate x 270\n");
	else
		ExecuteCommand( DisplayOn, "rotate world x 270\n");
}
void CRasView::OnStaticroty90() 
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate y 90\n");
	else
		ExecuteCommand( DisplayOn, "rotate world y 90\n");
}
void CRasView::OnStaticroty180()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate y 180\n");
	else
		ExecuteCommand( DisplayOn, "rotate world y 180\n");
}
void CRasView::OnStaticroty270()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate y 270\n");
	else
		ExecuteCommand( DisplayOn, "rotate world y 270\n");
}
void CRasView::OnStaticrotz90() 
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate z 90\n");
	else
		ExecuteCommand( DisplayOn, "rotate world z 90\n");
}
void CRasView::OnStaticrotz180()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate z 180\n");
	else
		ExecuteCommand( DisplayOn, "rotate world z 180\n");
}
void CRasView::OnStaticrotz270()
{	if( RotMode==RotMol )
		ExecuteCommand( DisplayOn, "rotate z 270\n");
	else
		ExecuteCommand( DisplayOn, "rotate world z 270\n");
}


//////////////////////////////////////////////////////////////////////////////////
// Atoms 

void CRasView::OnDisplaySpacefill() 
{	if( m_SpacefillRepres == SphereFlag )
		ExecuteCommand( DisplayOn, "spacefill " + m_SpacefillDisplay + '\n');
	else
		ExecuteCommand( DisplayOn, "star " + m_SpacefillDisplay + '\n');	}

void CRasView::OnUpdateDisplaySpacefill(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(Database != NULL);	}

void CRasView::OnDisplaySpacefillFull() 
{	DisableWireframe();
	SetRibbonStatus(False,0,0);
	DisableBackbone();
	OnDisplaySpacefill();
}

void CRasView::OnUpdateDisplaySpacefillFull(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplaySpacefillOff() 
{	ExecuteCommand( DisplayOn, "spacefill off\n");	}

void CRasView::OnUpdateDisplaySpacefillOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}


void CRasView::OnSpacefillSphere() 
{	m_SpacefillRepres = SphereFlag;
	OnDisplaySpacefill();
}

void CRasView::OnUpdateSpacefillSphere(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( m_SpacefillRepres == SphereFlag );	}

void CRasView::OnSpacefillStar() 
{	m_SpacefillRepres = StarFlag;
	OnDisplaySpacefill();
}

void CRasView::OnUpdateSpacefillStar(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( m_SpacefillRepres == StarFlag );	}

void CRasView::OnSpacefillFixedradius() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_RAD1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_RAD2);
    dlg.m_TexComment = s;
	dlg.m_ValueType = Type_Sphere;
	dlg.m_ValueNumber = ATOI( m_SpacefillDisplay );
	
	STOPTIMER
	if( dlg.DoModal() != IDOK )
	{	STARTTIMER
		return;
	}
	if( dlg.m_ValueNumber == 0 )
		m_SpacefillDisplay = "on";
	else
		m_SpacefillDisplay = dlg.m_ValueTex;
	OnDisplaySpacefill();
	STARTTIMER
}

void CRasView::OnSpacefillVanderwaals() 
{	m_SpacefillDisplay = "on";
	OnDisplaySpacefill();
}

void CRasView::OnSpacefillTemperature() 
{	m_SpacefillDisplay = "temperature";
	OnDisplaySpacefill();
}

void CRasView::OnSpacefillUser() 
{	m_SpacefillDisplay = "user";
	OnDisplaySpacefill();
}

void CRasView::OnUpdateSpacefillFixedradius(CCmdUI* pCmdUI) 
{	if( (m_SpacefillDisplay != "temperature")	&&
		(m_SpacefillDisplay != "user")			&&
		(m_SpacefillDisplay != "on")			)
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnUpdateSpacefillTemperature(CCmdUI* pCmdUI) 
{	if( m_SpacefillDisplay == "temperature" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnUpdateSpacefillUser(CCmdUI* pCmdUI) 
{	if( m_SpacefillDisplay == "user" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnUpdateSpacefillVanderwaals(CCmdUI* pCmdUI) 
{	if( m_SpacefillDisplay == "on" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnColorAtom() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp==0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "color atom cpk\n");
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);

	Command = "color atom " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnColourCpk() 
{	ExecuteCommand( DisplayOn, "colour atom cpk\n");	}

void CRasView::OnUpdateColourCpk(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourShapely() 
{	ExecuteCommand( DisplayOn, "colour atom shapely\n");	}

void CRasView::OnUpdateColourShapely(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourStructure() 
{	ExecuteCommand( DisplayOn, "colour atom structure\n");	}

void CRasView::OnUpdateColourStructure(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourGroup() 
{	ExecuteCommand( DisplayOn, "colour atom group\n");	}

void CRasView::OnUpdateColourGroup(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourChain() 
{	ExecuteCommand( DisplayOn, "colour atom chain\n");	}

void CRasView::OnUpdateColourChain(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourTemperature() 
{	ExecuteCommand( DisplayOn, "colour atom temperature\n");	}

void CRasView::OnUpdateColourTemperature(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourCharge() 
{	ExecuteCommand( DisplayOn, "colour atom charge\n");	}

void CRasView::OnUpdateColourCharge(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL);	}

void CRasView::OnColourUser() 
{	ExecuteCommand( DisplayOn, "colour atom user\n");	}

void CRasView::OnUpdateColourUser(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(Database != NULL);	}

void CRasView::OnColourAlt() 
{	ExecuteCommand( DisplayOn, "colour atom alt\n");	}

void CRasView::OnUpdateColourAlt(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColourModel() 
{	ExecuteCommand( DisplayOn, "colour atom model\n");	}

void CRasView::OnUpdateColourModel(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

////////////////////////////////////////////////////////////////////////////////////
// Bonds


/*=======*/
/* Bonds */
/*=======*/

void CRasView::OnDisplayWireframe()
{	if( m_BondsRadius )
		m_BondDisplay.Format("wireframe %d\n", m_BondsRadius);
	ExecuteCommand( DisplayOn, m_BondDisplay);
}

void CRasView::OnUpdateDisplayWireframe(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}
 
void CRasView::OnDisplayWireframeOff()	
{	ExecuteCommand( DisplayOn, "wireframe off\n");	}  

void CRasView::OnUpdateDisplayWireframeOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplayWireframeFull() 
{	SetRibbonStatus(false,0,0);
	DisableSpacefill();	
	DisableBackbone();
	int MarkAtoms_temp = MarkAtoms;
	MarkAtoms = 0;
	OnDisplayWireframe();
	MarkAtoms = MarkAtoms_temp;
}

void CRasView::OnUpdateDisplayWireframeFull(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplayWireframeWire() 
{	m_BondDisplay = "wireframe on\n";	
	m_BondsRadius = 0;
	OnDisplayWireframe();
}

void CRasView::OnUpdateDisplayWireframeWire(CCmdUI* pCmdUI) 
{	if( m_BondDisplay == "wireframe on\n" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnDisplayWireframeDash() 
{	m_BondDisplay = "dash on\n";	
	m_BondsRadius = 0;
	OnDisplayWireframe();
}

void CRasView::OnUpdateDisplayWireframeDash(CCmdUI* pCmdUI) 
{	if( m_BondDisplay == "dash on\n" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnDisplayWireframeCylinder() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_WIRE1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_WIRE2);
    dlg.m_TexComment = s;
	dlg.m_ValueType = Type_Bonds;
	dlg.m_ValueNumber = m_BondsRadius;
	
	STOPTIMER
	if(dlg.DoModal()==IDOK)
	{	m_BondsRadius = dlg.m_ValueNumber;
		OnDisplayWireframe();
	}
	STARTTIMER
}

void CRasView::OnUpdateDisplayWireframeCylinder(CCmdUI* pCmdUI) 
{	int a = ATOI( m_BondDisplay );
	if(  a != 0 )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}


/*=======*/
/* Bonds */
/*=======*/

void CRasView::OnSetBondmodeNone() 
{	ExecuteCommand( DisplayOff, "set bondmode none\n");	}

void CRasView::OnUpdateSetBondmodeNone(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( MarkAtoms == 0 );	}

void CRasView::OnSetBondmodeNobonded() 
{	ExecuteCommand( DisplayOff, "set bondmode not bonded\n");	}

void CRasView::OnUpdateSetBondmodeNobonded(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( MarkAtoms & NonBondFlag );	}

void CRasView::OnSetBondmodeAll() 
{	ExecuteCommand( DisplayOff, "set bondmode all\n");	}

void CRasView::OnUpdateSetBondmodeAll(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck( MarkAtoms & AllAtomFlag );	}

void CRasView::OnColorBonds() 
{	COLORREF ColorTemp = ((CMainFrame*)AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "color bonds none\n");
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color bonds " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnSetBonds() 
{	if( DrawDoubleBonds )
		ExecuteCommand( DisplayOn, "set bond false\n");
	else
		ExecuteCommand( DisplayOn, "set bond true\n");
}

void CRasView::OnUpdateSetBonds(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DrawDoubleBonds);	}


void CRasView::OnResetBond() 
{	ExecuteCommand( DisplayOn, "reset bonds\n");	}

void CRasView::OnUpdateResetBond(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( MoleculeList&&(RotBondList || BondOrderList) );	}



/*=========*/
/* H Bonds */
/*=========*/

void CRasView::OnDisplayHbondsOn()
{	ExecuteCommand( DisplayOn, "hbonds " + m_BondHDisplay + "\n");	} 

void CRasView::OnUpdateDisplayHbondsOn(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplayHbondsOff() 
{	ExecuteCommand( DisplayOn, "hbonds off\n");	}

void CRasView::OnUpdateDisplayHbondsOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplayHbondsDash() 
{	m_BondHDisplay = "on";
	OnDisplayHbondsOn();
}

void CRasView::OnDisplayHbondsCylinder() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_HB1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_HB2);
    dlg.m_TexComment = s;
	dlg.m_ValueType = Type_HBonds;
	dlg.m_ValueNumber = ATOI( m_BondHDisplay);
	
	STOPTIMER
	if(dlg.DoModal() != IDOK)
	{	STARTTIMER
		return;
	}
	if( dlg.m_ValueNumber == 0 )
		m_BondHDisplay = "on";
	else
		m_BondHDisplay = dlg.m_ValueTex;
	OnDisplayHbondsOn();
	STARTTIMER
}

void CRasView::OnUpdateDisplayHbondsDash(CCmdUI* pCmdUI) 
{	if( m_BondHDisplay == "on" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnUpdateDisplayHbondsCylinder(CCmdUI* pCmdUI) 
{	if( m_BondHDisplay != "on" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnDisplayHbondsBackbone()
{	ExecuteCommand( DisplayOn, "set hbonds backbone\n");
	OnDisplayHbondsOn();
}

void CRasView::OnUpdateDisplayHbondsBackbone(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(HBondMode);	}
	
void CRasView::OnDisplayHbondsSidechain() 
{	ExecuteCommand( DisplayOn, "set hbonds sidechain\n");
	OnDisplayHbondsOn();
}

void CRasView::OnUpdateDisplayHbondsSidechain(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(!HBondMode);	}

void CRasView::OnColorHbonds() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "colour hbonds type\n");	
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color hbonds" + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnColorHbondsType() 
{	ExecuteCommand( DisplayOn, "colour hbonds type\n");	}


/*==========*/
/* SS Bonds */
/*==========*/

void CRasView::OnDisplaySsbondsOn() 
{	ExecuteCommand( DisplayOn, "ssbonds " + m_BondSSDisplay + "\n");	} 
 
void CRasView::OnUpdateDisplaySsbondsOn(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplaySsbondsOff() 
{	ExecuteCommand( DisplayOn, "ssbonds off\n");	}

void CRasView::OnUpdateDisplaySsbondsOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnDisplaySsbondsDash() 
{	m_BondSSDisplay = "on";
	OnDisplaySsbondsOn();
}

void CRasView::OnDisplaySsbondsCylinder() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_SSB1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_SSB2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = ATOI( m_BondSSDisplay);
	dlg.m_ValueType = Type_SSBonds;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	if( dlg.m_ValueNumber == 0 )
		m_BondSSDisplay = "on";
	else
		m_BondSSDisplay = dlg.m_ValueTex;
	OnDisplaySsbondsOn();
	STARTTIMER
}

void CRasView::OnUpdateDisplaySsbondsDash(CCmdUI* pCmdUI) 
{	if( m_BondSSDisplay == "on" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnDisplaySsbondsBackbone() 
{	ExecuteCommand( DisplayOn, "set ssbonds backbone\n");
	OnDisplaySsbondsOn();
}

void CRasView::OnUpdateDisplaySsbondsBackbone(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(SSBondMode);	}

void CRasView::OnDisplaySsbondsSidechain() 
{	ExecuteCommand( DisplayOn, "set ssbonds sidechain\n");
	OnDisplaySsbondsOn();
}

void CRasView::OnUpdateDisplaySsbondsSidechain(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(!SSBondMode);	}

void CRasView::OnUpdateDisplaySsbondsCylinder(CCmdUI* pCmdUI) 
{	if( m_BondSSDisplay != "on" )
		pCmdUI->SetCheck(true);
	else	
		pCmdUI->SetCheck(false);
}

void CRasView::OnColorSsbonds() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "colour ssbonds none\n");
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color ssbonds" + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}


///////////////////////////////////////////////////////////////////////////////////
// Ribbons

void CRasView::OnRibbonsOn() 
{	CString type;

	if( RibbonType == TypeBackbone )
	{	SetRibbonStatus(False,0,0);
		type = "backbone ";
	} else if( RibbonType == TypeRibbons ) 
	{	type = "ribbons "; 
		DisableBackbone();
	} else if( RibbonType == TypeStrands )
	{	type = "strands "; 
		DisableBackbone();
	} else if( RibbonType == TypeCartoons )
	{	type = "cartoons "; 
		DisableBackbone();
	} else if( RibbonType == TypeTrace )
	{	type = "trace "; 
		DisableBackbone();
	}
	
	ExecuteCommand( DisplayOff, type + m_RibbonsCommand + "\n");
	ExecuteCommand( DisplayOn, "color " + type + m_RibbonsColor + "\n");
}

void CRasView::OnUpdateRibbonsOn(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(true);	}

void CRasView::OnRibbonsOff() 
{	if( RibbonType == TypeBackbone )
	{	ExecuteCommand( DisplayOff, "ribbons off\n");
		ExecuteCommand( DisplayOn, "backbone off\n");
	} else 
	{	ExecuteCommand( DisplayOff, "backbone off\n");
		ExecuteCommand( DisplayOn, "ribbons off\n");
	}		
}

void CRasView::OnUpdateRibbonsOff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnRibbonsOnly() 
{	if(RibbonType != TypeBackbone)
		DisableBackbone();
	DisableSpacefill();
	DisableWireframe();
	OnRibbonsOn();
}

void CRasView::OnUpdateRibbonsOnly(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnRibbonsRibbons()
{	RibbonType = TypeRibbons;
	int i = ATOI(m_RibbonsCommand);
	if( i==0 || i == 80 ) 
		m_RibbonsCommand = "";
	else  m_RibbonsCommand.Format("%d", i );
	OnRibbonsOn();
}

void CRasView::OnRibbonsStrands() 
{	RibbonType = TypeStrands;
	int i = ATOI(m_RibbonsCommand);
	if( m_RibbonsCommand.Find("dash") < 0 &&
		m_RibbonsCommand.Find("dots") < 0 )
	{	if( i==0 || i==80 )
			m_RibbonsCommand = "";
		else 
			m_RibbonsCommand.Format("%d", i);
	} else
	{	if( i==0 || i==80 )
			m_RibbonsCommand = "dash";
		else 
			m_RibbonsCommand.Format("dash %d", i);
	}
	OnRibbonsOn();
}

void CRasView::OnRibbonsCartoons() 
{	RibbonType = TypeCartoons;
	int i = ATOI( m_RibbonsCommand);
	if( i==0 || i == 80 ) 
		m_RibbonsCommand = "";
	else  m_RibbonsCommand.Format("%d", i );
	OnRibbonsOn();
}

void CRasView::OnRibbonsTrace() 
{	RibbonType = TypeTrace;
	int i = ATOI( m_RibbonsCommand);
	if( i > 250 ) i = 250;

	if( m_RibbonsCommand.Find("dash") >= 0 ||
		m_RibbonsCommand.Find("dots") >= 0 )
	{	if( i==0 || i==80 )
			m_RibbonsCommand = "dots";
		else 
			m_RibbonsCommand.Format("dots %d", i);
	} else 
		if( m_RibbonsCommand.Find("temperature") >= 0 )
		{	m_RibbonsCommand = "temperature";
		} else 
			if( i==0 || i==80 )
				m_RibbonsCommand = "";
			else 
				m_RibbonsCommand.Format("%d", i);
			
	OnRibbonsOn();
}

void CRasView::OnRibbonsBackbone() 
{	RibbonType = TypeBackbone;
	int i = ATOI( m_RibbonsCommand);
	if( i > 250 ) i = 250;	//ARCSIZE in pixutils

	if( m_RibbonsCommand.Find("dash") >= 0 ||
		m_RibbonsCommand.Find("dots") >= 0 )
	{	m_RibbonsCommand = "dash";
	} else 
		if( i==0 || i==80 )
			m_RibbonsCommand = "80";
		else 
			m_RibbonsCommand.Format("%d", i);

	OnRibbonsOn();
}

void CRasView::OnUpdateRibbonsRibbons(CCmdUI* pCmdUI) 
{	int b = (RibbonType == TypeRibbons);
	pCmdUI->SetCheck(b);
}

void CRasView::OnUpdateRibbonsStrands(CCmdUI* pCmdUI) 
{	int b = (RibbonType == TypeStrands);
	pCmdUI->SetCheck(b);
}

void CRasView::OnUpdateRibbonsCartoons(CCmdUI* pCmdUI) 
{	int b = (RibbonType == TypeCartoons);
	pCmdUI->SetCheck(b);
}

void CRasView::OnUpdateRibbonsTrace(CCmdUI* pCmdUI) 
{	int b = (RibbonType == TypeTrace);
	pCmdUI->SetCheck(b);
}

void CRasView::OnUpdateRibbonsBackbone(CCmdUI* pCmdUI) 
{	int b = (RibbonType == TypeBackbone);
	pCmdUI->SetCheck(b);
}

void CRasView::OnRibbonsSize() 
{	CDialogSetValue dlg;
    CString s;
	BOOL Dash_Flag = m_RibbonsCommand.Find( "dash" ) >= 0 || 
					 m_RibbonsCommand.Find( "dots" ) >= 0 ;

    if( RibbonType == TypeRibbons ) {
	    s.LoadString(IDS_LOCAL_RIBB1);
		dlg.m_TexValue = s;
    } else if( RibbonType == TypeStrands ) {
	    s.LoadString(IDS_LOCAL_RIBB2);
		dlg.m_TexValue = s;
    } else if( RibbonType == TypeCartoons ) {
	    s.LoadString(IDS_LOCAL_RIBB3);
		dlg.m_TexValue = s;
	} else
		return;

    s.LoadString(IDS_LOCAL_RIBB4);
    dlg.m_TexComment = s;
	dlg.m_ValueType = Type_Ribbons;
	dlg.m_ValueNumber = ATOI( m_RibbonsCommand);
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	if( dlg.m_ValueNumber == 0 )
	{	OnDisplayRibbonsStructure();
	} else
	{	if( !Dash_Flag )
			m_RibbonsCommand = dlg.m_ValueTex;
		else
			m_RibbonsCommand = "dash " + dlg.m_ValueTex;
		OnRibbonsOn();
	}
	STARTTIMER
}

void CRasView::OnUpdateRibbonsSize(CCmdUI* pCmdUI) 
{	if( (RibbonType == TypeRibbons) || 
	    (RibbonType == TypeStrands) ||
	    (RibbonType == TypeCartoons) )
	{	pCmdUI->Enable(true);
		int i = ATOI( m_RibbonsCommand);
		if(	i>0 )
			pCmdUI->SetCheck(true);
		else 
			pCmdUI->SetCheck(false);
	}
	else	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnDisplayBackboneCylinder() 
{	/*Radius*/
	CDialogSetValue dlg;
    CString s;
	BOOL Dash_Flag = m_RibbonsCommand.Find( "dash" ) >= 0 || 
					 m_RibbonsCommand.Find( "dots" ) >= 0 ;
    s.LoadString(IDS_LOCAL_CYL1);
	dlg.m_TexValue = s;
	if(RibbonType == TypeBackbone)
	{	s.LoadString(IDS_LOCAL_CYL2);
        dlg.m_TexComment = s;
		dlg.m_ValueType = Type_Bonds;
	} else if( RibbonType == TypeTrace )
	{	s.LoadString(IDS_LOCAL_CYL3);
        dlg.m_TexComment = s;
		dlg.m_ValueType = Type_Trace;
	} else return;
	dlg.m_ValueNumber = ATOI( m_RibbonsCommand );

	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}

	if( RibbonType == TypeTrace )
	{	if( Dash_Flag )
			m_RibbonsCommand = "dots " + dlg.m_ValueTex;
		else
			m_RibbonsCommand = dlg.m_ValueTex;	
	} else if( RibbonType == TypeBackbone )
	{	if( Dash_Flag )
			m_RibbonsCommand = "dash " + dlg.m_ValueTex;
		else
			m_RibbonsCommand = dlg.m_ValueTex;	
	}

	OnRibbonsOn();
	STARTTIMER
}

void CRasView::OnUpdateDisplayBackboneCylinder(CCmdUI* pCmdUI) 
{	if( (RibbonType == TypeBackbone) || (RibbonType == TypeTrace) )
	{	pCmdUI->Enable(true);
		int b = ATOI( m_RibbonsCommand);
		pCmdUI->SetCheck( b>0 );
	}
	else	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
		return;
	}
}

void CRasView::OnDisplayBackboneWire() 
{	RibbonType = TypeBackbone;
	m_RibbonsCommand = "0";
	OnRibbonsOn();
}

void CRasView::OnUpdateDisplayBackboneWire(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeBackbone )
	{	pCmdUI->Enable(true);
		if(	m_RibbonsCommand == "0")
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnDisplayBackboneDash() 
{	if( RibbonType == TypeStrands )
	{	int i = ATOI( m_RibbonsCommand);
		if( i>0 )
			m_RibbonsCommand.Format("dash %d", i);
		else 
			m_RibbonsCommand = "dash";
	} else
		m_RibbonsCommand = "dash";
	OnRibbonsOn();
}

void CRasView::OnUpdateDisplayBackboneDash(CCmdUI* pCmdUI) 
{	if( (RibbonType == TypeBackbone) || (RibbonType == TypeStrands) )
	{	pCmdUI->Enable(true);
		int s = m_RibbonsCommand.Find("dash");
		if( s>=0 )
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnSetStrandsNumber() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_STRAND1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_STRAND2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = SplineCount;
	dlg.m_ValueType = Type_Strands;
	
	STOPTIMER
	if( dlg.DoModal()==IDOK ) 
		ExecuteCommand( DisplayOn, ("set strands " + dlg.m_ValueTex + '\n'));
	STARTTIMER
}

void CRasView::OnUpdateSetStrandsNumber(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeStrands )
		pCmdUI->Enable(true);
	else	
		pCmdUI->Enable(false);
}

void CRasView::OnDisplayTraceDots() 
{	int i = ATOI( m_RibbonsCommand);
	if( i>0 )
		m_RibbonsCommand.Format("dots %d", i);
	else 
		m_RibbonsCommand = "dots";

	OnRibbonsOn();
}

void CRasView::OnUpdateDisplayTraceDots(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeTrace )
	{	pCmdUI->Enable(true);
		if(	m_RibbonsCommand == "dots")
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else  	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnDisplayTraceTemperature() 
{	m_RibbonsCommand = "temperature";
	OnRibbonsOn();
}

void CRasView::OnUpdateDisplayTraceTemperature(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeTrace )
	{	pCmdUI->Enable(true);
		if(	m_RibbonsCommand == "temperature" )
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else  	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnSetCartoonsHeight() 
{	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_CAR1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_CAR2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = CartoonHeight;
	dlg.m_ValueType = Type_Cartoons;
	
	STOPTIMER
	if(dlg.DoModal()==IDOK)
		ExecuteCommand( DisplayOn, ("set cartoons " + dlg.m_ValueTex + '\n'));
	STARTTIMER
}

void CRasView::OnUpdateSetCartoonsHeight(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeCartoons )
		pCmdUI->Enable(true);
	else	
		pCmdUI->Enable(false);
}

void CRasView::OnDisplayCartoonsArrow() 
{	if( !DrawBetaArrows )
		ExecuteCommand( DisplayOn, "set cartoons on\n");
	else
		ExecuteCommand( DisplayOn, "set cartoons off\n");
}

void CRasView::OnUpdateDisplayCartoonsArrow(CCmdUI* pCmdUI) 
{	if( RibbonType == TypeCartoons )
	{	pCmdUI->Enable(true);
		if( DrawBetaArrows == (int)true )
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else	
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}

void CRasView::OnDisplayRibbonsStructure() 
{	m_RibbonsCommand = "";
	OnRibbonsOn();
}

void CRasView::OnUpdateDisplayRibbonsStructure(CCmdUI* pCmdUI) 
{	if( (RibbonType != TypeBackbone) && 
	    (RibbonType != TypeTrace) )
	{	pCmdUI->Enable(true);
		if(	m_RibbonsCommand == "")
			pCmdUI->SetCheck(true);
		else
			pCmdUI->SetCheck(false);
	}
	else
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	}
}
 
void CRasView::OnRibbonsColor() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
	{	m_RibbonsColor = "none";
		if( (RibbonType == TypeRibbons) ||
			(RibbonType == TypeStrands) ||
			(RibbonType == TypeCartoons)||
			(RibbonType == TypeTrace)	 )
		{	ExecuteCommand( DisplayOff, "color backbone none\n");
			ExecuteCommand( DisplayOn, "color ribbons none\n");
		} else if( RibbonType == TypeBackbone )
		{	ExecuteCommand( DisplayOff, "color ribbons none\n");
			ExecuteCommand( DisplayOn, "color backbone none\n");
		}
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	m_RibbonsColor = Command;
	OnRibbonsOn();
}

void CRasView::OnRibbonsColorInside() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
		return;

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color ribbon1" + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

void CRasView::OnRibbonsColorOutside() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp == 0xFFFFFFFF )
		return;

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "color ribbon2" + Command + "\n";
	ExecuteCommand( DisplayOn, Command);
}

////////////////////////////////////////////////////////////////////////////////////
// Surfaces

/*======*/
/* Dots */
/*======*/

void CRasView::OnDisplayDotson()
{	if( !Database )
		return;
	Command.Format("dots %d\n",DotDensity);
	ExecuteCommand(DisplayOn,Command);
}

void CRasView::OnUpdateDotson(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( true );	}

void CRasView::OnDisplayDotsoff()
{	ExecuteCommand( DisplayOn, "dots off\n");
}

void CRasView::OnUpdateDotsoff(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(Database != NULL);	}

void CRasView::OnSetDotsSize() 
{	if( !Database )
		return;
	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_DOT1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_DOT2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = DotSize;
	dlg.m_ValueType = Type_DotSize;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	DotSize = dlg.m_ValueNumber;
    Command.Format("set dots %d\n",DotSize);
    ExecuteCommand(DisplayOn,Command);
	STARTTIMER
}

void CRasView::OnUpdateSetDotsSize(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}

void CRasView::OnSetDotsDensity() 
{	if( !Database )
		return;
	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_DD1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_DD2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = DotDensity;
	dlg.m_ValueType = Type_Dots;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	DotDensity = dlg.m_ValueNumber;
	OnDisplayDotson();
	STARTTIMER
}

void CRasView::OnUpdateSetDotsDensity(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

void CRasView::OnColorDots() 
{	if( !Database )
		return;

	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp==0xFFFFFFFF )
	{	Command.Format("color dots none\n");
        ExecuteCommand(DisplayOn,Command);
	} else
	{	Command.Format("color dots [%d,%d,%d]\n",
          GetRValue(ColorTemp),GetGValue(ColorTemp),GetBValue(ColorTemp));
        ExecuteCommand(DisplayOn,Command);
	}
}

void CRasView::OnColorDotsPotential() 
{	if( !Database )
		return;
	ExecuteCommand( DisplayOn,"color dots potential\n");
}

void CRasView::OnDotsVdwaals() 
{	if( !Database )
		return;
	ExecuteCommand( DisplayOff, "set solvent off\n");
	OnDisplayDotson();
}

void CRasView::OnDotsConolly() 
{	if( !Database )
		return;
	ExecuteCommand( DisplayOff, "set solvent on\n");
	OnDisplayDotson();
}

void CRasView::OnUpdateDotsVdwaals(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(!SolventDots);	}

void CRasView::OnUpdateDotsConolly(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(SolventDots);	}

void CRasView::OnDotsRadius() 
{	if( !Database )
		return;
	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_DRAD1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_DRAD2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = ProbeRadius;
	dlg.m_ValueType = Type_Radius;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	ExecuteCommand( DisplayOff, ("set radius " + dlg.m_ValueTex + '\n'));
	OnDisplayDotson();
	STARTTIMER
}


////////////////////////////////////////////////////////////////////////////////////
// View

/*==============*/
/* Light Source */
/*==============*/

void CRasView::OnViewLightsource() 
{	if( !Database )
		return;
	CDialogChangeLightSource dlg;
	STOPTIMER
	dlg.DoModal();	
	STARTTIMER
}

void CRasView::OnUpdateViewLightsource(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database!=NULL );	}


/*============*/
/* Background */
/*============*/

void CRasView::OnColorBackground() 
{	COLORREF ColorTemp = ((CMainFrame*) AfxGetApp( )->m_pMainWnd)->m_crColour;
	if( ColorTemp==0xFFFFFFFF )
	{	ExecuteCommand( DisplayOn, "background black\n");
		return;
	}

	int r = GetRValue(ColorTemp);
	int g = GetGValue(ColorTemp);
	int b = GetBValue(ColorTemp);

	Command.Format("[%d,%d,%d]",r,g,b);
	Command = "background " + Command + "\n";
	ExecuteCommand( DisplayOn, Command);

	GETDOC
	pDoc->SetModifiedFlag(true);
}

void CRasView::OnColorFade() 
{	if( !UseBackFade )
	   ExecuteCommand( DisplayOn, ("set backfade on\n"));
	else
	   ExecuteCommand( DisplayOn, ("set backfade off\n"));	
}

void CRasView::OnUpdateColorFade(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck(UseBackFade);
	}
}


/*========*/
/* Stereo */
/*========*/

void CRasView::OnViewStereo() 
{	if( UseStereo )
		SetStereoMode(False);
	else
		SetStereoMode(True);
	ReDrawFlag |= RFRefresh;
	RefreshView();
}

void CRasView::OnUpdateViewStereo(CCmdUI* pCmdUI) 
{	if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck(UseStereo);
	}
}

void CRasView::OnSetStereoAngle() 
{	if( !Database )
		return;
	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_STEREO1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_STEREO2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = (int)StereoAngle;
	dlg.m_ValueType = Type_Stereo;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	ExecuteCommand( DisplayOn, ("set stereo " + dlg.m_ValueTex + '\n'));
	STARTTIMER
}

void CRasView::OnUpdateSetStereoAngle(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}


/*========*/
/* Shadow */
/*========*/

void CRasView::OnViewShadows() 
{	if( UseShadow )
		ExecuteCommand( DisplayOn, ("set shadows off\n"));	
	else
		ExecuteCommand( DisplayOn, ("set shadows on\n"));
}

void CRasView::OnUpdateViewShadows(CCmdUI* pCmdUI) 
{	pCmdUI->Enable(false);
    /*if( Database == NULL )
	{	pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
	} else
	{	pCmdUI->Enable(true);
		pCmdUI->SetCheck(UseShadow);
	}*/
}


/*=====================*/
/* Clipping & DepthCue */
/*=====================*/

void CRasView::OnSlabWorldReset() 
{	ExecuteCommand( DisplayOn, "reset slab\n");	}

void CRasView::OnUpdateSlabWorldReset(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (MoleculeList&&MoleculeList->next)||(RotMode==RotAll) );	}

void CRasView::OnSetSlabHalf() 
{	ExecuteCommand( DisplayOn, "set slabmode half\n");	}

void CRasView::OnSetSlabHollow() 
{	ExecuteCommand( DisplayOn, "set slabmode hollow\n");	}

void CRasView::OnSetSlabReject() 
{	ExecuteCommand( DisplayOn, "set slabmode reject\n");	}

void CRasView::OnSetSlabSection() 
{	ExecuteCommand( DisplayOn, "set slabmode section\n");	}

void CRasView::OnSetSlabSolid() 
{	ExecuteCommand( DisplayOn, "set slabmode solid\n");	}

void CRasView::OnUpdateSetSlabHalf(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck((SlabMode == SlabHalf) && UseSlabPlane);	}

void CRasView::OnUpdateSetSlabHollow(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck((SlabMode == SlabHollow) && UseSlabPlane);	}

void CRasView::OnUpdateSetSlabReject(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck((SlabMode == SlabReject) && UseSlabPlane);	}

void CRasView::OnUpdateSetSlabSection(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck((SlabMode == SlabSection) && UseSlabPlane);	}

void CRasView::OnUpdateSetSlabSolid(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck((SlabMode == SlabClose) && UseSlabPlane);	}

void CRasView::OnDepthWorldReset() 
{	ExecuteCommand( DisplayOn, "reset depth\n");	}

void CRasView::OnUpdateDepthWorldReset(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( (MoleculeList&&MoleculeList->next)||(RotMode==RotAll) );	}

void CRasView::OnDepthcuemode1() 
{	ExecuteCommand( DisplayOn, "set depthcue 1\n");	}

void CRasView::OnUpdateDepthcuemode1(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DepthCueMode == 1);	}

void CRasView::OnDepthcuemode2() 
{	ExecuteCommand( DisplayOn, "set depthcue 2\n");	}

void CRasView::OnUpdateDepthcuemode2(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DepthCueMode == 2);	}

void CRasView::OnDepthcuemode3() 
{	ExecuteCommand( DisplayOn, "set depthcue 3\n");	}

void CRasView::OnUpdateDepthcuemode3(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DepthCueMode == 3);	}

void CRasView::OnDepthcuemode4() 
{	ExecuteCommand( DisplayOn, "set depthcue 4\n");	}

void CRasView::OnUpdateDepthcuemode4(CCmdUI* pCmdUI) 
{	pCmdUI->SetCheck(DepthCueMode == 4);	}

void CRasView::OnDepthWorld() 
{	if( !Database )
		return;
	CDialogSetValue dlg;
    CString s;
    s.LoadString(IDS_LOCAL_DWORLD1);
	dlg.m_TexValue = s;
    s.LoadString(IDS_LOCAL_DWORLD2);
    dlg.m_TexComment = s;
	dlg.m_ValueNumber = (int)WorldSize;
	dlg.m_ValueType = Type_MolSize;
	
	STOPTIMER
	if( dlg.DoModal()!=IDOK ) 
	{	STARTTIMER
		return;
	}
	ExecuteCommand( DisplayOn, ("set worlddepth " + dlg.m_ValueTex + '\n'));
	STARTTIMER
}

void CRasView::OnUpdateDepthWorld(CCmdUI* pCmdUI) 
{	pCmdUI->Enable( Database != NULL );	}

