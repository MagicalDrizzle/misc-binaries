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



/////////////////////////////////////////////////////////////
//rasgraphics.cpp
/////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "rastop.h"
#include "rasdoc.h"
#include "rasview.h"
#include "mainfrm.h"

#define GRAPHICS
#include "../rasmol/graphics.h"

HCURSOR CurrentCursor;
static int WaitStatus = 0;

void AllocateColourMap()
{
#ifdef EIGHTBIT			
    register COLORREF ref;      
    register int i;
	static int ColCount;
    
    if( ColourMap )
        ::DeleteObject(ColourMap);

    ColCount = 0;
    for( i=0; i<256; i++ )
        if( ULut[i] ) 
        {  Palette->palPalEntry[ColCount].peFlags = 0;
           Palette->palPalEntry[ColCount].peRed   = RLut[i];
           Palette->palPalEntry[ColCount].peGreen = GLut[i];
           Palette->palPalEntry[ColCount].peBlue  = BLut[i];

           BitInfo->bmiColors[ColCount].rgbBlue     = BLut[i];
           BitInfo->bmiColors[ColCount].rgbGreen    = GLut[i];
           BitInfo->bmiColors[ColCount].rgbRed      = RLut[i];
           BitInfo->bmiColors[ColCount].rgbReserved = 0;
           ColCount++;
        }   
    Palette->palNumEntries = ColCount;
    BitInfo->bmiHeader.biClrUsed = ColCount;   
    ColourMap = ::CreatePalette(Palette);

    for( i=0; i<256; i++ )
       if( ULut[i] )
       {   ref = RGB(RLut[i],GLut[i],BLut[i]);
		   Lut[i] = ::GetNearestPaletteIndex(ColourMap,ref);
       }
#endif
}

int CreateImage()
{
    register Long size;

    if( FBufHandle ) GlobalFree(FBufHandle);
    size = (Long)XRange*YRange*sizeof(Pixel)+16;
    FBufHandle = GlobalAlloc(GMEM_MOVEABLE,size);
    return( (int)FBufHandle );
}

void TransferImage()
{
#ifdef EIGHTBIT			
    HPALETTE OldCMap;
#endif
    HDC hDC;
        
    if( PixMap )
        ::DeleteObject(PixMap);

    BitInfo->bmiHeader.biWidth = XRange;
    BitInfo->bmiHeader.biHeight = YRange;
        
    hDC = ::GetDC(NULL);
    FBuffer = (Pixel  __huge*)GlobalLock(FBufHandle);

#ifdef EIGHTBIT			
    if( ColourMap )
    {   OldCMap = ::SelectPalette(hDC,ColourMap,FALSE);
		::RealizePalette(hDC);  /* GDI Bug?? */
    }
#endif
        
    PixMap = CreateDIBitmap( hDC, (BITMAPINFOHEADER __far *)BitInfo, 
                             CBM_INIT, FBuffer, BitInfo, DIB_RGB_COLORS);
        
#ifdef EIGHTBIT			
	if( ColourMap && OldCMap )                         
        ::SelectPalette(hDC,OldCMap,False);
#endif

    GlobalUnlock(FBufHandle);
    ::ReleaseDC(NULL,hDC);
}

void ClearImage()
{	if( PixMap && RefreshState )
    {   ::DeleteObject(PixMap);
        PixMap = NULL;
    }
}

void UpdateScrollBars()
{	GETFRAME
	if( UseSlabPlane || UseDepthPlane )
		pFrame->m_wndCommandBar.UpdatePannel();
	else
		pFrame->m_wndCommandBar.UpdateScrollBars();
}

void SetMouseUpdateStatus( int status )
{	MouseUpdateStatus = status;	}
                                             
void SetMouseCaptureStatus( int status )
{	if( status )
    {   if( !MouseCaptureStatus )
		{	GETFRAME
			GETVIEW
			if( pView != NULL )
				pView->SetCapture();
		}
    } else
        if( MouseCaptureStatus )
            ::ReleaseCapture();
    MouseCaptureStatus = status;
}

int LookUpColour(char * name, int *r, int *g, int *b )
{   UnusedArgument(name);
    UnusedArgument(r);
    UnusedArgument(g);
    UnusedArgument(b);

    return False;
}    

void EnableMenus( int flag )
{	GETFRAME
	pFrame->EnableMenus( flag );
}

