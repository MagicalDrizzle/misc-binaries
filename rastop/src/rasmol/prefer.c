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

/* Portion copyright (c) 2007, Philippe Valadon */

/* prefer.c
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

#define PREFER
#include "prefer.h"
#include "command.h"
#include "tokens.h"
#include "molecule.h"
/* #include "infile.h"   */
/* #include "abstree.h"  */
/* #include "transfor.h" */
#include "cmndline.h"
#include "render.h"
/* #include "pixutils.h" */
/* #include "repres.h"   */
#include "graphics.h"
/* #include "outfile.h"  */
/* #include "script.h"   */
#include "multiple.h"
/* #include "vector.h"   */
/* #include "brotate.h"  */
#include "langsel.h"
#include "help.h"

#include <direct.h> 

#define IsIdentChar(x)  ((isalnum(x))||((x)=='_')||((x)=='$'))


#define MAXPREFLEN      1024

#define MAXERROR        20

char CurPrefLine[MAXPREFLEN];

static int PrefTokenLength;
static Long PrefTokenValue;
static char PrefTokenIdent[128];
static char *PrefTokenStart;
static char *PrefTokenPtr;
static int CurPrefToken;


/*===============================*/
/*  Prefs Line Lexical Analysis  */
/*===============================*/

static int FetchPrefToken( void )
{
    register char ch;

    CurPrefToken = 0;
    ch = *PrefTokenPtr++;
    while( ch && (ch!='#') )
    {   if( isspace(ch) )
        {   ch = *PrefTokenPtr++;
            continue;
        }

        PrefTokenStart = PrefTokenPtr-1;
        if( isalpha(ch) )
         {   PrefTokenLength = 1;
             *PrefTokenIdent = ToUpper(ch);
             while( IsIdentChar(*PrefTokenPtr) && (PrefTokenLength<32) )
             {   ch = *PrefTokenPtr++;
                 PrefTokenIdent[PrefTokenLength++] = ToUpper(ch);
             }
             if( PrefTokenLength==32 )
             {   return(0);
             } else PrefTokenIdent[PrefTokenLength] = '\0';
             return( CurPrefToken = LookUpKeyword(PrefTokenIdent) );

         } else if( isdigit(ch) )
         {   PrefTokenValue = ch-'0';
             while( isdigit(*PrefTokenPtr) )
                 PrefTokenValue = 10*PrefTokenValue + (*PrefTokenPtr++)-'0';
             return( CurPrefToken = NumberTok );

         } else if( (ch=='\'') || (ch=='\"') || (ch=='`') )
         {   PrefTokenLength = 0;
             while( *PrefTokenPtr && (PrefTokenLength<128) && (*PrefTokenPtr!=ch) )
                 PrefTokenIdent[PrefTokenLength++] = *PrefTokenPtr++;

             if( ch != *PrefTokenPtr )
             {   return( 0 );
             } else PrefTokenPtr++;

             PrefTokenIdent[PrefTokenLength]='\0';
             return( CurPrefToken = StringTok );
         } else if( ispunct(ch) )
             return( CurPrefToken = ch );

         ch = *PrefTokenPtr++;
    }
    PrefTokenPtr--;
    return 0;
}


static int NextIf( int tok, int err )
{
    if( FetchPrefToken() != tok )
        return True;
    else return False;
}


static void FetchFloat( Long value, int scale )
{
    register int count;
    register int mant;

    if( !value && !isdigit(*PrefTokenPtr) )
    {   PrefTokenValue = 0;
        return;
    }

    mant = 0;
    count = 1;
    while( isdigit(*PrefTokenPtr) )
    {   if( count < scale )
        {   mant = 10*mant + (*PrefTokenPtr-'0');
            count *= 10;
        }
        PrefTokenPtr++;
    }

    mant = (scale*mant)/count;
    PrefTokenValue = value*scale + mant;
}

#define MAXCOL 34
#define IRGB(r, g ,b)  ( r + 256*g + 256*256*b)

typedef struct {
        Byte red;
        Byte grn;
        Byte blu;
        char *col;
    } RGBStruct;

