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

/* cmndline.c
 */

#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
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
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#define CMNDLINE
#include "cmndline.h"
#include "molecule.h"
#include "abstree.h"
#include "command.h"
#include "render.h"
#include "transfor.h"
#include "graphics.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"


/* Determine Mouse Sensitivity! */
#define IsClose(u,v) (((u)>=(v)-1) && ((u)<=(v)+1))


#define MM_NONE   0x00
#define MM_ROTX   0x01
#define MM_ROTY   0x02
#define MM_ROTZ   0x03
#define MM_TRNX   0x04
#define MM_TRNY   0x05
#define MM_ZOOM   0x06
#define MM_CLIP   0x07
#define MM_DEPT   0x08
#define MM_TRNZ   0x09

#define MM_PICK   0x01
#define MM_NEXT   0x02
#define MM_LABL   0x03
#define MM_SELE   0x04
#define MM_PREV   0x05



typedef struct {
        Byte dxfunc, dxinvrt;
        Byte dyfunc, dyinvrt;
        Byte click;
    } MouseMapping;


/* RasMol Mouse Bindings:
 *  XY Rotation:   Left (and any other) Button
 *  XY Translaion: Middle or Right Buttons
 *  Scale:         Shift (and Control) Left (and any other) Button
 *  Z Rotation:    Shift (and Control) Middle or Right Buttons
 *  Clipping:      Control Left (and any other) Button
 */

#ifdef INVERT
#define    INV    1
#else
#define INV 0
#endif
 
 
static MouseMapping MouseRasMol[32] = {
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*                     */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft                 */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PICK },  /*     Mid             */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft+Mid             */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PICK },  /*         Rgt         */
        { MM_TRNZ, 0, MM_TRNZ, INV, MM_PICK },  /* Lft    +Rgt         */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PICK },  /*     Mid+Rgt         */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft+Mid+Rgt         */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft     */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft        +Sft     */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*     Mid    +Sft     */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid    +Sft     */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*         Rgt+Sft     */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft    +Rgt+Sft     */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*     Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*                 Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_PREV },  /* Lft            +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*     Mid        +Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_PREV },  /* Lft+Mid        +Ctl */
        { MM_NONE, 0, MM_DEPT, INV, MM_PREV },  /*         Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /* Lft    +Rgt    +Ctl */
        { MM_NONE, 0, MM_DEPT, INV, MM_PREV },  /*     Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /* Lft+Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft+Ctl */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft        +Sft+Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*     Mid    +Sft+Ctl */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid    +Sft+Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*         Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft    +Rgt+Sft+Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /*     Mid+Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft+Ctl */
    };        
           
           
/* MSI (Biosym) Insight Bindings:
 *     Needs to be verified!
 */
static MouseMapping MouseInsight[32] = {
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*                     */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft                 */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PICK },  /*     Mid             */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_PICK },  /* Lft+Mid             */
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*         Rgt         */
        { MM_ROTZ, 0, MM_ROTZ, INV, MM_PICK },  /* Lft    +Rgt         */
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*     Mid+Rgt         */
        { MM_CLIP, 0, MM_CLIP, INV, MM_PICK },  /* Lft+Mid+Rgt         */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft     */
        { MM_ROTY, 0, MM_ROTX, INV, MM_NEXT },  /* Lft        +Sft     */
        { MM_TRNY, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft     */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid    +Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*         Rgt+Sft     */
        { MM_ROTZ, 0, MM_ROTZ, INV, MM_NEXT },  /* Lft    +Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*     Mid+Rgt+Sft     */
        { MM_CLIP, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*                 Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /* Lft            +Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PREV },  /*     Mid        +Ctl */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_PREV },  /* Lft+Mid        +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*         Rgt    +Ctl */
        { MM_ROTZ, 0, MM_ROTZ, INV, MM_PREV },  /* Lft    +Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*     Mid+Rgt    +Ctl */
        { MM_CLIP, 0, MM_CLIP, INV, MM_PREV },  /* Lft+Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft+Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_NEXT },  /* Lft        +Sft+Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft+Ctl */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_NEXT },  /* Lft+Mid    +Sft+Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*         Rgt+Sft+Ctl */
        { MM_ROTZ, 0, MM_ROTZ, INV, MM_NEXT },  /* Lft    +Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*     Mid+Rgt+Sft+Ctl */
        { MM_CLIP, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft+Ctl */
    };        