int OpenDisplay( HANDLE, int )
{	register int i,size;

    PixMap = NULL;
#ifdef EIGHTBIT			
    ColourMap = NULL;
#endif
    MouseCaptureStatus = False;
    MouseUpdateStatus = False;   
    UseHourGlass = True;
    DisableMenu = False;
	for( i=0; i<9; i++ )
		DialValue[i] = 0.0;

    ULut[0] = True;
    RLut[0] = GLut[0] = BLut[0] = 0;

	ClipBoardFactor = 2;
    ClipBoardSize = 0;

#ifdef EIGHTBIT			
     size = sizeof(LOGPALETTE) + 256*sizeof(PALETTEENTRY);
     Palette = (LOGPALETTE __far*)_fmalloc( size );
#else
    size = sizeof(BITMAPINFOHEADER);
#endif
    BitInfo = (BITMAPINFO __far*)_fmalloc( size );


#ifdef EIGHTBIT			
     if( !Palette || !BitInfo )
#endif
	if( !BitInfo )
        return( False );
        
#ifdef EIGHTBIT			
    Palette->palVersion = 0x300;
#endif   
    
    BitInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    BitInfo->bmiHeader.biCompression = BI_RGB;
    BitInfo->bmiHeader.biXPelsPerMeter = 0;
    BitInfo->bmiHeader.biYPelsPerMeter = 0;
    BitInfo->bmiHeader.biClrImportant = 0;
    BitInfo->bmiHeader.biSizeImage = 0;
#ifdef EIGHTBIT			
    BitInfo->bmiHeader.biBitCount = 8;
#else
    BitInfo->bmiHeader.biBitCount = 32;
#endif
    BitInfo->bmiHeader.biPlanes = 1;
#ifdef THIRTYTWOBIT			
    BitInfo->bmiHeader.biClrUsed = 0;
#endif
    /* Initialise Palette! */
    for( i=1; i<LutSize; i++ )
        ULut[i] = False;
    AllocateColourMap();

    return(true);                       
}

void CloseDisplay()
{	
#ifdef EIGHTBIT			
	if( ColourMap )
		::DeleteObject(ColourMap);
#endif
    if( PixMap )
        ::DeleteObject(PixMap);
}

void SetCanvasTitle( void )
{	GETFRAME
	GETVIEW
	if( pView==NULL )
		return;
	CRasDoc* pDoc = (CRasDoc *)pView->GetDocument( );
	if( pDoc==NULL )
		return;

	char buffer[80];
	char *src,*des;
	int i;

	des = buffer;
	i = 0;

	if( *SpaceName!='\0' )
	{	src = SpaceName;
		while( i<40 && *src)
		{	*des++ = *src++;
			i++;
		}
		if( i==40 )
			*(des-1) = '\46';

		if( pDoc->m_DocCopyNum > 1 )
		{	int temp = (int)des;
			des += sprintf(des," #%d", pDoc->m_DocCopyNum );
			i += (int)des - temp;
		}
	}

	if( strcmp(CurMolSet->title,SpaceName) || CurMolSet->copynum>1 )
	{	if( i )
		{	des += sprintf(des," - ");
			i +=3;
		}
		src = CurMolSet->title;
		while( i<76 && *src)
		{	*des++ = *src++;
			i++;
		}

		if( CurMolSet->copynum>1 )
			sprintf(des," #%d", CurMolSet->copynum );
		else
			*des = '\0';
	} else
		*des = '\0';

	buffer[80] = '\0';

    pDoc->SetTitle( buffer );
}