static RGBStruct ColTable[MAXCOL] = {
    {   0,   0,   0 , "Black"      },
    {   0,   0, 255 , "Blue"       },
    { 175, 214, 255 , "BlueTint"   },
    { 175, 117,  89 , "Brown"      },
    {   0, 255, 255 , "Cyan"       },
    { 255, 156,   0 , "Gold"       },
    { 125, 125, 125 , "Gray"       },
    {   0, 255,   0 , "Green"      },
    {  46, 139,  87 , "GreenBlue"  },
    { 152, 255, 179 , "GreenTint"  },
    { 255,   0, 101 , "HotPink"    },
    { 255,   0, 255 , "Magenta"    },
    { 255, 165,   0 , "Orange"     },
    { 255, 101, 117 , "Pink"       },
    { 255, 171, 187 , "PinkTint"   },
    { 160,  32, 240 , "Purple"     },
    { 255,   0,   0 , "Red"        },
    { 255,  69,   0 , "RedOrange"  },
    {   0, 250, 109 , "SeaGreen"   },
    {  58, 144, 255 , "SkyBlue"    },
    { 238, 130, 238 , "Violet"     },
    { 255, 255, 255 , "White"      },
    { 255, 255,   0 , "Yellow"     },
    { 246, 246, 117 , "YellowTint" },
        };



/*===========================================*/
/*  Generic PrefCommand Parsing & Execution  */
/*===========================================*/