/* MSI Quanta Mouse Bindings:
 *     Needs to be verified!
 */
static MouseMapping MouseQuanta[32] = {
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*                     */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft                 */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /*     Mid             */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft+Mid             */
        { MM_ROTZ, 0, MM_NONE, INV, MM_PICK },  /*         Rgt         */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft    +Rgt         */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /*     Mid+Rgt         */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /* Lft+Mid+Rgt         */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /*             Sft     */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft        +Sft     */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft     */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid    +Sft     */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*         Rgt+Sft     */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft    +Rgt+Sft     */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*                 Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /* Lft            +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /*     Mid        +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /* Lft+Mid        +Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_PREV },  /*         Rgt    +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /* Lft    +Rgt    +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /*     Mid+Rgt    +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /* Lft+Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_ZOOM, INV, MM_NEXT },  /*             Sft+Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft        +Sft+Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft+Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid    +Sft+Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*         Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft    +Rgt+Sft+Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid+Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_CLIP, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft+Ctl */
    };        
                 
/* Tripos Sybyl 6.2 Mouse Bindings:
 *    Tripos Graphics Manual, July 1995
 *  Selection:      Left Button
 *  Scale:          Middle & Right Buttons
 *  XY Rotation:    Right Button
 *  Z Rotation:     Left & Right Buttons
 *  XY Translation: Middle Button
 *  Z Translation:  Left & Middle Buttons
 */
 
static MouseMapping MouseSybyl[32] = {
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /*                     */
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /* Lft                 */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PICK },  /*     Mid             */
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /* Lft+Mid             */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PICK },  /*         Rgt         */
        { MM_ROTZ, 0, MM_NONE, INV, MM_PICK },  /* Lft    +Rgt         */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_PICK },  /*     Mid+Rgt         */
        { MM_NONE, 0, MM_NONE, INV, MM_PICK },  /* Lft+Mid+Rgt         */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft        +Sft     */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft+Mid    +Sft     */
        { MM_ROTY, 0, MM_ROTX, INV, MM_NEXT },  /*         Rgt+Sft     */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /* Lft    +Rgt+Sft     */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_NEXT },  /*     Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft     */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /*                 Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /* Lft            +Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_PREV },  /*     Mid        +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /* Lft+Mid        +Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_PREV },  /*         Rgt    +Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_PREV },  /* Lft    +Rgt    +Ctl */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_PREV },  /*     Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_PREV },  /* Lft+Mid+Rgt    +Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /*             Sft+Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft        +Sft+Ctl */
        { MM_TRNX, 0, MM_TRNY, INV, MM_NEXT },  /*     Mid    +Sft+Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft+Mid    +Sft+Ctl */
        { MM_ROTY, 0, MM_ROTX, INV, MM_NEXT },  /*         Rgt+Sft+Ctl */
        { MM_ROTZ, 0, MM_NONE, INV, MM_NEXT },  /* Lft    +Rgt+Sft+Ctl */
        { MM_ZOOM, 0, MM_ZOOM, INV, MM_NEXT },  /*     Mid+Rgt+Sft+Ctl */
        { MM_NONE, 0, MM_NONE, INV, MM_NEXT },  /* Lft+Mid+Rgt+Sft+Ctl */
    };        




static char *CurPrompt;

static MouseMapping *MouseBinding;
static int PointX,PointY;
static int InitX,InitY;
static int FastDrawFlag;