int  ClipboardImage( )
{	
    register BITMAPINFO __far *bitmap;
    register Pixel __huge *src;
    register Pixel __huge *dst;
    register long size,len;
    register HANDLE hand;
    register int cbfactor;
#ifdef EIGHTBIT			
    register int i;
#endif

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return false;
	CRasDoc* pDoc = pView->GetDocument();
	pDoc->FixRange();

    /* Adjust ClipBoardFactor */
    cbfactor = pDoc->GetMaxCBFactor( );
    if( !cbfactor )
        return False;

    if( pView->OpenClipboard() )
    {	EmptyClipboard();
        if( PixMap )
        {	BeginWait();
			if( cbfactor != 1 )
			{	XRange = XRange*cbfactor;
				if( int dx = XRange%4 )
					XRange += 4-dx;	
				YRange = YRange*cbfactor;
				Range =  MinFun(XRange,YRange);
				ZRange = Range;
				HRange = YRange>>1;
				WRange = XRange>>1;
                RefreshState = True;
				ReDrawFlag |= RFReSize;
				ClearImage();
				RefreshScreen();
			}
			len = (Long)XRange*YRange*sizeof(Pixel);
#ifdef EIGHTBIT			
            size = sizeof(BITMAPINFOHEADER) + 256*sizeof(RGBQUAD);
#else
            size = sizeof(BITMAPINFOHEADER);
#endif
            hand=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,size+len);
			if( hand )
            {   bitmap = (BITMAPINFO __far *)GlobalLock(hand);
                bitmap->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bitmap->bmiHeader.biWidth = XRange;
                bitmap->bmiHeader.biHeight = YRange;
                bitmap->bmiHeader.biPlanes = 1;
#ifdef EIGHTBIT			
                bitmap->bmiHeader.biBitCount = 8;
#else
                bitmap->bmiHeader.biBitCount = 32;
#endif
                bitmap->bmiHeader.biCompression = BI_RGB;
                bitmap->bmiHeader.biSizeImage = 0;
                bitmap->bmiHeader.biXPelsPerMeter = 0;
                bitmap->bmiHeader.biYPelsPerMeter = 0;
                bitmap->bmiHeader.biClrImportant = 0;
                bitmap->bmiHeader.biClrUsed = 0;

#ifdef EIGHTBIT			
                for( i=0; i<256; i++ )
                    if( ULut[i] )
                    {   bitmap->bmiColors[Lut[i]].rgbBlue  = BLut[i];
                        bitmap->bmiColors[Lut[i]].rgbGreen = GLut[i];
                        bitmap->bmiColors[Lut[i]].rgbRed   = RLut[i];
                    }
#endif
                /* Transfer the frame buffer */
                src = (Pixel __huge*)GlobalLock(FBufHandle);
#ifdef EIGHTBIT			
                dst = ((Pixel __huge*)bitmap)+size;
#else
                dst = ((Pixel __huge*)bitmap)+size/4;
#endif
				len = GlobalSize(FBufHandle)-16;
				memcpy(dst,src,(size_t)len);

                GlobalUnlock(FBufHandle);
                GlobalUnlock(hand);
                if( SetClipboardData(CF_DIB,hand) )
                    ClipBoardSize = len;
            }
			if( cbfactor != 1 )
			{	pDoc->FixRange();
				ReDrawFlag |= RFReSize;
                RefreshState = True;
				ClearImage();
				RefreshScreen();
			}

			EndWait();
        }

#ifdef EIGHTBIT			
        if( ColourMap )
        {   if( (hand = CreatePalette(Palette)) )
                SetClipboardData(CF_PALETTE,hand);
        }
#endif
        CloseClipboard();
    } else	
		return false;
return true;
}

static int ClipboardTransfer( int selection )
{	register int len, result, done;
	register int ok, size, sizebuf;
	register int i, plus;
    register HANDLE buf;
	register HANDLE clip;
	register char *src;
	register char *ptr;
    char buffer[80];

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return false;

	BeginWait();
	done = 0;
	size = 0;
	ok = 0;

	while( !ok )
	{	ok = 1;
		if( size==0 )
		{	sizebuf = 1024;
		} else if( size==1 )
		{	sizebuf = 16*1024;
		} else if( size==2 )
		{	sizebuf = 128*1024;	
		} else
		{	WriteString(MsgStrs(RTPSelBig));
			EndWait();
			return false;
		}

		buf = GlobalAlloc(GHND,sizebuf);

		if( buf )
		{	WriteAtomSelection(NULL,-1);
			src = (char *)GlobalLock(buf);	
			len = 0;
			do
			{	ptr = src+len;
				if( selection )
					result = WriteAtomSelection(ptr,1);	
				else
					result = WriteScriptPosition(ptr,1);
				len = strlen(src);
				/*Get a larger buffer*/
				if( result && len>=(sizebuf-MAXBUFFLEN) )
				{	size++;
					GlobalUnlock(buf);
					ok = 0;
					break;
				}
			} while (result);

			if( !ok )
				continue;		
		
		    if( pView->OpenClipboard() )
		    {	plus = 0;
				ptr = src;
				if( *src++=='\n' )
					plus++;
				for( i=1;i<len;i++ )
				{	if( *src=='\n'&&*(src-1)!='\r' )
						plus++;
					*src++;
				}
				src = ptr;

				if( clip=GlobalAlloc(GMEM_DDESHARE,len+plus+2) )
				{	ptr = (char *)GlobalLock(clip);
					if( *src=='\n' )
						*ptr++ = '\r';
					*ptr++ = *src++;
					for( i=1;i<len;i++ )
					{	if( *src=='\n'&&*(src-1)!='\r' )
							*ptr++ = '\r';
						*ptr++ = *src++;
					}
                    *ptr = '\0';
					GlobalUnlock(clip);

		            EmptyClipboard();
					if( SetClipboardData(CF_TEXT,clip) )
                        ClipBoardSize = len+plus+2;
					CloseClipboard();

					done = 1;
				}
			}
				
			GlobalUnlock(buf);    
			InvalidateCmndLine();
			if( done )
			{	if( selection && FileDepth==-1 )
				{	if( SelectCount==0 )
					{	WriteString(MsgStrs(RTPZeroAt));
					} else if( SelectCount>1 )
					{   sprintf(buffer,"%ld",
							(long)SelectCount);
						 WriteString(buffer);
                         WriteString(MsgStrs(RTPManyAt));
					} else WriteString(MsgStrs(RTPOneAt));
		
				} else if( FileDepth==-1 )
					WriteString(MsgStrs(RTPPosClip));
			}
		} else
		{	WriteString(MsgStrs(RTPNoMemSel));
			done = 0;
		}
	}
	EndWait();
	return done;
}

