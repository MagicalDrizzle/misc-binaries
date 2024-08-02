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
 * Christian Duqué, copyright (c) 2003 (RasTop 2.0.2-VF June 2003)
 */


////////////////////////////////////////////////////////////////////////////
// EchoFrm.cpp:
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "cmdwindow.h"
#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAXLIGNES 500


static int CharWide = 0;
static int CharHigh = 0;
static int margin = 10;
static int Compteur = 0;
static int PromptLength = 0;
	
static int brushmemory = 0;

//////////////////////////////////////////////////////////////////////
// CmdWindow

IMPLEMENT_DYNAMIC(CmdWindow, CDialog)

BEGIN_MESSAGE_MAP(CmdWindow, CDialog)
	//{{AFX_MSG_MAP(CmdWindow)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CmdWindow::CmdWindow()	{}
CmdWindow::~CmdWindow()	{}


BOOL CmdWindow::Create( UINT nIDTemplate, CWnd* pParentWnd )
{

	if( !CDialog::Create(nIDTemplate, pParentWnd) )
		return false;

    CString s;
    s.LoadString(IDS_LOCAL_CMDWND);

	SetWindowText(_T(s));
	m_EchoText.SubclassDlgItem(IDC_CMDWNDTEXT,this);
	m_EchoText.SetReadOnly(false);

    TEXTMETRIC Text;
    LOGFONT LogFont;
    HFONT TermFont;

    HDC hDC;
    
    LogFont.lfHeight     = -15;
    LogFont.lfWidth      = 8;
    LogFont.lfEscapement = 0;
    LogFont.lfWeight     = 0;
    LogFont.lfItalic     = 0;
    LogFont.lfUnderline  = 0;
    LogFont.lfStrikeOut  = 0;
    
    LogFont.lfCharSet        = ANSI_CHARSET;
    LogFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
    LogFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
    LogFont.lfQuality        = PROOF_QUALITY;
    LogFont.lfPitchAndFamily = FIXED_PITCH | FF_SWISS;
    LogFont.lfFaceName[0]    = '\0';
	m_FontEchoText.CreateFontIndirect( &LogFont);
	m_EchoText.SetFont (&m_FontEchoText);

    TermFont = CreateFontIndirect(&LogFont);
    hDC = ::GetDC(NULL);
    ::SelectObject(hDC,TermFont);
    ::GetTextMetrics(hDC,&Text);  
    ::ReleaseDC(NULL,hDC);
    
    CharWide = Text.tmAveCharWidth;
    CharHigh = Text.tmHeight + Text.tmExternalLeading;
    ::DeleteObject(TermFont);

	m_EchoText.SetMargins(margin,0);

	m_EchoText.TextBuffer.Empty();


    /* Note: Variales not yet initialized in InitialiseCmndLine() */
	CmdBkgColor = (int) RGB(0,0,0);  
	CmdTxtColor = (int) RGB(0,255,0);  
	m_brush.CreateSolidBrush((COLORREF)CmdBkgColor); 
    brushmemory = CmdBkgColor;

	return true;
}


HBRUSH CmdWindow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr;;
	
    pDC->SetBkColor((COLORREF) CmdBkgColor); 
    pDC->SetTextColor((COLORREF) CmdTxtColor);
    hbr = (HBRUSH) m_brush;    // fills the dialog rectangle

	return hbr;
}

void CmdWindow::ChangeTxtColor( COLORREF newtxtcolor ) {
    if (((COLORREF)CmdTxtColor) == newtxtcolor)
        return;
	CmdTxtColor = (int) newtxtcolor;  
    Invalidate(true);
	UpdateWindow( );
    PrefsModified = true;
}

void CmdWindow::ChangeBkgColor( COLORREF newbkgcolor ) {
    if ( ((COLORREF) CmdBkgColor) == newbkgcolor)
        return;
	CmdBkgColor=(int)newbkgcolor;  
    m_brush.DeleteObject();
	m_brush.CreateSolidBrush((COLORREF) CmdBkgColor); 
    Invalidate(true);
	UpdateWindow( );
    PrefsModified = true;
}

COLORREF CmdWindow::getTxtColor( void ) {
    return (COLORREF) CmdTxtColor;
}

