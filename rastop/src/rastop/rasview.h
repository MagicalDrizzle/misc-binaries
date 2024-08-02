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
// rasview.h
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RASVIEW_H_)
#define AFX_RASVIEW_H_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define DisplayOff		0
#define DisplayOn		1

#define	TypeRibbons		1 
#define	TypeStrands		2 
#define	TypeCartoons	3 
#define	TypeTrace		4 
#define	TypeBackbone	5 

/////////////////////////////////////////////////////////////////////////////
// CRasFrame frame

class CRasFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CRasFrame)
protected:
	CRasFrame();

// Attributes
protected:

// Operations
public:

// Overrides
	//{{AFX_VIRTUAL(CRasFrame)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRasFrame();
	//{{AFX_MSG(CRasFrame)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//CRasView

class CRasView : public CView
{
protected: 
	CRasView();
	DECLARE_DYNCREATE(CRasView)

// Attributes
public:

// Operations
public:
// Overrides
	//{{AFX_VIRTUAL(CRasView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	int m_drawingtime;

	virtual ~CRasView();
	void RefreshView();
	CRasDoc* GetDocument();
	void PrintImage( );
	void OnButtomDown(UINT nFlags, CPoint point);
	int ExecuteCommand(int display, CString str);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	static	CString m_PreviousCommand;

	static	CString m_SpacefillDisplay;
	static	int m_SpacefillRepres;

	static	CString m_BondDisplay;
	static	CString m_BondHDisplay;
	static	int m_BondsRadius;
	static	CString m_BondSSDisplay;

	static	int RibbonType;
	static	CString m_RibbonsCommand;
	static	CString m_RibbonsColor;
	
	void OnButtomUp(UINT nFlags, CPoint point);