int ClipboardSelection( )
{	return ClipboardTransfer(true);	}

int ClipboardPosition( )
{	return ClipboardTransfer(false);	}

int ClipboardPaste( )
{	register char *ptr;
	register char *des;
	register int len;
	register int clipboardtype;
    char buffer[80];

	GETFRAME
	GETVIEW
	if( pView==NULL )
		return false;

    InvalidateCmndLine();
	BeginWait();
				
	if( pView->OpenClipboard() )
	{	HANDLE data = GetClipboardData(CF_TEXT);
		ptr = (char*)GlobalLock(data);
		GlobalUnlock(data);	
		if( ptr==NULL )
		{	WriteString(MsgStrs(RTPNoDataClip));
			CloseClipboard();
			EndWait();
			return true;
		}
		len = 0;
		des = CurLine;
		while( len<MAXBUFFLEN && *ptr!='\n' && *ptr )
		{	*des++ = *ptr++;
			len++;
		}
		if( len==MAXBUFFLEN || !*ptr )
		{	WriteString(MsgStrs(RTPNoComdClip));
			CloseClipboard();
			EndWait();
			return true;
		}
		*des = '\0';
		*ptr++;

		if( strstr(CurLine,"#!rasmol")==0 )
		{	if( FileDepth==-1 )
				WriteString(MsgStrs(RTPNoComdClip));
			CloseClipboard();
			EndWait();
			return true;
		}

		if( strstr(CurLine,"selection")!=0 )
		{	clipboardtype = 1;
		} else if( strstr(CurLine,"position")!=0 )
		{	clipboardtype = 2;
		} else if( strstr(CurLine, "script")!=0 )
		{	clipboardtype = 3;
		} else
		{	WriteString(MsgStrs(RTPUnkCmd));
			CloseClipboard();
			EndWait();
			return true;
		}

        /* Make clipboard script-like */
        FileDepth++;

		while(1)
		{	len = 0;
			des = CurLine;
			while( len<MAXBUFFLEN && *ptr!='\n' && *ptr )
			{	*des++ = *ptr++;
				len++;
			}
			*des++ = '\n';
			*des = '\0';
			ExecuteCommand();
			if( !*ptr )
				break;
			*ptr++;
		}

        FileDepth--;

		if( clipboardtype==1 )
		{	if( FileDepth==-1 )
			{	if( SelectCount==0 )
				{	WriteString(MsgStrs(RTPNoSelClip));
				} else if( SelectCount>1 )
				{   sprintf(buffer,"%ld",(long)SelectCount);
					WriteString(buffer);
                WriteString(MsgStrs(RTPManSelClip));
				} else WriteString(MsgStrs(RTPOneSelClip));
			}
		} else if( clipboardtype==2 )
		{	if( FileDepth==-1 )
				WriteString(MsgStrs(RTPPastePos));
		} else if( clipboardtype==3 )
		{	if( FileDepth==-1 )
				WriteString(MsgStrs(RTPScrPaste));
		}

        if( DisplayMode )
            ReDrawFlag |= RFRefresh;
		pView->RefreshView();
		CloseClipboard();
	} else
	{	EndWait();
		return false;
	}

	EndWait();
	return true;
}