COLORREF CmdWindow::getBkgColor( void ) {
    return (COLORREF) CmdBkgColor;
}

void CmdWindow::EcrisDansEcho(CString& strText) 
{	CString   strBuffer = "";
	int i; 

    /* Awful hack to have color preference set in bacground window 
     * reset brush if necessary */
    if (brushmemory != CmdBkgColor) {
        m_brush.DeleteObject();
	    m_brush.CreateSolidBrush((COLORREF) CmdBkgColor); 
        brushmemory = CmdBkgColor;
        Invalidate(true);
    	UpdateWindow( );
    }


    if ( strText.IsEmpty() )
        return;

    if( CommandActive && strText == "\r\n" )
    {   m_EchoText.ProcessCommand(false);
        return;
    }

    if( strText.GetAt(0) == 0x08 )
    {   int l = m_EchoText.TextBuffer.GetLength();
        if( l<=PromptLength )
            return; //beep?
        m_EchoText.TextBuffer = m_EchoText.TextBuffer.Left(l-1);
        
    } else
    {
        m_EchoText.TextBuffer += strText;
        if( strText == "RasTop> " )
            PromptLength = m_EchoText.TextBuffer.GetLength();

        if( strText.Find('\n') != -1 )
            Compteur++;
	    if( Compteur>=MAXLIGNES )
	    {	char* ptr = m_EchoText.TextBuffer.LockBuffer();
		    int j = 0;
		    i = 0;
		    while( i<50 && *ptr )
		    {	if( *ptr++ =='\n' )
				    i++;
			    j++;
		    }
		    m_EchoText.TextBuffer.UnlockBuffer();
		    if( j>=0 )
		    {	Compteur -= i;
			    m_EchoText.TextBuffer = m_EchoText.TextBuffer.Right(m_EchoText.TextBuffer.GetLength()-j);
		    }
	    }
    }
    m_EchoText.SetWindowText(m_EchoText.TextBuffer);
}


void CmdWindow::OnSize(UINT nType, int cx, int cy) 
{
	if( WindowCommandLine )
	{	CDialog::OnSize(nType, cx, cy);
		RecordCommandWindow();
		PrefsModified = true;
	}

    RECT r;
    GetClientRect( &r );

    m_EchoText.MoveWindow(r.left,r.top,r.right-r.left,r.bottom-r.top, true );
	m_EchoText.SetMargins(margin,margin);
    r.left += margin;
    r.top += margin;
    r.right -= (r.left+margin);
    r.bottom -= (r.top);
	m_EchoText.SetRect( &r );
}


void CmdWindow::OnMove(int x, int y) 
{	if( WindowCommandLine )
	{	CDialog::OnMove(x, y);
		RecordCommandWindow();
		PrefsModified = true;
    }
}


void CmdWindow::OnClose() 
{   if( WindowCommandLine )
    {   WindowCommandLine  = FALSE;
        PrefsModified = TRUE;
    }	
	CDialog::OnClose();
}


/////////////////////////////////////////////////////////////////////////////
// CRasMolEdit

CRasMolEdit::CRasMolEdit()  {}
CRasMolEdit::~CRasMolEdit() {}

static int paint = 1;


BEGIN_MESSAGE_MAP(CRasMolEdit, CEdit)
	//{{AFX_MSG_MAP(CRasMolEdit)
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
    ON_MESSAGE( WM_SETTEXT, OnSetText )
END_MESSAGE_MAP()


/* Dummy mouse click to set the caret at end of text; sometimes works! */
void CRasMolEdit::SetCaretToEnd() 
{   if( !(GetFocus() == this) )
        return;

    int x = 2000;
	int y = 5000;
    WPARAM w = (y<<16) + (unsigned int)x;
    PostMessage(WM_LBUTTONDOWN,0,w);
    PostMessage(WM_LBUTTONUP,0,w);
}


void CRasMolEdit::ProcessCommand( int before )
{   
    if( CommandActive == false )    
        return;

    CString command = TextBuffer.Right( TextBuffer.GetLength()-PromptLength ) + "\r\n";
    CommandActive = false;
    GETFRAME
    CString s = "\r\n";
    if (before)
        pFrame->m_wndCmdWindow.EcrisDansEcho(s);
    GETVIEW
    if( pView )
        pView->ExecuteCommand( DisplayOn, command);
    if (!before)
        pFrame->m_wndCmdWindow.EcrisDansEcho(s);
}


void CRasMolEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    BYTE ks[256];
    GetKeyboardState(ks);
    WORD w;
    UINT scan=0;
    ToAscii(nChar,scan,ks,&w,0);
    char ch = char(w);

    if( nChar =='V' ) {
        if( HIWORD(GetKeyState(VK_CONTROL)) )
        {   if( OpenClipboard() ) {
                HANDLE data = GetClipboardData(CF_TEXT);
                char *ptr = (char*)GlobalLock(data);
                GlobalUnlock(data);	
                if( ptr==NULL ) {
                    CloseClipboard();
                    return;
                }
                int len = 0;
                char *des;
		        while(1)
		        {	len = 0;
			        des = CurLine;
			        while( len<(MAXBUFFLEN-2) && *ptr!='\n' && *ptr )
			        {	if (*ptr=='\r') {
                            ptr++;
                            continue;
                        }
                        *des++ = *ptr++;
				        len++;
			        }
                    if( len )
			        {   if( *ptr=='\n' ) {
                            *des = '\0';
                            if (!strcmp(CurLine,"\r"))
                                break;
                            TextBuffer += CurLine;
                            /* Send a '\n' that ExecuteCommand() */
                            InvalidateCmndLine();   
                            ResetCommandLine(0);
                        } else {
                            *des = '\0';
                            TextBuffer += CurLine;
                            SetWindowText(TextBuffer);
                        }
                    }
			        if( !*ptr )
				        break;
			        *ptr++;
		        }
            CloseClipboard();
            }
            return;
        }
    } else if (nChar =='C') {
        if (HIWORD(GetKeyState(VK_CONTROL))) {
            if( OpenClipboard() ) {
                int a,b;
                GetSel(a,b);
                if (a<b) {
                    char *src, *des;
                    HANDLE clip;
                    CString cb = TextBuffer.Mid(a,b-a);
                    clip=GlobalAlloc(GMEM_DDESHARE,cb.GetLength()+1);
                    src = cb.LockBuffer();
                    des = (char *)GlobalLock(clip);
                    while(*src)
                        *des++=*src++;
                    *des = '\0';
                    cb.UnlockBuffer();
                    GlobalUnlock(clip);
                    EmptyClipboard();
                    SetClipboardData(CF_TEXT,clip);
                    CloseClipboard();
                }

            }
        }
    }


    if( ch==0x09 )  /* Tab */
    {   ProcessCharacter(nChar);	
    } else if( ch==0x0D )
    {   ProcessCommand(true);
    } else if( ((ch>=' ') && (ch<='~')) || (ch==0x08) || (ch==0x0A) )
    {   if( CommandActive == false )    
            return;
        ProcessCharacter(ch);
        ProcessCharacter(0);    // Flush
    }
}


void CRasMolEdit::OnSetFocus(CWnd* pOldWnd) 
{
    static int cdone = 0;

	CEdit::OnSetFocus(pOldWnd);
    int c = GetLineCount();
	LineScroll(c,0);
    if( !cdone )
        CreateSolidCaret(5,CharHigh-3);

    SetCaretToEnd();	
    ShowCaret();
    UpdateWindow( );
}


/* Where the LineScroll function finally works! */
LRESULT CRasMolEdit::OnSetText(WPARAM wParam, LPARAM lParam)
{   GETFRAME
    paint = 0;
    LRESULT l = Default();
    int c = GetLineCount();
	LineScroll(c,0);
    SetCaretToEnd();	
    ShowCaret();
    paint = 1;
    UpdateWindow( );
    return l ;
} 

void CRasMolEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{   // Stop keyboard input to reach CEdit
}

void CRasMolEdit::OnPaint() 
{   if( paint )
        CEdit::OnPaint();
}

BOOL CRasMolEdit::OnEraseBkgnd(CDC* pDC) 
{   if( !paint ) 
        return true;
	return CEdit::OnEraseBkgnd(pDC);
}
