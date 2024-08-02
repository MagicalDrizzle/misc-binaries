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
// RasTop.cpp
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RasTop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRasTopApp

BEGIN_MESSAGE_MAP(CRasTopApp, CWinApp)
	//{{AFX_MSG_MAP(CRasTopApp)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpenRasDoc)
	ON_COMMAND(IDD_ABOUTRASTOP, OnAboutrastop)
	ON_COMMAND(ID_FILE_NEW, OnFileNewRasDoc)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CRasTopApp::CRasTopApp()	{}

CRasTopApp theApp;

BOOL CRasTopApp::InitInstance()
{
	Enable3dControls();
	AfxInitRichEdit() ;

    /* Load MRU List */
	LoadStdProfileSettings(4);

	/* Hack to stop display of yellow light status */
    FileDepth = -1;

	CMultiDocTemplate* pRasTemplate;
	pRasTemplate = new CMultiDocTemplate(
		IDR_RASTYPE,
		RUNTIME_CLASS(CRasDoc),
		RUNTIME_CLASS(CRasFrame), 
		RUNTIME_CLASS(CRasView));
	AddDocTemplate( pRasTemplate );

	CMainFrame* pMainFrame = new CMainFrame;
	if ( !pMainFrame->LoadFrame(IDR_MAINFRAME) )
		return false;
	m_pMainWnd = pMainFrame;

	pMainFrame->EnableToolTips( true );

	/* Enable drag/drop open */
	m_pMainWnd->DragAcceptFiles();

	/* Enable DDE Execute open */
	EnableShellOpen();
	RegisterShellFileTypes( true );


    /* RasMol Inititalization */
	if( !InitialiseRasmol() )
	{	TRACE0("TRACE\tFailed to initialize Rasmol\n");
		return false; 
	}

	/* Parse command line for standard shell commands, DDE, file open */
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if( !ProcessShellCommand(cmdInfo) )
		return false;

	CRasView* pView = pMainFrame->GetActiveRasView();
	if( pView )
    {	CMDIChildWnd *pParent = (CMDIChildWnd *) pView->GetParent( );
        pParent->ShowWindow(SW_SHOWMAXIMIZED);
        pView->RefreshView();
    }

	/* If preferences wrongly set */
	if( MouseMode != MMRasMol )
	{   SetMouseMode( MMRasMol );
        PrefsModified = true;
	}

    /* See InitialiseRasmol() */
    ShowRasMol();

	return true;
}

CDocument* CRasTopApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{	CString temp = lpszFileName;
	CRasDoc* pDoc = CRasDoc::OpenRSMFile(temp);
	return(pDoc);
}

void CRasTopApp::OnFileOpenRasDoc() 
{	//CWinApp::OnFileOpen();
    
    OnFileNewRasDoc() ;
	GETFRAME
	CRasFrame *pChild = (CRasFrame *) pFrame->GetActiveFrame();
	if(pChild)
	{	CRasDoc *pDoc = (CRasDoc *)pChild->GetActiveDocument( );
		if( pDoc )
		{	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CRasDoc)));
			
	        char BASED_CODE Filter[] = 
		    "Scripts or Files (*.rsm;*.pdb;*.cif;*.scr;*.top)|*.rsm; *.pdb; *.cif; *.scr; *.top|All Files (*.*)|*.*||";
 
	        CFileDialog fdlg(true, ".rsm", NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | 
		    OFN_OVERWRITEPROMPT , Filter, NULL);

	        if( fdlg.DoModal()==IDOK )
		        pDoc->OpenRSMFile(fdlg.GetPathName()); 
		}
	}

}

void CRasTopApp::OnFileNewRasDoc() 
{	// Save the active document data before opening a new one
    int wasmax = 1;
	GETFRAME
	CRasFrame *pChild = (CRasFrame *) pFrame->GetActiveFrame();
	if(pChild)
	{	CRasDoc *pDoc = (CRasDoc *)pChild->GetActiveDocument( );
		if( pDoc )
		{	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(CRasDoc)));
            GETVIEW
	        if( pView )
            {	CMDIChildWnd *pParent = (CMDIChildWnd *) pView->GetParent( );
                WINDOWPLACEMENT wpl;
                pParent->GetWindowPlacement(&wpl);
                if( wpl.showCmd != SW_SHOWMAXIMIZED )
                    wasmax = 0;
            }
			pDoc->ManageDisplay(false);

		}
	}
	CWinApp::OnFileNew();
    if(wasmax) {
        GETVIEW
	    if( pView )
        {	CMDIChildWnd *pParent = (CMDIChildWnd *) pView->GetParent( );
            pParent->ShowWindow(SW_SHOWMAXIMIZED);
            pView->RefreshView();
        }
    }
}

void CRasTopApp::OnAboutrastop() 
{	CAboutRasTop aboutDlg;
	aboutDlg.DoModal();	
}

int CRasTopApp::ExitInstance() 
{   ExitRasmol();
	return CWinApp::ExitInstance();
}


BOOL CRasTopApp::OnIdle(LONG lCount) 
{   
    if( !CommandActive )
        ResetCommandLine(0);
	return CWinApp::OnIdle(lCount);
}


/////////////////////////////////////////////////////////////////////////////
// CAboutRasTop dialog

CAboutRasTop::CAboutRasTop(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutRasTop::IDD, pParent)
{	//{{AFX_DATA_INIT(CAboutRasTop)
	//}}AFX_DATA_INIT
}