int  PrintImage( )
{	GETFRAME
	GETVIEW
	if( !pView )
		return false;
	pView->PrintImage( );
	return true;
}

void BeginWait(  )
{	WaitStatus++;
	GETFRAME
	pFrame->m_wndCommandBar.ChangeStatusLight(-1); 
	//Force cursor
	HCURSOR Cursor = AfxGetApp()->LoadStandardCursor(IDC_WAIT);
	CurrentCursor = ::GetCursor();
	::SetCursor(Cursor);
	if( ::ShowCursor(true)>= 1 )
		::ShowCursor(false);
}

void EndWait(  )
{	WaitStatus--;
	if( WaitStatus<0 )
		WaitStatus = 0;
	if( WaitStatus )
		return;

	GETFRAME
	pFrame->m_wndCommandBar.ChangeStatusLight(true); 

	if( FileDepth!=-1 )
		CurrentCursor = AfxGetApp()->LoadCursor(IDC_RASCURSOR);
	::SetCursor(CurrentCursor);
	if( ::ShowCursor(true)>= 1 )
		::ShowCursor(false);
}



void ShowMain( int b )
{   GETFRAME
    pFrame->ShowControlBar(&(pFrame->m_wndMainBar), b, false);
}

void ShowSelection( int b  )
{   GETFRAME
    pFrame->ShowControlBar(&(pFrame->m_wndSelectBar), b, false);
}

void ShowScripting( int b)
{   GETFRAME
    pFrame->ShowControlBar(&(pFrame->m_wndScriptBar), b, false);
}

void ShowColour( int b  )
{   GETFRAME

	CPoint point(ColourLeft,ColourTop);
	pFrame->FloatControlBar(&(pFrame->m_wndColorBar),point,CBRS_ALIGN_TOP );
    pFrame->m_wndColorBar.BringWindowToTop( );
    pFrame->ShowControlBar(&(pFrame->m_wndColorBar), b, false);
}

void ShowCommandPanel( int b  )
{   GETFRAME
    pFrame->ShowControlBar(&(pFrame->m_wndCommandBar), b, false);
}


void CancelCmndLinePlacement() {
    CommandLeft = 100;
    CommandTop = 100;
    CommandWidth = 100;
    CommandHeight = 50;
    GETFRAME
	pFrame->m_wndCmdWindow.MoveWindow(CommandLeft,CommandTop,CommandWidth,CommandHeight,TRUE );
    pFrame->m_wndCmdWindow.ShowWindow(SW_SHOWNORMAL);
}

void ShowCommandLine( int b  )
{   GETFRAME
    if( b )
    {   /* Check correcftnes of placement */
        if ( (CommandWidth + CommandLeft)<30 ) {
            CancelCmndLinePlacement();
            return;
        }
        if ( CommandTop<10 ) {
            CancelCmndLinePlacement();
            return;
        }
        if ( (CommandLeft)>(::GetSystemMetrics(SM_CXMAXIMIZED)-20) ) {
            CancelCmndLinePlacement();
            return;
        }
        if ( (CommandTop)>(::GetSystemMetrics(SM_CYMAXIMIZED)-20) ) {
            CancelCmndLinePlacement();
            return;
        }


        RECT rec;
        rec.left = CommandLeft; 
        rec.top = CommandTop; 
        rec.right = CommandWidth + CommandLeft; 
        rec.bottom = CommandHeight + CommandTop; 

        POINT pt;
        pt.x = 0;
        pt.y = 0;

        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        wp.flags = 0;
        wp.showCmd = SW_SHOWNORMAL;
        wp.ptMinPosition = pt;
        wp.ptMaxPosition = pt;
        wp.rcNormalPosition = rec;

        pFrame->m_wndCmdWindow.SetWindowPlacement(&wp);


	} else 
		pFrame->m_wndCmdWindow.ShowWindow(SW_HIDE);
}

void ShowStatus( int b  )
{   GETFRAME
    pFrame->ShowControlBar(&(pFrame->m_wndStatusBar), b, false);
}