/* Return true for '\n' */
int ProcessCharacter( int ch )
{
//    if( !ch ) return( False );

    if( IsPaused )
    {   if( (ch==0x04) || (ch==0x1a) || (ch==0x1b) )
        {   InterruptPauseCommand();
        } else ResumePauseCommand();
        return( False );
    }

    /* 32 to 126 - 0x20 to 0x7E */
    if( (ch>=' ') && (ch<='~') )
    {   if( !CommandActive ) 
            return False;
        WriteChar(ch);
    } else
        switch( ch )
        {    case( 0x00 ):  /* Flush character */
                            WriteChar(0x00);
                            break;

             case( 0x7f ):  /* DEL and ^H */
             case( 0x08 ):  /* Backspace */
                            if( !CommandActive ) 
                                return False;
                            WriteChar(0x08);
                            break;

             case( 0x09 ):  /* Tab */
                            /* Change display mode if pressed */
                            if( Database )
                            {   ReDrawFlag |= RFRefresh | RFColour;
                                if( DisplayMode )
                                    DisplayMode = 0;
                                else
                                    DisplayMode = 1;
                                RefreshView();
                            }
                            return False;

             case( 0x0d ):  
             case( 0x0a ):  if( !CommandActive ) 
                                return False;
                            WriteChar('\n');
                            CommandActive = False;
                            return( True );

        }
    return False;
}


void ResetCommandLine( int state )
{    
    if( state )
    {   switch( CurState=state )
        {   case(1):   CurPrompt="RasMol> ";    break;
            case(2):   CurPrompt="RasTop> ";    break;
        }
    }

    if( WindowCommandLine )
    {   if( !CommandActive )
        {   CommandActive = True;
            WriteString(CurPrompt);
        }
    }

    CurLine[0] = 0;
    Recycle = (char *)0;
}


void InvalidateCmndLine( void )
{   if( CommandActive )
    {   WriteChar('\n');
        CommandActive=False;
    }    
}

static void ClampDial( int dial, double value )
{
    register double temp;

    temp = DialValue[dial] + value;
    if( temp > 1.0 )
    {   DialValue[dial] = 1.0;
    } else if( temp < -1.0 )
    {   DialValue[dial] = -1.0;
    } else DialValue[dial] = temp;
}

static void FreeDial( int dial, double value )
{   DialValue[dial] = DialValue[dial] + value;
}

static void WrapDial( int dial, double value )
{
    register double temp;

    temp = DialValue[dial] + value;
    while( temp < -1.0 )  temp += 2.0;
    while( temp > 1.0 )   temp -= 2.0;
    DialValue[dial] = temp;
}


void ProcessMouseDown( int x, int y, int stat )
{   
    register MouseMapping *map;
    
    map = &MouseBinding[stat];
    if( map->dxfunc || map->dyfunc )
        SetMouseCaptureStatus(True);
    InitX = PointX = x;
    InitY = PointY = y;
    HeldButton = True;
    if (PickMode == PickAtom ) {
        DrawArea = True;
    }
}


void ProcessMouseUp( int x, int y, int stat )
{                      
    register MouseMapping *map;
    register int area;

    area = DrawArea;
    DrawArea = False;
    
    map = &MouseBinding[(stat&31)];
    SetMouseCaptureStatus(False);

    if( !HeldButton )
        return;

    HeldButton = False;

    /* Refresh fast drawing */
    if( FastDrawFlag )
    {   ReDrawFlag |= RFRefresh;
        FastDrawFlag = False;
    }
                                  
    if( Database && IsClose(x,InitX) && IsClose(y,InitY) )
    {   
#ifdef INVERT
        y = YRange - y;
#endif        
        switch( map->click )
        {   case(MM_PICK):  if( PickAtoms(False,x,y) )
                            {   AdviseUpdate(AdvPickNumber);
                                AdviseUpdate(AdvPickAtom);
                                AdviseUpdate(AdvPickCoord);
                            }
                            break;
                   
            case(MM_NEXT):  if( PickAtoms(True,x,y) )
                            {   AdviseUpdate(AdvPickNumber);
                                AdviseUpdate(AdvPickAtom);
                                AdviseUpdate(AdvPickCoord);
                            }
                            break;
                   
            case(MM_PREV):  if( PickAtoms(-1,x,y) )
                            {   AdviseUpdate(AdvPickNumber);
                                AdviseUpdate(AdvPickAtom);
                                AdviseUpdate(AdvPickCoord);
                            }
                            break;
        }
    } else if( Database && area )
    {   /* SelectArea treats inversion */
        if( CurMolSet->visible || (!CurMolSet->visible && DisplayMode) )
        {   switch( map->click )
            {   case(MM_PICK):  SelectArea(False,True,InitX,InitY,x,y);
                                break;
                   
                case(MM_NEXT):  SelectArea(True,True,InitX,InitY,x,y);
                                break;
                   
                case(MM_PREV):  SelectArea(-1,True,InitX,InitY,x,y);
                                break;
            }
        }
    }
}

