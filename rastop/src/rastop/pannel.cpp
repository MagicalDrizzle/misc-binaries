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




////////////////////////////////////////////////////////////////////////////
// pannel.cpp:
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "pannel.h"
#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"

#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define LINECOUNTMAX 200

#define IsAlmostZero(x) (((x)>-0.005)&&((x)<=0.005))

static CString Off;

//////////////////////////////////////////////////////////////////////
// CPannelBar

IMPLEMENT_DYNAMIC(CPannelBar, CDialogBar)

BEGIN_MESSAGE_MAP(CPannelBar, CDialogBar)
	//{{AFX_MSG_MAP(CPannelBar)
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_RADIO_ROTATE, OnRotateButton)
	ON_BN_CLICKED(IDC_RADIO_TRANSLATE, OnTranszoomButton)
	ON_COMMAND(IDC_BUTTON_SPIN, OnSpinButton)
	ON_COMMAND(IDC_BUTTON_WORLD, OnWorldButton)
    ON_COMMAND(IDC_BUTTON_DEPTHCUE, OnDepthCueButton)
    ON_COMMAND(IDC_BUTTON_SLAB, OnSlabButton)
    ON_COMMAND(IDC_BUTTON_DEPTH, OnDepthButton)
	ON_BN_CLICKED(IDC_BUTTON_RESET, OnResetButton)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_COMMAND, OnCloseButton)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON_CLOSE_COMMAND, OnUpdateCloseButton)
	ON_UPDATE_COMMAND_UI(IDC_SLID_LIGHT, OnUpdateLightSlid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPannelBar::CPannelBar()	{
    Off.LoadString(IDS_LOCAL_OFF);
}
CPannelBar::~CPannelBar()	{}

BOOL CPannelBar::Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID)
{	if( !CDialogBar::Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), nStyle, nID) )
		return false;
	return TRUE;
}

