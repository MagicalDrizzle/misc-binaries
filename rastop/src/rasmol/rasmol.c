/***************************************************************************
 *                            RasMol 2.7.2.1.1                             *
 *                                                                         *
 *                                 RasMol                                  *
 *                 Molecular Graphics Visualisation Tool                   *
 *                            26 January 2004                              *
 *                                                                         *
 *                   Based on RasMol 2.6 by Roger Sayle                    *
 * Biomolecular Structures Group, Glaxo Wellcome Research & Development,   *
 *                      Stevenage, Hertfordshire, UK                       *
 *         Version 2.6, August 1995, Version 2.6.4, December 1998          *
 *                   Copyright (C) Roger Sayle 1992-1999                   *
 *                                                                         *
 *                          and Based on Mods by                           *
 *Author             Version, Date             Copyright                   *
 *Arne Mueller       RasMol 2.6x1   May 98     (C) Arne Mueller 1998       *
 *Gary Grossman and  RasMol 2.5-ucb Nov 95     (C) UC Regents/ModularCHEM  *
 *Marco Molinaro     RasMol 2.6-ucb Nov 96         Consortium 1995, 1996   *
 *                                                                         *
 *Philippe Valadon   RasTop 1.3     Aug 00     (C) Philippe Valadon 2000   *
 *                                                                         *
 *Herbert J.         RasMol 2.7.0   Mar 99     (C) Herbert J. Bernstein    * 
 *Bernstein          RasMol 2.7.1   Jun 99         1998-2001               *
 *                   RasMol 2.7.1.1 Jan 01                                 *
 *                   RasMol 2.7.2   Aug 00                                 *
 *                   RasMol 2.7.2.1 Apr 01                                 *
 *                   RasMol 2.7.2.1.1 Jan 04                               *
 *                                                                         *
 *                    and Incorporating Translations by                    *
 *  Author                               Item                      Language*
 *  Isabel Serván Martínez,                                                *
 *  José Miguel Fernández Fernández      2.6   Manual              Spanish *
 *  José Miguel Fernández Fernández      2.7.1 Manual              Spanish *
 *  Fernando Gabriel Ranea               2.7.1 menus and messages  Spanish *
 *  Jean-Pierre Demailly                 2.7.1 menus and messages  French  *
 *  Giuseppe Martini, Giovanni Paolella, 2.7.1 menus and messages          *
 *  A. Davassi, M. Masullo, C. Liotto    2.7.1 help file           Italian *
 *                                                                         *
 *                             This Release by                             *
 * Herbert J. Bernstein, Bernstein + Sons, P.O. Box 177, Bellport, NY, USA *
 *                       yaya@bernstein-plus-sons.com                      *
 *               Copyright(C) Herbert J. Bernstein 1998-2001               *
 *                                                                         *
 * Please read the file NOTICE for important notices which apply to this   *
 * package. If you are not going to make changes to RasMol, you are not    *
 * only permitted to freely make copies and distribute them, you are       *
 * encouraged to do so, provided you do the following:                     *
 *   * 1. Either include the complete documentation, especially the file   *
 *     NOTICE, with what you distribute or provide a clear indication      *
 *     where people can get a copy of the documentation; and               *
 *   * 2. Please give credit where credit is due citing the version and    *
 *     original authors properly; and                                      *
 *   * 3. Please do not give anyone the impression that the original       *
 *     authors are providing a warranty of any kind.                       *
 *                                                                         *
 * If you would like to use major pieces of RasMol in some other program,  *
 * make modifications to RasMol, or in some other way make what a lawyer   *
 * would call a "derived work", you are not only permitted to do so, you   *
 * are encouraged to do so. In addition to the things we discussed above,  *
 * please do the following:                                                *
 *   * 4. Please explain in your documentation how what you did differs    *
 *     from this version of RasMol; and                                    *
 *   * 5. Please make your modified source code available.                 *
 *                                                                         *
 * This version of RasMol is not in the public domain, but it is given     *
 * freely to the community in the hopes of advancing science. If you make  *
 * changes, please make them in a responsible manner, and please offer us  *
 * the opportunity to include those changes in future versions of RasMol.  *
 ***************************************************************************/

/* rasmol.c
 */

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
#include <shellapi.h>
#include <malloc.h>
#endif
#ifdef APPLEMAC
#include <Types.h>
#include <Errors.h>
#ifdef __CONDITIONALMACROS__
#include <Printing.h>
#else
#include <PrintTraps.h>
#endif
#endif
#ifndef sun386
#include <stdlib.h>
#endif

#include <string.h>
#if defined(IBMPC) || defined(VMS) || defined(APPLEMAC)
#include "string_case.h"
#else
#include <strings.h>
#endif
#include <ctype.h>
#include <stdio.h>

#if !defined(IBMPC) && !defined(VMS) && !defined(APPLEMAC)
#include <pwd.h>
#endif