static int ExecutePrefCommand( void )
{
    PrefTokenPtr = CurPrefLine;
    if( !FetchPrefToken() )
    {   PrefTokenPtr = NULL;
        return True;
    }


    switch( CurPrefToken )
    {    
        case(ClipboardTok):
                           FetchPrefToken();
                           if( CurPrefToken == NumberTok )
                           {   ClipBoardFactor = PrefTokenValue;
                               if( ClipBoardFactor<1 )
                                   ClipBoardFactor = 1;
                               if( ClipBoardFactor> MAXCLIPBOARDFACTOR )
                                   ClipBoardFactor = MAXCLIPBOARDFACTOR;
                           } else return( False );
                           break;
               
        case(HelpTok):     FetchPrefToken();
                           if( CurPrefToken==HTMLTok )
                           {   FetchPrefToken();
                               if( !CurPrefToken )
                                   break;
                               else if( CurPrefToken==StringTok )
                                   strcpy(HelpHTMLSource,PrefTokenIdent);
                               else
                                   strcpy(HelpHTMLSource,PrefTokenStart);
                               CurPrefToken = 0;
                           } else return( False );                          
                           break;

        case(ScriptTok):   FetchPrefToken();
                           if( CurPrefToken==DefaultTok )
                           {   FetchPrefToken();
                               if( !CurPrefToken )
                                   break;
                               else if( CurPrefToken==StringTok )
                                   strcpy(DefaultScriptName,PrefTokenIdent);
                               else
                                   strcpy(DefaultScriptName,PrefTokenStart);
                               CurPrefToken = 0;
                           } else return( False );                              
                           break;

        case(SetTok):      FetchPrefToken();
                           if( CurPrefToken==MouseTok )
                           {   FetchPrefToken();
                               if( CurPrefToken==RasMolTok )
                               {   if( Interactive )
                                       SetMouseMode( MMRasMol );
                               } else if( CurPrefToken==InsightTok )
                               {   if( Interactive )
                                       SetMouseMode( MMInsight );
                               } else if( CurPrefToken==QuantaTok )
                               {   if( Interactive )
                                       SetMouseMode( MMQuanta );
                               } else if( CurPrefToken==SybylTok )
                               {   if( Interactive )
                                       SetMouseMode( MMSybyl );
                               } else return( False );
                          } else if( CurPrefToken==SequenceTok )
                          {    FetchPrefToken();
                               if( CurPrefToken==FalseTok )
                               {   SeqFormat = False;
                               } else if( CurPrefToken==TrueTok )
                               {   SeqFormat = True;
                               } else return( False );
                          } else if( CurPrefToken == DepthCueTok )
                          {    FetchPrefToken();
                               if( CurPrefToken==NumberTok )
                               {   if( (PrefTokenValue>0) && (PrefTokenValue<=4))
                                       DepthCueMode = PrefTokenValue;
                               }
                          } else if( CurPrefToken==CoordTok )
                          {    FetchPrefToken();
                               if( CurPrefToken==MoleculeTok )
                               {   SysCoor = MolCoor;
                               } else if( CurPrefToken==WorldTok )
                               {   SysCoor = WorldCoor;
                               } else if( CurPrefToken==ScreenTok )
                               {      SysCoor = ScreenCoor;
                               }  else if( CurPrefToken==AbsoluteTok )
                               {      SysCoor = AbsCoor;
                               } else return( False );
                          }                          
                          break;

        case(WindowTok):  FetchPrefToken();
                          if( CurPrefToken==MainTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowMain = TRUE;
                              else
                                  WindowMain = FALSE;
                          } else if( CurPrefToken==SelectedTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowSelection = TRUE;
                              else
                                  WindowSelection = FALSE;
                          } else if( CurPrefToken==ScriptTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowScripting = TRUE;
                              else
                                  WindowScripting = FALSE;
                          } else if( CurPrefToken==ColourTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok ) {
                                  WindowColour = TRUE;
                              } else if( CurPrefToken==FalseTok ) {
                                  WindowColour = FALSE;
                              } else if (CurPrefToken==NumberTok) {
                                  int a;
                                  a = PrefTokenValue;
                                  FetchPrefToken();
                                  if (CurPrefToken==NumberTok) {
                                      ColourLeft = a;
                                      ColourTop = PrefTokenValue;
                                  }
                              }
                          } else if( CurPrefToken==CommandPanelTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowCommandPanel = TRUE;
                              else
                                  WindowCommandPanel = FALSE;
                          } else if( CurPrefToken==CommandLineTok )
                          {   int done,a,b,c;

                              FetchPrefToken();
                              if( CurPrefToken == '-' )
                              {   FetchPrefToken();
                                  done = True;
                              } else done = False;
							  if( CurPrefToken==NumberTok ) 
                              {   if (done) PrefTokenValue = -PrefTokenValue;
                                  a = PrefTokenValue;

                                  FetchPrefToken();
                                  if( CurPrefToken == '-' )
                                  {   FetchPrefToken();
                                      done = True;
                                  } else done = False;
                                  if( CurPrefToken==NumberTok ) 
                                  {   if (done) PrefTokenValue = -PrefTokenValue;
                                      b = PrefTokenValue;

                                      FetchPrefToken();
                                      if( CurPrefToken == '-' )
                                      {   FetchPrefToken();
                                          done = True;
                                      } else done = False;
                                      if( CurPrefToken==NumberTok ) 
                                      {   if (done) PrefTokenValue = -PrefTokenValue;
                                          c = PrefTokenValue;

                                          FetchPrefToken();
                                          if( CurPrefToken == '-' )
                                          {   FetchPrefToken();
                                              done = True;
                                          } else done = False;
                                          if( CurPrefToken==NumberTok ) 
										  {   if (done) PrefTokenValue = -PrefTokenValue;
                                              CommandLeft = a;
											  CommandTop = b;
											  CommandWidth = c;
											  CommandHeight = PrefTokenValue;
											  WindowCommandLine = TRUE;
                                          } else WindowCommandLine = FALSE;
                                      } else WindowCommandLine = FALSE;
                                  } else WindowCommandLine = FALSE;
                              } else if( CurPrefToken==BackgroundTok ) {
                                  FetchPrefToken();
                                  if( IsColourToken(CurPrefToken) ) {
                                    int n = Token2Colour(CurPrefToken);
                                    CmdBkgColor = IRGB(ColTable[n].red,
                                        ColTable[n].grn,ColTable[n].blu);
                                  }
                              } else if( CurPrefToken==ColourTok ) {
                                  FetchPrefToken();
                                  if( IsColourToken(CurPrefToken) ) {
                                    int n = Token2Colour(CurPrefToken);
                                    CmdTxtColor = IRGB(ColTable[n].red,
                                        ColTable[n].grn,ColTable[n].blu);
                                  }

                              } else WindowCommandLine = FALSE;

                          } else if( CurPrefToken==StatusTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowStatus = TRUE;
                              else
                                  WindowStatus = FALSE;
                          } else if( CurPrefToken==MaximizedTok )
                          {   FetchPrefToken();
                              if( CurPrefToken==TrueTok )
                                  WindowMaximized = TRUE;
                             /* else WindowMaximized = -1; */
                          } else if( CurPrefToken==RasMolTok )  
                          {   int done;

                              FetchPrefToken();
                              if( CurPrefToken == '-' )
                              {   FetchPrefToken();
                                  done = True;
                              } else done = False;
                              if( CurPrefToken==NumberTok ) 
                              {   if (done) PrefTokenValue = -PrefTokenValue;
                                  WindowLeft = PrefTokenValue;

                                  FetchPrefToken();
                                  if( CurPrefToken == '-' )
                                  {   FetchPrefToken();
                                      done = True;
                                  } else done = False;
                                  if( CurPrefToken==NumberTok ) 
                                  {   if (done) PrefTokenValue = -PrefTokenValue;
                                      WindowTop = PrefTokenValue;

                                      FetchPrefToken();
                                      if( CurPrefToken == '-' )
                                      {   FetchPrefToken();
                                          done = True;
                                      } else done = False;
                                      if( CurPrefToken==NumberTok ) 
                                      {   if (done) PrefTokenValue = -PrefTokenValue;
                                          WindowWidth = PrefTokenValue;

                                          FetchPrefToken();
                                          if( CurPrefToken == '-' )
                                          {   FetchPrefToken();
                                              done = True;
                                          } else done = False;
                                          if( CurPrefToken==NumberTok ) 
                                          {   if (done) PrefTokenValue = -PrefTokenValue;
                                              WindowHeight = PrefTokenValue;
                                              WindowMaximized = FALSE;
                                          } else WindowMaximized = -1;
                                      } else WindowMaximized = -1;
                                  } else WindowMaximized = -1;
                              } else WindowMaximized = -1;
                          }
                          break;

        case(LangTok):
            FetchPrefToken();
            if( CurPrefToken==IdentTok)
            {   SwitchLang(PrefTokenIdent);
            }
            break;
            
            
        case(ExitTok):    break;
        default:          return( False );
    }

    if( CurPrefToken )
        if( FetchPrefToken() )
            return( False );
    PrefTokenPtr = NULL;
    return True;
}