static int BindingActive( int stat )
{
    return( MouseBinding[stat].dxfunc || MouseBinding[stat].dyfunc );
}

void SetMouseMode( int mode )
{
    register int stat;
    
    switch( mode )
    {   case(MMRasMol):  MouseBinding = MouseRasMol;  
                         MouseMode = MMRasMol;
                         break;
        case(MMInsight): MouseBinding = MouseInsight; 
                         MouseMode = MMInsight;
                         break;
        case(MMQuanta):  MouseBinding = MouseQuanta;
                         MouseMode = MMQuanta;
                         break;
        case(MMSybyl):   MouseBinding = MouseSybyl;
                         MouseMode = MMSybyl;
                         break;
    }

    /* Should also test for BindingActive(MMSft|MMCtl)! */
    stat = BindingActive(MMSft) || BindingActive(MMCtl);
    SetMouseUpdateStatus(stat);            
}

static void PerformMouseFunc( int func, int delta, int max )
{   register double value;

    switch( func )
    {   case(MM_ROTX):  if( (PickMode==PickRBond) && BondSelected && 
                            (RotMode==RotMol) )
                        {   WrapDial( DialBRot, (double)(2*delta)/max );
                            ReDrawFlag |= RFRotBond;
                        } else
                        {   WrapDial( DialRX, (double)(2*delta)/max );
                            ReDrawFlag |= RFRotateX;
                        }
                        break;
                        
        case(MM_ROTY):  if( (PickMode==PickRBond) && BondSelected && 
                            (RotMode==RotMol) )
                        {   WrapDial( DialBRot, (double)(-2*delta)/max );
                            ReDrawFlag |= RFRotBond;
                        } else
                        {   WrapDial( DialRY, (double)(2*delta)/max );
                            ReDrawFlag |= RFRotateY;
                        }
                        break;
                        
        case(MM_ROTZ):  WrapDial( DialRZ, (double)(2*delta)/max );
                        ReDrawFlag |= RFRotateZ;
                        break;
                        
        case(MM_TRNZ):  ClampDial( DialZoom, (double)(2*delta)/max );
                        ReDrawFlag |= RFTransZ;
                        break;
                        
        case(MM_ZOOM):  ClampDial( DialZoom, (double)(2*delta)/max );
                        ReDrawFlag |= RFZoom;
                        break;
                        
        case(MM_TRNX):  FreeDial( DialTX, (double)delta/max );
                        ReDrawFlag |= RFTransX;
                        break;

        case(MM_TRNY):  FreeDial( DialTY, (double)delta/max );
                        ReDrawFlag |= RFTransY;
                        break;

        case(MM_CLIP):  value = (double)delta/max;
                        /* Plane never activated */
                        if( WClip[0]== -999999. )
                            WClip[0] = MOffset[2]+Scale*LocalRadius;
                        else                
                            WClip[0] -= Scale*LocalRadius*value;
                        ReDrawFlag |= RFSlab;
                        UseSlabPlane = True;
                        UseShadow = False;
                        break;

        case(MM_DEPT):  value = (double)delta/max;
                        /* Plane never activated */
                        if( WClip[1]== -999999. )
                            WClip[1] = MOffset[2]-Scale*LocalRadius;
                        else                
                            WClip[1] -= Scale*LocalRadius*value;
                        ReDrawFlag |= RFSlab;
                        UseDepthPlane = True;
                        UseShadow = False;
                        break;
    }     
}

