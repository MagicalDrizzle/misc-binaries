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



//////////////////////////////////////////////////////////////////////
// pannel.h
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PANNEL_H__INCLUDED_)

#define AFX_PANNEL_H__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define IDT_TIMER_0 WM_USER + 200
#define IDT_TIMER_1 IDT_TIMER_0 + 1

/////////////////////////////////////////////////////////////////////////////
// CPannelBar

class CPannelBar : public CDialogBar
{
	DECLARE_DYNAMIC(CPannelBar)
    public:
		CPannelBar();
		virtual ~CPannelBar();

    // Attributes
    public:
	    CFont m_FontPannel;
	    CFont m_FontBuddy;

		void ChangeStatusLight(int Status);
		void UpdatePannel();
		void UpdateScrollBars();
	    void KillTimerPannel();
	    void SetTimerPannel();
		void ResetSpinMode();
		void OnAddInfoline(CString& strText);
		virtual BOOL OnInitDialogBar();

    protected:
		BOOL m_InfoTimerStatus;
		CStatic infopointer;
		CBitmap infopointeroff;
		CBitmap infopointeron;
		CEdit m_Infoline;
		CScrollBar m_ScrollX;
		CScrollBar m_ScrollY;
		CScrollBar m_ScrollZ;
		CButton m_moderotate;
		CButton m_modetranslate;
		CButton m_reset;
		CButton m_modespin;
		CButton m_world;
        CButton m_depthcue;
		CButton m_butslab;
        CButton m_butdepth;
		BOOL m_spinStatus;
		CSliderCtrl m_Light;
		CSpinButtonCtrl m_Slab;
		CEdit m_BuddySlab;
		CSpinButtonCtrl m_Depth;
		CEdit m_BuddyDepth;
		CSpinButtonCtrl m_Specular;
		CEdit m_BuddySpec;
		CSpinButtonCtrl m_Shade;
		CEdit m_BuddyShade;
		UINT m_PannelTimerID_0;
		UINT m_PannelTimerID_1;
		double SpinX, SpinY, SpinZ;
		CBitmapButton m_CloseCommandView;
		CStatic m_RasmolStatus;
		CBitmap StatusLight;

		void AppendString(CString str);
		void RefreshView();
		void DisplaySlabBuddy();
		void DisplayDepthBuddy();

	// Operations
	public:
	//{{AFX_VIRTUAL(CPannelBar)
	//}}AFX_VIRTUAL

	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);

	protected:
	//{{AFX_MSG(CPannelBar)
	afx_msg void OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar );
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnRotateButton();
	afx_msg void OnTranszoomButton();
	afx_msg void OnSpinButton();
	afx_msg void OnResetButton();
	afx_msg void OnDepthCueButton();
	afx_msg void OnSlabButton();
	afx_msg void OnDepthButton();
	afx_msg void OnCloseButton();
	afx_msg void OnWorldButton();
	afx_msg void OnUpdateCloseButton(CCmdUI *pCmdUI);
	afx_msg void OnUpdateLightSlid(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif // !defined(AFX_PANNEL_H__INCLUDED_)