	//{{AFX_MSG(CRasView)
	afx_msg void OnCopyImage();
	afx_msg void OnUpdateCopyImage(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnColorBackground();
	afx_msg void OnColourChain();
	afx_msg void OnColourCpk();
	afx_msg void OnColourGroup();
	afx_msg void OnColourShapely();
	afx_msg void OnColourStructure();
	afx_msg void OnColourTemperature();
	afx_msg void OnColourUser();
	afx_msg void OnColourCharge();
	afx_msg void OnColorAtom();
	afx_msg void OnColorBonds();
	afx_msg void OnColorDots();
	afx_msg void OnColorHbonds();
	afx_msg void OnColorLabels();
	afx_msg void OnColorSsbonds();
	afx_msg void OnColorDotsPotential();
	afx_msg void OnDisplayDotson();
	afx_msg void OnDisplayDotsoff();
	afx_msg void OnSetDotsDensity();
	afx_msg void OnDotsConolly();
	afx_msg void OnUpdateDotsConolly(CCmdUI* pCmdUI);
	afx_msg void OnDotsRadius();
	afx_msg void OnDotsVdwaals();
	afx_msg void OnUpdateDotsVdwaals(CCmdUI* pCmdUI);
	afx_msg void OnDisplayWireframe();
	afx_msg void OnDisplayWireframeFull();
	afx_msg void OnDisplayWireframeOff();
	afx_msg void OnDisplayWireframeWire();
	afx_msg void OnUpdateDisplayWireframeWire(CCmdUI* pCmdUI);
	afx_msg void OnDisplayWireframeDash();
	afx_msg void OnUpdateDisplayWireframeDash(CCmdUI* pCmdUI);
	afx_msg void OnDisplayWireframeCylinder();
	afx_msg void OnUpdateDisplayWireframeCylinder(CCmdUI* pCmdUI);
	afx_msg void OnDisplayBackboneCylinder();
	afx_msg void OnDisplayBackboneDash();
	afx_msg void OnDisplayBackboneWire();
	afx_msg void OnUpdateDisplayBackboneCylinder(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayBackboneDash(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayBackboneWire(CCmdUI* pCmdUI);
	afx_msg void OnDisplayBallandstick();
	afx_msg void OnDisplaySpacefill();
	afx_msg void OnDisplaySticks();
	afx_msg void OnSpacefillFixedradius();
	afx_msg void OnSpacefillTemperature();
	afx_msg void OnSpacefillUser();
	afx_msg void OnSpacefillVanderwaals();
	afx_msg void OnDisplaySpacefillFull();
	afx_msg void OnDisplaySpacefillOff();
	afx_msg void OnUpdateSpacefillFixedradius(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpacefillTemperature(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpacefillUser(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSpacefillVanderwaals(CCmdUI* pCmdUI);
	afx_msg void OnColorFade();
	afx_msg void OnUpdateColorFade(CCmdUI* pCmdUI);
	afx_msg void OnViewStereo();
	afx_msg void OnUpdateViewStereo(CCmdUI* pCmdUI);
	afx_msg void OnDisplayHbondsCylinder();
	afx_msg void OnUpdateDisplayHbondsCylinder(CCmdUI* pCmdUI);
	afx_msg void OnDisplayHbondsDash();
	afx_msg void OnUpdateDisplayHbondsDash(CCmdUI* pCmdUI);
	afx_msg void OnDisplayHbondsOff();
	afx_msg void OnDisplayHbondsOn();
	afx_msg void OnDisplaySsbondsCylinder();
	afx_msg void OnUpdateDisplaySsbondsCylinder(CCmdUI* pCmdUI);
	afx_msg void OnDisplaySsbondsDash();
	afx_msg void OnUpdateDisplaySsbondsDash(CCmdUI* pCmdUI);
	afx_msg void OnDisplaySsbondsOff();
	afx_msg void OnDisplaySsbondsOn();
	afx_msg void OnDisplayWireframeOnly();
	afx_msg void OnDisplayHbondsBackbone();
	afx_msg void OnUpdateDisplayHbondsBackbone(CCmdUI* pCmdUI);
	afx_msg void OnDisplayHbondsSidechain();
	afx_msg void OnUpdateDisplayHbondsSidechain(CCmdUI* pCmdUI);
	afx_msg void OnDisplaySsbondsBackbone();
	afx_msg void OnUpdateDisplaySsbondsBackbone(CCmdUI* pCmdUI);
	afx_msg void OnDisplaySsbondsSidechain();
	afx_msg void OnUpdateDisplaySsbondsSidechain(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsBackbone();
	afx_msg void OnUpdateRibbonsBackbone(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsCartoons();
	afx_msg void OnUpdateRibbonsCartoons(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsColor();
	afx_msg void OnRibbonsColorInside();
	afx_msg void OnRibbonsOff();
	afx_msg void OnRibbonsOn();
	afx_msg void OnRibbonsOnly();
	afx_msg void OnRibbonsRibbons();
	afx_msg void OnUpdateRibbonsRibbons(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsSize();
	afx_msg void OnRibbonsStrands();
	afx_msg void OnUpdateRibbonsStrands(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsTrace();
	afx_msg void OnUpdateRibbonsTrace(CCmdUI* pCmdUI);
	afx_msg void OnRibbonsColorOutside();
	afx_msg void OnUpdateRibbonsSize(CCmdUI* pCmdUI);
	afx_msg void OnSetStrandsNumber();
	afx_msg void OnUpdateSetStrandsNumber(CCmdUI* pCmdUI);
	afx_msg void OnDisplayTraceDots();
	afx_msg void OnUpdateDisplayTraceDots(CCmdUI* pCmdUI);
	afx_msg void OnDisplayTraceTemperature();
	afx_msg void OnUpdateDisplayTraceTemperature(CCmdUI* pCmdUI);
	afx_msg void OnSetCartoonsHeight();
	afx_msg void OnUpdateSetCartoonsHeight(CCmdUI* pCmdUI);
	afx_msg void OnDisplayRibbonsStructure();
	afx_msg void OnUpdateDisplayRibbonsStructure(CCmdUI* pCmdUI);
	afx_msg void OnDisplayCartoonsArrow();
	afx_msg void OnUpdateDisplayCartoonsArrow(CCmdUI* pCmdUI);
	afx_msg void OnSetSlabHalf();
	afx_msg void OnUpdateSetSlabHalf(CCmdUI* pCmdUI);
	afx_msg void OnSetSlabHollow();
	afx_msg void OnUpdateSetSlabHollow(CCmdUI* pCmdUI);
	afx_msg void OnSetSlabReject();
	afx_msg void OnUpdateSetSlabReject(CCmdUI* pCmdUI);
	afx_msg void OnSetSlabSection();
	afx_msg void OnUpdateSetSlabSection(CCmdUI* pCmdUI);
	afx_msg void OnSetSlabSolid();
	afx_msg void OnUpdateSetSlabSolid(CCmdUI* pCmdUI);
	afx_msg void OnViewShadows();
	afx_msg void OnUpdateViewShadows(CCmdUI* pCmdUI);
	afx_msg void OnDisplayAxes();
	afx_msg void OnUpdateDisplayAxes(CCmdUI* pCmdUI);
	afx_msg void OnMoleculeSeq();
	afx_msg void OnUpdateMoleculeSeq(CCmdUI* pCmdUI);
	afx_msg void OnMoleculeStruc();
	afx_msg void OnMoleculeSym();
	afx_msg void OnUpdateMoleculeSym(CCmdUI* pCmdUI);
	afx_msg void OnMoleculeInfo();
	afx_msg void OnUpdateMoleculeInfo(CCmdUI* pCmdUI);
	afx_msg void OnStaticrotx180();
	afx_msg void OnStaticrotx270();
	afx_msg void OnStaticrotx90();
	afx_msg void OnStaticroty180();
	afx_msg void OnStaticroty270();
	afx_msg void OnStaticroty90();
	afx_msg void OnStaticrotz180();
	afx_msg void OnStaticrotz270();
	afx_msg void OnStaticrotz90();
	afx_msg void OnLabelsOn();
	afx_msg void OnLabelsOff();
	afx_msg void OnLabelsDefault();
	afx_msg void OnDisplayBoundbox();
	afx_msg void OnDisplayUnitcell();
	afx_msg void OnMoleculeConnect();
	afx_msg void OnMoleculeRenumber();
	afx_msg void OnUpdateDisplayBoundbox(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayUnitcell(CCmdUI* pCmdUI);
	afx_msg void OnColorHbondsType();
	afx_msg void OnEditMolecularFile();
	afx_msg void OnSetConnect();
	afx_msg void OnUpdateSetConnect(CCmdUI* pCmdUI);
	afx_msg void OnDistanceRemove();
	afx_msg void OnDistanceShow();
	afx_msg void OnMonitorColour();
	afx_msg void OnUpdateDistanceShow(CCmdUI* pCmdUI);
	afx_msg void OnLabelsSettings();
	afx_msg void OnUpdateMoleculeConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoleculeRenumber(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMoleculeStruc(CCmdUI* pCmdUI);
	afx_msg void OnColorAxe();
	afx_msg void OnUpdateColorAxe(CCmdUI* pCmdUI);
	afx_msg void OnColorBox();
	afx_msg void OnUpdateColorBox(CCmdUI* pCmdUI);
	afx_msg void OnSetBonds();
	afx_msg void OnUpdateSetBonds(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourChain(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourCharge(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourCpk(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourGroup(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourShapely(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourStructure(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourTemperature(CCmdUI* pCmdUI);
	afx_msg void OnUpdateColourUser(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayBallandstick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDotsoff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDotson(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayHbondsOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayHbondsOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySpacefill(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySpacefillFull(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySpacefillOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySsbondsOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySsbondsOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplaySticks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayWireframe(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayWireframeFull(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayWireframeOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisplayWireframeOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDistanceRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelsDefault(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelsOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLabelsOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonsOff(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonsOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRibbonsOnly(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetDotsDensity(CCmdUI* pCmdUI);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnWindowTileVert();
	afx_msg void OnWindowTileHorz();
	afx_msg void OnWindowCascade();
	afx_msg void OnFilePrint();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnWindowTile();
	afx_msg void OnSpacefillSphere();
	afx_msg void OnUpdateSpacefillSphere(CCmdUI* pCmdUI);
	afx_msg void OnSpacefillStar();
	afx_msg void OnUpdateSpacefillStar(CCmdUI* pCmdUI);
	afx_msg void OnColourAlt();
	afx_msg void OnUpdateColourAlt(CCmdUI* pCmdUI);
	afx_msg void OnColourModel();
	afx_msg void OnUpdateColourModel(CCmdUI* pCmdUI);
	afx_msg void OnSetCisangle();
	afx_msg void OnUpdateSetCisangle(CCmdUI* pCmdUI);
	afx_msg void OnSetBondmodeNobonded();
	afx_msg void OnUpdateSetBondmodeNobonded(CCmdUI* pCmdUI);
	afx_msg void OnSetBondmodeNone();
	afx_msg void OnUpdateSetBondmodeNone(CCmdUI* pCmdUI);
	afx_msg void OnSetBondmodeAll();
	afx_msg void OnUpdateSetBondmodeAll(CCmdUI* pCmdUI);
	afx_msg void OnMoleculePhipsi();
	afx_msg void OnUpdateMoleculePhipsi(CCmdUI* pCmdUI);
	afx_msg void OnMoleculeRpp();
	afx_msg void OnUpdateMoleculeRpp(CCmdUI* pCmdUI);
	afx_msg void OnCopyPosition();
	afx_msg void OnUpdateCopyPosition(CCmdUI* pCmdUI);
	afx_msg void OnSelectionCopy();
	afx_msg void OnUpdateSelectionCopy(CCmdUI* pCmdUI);
	afx_msg void OnPaste();
	afx_msg void OnUpdatePaste(CCmdUI* pCmdUI);
	afx_msg void OnEditRasmolCommand();
	afx_msg void OnSelectDefine();
	afx_msg void OnUpdateSelectDefine(CCmdUI* pCmdUI);
	afx_msg void OnSelectAll();
	afx_msg void OnUpdateSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnSelectInverse();
	afx_msg void OnUpdateSelectInverse(CCmdUI* pCmdUI);
	afx_msg void OnSelectPrevious();
	afx_msg void OnUpdateSelectPrevious(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectAtm();
	afx_msg void OnUpdateShowSelectAtm(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectGrp();
	afx_msg void OnUpdateShowSelectGrp(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectChn();
	afx_msg void OnUpdateShowSelectChn(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectCoor();
	afx_msg void OnUpdateShowSelectCoor(CCmdUI* pCmdUI);
	afx_msg void OnSelectExpression();
	afx_msg void OnUpdateSelectExpression(CCmdUI* pCmdUI);
	afx_msg void OnSelectWithin();
	afx_msg void OnUpdateSelectWithin(CCmdUI* pCmdUI);
	afx_msg void OnSelectHydrogens();
	afx_msg void OnUpdateSelectHydrogens(CCmdUI* pCmdUI);
	afx_msg void OnSelectBondmode();
	afx_msg void OnUpdateSelectBondmode(CCmdUI* pCmdUI);
	afx_msg void OnSelectHeteAtoms();
	afx_msg void OnUpdateSelectHeteAtoms(CCmdUI* pCmdUI);
	afx_msg void OnSelectModel();
	afx_msg void OnUpdateSelectModel(CCmdUI* pCmdUI);
	afx_msg void OnRestrict();
	afx_msg void OnUpdateRestrict(CCmdUI* pCmdUI);
	afx_msg void OnSelectShow();
	afx_msg void OnUpdateSelectShow(CCmdUI* pCmdUI);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSetDotsSize();
	afx_msg void OnUpdateSetDotsSize(CCmdUI* pCmdUI);
	afx_msg void OnViewLightsource();
	afx_msg void OnUpdateViewLightsource(CCmdUI* pCmdUI);
	afx_msg void OnDisplaySphere();
	afx_msg void OnUpdateDisplaySphere(CCmdUI* pCmdUI);
	afx_msg void OnDisplayStar();
	afx_msg void OnUpdateDisplayStar(CCmdUI* pCmdUI);
	afx_msg void OnDisplayNothing();
	afx_msg void OnUpdateDisplayNothing(CCmdUI* pCmdUI);
	afx_msg void OnSelectCentre();
	afx_msg void OnUpdateSelectCentre(CCmdUI* pCmdUI);
	afx_msg void OnDisplayWorldaxes();
	afx_msg void OnUpdateDisplayWorldaxes(CCmdUI* pCmdUI);
	afx_msg void OnSetStereoAngle();
	afx_msg void OnUpdateSetStereoAngle(CCmdUI* pCmdUI);
	afx_msg void OnMoleculeShow();
	afx_msg void OnUpdateMoleculeShow(CCmdUI* pCmdUI);
	afx_msg void OnCentreWorld();
	afx_msg void OnUpdateCentreWorld(CCmdUI* pCmdUI);
	afx_msg void OnCentreOrigin();
	afx_msg void OnUpdateCentreOrigin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOrigintocentre(CCmdUI* pCmdUI);
	afx_msg void OnOrigintocentre();
	afx_msg void OnSlabWorldReset();
	afx_msg void OnUpdateSlabWorldReset(CCmdUI* pCmdUI);
	afx_msg void OnDepthWorldReset();
	afx_msg void OnUpdateDepthWorldReset(CCmdUI* pCmdUI);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMoleculeDelete();
	afx_msg void OnUpdateMoleculeDelete(CCmdUI* pCmdUI);
	afx_msg void OnResetBond();
	afx_msg void OnUpdateResetBond(CCmdUI* pCmdUI);
	afx_msg void OnWorldInfo();
	afx_msg void OnUpdateWorldInfo(CCmdUI* pCmdUI);
	afx_msg void OnDepthcuemode1();
	afx_msg void OnDepthcuemode2();
	afx_msg void OnDepthcuemode3();
	afx_msg void OnDepthcuemode4();
	afx_msg void OnUpdateDepthcuemode1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthcuemode2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthcuemode3(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDepthcuemode4(CCmdUI* pCmdUI);
	afx_msg void OnDepthWorld();
	afx_msg void OnUpdateDepthWorld(CCmdUI* pCmdUI);
	afx_msg void OnCentreCentretoorigin();
	afx_msg void OnUpdateCentreCentretoorigin(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG 
inline CRasDoc* CRasView::GetDocument()
   { return (CRasDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_RASVIEW_H_)