void RecordRasMolWindow( void )
{   
    WINDOWPLACEMENT wpl;
    GETFRAME
    pFrame->GetWindowPlacement(&wpl);

    if( wpl.showCmd == SW_SHOWMAXIMIZED )
    {   WindowMaximized = 1;
        return;
    } else if ( wpl.showCmd == SW_SHOWMINIMIZED )
    {   WindowMaximized = -1;
        return;
    }

    WindowLeft = wpl.rcNormalPosition.left;
    WindowTop = wpl.rcNormalPosition.top;
    WindowWidth = wpl.rcNormalPosition.right - WindowLeft;
    WindowHeight = wpl.rcNormalPosition.bottom - WindowTop;
    WindowMaximized = 0;
}

void RecordCommandWindow( void )
{   
    WINDOWPLACEMENT wpl;
    GETFRAME
	
	if( &(pFrame->m_wndCmdWindow) == 0 )
		return;

	pFrame->m_wndCmdWindow.GetWindowPlacement(&wpl);

    if( wpl.showCmd == SW_SHOWMAXIMIZED )
    {   return;
    } else if ( wpl.showCmd == SW_SHOWMINIMIZED )
    {   return;
    }

    CommandLeft = wpl.rcNormalPosition.left;
    CommandTop = wpl.rcNormalPosition.top;
    CommandWidth = wpl.rcNormalPosition.right - CommandLeft;
    CommandHeight = wpl.rcNormalPosition.bottom - CommandTop;

}

void RecordColorWindow( void )
{
    GETFRAME
	
	if( &(pFrame->m_wndColorBar) == 0 )
		return;  
    
    RECT Rect;
    int done = False;
    /* Get the miniframe around the color toolbar */
    CWnd* pw = pFrame->m_wndColorBar.GetParent();
    CWnd* pw2;
    if (pw) {
        pw2 = pw->GetParent();
            if (pw2) {
                pw2->GetWindowRect(&Rect);
                done = True;
            }
    }
    if (done) {
        if ( ColourLeft!=Rect.left || ColourTop!=Rect.top) {
            ColourLeft = Rect.left;
            ColourTop = Rect.top;
            PrefsModified = True;
        }
    }
}

void CancelPlacement() {
    WindowLeft = 100;
    WindowTop = 100;
    WindowWidth = 100;
    WindowHeight = 100;
    WindowMaximized = -1;
    ShowRasMol();
}

void ShowRasMol( void )
{   GETFRAME

    if( WindowMaximized == 1 )
    {   pFrame->ShowWindow(SW_SHOWMAXIMIZED);
    } else if( WindowMaximized == -1 )
    {   int x = ::GetSystemMetrics(SM_CXMAXIMIZED);
	    int y = ::GetSystemMetrics(SM_CYMAXIMIZED);
        int a = x/7;
        int b = y/7;
	    x = 2*x/3;
	    y = 2*y/3;
		pFrame->MoveWindow( a, b, x, y, TRUE );
        pFrame->ShowWindow(SW_SHOWNORMAL);
        /* Reset window size values */
        WindowLeft = a;
        WindowTop = b;
        WindowWidth = x;
        WindowHeight = y;

    } else
    {   /* Check correcftnes of placement */
        if ( (WindowWidth + WindowLeft)<30 ) {
            CancelPlacement();
            return;
        }
        if ( WindowTop<10 ) {
            CancelPlacement();
            return;
        }
        if ( (WindowLeft)>(::GetSystemMetrics(SM_CXMAXIMIZED)-20) ) {
            CancelPlacement();
            return;
        }
        if ( (WindowTop)>(::GetSystemMetrics(SM_CYMAXIMIZED)-20) ) {
            CancelPlacement();
            return;
        }


        RECT rec;
        rec.left = WindowLeft; 
        rec.top = WindowTop; 
        rec.right = WindowWidth + WindowLeft; 
        rec.bottom = WindowHeight + WindowTop; 

        POINT pt;
        pt.x = 0;
        pt.y = 0;

        WINDOWPLACEMENT wp;
        wp.length = sizeof(WINDOWPLACEMENT);
        wp.flags = 0;
        wp.showCmd = SW_SHOWNORMAL;
        wp.ptMinPosition = pt;
        wp.ptMaxPosition = pt;
        wp.rcNormalPosition = rec;

        pFrame->SetWindowPlacement(&wp);
    }
	pFrame->UpdateWindow();


}