static void ReDial( double SaveValue[10] )
{
    register int index;
    register double theta;
    register double cost, sint;
    register int lix,liy,liz;
    
    if( (RotMode==RotAll)||(PickMode==PickLight)||(PickMode==PickRBond) )
    {   if( PickMode==PickLight ) 
        {    theta = - PI*(DialValue[DialRX] - SaveValue[DialRX]);
            cost = cos(theta);
            sint = sin(theta);
            lix = LightX;
            liy = (int)( LightY*cost - LightZ*sint );
            liz = (int)( LightY*sint + LightZ*cost );

            theta = - PI*(DialValue[DialRY] - SaveValue[DialRY]);
            cost = cos(theta);
            sint = sin(theta);
            
            ChangeLight(False,(int)(lix*cost-liz*sint),
                        liy,(int)(lix*sint+liz*cost));

        } else if( RotMode==RotAll )
        {   WRotValue[DialRX] = DialValue[DialRX];
            WRotValue[DialRY] = DialValue[DialRY];
            WRotValue[DialRZ] = DialValue[DialRZ];
            WTransValue[0] = DialValue[DialTX];
            WTransValue[1] = DialValue[DialTY];

        } else if( (PickMode==PickRBond) && BondSelected )
        {   BondSelected->BRotValue = DialValue[DialBRot];
        }  

        for( index=0;index<7;index++ )
            if( !(index==DialZoom) )
                DialValue[index] = SaveValue[index];
    }
}