BOOL CPannelBar::OnInitDialogBar()
{
	CWnd* pWndControl =  GetDlgItem((int)IDC_STATIC_LIGHT);
	pWndControl->SetFont( &m_FontPannel, false );
	pWndControl =  GetDlgItem((int)IDC_STATIC_SPECULAR);
	pWndControl->SetFont( &m_FontPannel, false );
	pWndControl =  GetDlgItem((int)IDC_STATIC_SHADE);
	pWndControl->SetFont( &m_FontPannel, false );

	m_RasmolStatus.SubclassDlgItem(IDC_RASMOL_STATUS, this);
	StatusLight.LoadBitmap(IDB_LIGHT_ON);
	m_RasmolStatus.SetBitmap(StatusLight.operator HBITMAP());

	m_Infoline.SubclassDlgItem(IDC_INFOLINE,this);
	m_Infoline.SetReadOnly(true );
	m_Infoline.SetFont(&m_FontPannel, false);

	infopointer.SubclassDlgItem(IDC_INFOPOINTER,this);
	infopointeroff.LoadBitmap( IDB_INFOPOINTEROFF );
	infopointer.SetBitmap(infopointeroff.operator HBITMAP());
	m_PannelTimerID_1 = 0;
	m_InfoTimerStatus = true;

	m_ScrollX.SubclassDlgItem (IDC_ROTATEX,this);
	m_ScrollX.SetScrollRange(0,200,true);
	m_ScrollX.SetScrollPos(100,true);

	m_ScrollY.SubclassDlgItem(IDC_ROTATEY,this);
	m_ScrollY.SetScrollRange(0,200,true);
	m_ScrollY.SetScrollPos(100,true);

	m_ScrollZ.SubclassDlgItem(IDC_ROTATEZ,this);
	m_ScrollZ.SetScrollRange(0,200,true);
	m_ScrollZ.SetScrollPos(100,true);

	m_moderotate.SubclassDlgItem(IDC_RADIO_ROTATE,this);
	m_moderotate.SetCheck(true );
	m_modetranslate.SubclassDlgItem(IDC_RADIO_TRANSLATE,this);
	m_modetranslate.SetCheck(false );
	m_moderotate.SetFont(&m_FontPannel,false);
	m_modetranslate.SetFont(&m_FontPannel,false);
	
	m_reset.SubclassDlgItem(IDC_BUTTON_RESET,this);
	m_reset.SetState(false);
	m_reset.SetCheck(false);
	m_reset.SetFont(&m_FontPannel,false);

	m_modespin.SubclassDlgItem(IDC_BUTTON_SPIN,this);
	m_modespin.SetState(false);
	m_modespin.SetCheck(false);
	SpinX = SpinY = SpinZ = 0;
	m_PannelTimerID_0 = 0;
	m_spinStatus = false;
	m_modespin.SetFont(&m_FontPannel,false);

	m_world.SubclassDlgItem(IDC_BUTTON_WORLD,this);
	m_world.SetState(false);
	m_world.SetCheck(false);
	m_world.SetFont(&m_FontPannel,false);

	m_depthcue.SubclassDlgItem(IDC_BUTTON_DEPTHCUE,this);
	m_depthcue.SetState(false);
	m_depthcue.SetCheck(false);
	m_depthcue.SetFont(&m_FontPannel,false);

	m_butslab.SubclassDlgItem(IDC_BUTTON_SLAB,this);
	m_butslab.SetState(false);
	m_butslab.SetCheck(false);
	m_butslab.SetFont(&m_FontPannel,false);

	m_butdepth.SubclassDlgItem(IDC_BUTTON_DEPTH,this);
	m_butdepth.SetState(false);
	m_butdepth.SetCheck(false);
	m_butdepth.SetFont(&m_FontPannel,false);

	m_Slab.SubclassDlgItem(IDC_SPIN_SLAB, this);
	m_Slab.SetRange(0,1);
	m_Slab.SetPos(0);

	m_BuddySlab.SubclassDlgItem(IDC_BUDDY_SLAB,this);
	m_BuddySlab.SetReadOnly(true);
	m_BuddySlab.SetFont(&m_FontBuddy, false);
	m_BuddySlab.SetWindowText(Off);

	m_Depth.SubclassDlgItem(IDC_SPIN_DEPTH, this);
	m_Depth.SetRange(0,1);
	m_Depth.SetPos(1);

	m_BuddyDepth.SubclassDlgItem(IDC_BUDDY_DEPTH,this);
	m_BuddyDepth.SetReadOnly(true);
	m_BuddyDepth.SetFont(&m_FontBuddy, false);
	m_BuddyDepth.SetWindowText(Off);

	m_Light.SubclassDlgItem(IDC_SLID_LIGHT, this );
	m_Light.SetRange(0,100,false);
	m_Light.SetTicFreq(10);
	m_Light.SetLineSize(1);
	m_Light.SetPageSize(5);
	m_Light.SetPos((int)(DefaultAmbient*100));
	m_Light.SetRange(0,100,true);

	m_Specular.SubclassDlgItem(IDC_SPIN_SPECULAR,this);
	m_Specular.SetRange(16,0);
	m_Specular.SetPos(16);

	m_BuddySpec.SubclassDlgItem(IDC_BUDDY_SPEC,this);
	m_BuddySpec.SetReadOnly(true);
	m_BuddySpec.SetFont(&m_FontBuddy, false);
	m_BuddySpec.SetWindowText(Off);

	m_Shade.SubclassDlgItem(IDC_SPIN_SHADE,this);
	m_Shade.SetRange(100,0);
	m_Shade.SetPos(50);

	m_BuddyShade.SubclassDlgItem(IDC_BUDDY_SHADE,this);
	m_BuddyShade.SetReadOnly(true);
	m_BuddyShade.SetFont(&m_FontBuddy, false);
	m_BuddyShade.SetWindowText("50");

	m_CloseCommandView.SubclassDlgItem(IDC_BUTTON_CLOSE_COMMAND,this);
	m_CloseCommandView.LoadBitmaps(IDB_COMMAND_CLOSEU,IDB_COMMAND_CLOSED,0,0);

	return true;
}

void CPannelBar::OnAddInfoline(CString& strText) 
{	
	AppendString (strText);	

	if( m_InfoTimerStatus )
	{	m_InfoTimerStatus = false;
		infopointeroff.DeleteObject();
		infopointeroff.LoadBitmap(IDB_INFOPOINTERON);
		infopointer.SetBitmap(infopointeroff.operator HBITMAP());
		m_PannelTimerID_1 = SetTimer(IDT_TIMER_1,350,NULL);
		if( m_PannelTimerID_1 == 0 ) 
		{	infopointeroff.DeleteObject();
			infopointeroff.LoadBitmap(IDB_INFOPOINTEROFF);
			infopointer.SetBitmap(infopointeroff.operator HBITMAP());
		}
	}
}