void CAboutRasTop::DoDataExchange(CDataExchange* pDX)
{	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutRasTop)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutRasTop, CDialog)
	//{{AFX_MSG_MAP(CAboutRasTop)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutRasTop::OnInitDialog() 
{
	CDialog::OnInitDialog();
	char Text[256];

#ifdef _WIN32
    auto OSVERSIONINFO osinfo;
    auto SYSTEM_INFO sysinfo;
    register char *winver;
    register char *cpu;
    register int count;

    osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#ifdef XPROCARCH
    GetVersionEx(&osinfo);
    if( osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {   winver = "Windows NT";
    } else if( osinfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS )
    {   winver = "Windows95";
    } else if( osinfo.dwPlatformId == VER_PLATFORM_WIN32s )
    {   winver = "Win32s";
    } else winver = "Win32";
#else
    winver = "Windows";
#endif

#ifdef XPROCARCH
    if( osinfo.dwPlatformId == VER_PLATFORM_WIN32_NT )
    {   GetSystemInfo(&sysinfo);
        switch( sysinfo.wProcessorArchitecture )
        {   case PROCESSOR_ARCHITECTURE_INTEL:
                switch( sysinfo.wProcessorLevel )
                {   case 3:  cpu = "Intel 386";     break;
                    case 4:  cpu = "Intel 486";     break;
                    case 5:  cpu = "Intel Pentium"; break;
                    default: cpu = "Unknown Intel"; break;
                }
                break;

            case PROCESSOR_ARCHITECTURE_MIPS:
                switch( sysinfo.wProcessorLevel )
                {   case  1: cpu = "MIPS R2000";    break;
                    case  2: cpu = "MIPS R3000";    break;
                    case  3: cpu = "MIPS R6000";    break;
                    case  4: cpu = "MIPS R4000";    break;
                    case  6: cpu = "MIPS R6000A";   break;
                    case  9: cpu = "MIPS R10000";   break;
                    case 10: cpu = "MIPS R4200";    break;
                    case 11: cpu = "MIPS R4300";    break;
                    case 16: cpu = "MIPS R8000";    break;
                    case 32: cpu = "MIPS R4600";    break;
                    case 33: cpu = "MIPS R4700";    break;
                    case 34: cpu = "MIPS R4650";    break;
                    case 35: cpu = "MIPS R5000";    break;
                    default: cpu = "Unknown MIPS";  break;
                }
                break;

            case PROCESSOR_ARCHITECTURE_ALPHA:
                switch( sysinfo.wProcessorLevel )
                {   case 21064:  cpu = "Alpha 21064";  break;
                    case 21066:  cpu = "Alpha 21066";  break;
                    case 21164:  cpu = "Alpha 21164";  break;
                    default: cpu = "Unknown Alpha";    break;
                }
                break;

            case PROCESSOR_ARCHITECTURE_PPC:
                switch( sysinfo.wProcessorLevel )
                {   case 1:  cpu = "PPC 601";     break;
                    case 3:  cpu = "PPC 603";     break;
                    case 4:  cpu = "PPC 604";     break;
                    case 6:  cpu = "PPC 603+";    break;
                    case 9:  cpu = "PPC 604+";    break;
                    case 20: cpu = "PPC 620";     break;
                    default: cpu = "Unknown PPC"; break;
                }
                break;

            default:
                cpu = "unrecognised";
                break;
        }
    } else /* Windows 95 or Windows98 */
    {  
#endif 
        GetSystemInfo(&sysinfo);
        switch( sysinfo.dwProcessorType )
        {   case (386):   cpu = "Intel 386";       break;
            case (486):   cpu = "Intel 486";       break;
            case (586):   cpu = "Intel Pentium";   break;
            case (860):   cpu = "Intel i860";      break;
            case (2000):  cpu = "MIPS R2000";      break;
            case (3000):  cpu = "MIPS R3000";      break;
            case (4000):  cpu = "MIPS R4000";      break;
            case (4400):  cpu = "MIPS R4400";      break;
            case (4600):  cpu = "MIPS R4600";      break;
            case (5000):  cpu = "MIPS R5000";      break;
            case (8000):  cpu = "MIPS R8000";      break;
            case (10000): cpu = "MIPS R10000";     break;
            case (21064): cpu = "DEC Alpha 21064"; break;
            case (21066): cpu = "DEC Alpha 21066"; break;
            case (21164): cpu = "DEC Alpha 21164"; break;
            default:     cpu = "unrecognised";
        }
#ifdef XPROCARCH
    }
#endif

    count = sysinfo.dwNumberOfProcessors;
    if( count > 1 )
    {   sprintf(Text,"%s on %d %s CPUs\n",winver,count,cpu);
    } else sprintf(Text,"%s on a single %s CPU",winver,cpu);
#else
    register DWORD flags;
    register char *cpu;

    flags = GetWinFlags();
    if( flags & WF_CPU286 )
    {      cpu = "286";
    } else if( flags & WF_CPU386 )
    {      cpu = "386";
    } else cpu = "486";
                              
    if( !(flags&WF_80x87) )
    {   sprintf(Text,"%s without maths coprocessor",cpu);
    } else sprintf(Text,"%s with maths coprocessor",cpu); 
#endif

	this->SetDlgItemText(IDD_HARDWARE,Text);         
	return ( true );
}


/////////////////////////////////////////////////////////////////////////////
// Globals

void RasMolExit()
{	PostQuitMessage(0);
    // PV: Tell me about a more decent way!
    exit(0);
}

void RasMolFatalExit(char *msg)
{	::MessageBox(NULL, msg,"RasMol Fatal Error!",
	MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL );
	CloseDisplay();
    exit(1);    
}

void AdviseUpdate( int )	{}