/*======================*/
/*  Preference Writing  */
/*======================*/

void WriteScriptPrefs(FILE *fp)
{   int n;

    fprintf(fp,"#!rasmol -preferences\n");
    fprintf(fp,"# Creator: %s Version %s\n\n", VER_NAME, VERSION);

    /* GUI */
    if( WindowMaximized == 1 )
    {   fputs("window maximized on\n",fp);
    } else if( WindowMaximized == -1 )
    {   fputs("window maximized off\n",fp);
    } else
    {   fprintf(fp,"window rasmol %d %d %d %d\n",
          WindowLeft,WindowTop,WindowWidth,WindowHeight);
        fputs("window maximized off\n",fp);
    }

    if( WindowMain )
        fputs("window main on\n",fp);
    else
        fputs("window main off\n",fp);

    if( WindowSelection )
        fputs("window selection on\n",fp);
    else
        fputs("window selection off\n",fp);

    if( WindowScripting )
        fputs("window script on\n",fp);
    else
        fputs("window script off\n",fp);


    if( WindowCommandPanel )
        fputs("window commandpanel on\n",fp);
    else
        fputs("window commandpanel off\n",fp);

    fprintf(fp,"window commandline %d %d %d %d\n",
      CommandLeft,CommandTop,CommandWidth,CommandHeight);

    /* Get color background */
    for (n=0; n<MAXCOL; n++) {
        int c = IRGB(ColTable[n].red,ColTable[n].grn,ColTable[n].blu);
        if (CmdBkgColor == c) {
            fprintf(fp,"window commandline background %s\n",ColTable[n].col);
            break;
        }
    }
    /* Get color text */
    for (n=0; n<MAXCOL; n++) {
        int c = IRGB(ColTable[n].red,ColTable[n].grn,ColTable[n].blu);
        if (CmdTxtColor == c) {
            fprintf(fp,"window commandline color %s\n",ColTable[n].col);
            break;
        }
    }
        
    if( !WindowCommandLine )
        fputs("window commandline off\n",fp);

    if( WindowStatus )
        fputs("window status on\n",fp);
    else
        fputs("window status off\n",fp);

    if( WindowColour )
        fputs("window color on\n\n",fp);
    else
        fputs("window color off\n\n",fp);
    fprintf(fp,"window color %d %d\n",ColourLeft,ColourTop);


    /* Mouse mode */
    if( MouseMode == MMRasMol )
    {    fputs("set mouse rasmol\n",fp);
    } else if( MouseMode == MMInsight )
    {    fputs("set mouse insight\n",fp);
    } else if( MouseMode == MMQuanta )
    {    fputs("set mouse quanta\n",fp);
    } else if( MouseMode == MMSybyl )
    {    fputs("set mouse sybyl\n",fp);
    }
    fputc('\n',fp);

    /* Help HTML source */
    fprintf(fp,"help HTML %s\n",HelpHTMLSource);
    fputc('\n',fp);

    /* Default template source */
    if( DefaultScriptName[0] != '\0' ) {
        fprintf(fp,"script default %s\n",DefaultScriptName);
        fputc('\n',fp);
    }

    /* Clipboard zoom factor */
    fprintf(fp,"clipboard %d\n",ClipBoardFactor);
    fputc('\n',fp);

    /* Sequence ouptput format */
    if( SeqFormat==True)
    {    fputs("set sequence true\n",fp);
    } else if( SeqFormat==False)
    {    fputs("set sequence false\n",fp);
    }
    fputc('\n',fp);

    /* DepthCue Mode */
    fprintf(fp,"set depthcue %d\n",DepthCueMode);
    fputc('\n',fp);

    /* Coordinate System */
    if( SysCoor == MolCoor )
        fputs("set coordinate molecule\n",fp);
    else if( SysCoor == WorldCoor )
        fputs("set coordinate world\n",fp);
    else if( SysCoor == ScreenCoor )
        fputs("set coordinate screen\n",fp);
    else if( SysCoor == AbsCoor )
        fputs("set coordinate absolute\n",fp);
    fputc('\n',fp);

    fputs("exit\n",fp);
    fclose(fp);
}