void CPannelBar::AppendString(CString str)
{	static int InfoLineCount = 0;
	CString   strBuffer = "";
	int i; 

	if( str.IsEmpty() )	
		return;
    if( !str.Compare("\r\n") )
        return;
	m_Infoline.GetWindowText(strBuffer);
	strBuffer += str;

	InfoLineCount++;
	if( InfoLineCount>=LINECOUNTMAX )
	{	char* ptr = strBuffer.LockBuffer();
		int j = 0;
		i = 0;
		while( i<50 && *ptr )
		{	if( *ptr++ =='\n' )
				i++;
			j++;
		}
		strBuffer.UnlockBuffer();
		if( j>=0 )
		{	InfoLineCount -= i;
			strBuffer = strBuffer.Right(strBuffer.GetLength()-j-1);
		}
	}

	m_Infoline.SetWindowText(strBuffer);
    int l = m_Infoline.GetLineCount();
    int len = strBuffer.GetLength();
    if( len && (strBuffer.GetAt(len-1)=='\n') )
        l -= 3; // One for \r, one for \n, one for the double line
    m_Infoline.LineScroll(l,0);
	ChangeStatusLight(true);
}

void CPannelBar::ChangeStatusLight(int Status)
{	UINT nIDLight;
 
	StatusLight.DeleteObject();

	switch( Status )
	{	case(0):	nIDLight = 	IDB_LIGHT_OFF;
					break;
		case(1):	if( IsPaused )
					{	CWnd *pFoc = GetFocus( );
						if( pFoc )
						{	if( !pFoc->IsKindOf(RUNTIME_CLASS(CRasView)) )
								nIDLight = IDB_LIGHT_STOP;
							else
								nIDLight = IDB_LIGHT_PAUSE;
						}
					} else if( FileDepth!=-1 )
					{	nIDLight = IDB_LIGHT_SCRIPT;
					} else
					{	nIDLight = IDB_LIGHT_ON;
					}
					break;
		case(-1):	nIDLight = 	IDB_LIGHT_STOP;
	}

	StatusLight.LoadBitmap(nIDLight);
	m_RasmolStatus.SetBitmap(StatusLight.operator HBITMAP());

	CRect rec;
	m_RasmolStatus.GetClientRect((LPRECT) rec) ;
	CDC* pDC = m_RasmolStatus.GetDC();
	pDC->BitBlt( 0, 0, rec.right, rec.bottom, NULL, 0, 0, SRCCOPY);
	m_RasmolStatus.ReleaseDC(pDC);
	m_RasmolStatus.Invalidate(true);
	m_RasmolStatus.UpdateWindow();
}