void ProcessMouseMove( int x, int y, int stat )
{   
    register MouseMapping *map;
    register int dx,dy;
    double SaveValue[10];
    register int index;

    /* Cursor Mode */
    if( CursorMode!=CURRasmol ) /* Picking! */
    {    if( (PickMode==PickAtom) ||
            (PickMode==PickGroup)||
            (PickMode==PickChain)||
            (PickMode==PickBond) )
        {   if( stat&MMSft)
                CursorMode = CURPickplus;
            else if( stat&MMCtl )
                CursorMode = CURPickminus;
             else
                CursorMode = CURPicking;
        } else
            CursorMode = CURPicking;
    }

    if ( (RotMode==RotAll) || (PickMode==PickLight) || (PickMode==PickRBond) )
    {    for( index=0;index<10;index++ )
            SaveValue[index] = DialValue[index];
                    
        if( PickMode==PickLight ) 
        {   /* Do nothing */
        } else if( RotMode==RotAll ) 
        {    DialValue[DialRX] = WRotValue[DialRX];
            DialValue[DialRY] = WRotValue[DialRY];
            DialValue[DialRZ] = WRotValue[DialRZ];
            DialValue[DialTX] = WTransValue[0];
            DialValue[DialTY] = WTransValue[1];
        } else if( (PickMode==PickRBond) && BondSelected )
        {    DialValue[DialBRot] = BondSelected->BRotValue;
        }
    }
    
    map = &MouseBinding[stat];
    if( map->dxfunc || map->dyfunc || DrawArea)
    {   SetMouseCaptureStatus(True);
        if( !HeldButton )
        {   InitX = PointX = x;
            InitY = PointY = y;
            HeldButton = True;
            ReDial( SaveValue );
            return;
        }

        FastDraw = True;
        FastDrawFlag = True;
    
        if( map->dxinvrt )
        {      dx = PointX - x;
        } else dx = x - PointX;  
    
        if( map->dyinvrt )
        {      dy = PointY - y;
        } else dy = y - PointY;

        if( !DrawArea )
        {    if( IsClose(x,InitX) && IsClose(y,InitY) )
            {   ReDial( SaveValue );
                return;
            } 
            if( dx ) PerformMouseFunc(map->dxfunc,dx,XRange);
            if( dy ) PerformMouseFunc(map->dyfunc,dy,YRange);
        } else 
        {    if( (x<0&&dx<0)||(y<0&&-dy<0)||(x>XRange&&dx>0)||(y>YRange&&-dy>0) )
            {    
                if( !IsClose(x,InitX) && dx ) PerformMouseFunc(MM_TRNX,-dx,XRange);
                if( !IsClose(y,InitY) && dy ) PerformMouseFunc(MM_TRNY,-dy,YRange);
                InitX -= dx;
#ifdef INVERT    /*desinvert!*/
                InitY += dy;
#else
                InitY -= dy;
#endif        
            }
    
            if( CurMolSet->visible || (!CurMolSet->visible && DisplayMode) )
            {   switch( map->click )
                {   case(MM_PICK):  SelectArea(False,False,InitX,InitY,x,y);
                                    break;
                       
                    case(MM_NEXT):  SelectArea(True,False,InitX,InitY,x,y);
                                    break;
                       
                    case(MM_PREV):  SelectArea(-1,False,InitX,InitY,x,y);
                                    break;
                }
            }

            ReDrawFlag |= RFRefresh;
        }

#if !defined RASTOPWIN
        if( ReDrawFlag & (RFRotateX|RFRotateY) )
            UpdateScrollBars();
#endif
   
        PointX = x;
        PointY = y; 
    } else              
    {   SetMouseCaptureStatus(False);
        HeldButton = False;
#ifdef INVERT
        y = YRange - y;
#endif        
        IdentifyAtom(x,y);
    }

    ReDial( SaveValue );

    /* Prepare Transform */
    if( ReDrawFlag&RFZoom )
    {   PrepareWorldZoom(1);
    } else if( ReDrawFlag&RFTransZ ) {
        ReDrawFlag &= ~RFTransZ;
        ReDrawFlag |= RFZoom;
        PrepareWorldZoom(0);
    } else if( (WRotValue[0]!=WLastRX) ||
               (WRotValue[1]!=WLastRY) || 
               (WRotValue[2]!=WLastRZ) )
    {   PrepareWorldRotate(1);
    } else if( (DialValue[0]!=LastRX) ||
               (DialValue[1]!=LastRY) || 
               (DialValue[2]!=LastRZ) )
    {   PrepareMoleculeRotate();
    } else if( (WTransValue[0]!=WLastTX) ||
               (WTransValue[1]!=WLastTY) || 
               (WTransValue[2]!=WLastTZ) )
    {   PrepareWorldTranslate();
    } else if( (DialValue[4]!=LastTX) ||
               (DialValue[5]!=LastTY) || 
               (DialValue[6]!=LastTZ) )
    {   PrepareMoleculeTranslate();
    } else if( ReDrawFlag&RFRotBond )
    {   PrepareBondRotate();
    } 

}

void ProcessWheelDial( double dial, int stat )
{   register double dialtemp;

    if( !stat )
    {   if( RotMode==RotAll )
        {   dialtemp = DialValue[DialRZ];
            DialValue[DialRZ] = WRotValue[DialRZ];
        }

        WrapDial( DialRZ, dial );
        ReDrawFlag |= RFRotateZ;

        if( RotMode==RotAll )
        {   WRotValue[DialRZ] = DialValue[DialRZ];
            DialValue[DialRZ] = dialtemp;
            PrepareWorldRotate(1);
        } else
            PrepareMoleculeRotate();

    } else if( (stat&MMSft) || (stat&MMCtl ) )
    {   if( RotMode==RotAll || (MoleculeList&&!MoleculeList->next) )
        {   ClampDial( DialZoom, dial );
            ReDrawFlag |= RFZoom;
            PrepareWorldZoom(0);
        } else
        {   FreeDial( DialTZ, dial );
            ReDrawFlag |= RFTransZ;
            PrepareWorldTranslate();
        }
    }
}

void InitialiseCmndLine( void )
{
    CommandActive = False;

    SetMouseMode(MMRasMol);
    CursorMode = CURRasmol;

    HeldButton = False;
    FastDrawFlag = False;
    CmdBkgColor = 0x000000;
    CmdTxtColor = 0x00ff00;
    GUItoCmdEcho = False;
}