/*================*/
/*  Read & Write  */
/*================*/


static void getPrefFileName( void )
{
#if defined(IBMPC)
    char *path = 0;
	char *c;
	HKEY key;
    DWORD type, size;

    if( RegOpenKeyEx(
            HKEY_CURRENT_USER,
            "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
            0,
            KEY_READ,
            &key) != 0)
        return;

    if( RegQueryValueEx(key, "Desktop", 0, &type, 0, &size) == 0 && type == REG_SZ )
    {	path = (char*) malloc(size);
		if (RegQueryValueEx(key, "Desktop", 0, 0, (unsigned char*)path, &size) != 0)
        {	free(path);
			path = 0;
        }
    }
	RegCloseKey(key);
    
    c = strrchr(path, '\\');
    if (c == NULL) return;
    *c = '\0';
	if( path )
	{	strcpy(PrefFileName, path);
		free(path);
	}
#endif   

// PV no need to create a folder in windows, but the PrefFileName
// "C:/Documents and Settings/userdir/rasmol/rasmolprefs" might be better.

    // create folder here
	if( *PrefFileName )
		strcat(PrefFileName,"\\rasmolprefs");
}

static int LoadPrefFile( FILE *fp )
{
    register int ch,len;
    register int stat;
    register int errorcount;

    errorcount = 0;
    do {
        len = 0;
        ch = getc(fp);
        while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
        {   if( len<MAXPREFLEN )
                CurPrefLine[len++] = ch;
            ch = getc(fp);
        }

        if( ch == '\r' )
        {   ch = getc(fp);
            if( ch != '\n' )
                ungetc(ch,fp);
        }

        if( len<MAXPREFLEN )
        {   CurPrefLine[len] = '\0';
            stat = ExecutePrefCommand();
            if( !stat )
            {   errorcount++;
                if( errorcount>MAXERROR )
                {   fclose( fp );
                    return False;
                }
            }
         } else 
             return False;
    } while( ch!=EOF );
    fclose( fp );
    return True;
}

/* Read any preference file located in rasmol/lib directory */
static void ReadProjectPrefs ( void ) {
    char Path[1024];
    register char *dst;
	FILE *fp;

    if(!*RasMolPath)
        return;

    *Path = '\0';
    dst = RasMolPath;
    dst = strcpy(Path,dst);
    while( *dst++ ) {};
    dst--;
    dst = strcpy(dst,"lib");
    dst += 3;
    *dst++ = DirChar;
    strcpy(dst,"rasmolprefs");
    CurPrefToken = 0;
    PrefTokenPtr = NULL;
    fp = fopen(Path,"rb");
    if( fp )
    {   LoadPrefFile(fp);
        PrefsModified = False;
    }
}

void ReadPrefs( void )
{   
#ifdef PREFERENCES
	FILE *fp;

    ReadProjectPrefs();
    
	if( !*PrefFileName )
    {   WriteString(MsgStrs(PrfNotFound));
    } else
    {   CurPrefToken = 0;
        PrefTokenPtr = NULL;
        fp = fopen(PrefFileName,"rb");
        if( fp )
        {   if( !LoadPrefFile(fp) )
            {   WriteString(MsgStrs(PrfResetDef));
                PrefsModified = True;
                WritePrefs( );
            } else
            {   /*WriteString(MsgStrs(PrfLoaded));*/
                PrefsModified = False;
            }
        } else
        {   PrefsModified = True;
            WritePrefs( );
            if( !PrefsModified )
                WriteString(MsgStrs(PrfCreate));
        }
    }
#endif
}


void WritePrefs( void )
{    
#if defined PREFERENCES
	FILE *fp;
    
	if( !PrefsModified || !*PrefFileName )
        return;

    fp = fopen(PrefFileName,"w");

    if( fp )
    {   WriteScriptPrefs(fp);
        PrefsModified = False;
    }    
#endif
}


void InitialisePrefs( void )
{
    *PrefFileName = '\0';
    PrefsModified= False;

#if defined PREFERENCES
    getPrefFileName();
#endif   
}