void CPannelBar::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{	

    CString Command;
	if( !Database )
		return;

	//improve control handling
	pScrollBar->Invalidate(true);
	pScrollBar->UpdateWindow();

	//remove redundant messages
	MSG msg;
	if( ::PeekMessage(&msg, m_hWnd, WM_LBUTTONUP, WM_LBUTTONUP, PM_NOREMOVE) )
		while(::PeekMessage(&msg, m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;

	if(	pScrollBar==&m_ScrollX ||
		pScrollBar==&m_ScrollY ||
		pScrollBar==&m_ScrollZ )
	{

	/*	Non correspondance between DialValues and scrollbars
	 *	DialValue	0	rotate	X (y screen values, inverted on IBMPC)
	 *	DialValue	1	rotate	Y
	 *	DialValue	2	rotate	Z
	 *	DialValue	3	zoom
	 *	DialValue	4	translate X
	 *	DialValue	5	translate Y
	 */
	
    TRACE1("Pannel Scroll, nSB = %d\n",nSBCode);
	
	if( m_spinStatus && (nSBCode==SB_LINEDOWN || nSBCode==SB_LINEUP) )	
	{	int s = pScrollBar->GetDlgCtrlID( ); 
		double t = (nSBCode == SB_LINEUP) ? -0.0005:0.0005;
		switch(s)
		{	case(IDC_ROTATEX):	SpinX += -t;break;
			case(IDC_ROTATEY):	SpinY += t;	break;
			case(IDC_ROTATEZ):	SpinZ += t;	break;
			default:            return;
		}
		return;
	}

	int pos = pScrollBar->GetScrollPos();

	switch( nSBCode )
	{	case(SB_LINEDOWN):  pos += 4;   break;
		case(SB_PAGEDOWN):  pos += 20;  break;
		case(SB_PAGEUP):    pos -= 20;  break;
		case(SB_LINEUP):    pos -= 4;   break;
		case(SB_THUMBTRACK):
		case(SB_THUMBPOSITION):
							if( pos == (int)nPos )
								return;
							pos = nPos;	break;
		case(SB_ENDSCROLL):
		case(SB_LEFT):	
		case(SB_RIGHT):		
		default:			//button up
							//ReDrawFlag |= RFRefresh;
							//pView->RefreshView();
							return;
	}

	if( Database )
	{	int s = pScrollBar->GetDlgCtrlID(); 
		int DialFlag;
		int Switch = m_modetranslate.GetCheck();
		double m;
	    int wrap = 0;

		switch( s )
		{	case(IDC_ROTATEX):	DialFlag = 0 + 4*Switch;	break;
			case(IDC_ROTATEY):	DialFlag = 1 + 4*Switch;	break;
			case(IDC_ROTATEZ):	DialFlag = 2 + Switch;		break;
		}

        if( DialFlag==3 )
        {   if( pos>200 )
                pos = 200;
            else if( pos<-200 )
                pos = -200;
        } else if( pos>200 && nSBCode==SB_THUMBTRACK ) 
	    {	while( pos>200 )
            {   pos -= 200;
		        wrap += 1;
            }
	    } else if( pos<0 && nSBCode==SB_THUMBTRACK ) 
	    {	while( pos<0 )
            {   pos += 200;
		        wrap -= 1;
            }
	    }
	    pScrollBar->SetScrollPos( pos, true );

		switch( DialFlag )
		{	case(0):	if( RotMode==RotMol )
                        {   double tokval = 1 - (double)pos/100.0 - DialValue[0];
                            tokval *= -180.0;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate molecule x %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        } else
                        {   double tokval = 1 - (double)pos/100.0 - WRotValue[0];
                            tokval *= -180.0;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate world x %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        }	
						break;
			case(1):	if( RotMode==RotMol )
                        {   double tokval = (double)pos/100.0 -1 - DialValue[1];
                            tokval *= 180.00;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate molecule y %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        } else
                        {   double tokval = (double)pos/100.0 -1 - WRotValue[1];
                            tokval *= 180.00;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate world y %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        }	
						break;
			case(2):	if( RotMode==RotMol )
                        {   double tokval = 1 - (double)pos/100.0 - DialValue[2];
                            tokval *= -180.00;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate molecule z %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        } else
                        {   double tokval = 1 - (double)pos/100.0 - WRotValue[2];
                            tokval *= -180.00;
                            if( !IsAlmostZero(tokval) )
                            {   Command.Format("rotate world z %.2f\n",tokval);
                                pView->ExecuteCommand(DisplayOn,Command);
                            }
                        }	
						break;
			case(3):	if( nSBCode==SB_LINEDOWN || nSBCode==SB_LINEUP ) {
                            if( (pos/100.0 - 1)<0.0 )
                                Command.Format("zoom world %d\n",pos);
                            else
                                Command.Format("zoom world %d\n",(int)(MaxZoom*(pos - 100))+100);
                        } else {
                            if( (pos/100.0 - 1)<0.0 )
                                Command.Format("zoom %d\n",pos);
                            else
                                Command.Format("zoom %d\n",(int)(MaxZoom*(pos - 100))+100);
                        }
                        pView->ExecuteCommand(DisplayOn,Command);
                        break;
			case(4):	m = 1;
						if( RotMode==RotMol )
						{	if( DialValue[4]!=0 )
								m = DialValue[4]/fabs(DialValue[4]);
							double newdv = 
							((int)((DialValue[4]+m)/2)+wrap)*2 + pos/100.0 - 1;
                            newdv -= LastTX-MOffset[0]/Range;
                            Command.Format("translate x %.3f\n",100*newdv);
                            pView->ExecuteCommand(DisplayOn, Command);
						} else
						{	if( WTransValue[0]!=0 )
								m = WTransValue[0]/fabs(WTransValue[0]);
							double newdv = 
							((int)((WTransValue[0]+m)/2.01)+wrap)*2 + pos/100.0 - 1;
                            newdv -= WLastTX-Wo[0]/Range;
                            Command.Format("translate world x %.3f\n",100*newdv);
                            pView->ExecuteCommand(DisplayOn, Command);
						} 
						break;
			case(5):	m = 1;
						if( RotMode==RotMol )
						{	if( DialValue[5]!=0 )
								m = DialValue[5]/fabs(DialValue[5]);
							double newdv = 
							((int)((DialValue[5]+m)/2)+wrap)*2 + 1 - pos/100.0;
                            newdv -= LastTY-MOffset[1]/Range;
                            Command.Format("translate y %.3f\n",-100*newdv);
                            pView->ExecuteCommand(DisplayOn, Command);
						} else
						{	if( WTransValue[1]!=0 )
								m = WTransValue[1]/fabs(WTransValue[1]);
							double newdv = 
							-((int)((WTransValue[1]+m)/2.01)+wrap)*2 + 1 - pos/100.0;
                            newdv -= WLastTY-Wo[1]/Range;
                            Command.Format("translate world y %.3f\n",-100*newdv);
                            pView->ExecuteCommand(DisplayOn, Command);
						}
						
						break;
		}
	}
	return;
	} else if( &m_Slab==(CSpinButtonCtrl *)pScrollBar )
	{ 	double inc;
        static double acc = 1.0/1.2;

		if( !Database ) return;
        if( nSBCode==SB_ENDSCROLL)
        {   acc = 1.0/1.2;
            return;
        }
        if( nSBCode!=SB_THUMBPOSITION ) return;

        acc *= 1.2;
        if( acc >= 200.0 ) acc = 200.0;
        if( ((int)nPos == 0)) inc = 1.0;
        else inc = -1.0;
        inc *= acc;
		Command.Format("slab world %.3f\n",-inc/50);
	    pView->ExecuteCommand(DisplayOn, Command);
        
	} else if( &m_Depth==(CSpinButtonCtrl *)pScrollBar )
	{ 	double inc;
        static double acc = 1.0/1.2;

		if( !Database ) return;
        if( nSBCode==SB_ENDSCROLL)
        {   // reset
            acc = 1.0/1.2;
            return;
        }
        if( nSBCode!=SB_THUMBPOSITION ) return;

        acc *= 1.2;
        if( acc >= 100.0 ) acc = 100.0;
        if( ((int)nPos == 0)) inc = 1.0;
        else inc = -1.0;
        inc *= acc;
		Command.Format("depth world %.3f\n",-inc/50);
	    pView->ExecuteCommand(DisplayOn, Command);
        
	} else if( &m_Light	== (CSliderCtrl *) pScrollBar )
	{	if( nSBCode==SB_ENDSCROLL ) return;
        int pos = m_Light.GetPos();
		if( pos>100 ) pos = 100;
		if( pos<0 )   pos = 0; 
		if(Database)
		{	if( Ambient!=pos/100.0 )
			{	Command.Format("set ambient %d\n",pos);
                pView->ExecuteCommand(DisplayOn,Command);
			}		
		}
	} else if( &m_Specular==(CSpinButtonCtrl *)pScrollBar )
	{	if( nSBCode==SB_ENDSCROLL ) return;
        int pos = LOWORD(m_Specular.GetPos());
		if( pos>16 ) pos = 16;
		if( pos<0 )  pos = 0; 
		if( Database )
		{	int expos = 16;
			if( !FakeSpecular )			expos = 16;
			else if( SpecPower<10 )		expos = SpecPower;
			else if( SpecPower<15 )		expos = 10;
			else if( SpecPower<20 )		expos = 11;
			else if( SpecPower<30 )		expos = 12;
			else if( SpecPower<50 )		expos = 13;
			else if( SpecPower<80 )		expos = 14;
			else if( SpecPower<100)		expos = 15;

            if( expos!=pos )		
			{	int npower;
				switch(pos)
				{	case(11):	npower = 15;    break;
				 	case(12):	npower = 20;    break;
				 	case(13):	npower = 30;    break;
					case(14):	npower = 50;    break;
					case(15):	npower = 80;    break;
					case(16):	npower = 100;   break;
					default:	npower = pos;
				}
                if( npower!=100 )
                {   if( !FakeSpecular )
                        pView->ExecuteCommand(DisplayOff,"set specular on\n");
                    Command.Format("set specpower %d\n",npower);
                    pView->ExecuteCommand(DisplayOn,Command);
                } else 
                {   pView->ExecuteCommand(DisplayOff,"set specpower 100\n");
                    pView->ExecuteCommand(DisplayOn,"set specular off\n");
                }

			}	
		}
	} else if( &m_Shade==(CSpinButtonCtrl *)pScrollBar )
	{	if( nSBCode==SB_ENDSCROLL ) return;
        int pos = LOWORD(m_Shade.GetPos());
		if( pos>100 ) pos = 100;
		if( pos<0 )   pos = 0; 
		if( Database )
		{	if( pos!=ShadePower )
			{	Command.Format("set shadepower %d\n",pos);
                pView->ExecuteCommand(DisplayOn,Command);
			}
		}
	}			
}

void CPannelBar::OnTimer(UINT nIDEvent) 
{	if( nIDEvent==m_PannelTimerID_1 )
	{	m_InfoTimerStatus = true;
		if( KillTimer((UINT_PTR)m_PannelTimerID_1) )
			m_PannelTimerID_1 = 0;
		infopointeroff.DeleteObject();
		infopointeroff.LoadBitmap(IDB_INFOPOINTEROFF);
		infopointer.SetBitmap(infopointeroff.operator HBITMAP());
		return;
	} else if( nIDEvent==m_PannelTimerID_0 )
	{	KillTimerPannel();
		if( !Database )
			ResetSpinMode();

		GETFRAME
		GETVIEW

		/*Setting of dial variation to drawing time
		 * 0.001 dial unit per 25 ms
		 */
		if( RotMode==RotMol )
		{   if( pView->m_drawingtime>=2 )
		    {	DialValue[0] += (SpinX*pView->m_drawingtime)/25;
		    	DialValue[1] += (SpinY*pView->m_drawingtime)/25;
		    	DialValue[2] -= (SpinZ*pView->m_drawingtime)/25;
			} else 
			{	DialValue[0] += SpinX;
				DialValue[1] += SpinY;
				DialValue[2] -= SpinZ;
			}

			for( int i = 0; i<3;i++ )
			{	if(DialValue[i] > 1.0) DialValue[i] -= 2.0;
				if(DialValue[i] < -1.0) DialValue[i] += 2.0;
			}
            ReDrawFlag |= RFRotate;
            PrepareMoleculeRotate();
		} else
		{   if( pView->m_drawingtime>=2 )
		    {	WRotValue[0] += (SpinX*pView->m_drawingtime)/25;
		    	WRotValue[1] += (SpinY*pView->m_drawingtime)/25;
		    	WRotValue[2] -= (SpinZ*pView->m_drawingtime)/25;
			} else 
			{	WRotValue[0] += SpinX;
				WRotValue[1] += SpinY;
				WRotValue[2] -= SpinZ;
			}

			for( int i = 0; i<3;i++ )
			{	if(WRotValue[i] > 1.0) WRotValue[i] -= 2.0;
				if(WRotValue[i] < -1.0) WRotValue[i] += 2.0;
			}

            ReDrawFlag |= RFRotate;
            PrepareWorldRotate(1);
		}

		RefreshView();		
		if( UseSlabPlane || UseDepthPlane )
			UpdatePannel();
		else
			UpdateScrollBars();
		SetTimerPannel();
	}
}

void CPannelBar::OnRotateButton()		{	UpdateScrollBars();	}
void CPannelBar::OnTranszoomButton()	{	UpdateScrollBars();	}

void CPannelBar::SetTimerPannel()
{	static BOOL OkTimer = true;
	if( OkTimer&&m_spinStatus )
	{	m_PannelTimerID_0 = SetTimer(IDT_TIMER_0, 1, NULL);
		if( m_PannelTimerID_0==0 )
		{	MessageBox("Unable to obtain a timer for spinmode!",
			 NULL, MB_OK|MB_SYSTEMMODAL);
			OkTimer =  false;
		}
		FastDraw = true;
	}
}

void CPannelBar::KillTimerPannel()
{	if( m_spinStatus )
	{	if( KillTimer(m_PannelTimerID_0) )
			m_PannelTimerID_0 = 0;
		FastDraw = false;
	}
}

void CPannelBar::ResetSpinMode()
{	KillTimerPannel();	
	m_spinStatus = false;
	m_modespin.SetCheck(false);
	SpinX = SpinY = SpinZ = 0;
		
}

void CPannelBar::OnSpinButton() 
{	if( !Database )
	{	m_modespin.SetCheck(false);
		return;
	}

	if( !m_spinStatus )
	{	m_spinStatus = true;
		m_modespin.SetCheck(true);
		if( (SpinX==0) && (SpinY==0) && (SpinZ==0) )
			SpinY = 0.001;
		SetTimerPannel();
	} else
		ResetSpinMode();
}

void CPannelBar::OnWorldButton() 
{	if( !Database )
	{	m_world.SetCheck(false);
		return;
	}

	GETFRAME
	GETVIEW
	if( pView!=NULL ) 
	{	if( RotMode==RotMol )
	    {	if( DisplayMode )
			{	pView->ExecuteCommand(DisplayOn, "rotate world on\n");
			} else
			{	pView->ExecuteCommand(DisplayOff, "rotate world on\n");
				UpdatePannel();
			}
		    m_world.SetCheck(true);
	    } else
	    {	if( DisplayMode )
			{	pView->ExecuteCommand(DisplayOn, "rotate world off\n");
			} else
			{	pView->ExecuteCommand(DisplayOff, "rotate world off\n");
				UpdatePannel();
			}
		    m_world.SetCheck(false);
		}
	}
}

void CPannelBar::OnSlabButton() 
{	
	GETFRAME
	GETVIEW
	if( pView!=NULL ) 
	{	if( !UseSlabPlane )
	    {	pView->ExecuteCommand(DisplayOn, "slab on\n");
		    m_butslab.SetCheck(true);
	    } else
	    {	pView->ExecuteCommand(DisplayOn, "slab off\n");
		    m_butslab.SetCheck(false);
		}
	}
}

void CPannelBar::OnDepthButton() 
{	
	GETFRAME
	GETVIEW
	if( pView!=NULL ) 
	{	if( !UseDepthPlane )
	    {	pView->ExecuteCommand(DisplayOn, "depth on\n");
		    m_butdepth.SetCheck(true);
	    } else
	    {	pView->ExecuteCommand(DisplayOn, "depth off\n");
		    m_butdepth.SetCheck(false);
		}
	}
}

void CPannelBar::OnDepthCueButton() 
{	
	GETFRAME
	GETVIEW
	if( pView!=NULL ) 
	{	if( !UseDepthCue )
	    {	pView->ExecuteCommand(DisplayOn, "set depthcue on\n");
		    m_depthcue.SetCheck(true);
	    } else
	    {	pView->ExecuteCommand(DisplayOn, "set depthcue off\n");
		    m_depthcue.SetCheck(false);
		}
	}
}

void CPannelBar::OnResetButton() 
{	
	m_reset.SetCheck(false);

	if( !Database )
		return;	
	
	GETFRAME
	GETVIEW
	if( pView!=NULL ) 
	{	pView->ExecuteCommand(DisplayOn, "reset\n");
	}
	::SetPickMode( PickIdent );
	ResetSpinMode();
}

void CPannelBar::OnUpdateLightSlid(CCmdUI *pCmdUI)
{	m_Light.SetRange(0, 100, false);	}

void CPannelBar::OnCloseButton()
{	GETFRAME
	pFrame->DesactivateCommandBar();

	//Give back focus to view during paused script
	GETVIEW
	if( pView != NULL )
		pView->SetFocus();
}

void CPannelBar::OnUpdateCloseButton(CCmdUI *pCmdUI)
{	pCmdUI->Enable(true);	}
 
void CPannelBar::RefreshView()
{	if( !Database )
		return;
	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	if( UseSlabPlane || UseDepthPlane )
		pView->RefreshView();
	else
		pView->Invalidate(true);
}

void CPannelBar::DisplaySlabBuddy()
{	CString buddy;
	double pos;

	if( Database && (WClip[0] != -999999.))
	{	pos = GetClipCoord(SysCoor,CurMolSet,0);
		if( SysCoor == MolCoor )
		{	buddy.Format("%.1f",pos);
			buddy += "%";
		} else
		{	buddy.Format("%.2f",pos);
		}
		m_BuddySlab.SetWindowText(buddy);
		m_BuddySlab.UpdateWindow( );
	} else
	{	m_Slab.SetPos(1200);
		m_BuddySlab.SetWindowText(Off);
	}
}

void CPannelBar::DisplayDepthBuddy()
{	CString buddy;
	double pos;
	
	if( Database && (WClip[1] != -999999.) )
	{	pos = GetClipCoord(SysCoor,CurMolSet,1);
		if( SysCoor == MolCoor )
		{	buddy.Format("%.1f",pos);
			buddy += "%";
		} else
		{	buddy.Format("%.2f",pos);
		}
		m_BuddyDepth.SetWindowText(buddy);
		m_BuddyDepth.UpdateWindow( );
	} else
	{	m_Depth.SetPos(200);
		m_BuddyDepth.SetWindowText(Off);
	}
}

void CPannelBar::UpdateScrollBars()
{	int pos;

	if( m_moderotate.GetCheck() )
	{   if( RotMode == RotMol )
		{	pos = 100 + (int)(100.0*DialValue[0]);
			m_ScrollX.SetScrollPos( 200 - pos, true );

			pos = 100 + (int)(100.0*DialValue[1]);
			m_ScrollY.SetScrollPos( pos, true );
	
			pos = 100 - (int)(100.0*DialValue[2]);
			m_ScrollZ.SetScrollPos( pos, true );
		} else
		{	pos = 100 + (int)(100.0*WRotValue[0]);
			m_ScrollX.SetScrollPos( 200 - pos, true );

			pos = 100 + (int)(100.0*WRotValue[1]);
			m_ScrollY.SetScrollPos( pos, true );
	
			pos = 100 - (int)(100.0*WRotValue[2]);
			m_ScrollZ.SetScrollPos( pos, true );
		}
	} else
	{   double s,t;
	
		if( RotMode==RotMol )
		{	t = DialValue[4];
			s = 1;
			if( t!=0 ) s = fabs(DialValue[4])/t;
			while( s*t>1 ) t -= 2*s;
			pos = 100 + (int)(100.0*t);
			m_ScrollX.SetScrollPos( pos, true );
		
		    t = DialValue[5];
			s = 1;
			if( t!=0 ) s = fabs(DialValue[5])/t;
			while( s*t>1 ) t -= 2*s;
			pos = 100 + (int)(100.0*t);
			m_ScrollY.SetScrollPos( 200 - pos, true );
		} else
		{	t = WTransValue[0];
			s = 1;
			if( t!=0 ) s = fabs(WTransValue[0])/t;
			while( s*t>1 ) t -= 2*s;
			pos = 100 + (int)(100.0*t);
			m_ScrollX.SetScrollPos( pos, true );
		
		    t = WTransValue[1];
			s = 1;
			if( t!=0 ) s = fabs(WTransValue[1])/t;
			while( s*t>1 ) t -= 2*s;
			pos = 100 + (int)(100.0*t);
			m_ScrollY.SetScrollPos( 200 - pos, true );
		}
			
	    pos = 100 + (int)(100.0*DialValue[3]);
		m_ScrollZ.SetScrollPos( pos, true );
	}
}

void CPannelBar::UpdatePannel()
{	double pos;
	CString buddy;

	if( !Database )
    {   m_Light.SetPos(50);
        DisplaySlabBuddy();
        DisplayDepthBuddy();
        m_BuddySpec.SetWindowText(Off);
	    m_BuddyShade.SetWindowText(Off);
        return;
    }

	UpdateScrollBars();

	m_Light.SetPos((int)(100*Ambient));

	DisplaySlabBuddy();
	DisplayDepthBuddy();

	pos = 16;
	if( FakeSpecular )
	{	if( SpecPower<10 )
		{	pos = SpecPower;
		} else if( SpecPower<15 )
		{	pos = 10;
		} else if( SpecPower<20 )
		{	pos = 11;
		} else if( SpecPower<30 )
		{	pos = 12;
		} else if( SpecPower<50 )
		{	pos = 13;
		} else if( SpecPower<80 )
		{	pos = 14;
		} else if( SpecPower<100 )
		{	pos = 15;
		}
	}
	buddy.Format("%d",SpecPower);
	if( !FakeSpecular )
		buddy = Off;
	m_BuddySpec.SetWindowText(buddy);
	m_Specular.SetPos((int)pos);

	buddy.Format("%d",ShadePower);
	m_BuddyShade.SetWindowText(buddy);
	m_Shade.SetPos(ShadePower);

	m_world.SetCheck(RotMode!=RotMol);
	m_butslab.SetCheck(UseSlabPlane);
	m_butdepth.SetCheck(UseDepthPlane);
	m_depthcue.SetCheck(UseDepthCue);
}