#define RASMOL
#include "graphics.h"
#include "molecule.h"
#include "infile.h"
#include "abstree.h"
#include "transfor.h"
#include "cmndline.h"
#include "command.h"
#include "render.h"
#include "repres.h"
#include "pixutils.h"
#include "outfile.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "prefer.h"
#include "langsel.h"
#include "help.h"




static void GetRasMolPath( void )
{  
    register int i,d;
 
#ifdef MSWIN
    LPTSTR* name = NULL;
#endif

    *RasMolPath = '\0';

#ifdef MSWIN
    SearchPath(NULL,"RasTop.exe","exe",1023,RasMolPath,name);
#endif

    i = d = 0;
    while( RasMolPath[i] )
        if( RasMolPath[i++]==DirChar )
            d = i;
    RasMolPath[d] = '\0';

}

static void WriteCredits( void )
{
    register FILE *fp;
    char * dst;
    char creditsdir[1024];
    char buffer[82];

    if( *RasMolPath )
    {   dst = strcpy(creditsdir,RasMolPath);
        while( *dst++ ) {};
        dst--;
        dst = strcpy(dst,"credits.txt");

        fp=fopen(creditsdir,"rb");
        if( !fp )
            return;
        while( xfgets(buffer,80,fp) )
            WriteString(buffer);
        fclose(fp);
    }

}


int  InitialiseRasmol( void ) 
{   static char VersionStr[255];
	
	Interactive = TRUE;
    CalcBondsFlag = TRUE;
    ReDrawFlag = RFInitial;
    CommandActive = FALSE;
    AllowWrite = FALSE;
    FileDepth = -1;
    SeqFormat = TRUE;
    if( !OpenDisplay(NULL,0) )
		return(FALSE);

    /* Full GUI */
    WindowMain = TRUE;
    WindowSelection = TRUE;
    WindowScripting = TRUE;
    WindowCommandPanel = TRUE;
    WindowCommandLine = TRUE;
    WindowColour = TRUE;
    WindowStatus = TRUE;
    /* WindowMaximized
     * = 1 when maximized
     * = -1 when minimized (restore default)
     * = 0 otherwise (restore old values)
     */
    WindowMaximized = -1;
    WindowLeft = 100;
    WindowTop = 100;
    WindowWidth = 500;
    WindowHeight = 500;

    CommandLeft = 150;
    CommandTop = 250;
    CommandWidth = 500;
    CommandHeight = 250;

    ColourLeft = 550;
    ColourTop = 150;

    GetRasMolPath();
    WriteCredits();

    InitialiseCmndLine();
    InitialiseCommand(); 
    InitialiseTransform();
    InitialiseDatabase();
	InitialiseRenderer();
    InitialisePixUtils();
    InitialiseAbstree();
    InitialiseOutFile();
    InitialiseRepres();
    InitialiseMultiple();
    InitialiseBRotate();
    InitialiseLangSel();
    InitialisePrefs();
    InitializeHelp();

	ReadPrefs();

    ShowMain(WindowMain);
    ShowSelection(WindowSelection);
    ShowScripting(WindowScripting);
    ShowCommandPanel(WindowCommandPanel);
    ShowCommandLine(WindowCommandLine);
    ShowColour(WindowColour);
    ShowStatus(WindowStatus);
    /* Transfered to initinstance for smooth opening
    ShowRasMol(); */

    // Make sure to reset
    PrefsModified = FALSE;

    WriteString("\n");
    ResetCommandLine(2);

    return( TRUE );
}


void ExitRasmol( void )
{
    WritePrefs();
#ifdef EIGHTBIT			
	if(Palette)		free(Palette);
#endif
	if(BitInfo)		free(BitInfo);
	if(CurMolecule) free(CurMolecule);
	if(CurChain)	free(CurChain);
	if(CurGroup)	free(CurGroup);
	if(CurAtom)		free(CurAtom);
	if(Array)		free(Array);
	if(LookUp)		free(LookUp);
	if(HashTable)	free(HashTable);
	if(ColConst)	free(ColConst);
	UpdatePixutils();
	UpdateRender(1);
	UpdateOutfile();
	PurgeDatabase();
    ExitLangSel();

}


void RefreshScreen()
{	
    /* Force Drawing */
    if( IsPaused )
       RefreshState = True;

    if( ReDrawFlag )
    {	
        if( ReDrawFlag & RFReSize )
        {  	ReSizeScreen();
            PrepareWorldZoom(0);
        }

		if( ReDrawFlag & RFColour )
		{	ClearImage();
			DefineColourMap();
		}

		if( Database )
		{	DrawWorld();
            if( RefreshState )
                TransferImage();
		} else if( RefreshState )
		{   ClearBuffers();		
		    TransferImage();
		}

		if( !FileModified && (ReDrawFlag & RFSave) )
		{	/*Not modified if RFInitial&~RFSize*/
			if( !((ReDrawFlag & 0xFF)==0xFF) )
			    FileModified = True;
		}
	
		FastDraw = False;
		ReDrawFlag = 0;
    }
}



