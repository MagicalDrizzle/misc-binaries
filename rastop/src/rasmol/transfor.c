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


/* transfor.c
 */
#include "rasmol.h"

#ifdef IBMPC
#include <windows.h>
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
#include <stdio.h>
#include <math.h>

#define TRANSFORM
#include "molecule.h"
#include "abstree.h"
#include "transfor.h"
#include "cmndline.h"
#include "command.h"
#include "pixutils.h"
#include "render.h"
#include "repres.h"
#include "graphics.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"



#define CPKMAX  16
static ShadeRef CPKShade[] = {
     { 0, 0, 200, 200, 200 },       /*  0 Light Grey   */
     { 0, 0, 143, 143, 255 },       /*  1 Sky Blue     */
     { 0, 0, 240,   0,   0 },       /*  2 Red          */
     { 0, 0, 255, 200,  50 },       /*  3 Yellow       */
     { 0, 0, 255, 255, 255 },       /*  4 White        */
     { 0, 0, 255, 192, 203 },       /*  5 Pink         */
     { 0, 0, 218, 165,  32 },       /*  6 Golden Rod   */
     { 0, 0,   0,   0, 255 },       /*  7 Blue         */
     { 0, 0, 255, 165,   0 },       /*  8 Orange       */
     { 0, 0, 128, 128, 144 },       /*  9 Dark Grey    */
     { 0, 0, 165,  42,  42 },       /* 10 Brown        */
     { 0, 0, 160,  32, 240 },       /* 11 Purple       */
     { 0, 0, 255,  20, 147 },       /* 12 Deep Pink    */
     { 0, 0,   0, 255,   0 },       /* 13 Green        */
     { 0, 0, 178,  34,  34 },       /* 14 Fire Brick   */
     { 0, 0,  34, 139,  34 } };     /* 15 Forest Green */


static ShadeRef Shapely[] = {
     { 0, 0, 140, 255, 140 },    /* ALA */
     { 0, 0, 255, 255, 255 },    /* GLY */
     { 0, 0,  69,  94,  69 },    /* LEU */
     { 0, 0, 255, 112,  66 },    /* SER */
     { 0, 0, 255, 140, 255 },    /* VAL */
     { 0, 0, 184,  76,   0 },    /* THR */
     { 0, 0,  71,  71, 184 },    /* LYS */
     { 0, 0, 160,   0,  66 },    /* ASP */
     { 0, 0,   0,  76,   0 },    /* ILE */
     { 0, 0, 255, 124, 112 },    /* ASN */
     { 0, 0, 102,   0,   0 },    /* GLU */
     { 0, 0,  82,  82,  82 },    /* PRO */
     { 0, 0,   0,   0, 124 },    /* ARG */
     { 0, 0,  83,  76,  66 },    /* PHE */
     { 0, 0, 255,  76,  76 },    /* GLN */
     { 0, 0, 140, 112,  76 },    /* TYR */
     { 0, 0, 112, 112, 255 },    /* HIS */
     { 0, 0, 255, 255, 112 },    /* CYS */
     { 0, 0, 184, 160,  66 },    /* MET */
     { 0, 0,  79,  70,   0 },    /* TRP */

     { 0, 0, 255,   0, 255 },    /* ASX */
     { 0, 0, 255,   0, 255 },    /* GLX */
     { 0, 0, 255,   0, 255 },    /* PCA */
     { 0, 0, 255,   0, 255 },    /* HYP */

     { 0, 0, 160, 160, 255 },    /*   A */
     { 0, 0, 255, 140,  75 },    /*   C */
     { 0, 0, 255, 112, 112 },    /*   G */
     { 0, 0, 160, 255, 160 },    /*   T */

     { 0, 0, 184, 184, 184 },    /* 28 -> BackBone */
     { 0, 0,  94,   0,  94 },    /* 29 -> Special  */
     { 0, 0, 255,   0, 255 } };  /* 30 -> Default  */

     
static ShadeRef AminoShade[] = {
     { 0, 0, 230,  10,  10 },    /*  0  ASP, GLU      */
     { 0, 0,  20,  90, 255 },    /*  1  LYS, ARG      */
     { 0, 0, 130, 130, 210 },    /*  2  HIS           */
     { 0, 0, 250, 150,   0 },    /*  3  SER, THR      */
     { 0, 0,   0, 220, 220 },    /*  4  ASN, GLN      */
     { 0, 0, 230, 230,   0 },    /*  5  CYS, MET      */
     { 0, 0, 200, 200, 200 },    /*  6  ALA           */
     { 0, 0, 235, 235, 235 },    /*  7  GLY           */
     { 0, 0,  15, 130,  15 },    /*  8  LEU, VAL, ILE */
     { 0, 0,  50,  50, 170 },    /*  9  PHE, TYR      */
     { 0, 0, 180,  90, 180 },    /* 10  TRP           */
     { 0, 0, 220, 150, 130 },    /* 11  PRO, PCA, HYP */
     { 0, 0, 190, 160, 110 } };  /* 12  Others        */

static int AminoIndex[] = {
      6, /*ALA*/   7, /*GLY*/   8, /*LEU*/   3,  /*SER*/
      8, /*VAL*/   3, /*THR*/   1, /*LYS*/   0,  /*ASP*/
      8, /*ILE*/   4, /*ASN*/   0, /*GLU*/  11,  /*PRO*/
      1, /*ARG*/   9, /*PHE*/   4, /*GLN*/   9,  /*TYR*/
      2, /*HIS*/   5, /*CYS*/   5, /*MET*/  10,  /*TRP*/
      4, /*ASX*/   4, /*GLX*/  11, /*PCA*/  11   /*HYP*/
                          };

static ShadeRef HBondShade[] = {
     { 0, 0, 255, 255, 255 },    /* 0  Offset =  2   */
     { 0, 0, 255,   0, 255 },    /* 1  Offset =  3   */
     { 0, 0, 255,   0,   0 },    /* 2  Offset =  4   */
     { 0, 0, 255, 165,   0 },    /* 3  Offset =  5   */
     { 0, 0,   0, 255, 255 },    /* 4  Offset = -3   */
     { 0, 0,   0, 255,   0 },    /* 5  Offset = -4   */
     { 0, 0, 255, 255,   0 } };  /* 6  Others        */


static ShadeRef StructShade[] = {
     { 0, 0, 255, 255, 255 },    /* 0  Default     */
     { 0, 0, 255,   0, 128 },    /* 1  Alpha Helix */
     { 0, 0, 255, 200,   0 },    /* 2  Beta Sheet  */
     { 0, 0,  96, 128, 255 } };  /* 3  Turn        */

static ShadeRef PotentialShade[] = {
     { 0, 0, 255,   0,   0 },    /* 0  Red     25 < V       */
     { 0, 0, 255, 165,   0 },    /* 1  Orange  10 < V <  25 */
     { 0, 0, 255, 255,   0 },    /* 2  Yellow   3 < V <  10 */
     { 0, 0,   0, 255,   0 },    /* 3  Green    0 < V <   3 */
     { 0, 0,   0, 255, 255 },    /* 4  Cyan    -3 < V <   0 */
     { 0, 0,   0,   0, 255 },    /* 5  Blue   -10 < V <  -3 */
     { 0, 0, 160,  32, 240 },    /* 6  Purple -25 < V < -10 */
     { 0, 0, 255, 255, 255 } };  /* 7  White        V < -25 */

     
/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(ptr=group->alist;ptr;ptr=ptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext) 
#define ForEachBack  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(bptr=chain->blist;bptr;bptr=bptr->bnext)

#define MatchChar(a,b)   (((a)=='#')||((a)==(b)))

static int MaskColour[MAXMASK];
static int MaskShade[MAXMASK];

static double ScaleFactor;    /* Scale factor */


void DetermineClipping( void )
{
    register int temp;
    register int max;

    max = 0;
    if( (DrawAtoms || DrawStars) && (MaxAtomRadius>max) )  max = MaxAtomRadius;
    if( DrawBonds && (MaxBondRadius>max) )  max = MaxBondRadius;
       
    temp = (int)(Scale*LocalRadius);
    if( (YOffset>=temp) && (XOffset>=temp) && (YOffset+temp<YRange) )
    {   /* UseScreenClip = (XOffset+temp) >= (SRange<<1); */
        if( UseStereo )
        {   UseScreenClip = (XOffset+temp) >= (XRange>>1);
        } else UseScreenClip = (XOffset+temp) >= XRange;
    } else UseScreenClip = True;
}


void SetRadiusValue( int rad , int flag)
{
    register int irad,change;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    if( rad == 0 )
        return;

    irad = (int)(Scale*rad);
    MaxAtomRadius = 0;
    DrawAtoms = False;
    DrawStars = False; 
    change = False;

    ForEachAtom
        if( ptr->flag & SelectFlag )
        {   if( irad>MaxAtomRadius )
                MaxAtomRadius = irad;
            if (flag == SphereFlag )
            {    ptr->flag |= SphereFlag;
                ptr->flag &= ~StarFlag;
            } else
            {    ptr->flag |= StarFlag;
                ptr->flag &= ~SphereFlag;
            }
            ptr->radius = rad;
            ptr->irad = irad;
            change = True;
        } else if( ptr->flag & SphereFlag )
        {    DrawAtoms = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        } else if( ptr->flag & StarFlag )
        {    DrawStars = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        }

    if( change )
    {   if (flag == SphereFlag )
        { DrawAtoms = True;
        } else { 
          DrawStars = True;
        }
        DetermineClipping();
        VoxelsClean = False;
        BucketFlag = False;
    }
}


void SetRadiusTemperature( int flag )
{
    register int rad,irad,change;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    MaxAtomRadius = 0;
    DrawAtoms = False;
    DrawStars = False; 
    change = False;

    ForEachAtom
        if( (ptr->flag&SelectFlag) && (ptr->temp>0) )
        {   rad = (5*ptr->temp)>>1;
            if( rad>750 ) rad = 750;

            irad = (int)(Scale*rad);
            if( irad>MaxAtomRadius )
                MaxAtomRadius = irad;
            if (flag == SphereFlag )
            {    ptr->flag |= SphereFlag;
                ptr->flag &= ~StarFlag;
            } else 
            {    ptr->flag |= StarFlag;
                ptr->flag &= ~SphereFlag;
            }
            ptr->radius = rad;
            ptr->irad = irad;
            change = True;
        } else if( ptr->flag & SphereFlag )
        {    DrawAtoms = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        } else if( ptr->flag & StarFlag )
        {    DrawStars = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        }

    if( change )
    {   if (flag == SphereFlag )
        { DrawAtoms = True;
        } else { 
          DrawStars = True;
        }
        DetermineClipping();
        VoxelsClean = False;
        BucketFlag = False;
    }
}


void SetVanWaalRadius( int flag, int percent )
{
    register int rad,change;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register double ratio;

    if( !Database )
        return;

    MaxAtomRadius = 0;
    DrawAtoms = False;
    DrawStars = False; 
    change = False;
    ratio = (double)percent/100;

    ForEachAtom
        if( ptr->flag&SelectFlag )
        {   rad = ElemVDWRadius(ptr->elemno);
            if( percent<100 )
                rad = (int) (ratio*rad);
            ptr->irad = (int)(Scale*rad);
            ptr->radius = rad;
            change = True;

            if (flag == SphereFlag )
            {    ptr->flag |= SphereFlag;
                ptr->flag &= ~StarFlag;
            } else 
            {    ptr->flag |= StarFlag;
                ptr->flag &= ~SphereFlag;
            }
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        } else if( ptr->flag & SphereFlag )
        {    DrawAtoms = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        } else if( ptr->flag & StarFlag )
        {    DrawStars = True;
            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
        }

    if( change )
    {   if (flag == SphereFlag )
        { DrawAtoms = True;
        } else { 
          DrawStars = True;
        }
        DetermineClipping();
        VoxelsClean = False;
        BucketFlag = False;
    }
}


void DisableSpacefill( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    MaxAtomRadius = 0;
    DrawAtoms = False;
    DrawStars = False;
    
    ForEachAtom
        if( !(ptr->flag&SelectFlag) )
        {   if( ptr->flag&SphereFlag )
            {   if( ptr->irad>MaxAtomRadius )
                    MaxAtomRadius = ptr->irad;
                DrawAtoms = True;
            } 
            if( ptr->flag&StarFlag )
            {   if( ptr->irad>MaxAtomRadius )
                    MaxAtomRadius = ptr->irad;
                DrawStars = True;
            }
        } else if( ptr->flag&SphereFlag || ptr->flag&StarFlag )
        {    ptr->flag &= ~SphereFlag;
            ptr->flag &= ~StarFlag;
        }

    DetermineClipping();
    VoxelsClean = False;
    BucketFlag = False;
}


void EnableWireframe( int mask, int rad, int arad )
{
    register Bond __far *bptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int flag, irad, iarad;
    register int starrad, istarrad, change;

    if( !Database )
        return;

    DrawBonds = False;
    MaxBondRadius = 0;
    irad = (int)(Scale*rad);
    if ( arad < rad ) 
    {  iarad = (int)(Scale*arad);
    } else 
    {  iarad = irad;
    }

    ForEachBond
    {   flag = ZoneBoth? bptr->dstatom->flag & bptr->srcatom->flag
                       : bptr->dstatom->flag | bptr->srcatom->flag;

        if( flag&SelectFlag )
        {   DrawBonds = True;
            bptr->flag &= ~DrawBondFlag;
            bptr->flag |= mask;
            if( mask == CylinderFlag )
            {   if( irad>MaxBondRadius )
                    MaxBondRadius = irad;
                bptr->radius = rad;
                bptr->irad = irad;
                if ( arad < rad ) 
                {  bptr->aradius = arad;
                  bptr->iarad = iarad;
                } else 
                {  bptr->aradius = rad;
                  bptr->iarad = irad;
                }
            }
        } else if( bptr->flag&DrawBondFlag )
        {    DrawBonds = True;
             if( bptr->flag&CylinderFlag )
                 if( bptr->irad>MaxBondRadius )
                     MaxBondRadius = bptr->irad;
        }
    }

    if ( MarkAtoms & (AllAtomFlag | NonBondFlag) )
    { starrad = 75;
      istarrad = (int)(Scale*starrad);
      change = False;
      ForEachAtom
      { if ( (ptr->flag & SelectFlag) &&
          (MarkAtoms&AllAtomFlag) || (ptr->flag&NonBondFlag) ) 
        {    if( rad == 0 )
            {    ptr->flag |= StarFlag;
                ptr->flag &= ~SphereFlag;
            } else
            {    ptr->flag |= SphereFlag;
                ptr->flag &= ~StarFlag;
            }
            ptr->radius = starrad;
            ptr->irad = istarrad;
            change = True;
        }
      }
      if ( change )
      {  if ( rad == 0 )
         {    DrawStars = True;
         } else {
            DrawAtoms = True;
         }
         MaxAtomRadius = istarrad;
         DetermineClipping();
         VoxelsClean = False;
         BucketFlag = False;        
      }
    }
    DetermineClipping();
}


void DisableWireframe( void )
{
    register Bond __far *bptr;
    register int flag;

    if( !Database || !DrawBonds )
        return;

    DrawBonds = False;
    MaxBondRadius = 0;

    ForEachBond
    {   flag = ZoneBoth? bptr->dstatom->flag & bptr->srcatom->flag
                       : bptr->dstatom->flag | bptr->srcatom->flag;

        if( flag&SelectFlag )
        {   bptr->flag &= ~DrawBondFlag;
        } else if( bptr->flag&DrawBondFlag )
        {   DrawBonds = True;
            if( bptr->flag&CylinderFlag )
                if( bptr->irad>MaxBondRadius )
                    MaxBondRadius = bptr->irad;
        }
    }
    DetermineClipping();
}


void EnableBackbone( int mask, int rad, int arad )
{
    register Chain __far *chain;
    register Bond __far *bptr;
    register int flag,irad,iarad;

    if( !Database )
        return;

    irad = (int)(Scale*rad);
    if ( arad < rad ) {
      iarad = (int)(Scale*arad);
    } else {
      iarad = irad;
    }

    ForEachBack
    {   flag = ZoneBoth? bptr->dstatom->flag & bptr->srcatom->flag
                       : bptr->dstatom->flag | bptr->srcatom->flag;

        if( flag&SelectFlag )
        {   bptr->flag &= ~DrawBondFlag;
            bptr->flag |= mask;
            if( mask == CylinderFlag )
            {   bptr->radius = rad;
                bptr->irad = irad;
                if (arad < rad) {
                  bptr->aradius = arad;
                  bptr->iarad = iarad;
                } else {
                  bptr->aradius = rad;
                  bptr->iarad = irad;
                }
            }
        } 
    }
}


void DisableBackbone( void )
{
    register Chain __far *chain;
    register Bond __far *bptr;

    if( !Database )
        return;

    if( ZoneBoth )
    {   ForEachBack
            if( (bptr->dstatom->flag&bptr->srcatom->flag) & SelectFlag )
                bptr->flag &= ~DrawBondFlag;
    } else ForEachBack
        if( (bptr->dstatom->flag|bptr->srcatom->flag) & SelectFlag )
            bptr->flag &= ~DrawBondFlag;
}


void SetHBondStatus( int hbonds, int enable, int rad, int arad )
{
    register HBond __far *list;
    register HBond __far *ptr;
    register RAtom __far *src;
    register RAtom __far *dst;
    register int flag, irad, iarad;

    if( !Database )
        return;

    if( hbonds )
    {   if( enable && (Info.hbondcount<0) )
            CalcHydrogenBonds();
        list = Database->hlist;
    } else 
    {   if( enable && (Info.ssbondcount<0) )
            FindDisulphideBridges();
        list = Database->slist;
    }

    irad = (int)(Scale*rad);
    if ( arad < rad ) {
      iarad = (int)(Scale*arad);
    } else {
      iarad = irad;
    }

    for( ptr=list; ptr; ptr=ptr->hnext )
    {   src = ptr->src;  dst = ptr->dst;

        flag = ZoneBoth? src->flag&dst->flag : src->flag|dst->flag;
        if( flag & SelectFlag ) 
        {   ptr->flag &= ~DrawBondFlag;
            if( enable )
            {   if( rad )
                {   ptr->flag |= CylinderFlag;
                    ptr->radius = rad;
                    ptr->irad = irad;
                    if ( arad < rad ) {
                      ptr->aradius = arad;
                      ptr->iarad = iarad;
                    } else {
                      ptr->aradius = rad;
                      ptr->iarad = irad;
                    }
                } else ptr->flag |= WireFlag;
            }
        }
    }
}


void SetRibbonStatus( int enable, int flag, int width )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    /* Ribbons already disabled! */
    if( !enable && !DrawRibbon )
        return;

    if( Info.helixcount < 0 )
        DetermineStructure(False);

    DrawRibbon = False;
    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
            if( enable )
            {   if( group->flag & DrawKnotFlag )
                    DrawRibbon = True;
                
                for( ptr=group->alist; ptr; ptr=ptr->anext )
                    if( IsAlphaCarbon(ptr->refno) )
                    {   if( ptr->flag&SelectFlag )
                        {   group->flag &= ~DrawKnotFlag;
                            group->flag |= flag;
                            if( !width )
                            {   if( group->struc & (HelixFlag|SheetFlag) )
                                {      group->width = 380;
                                } else group->width = 100;
                            } else group->width = width;
                            DrawRibbon = True;
                        }
                        break;

                    } else if( IsSugarPhosphate(ptr->refno) )
                    {   if( ptr->flag&SelectFlag )
                        {   group->width = width? width : 720;
                            group->flag &= ~DrawKnotFlag;
                            group->flag |= flag;
                            DrawRibbon = True;
                        }
                        break;
                    }


            } else  /* Disable Ribbon */
                if( group->flag & DrawKnotFlag )
                {   for( ptr=group->alist; ptr; ptr=ptr->anext )
                        if( IsAlphaCarbon(ptr->refno) ||
                            IsSugarPhosphate(ptr->refno) )
                        {   if( ptr->flag&SelectFlag )
                                group->flag &= ~DrawKnotFlag;
                            break;
                        }
                    if( group->flag & DrawKnotFlag ) 
                        DrawRibbon = True;
                }
}


void SetRibbonCartoons( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    if( Info.helixcount < 0 )
        DetermineStructure(False);

    /* DrawBetaArrows = True; */
    /* CartoonHeight = 120;   */

    DrawRibbon = False;
    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
        {   if( group->flag & DrawKnotFlag )
                DrawRibbon = True;
                
            for( ptr=group->alist; ptr; ptr=ptr->anext )
                if( IsAlphaCarbon(ptr->refno) )
                {   if( ptr->flag&SelectFlag )
                    {   group->flag &= ~DrawKnotFlag;
                        if( group->struc & (HelixFlag|SheetFlag) )
                        {   group->flag |= CartoonFlag;
                            group->width = 380;
                        } else 
                        {   group->flag |= TraceFlag;
                            group->width = 100;
                        }
                        DrawRibbon = True;
                    }
                    break;

                } else if( IsSugarPhosphate(ptr->refno) )
                {   if( ptr->flag&SelectFlag )
                    {   group->flag &= ~DrawKnotFlag;
                        group->flag |= RibbonFlag;
                        group->width = 720;
                        DrawRibbon = True;
                    }
                    break;
                }
            }
}


void SetTraceTemperature( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int init,flag;
    register int min = 0,max = 0;
    register double coeff;

    if( !Database )
        return;

    flag = 0;
    init = False;
    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
            for( ptr=group->alist; ptr; ptr=ptr->anext )
                if( IsAlphaCarbon(ptr->refno) || IsSugarPhosphate(ptr->refno) )
                {   flag |= ptr->flag;
                    if( init )
                    {   if( ptr->temp<min ) 
                        {   min = ptr->temp;
                        } else if( ptr->temp>max )
                            max = ptr->temp;
                    } else
                    {   min = max = ptr->temp;
                        init = True;
                    }
                    break;
                }

    /* No groups selected! */
    if( !(flag&SelectFlag) )
        return;

    if( Info.helixcount < 0 )
        DetermineStructure(False);

    if( max != min )
    {   coeff = 200.0/(max-min);
    } else coeff = 0.0;

    DrawRibbon = False;
    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
        {   if( group->flag & DrawKnotFlag )
                DrawRibbon = True;
                
            for( ptr=group->alist; ptr; ptr=ptr->anext )
                if( IsAlphaCarbon(ptr->refno) || IsSugarPhosphate(ptr->refno) )
                {   if( ptr->flag&SelectFlag )
                    {   group->width = (int)(coeff*(ptr->temp-min))+50;
                        group->flag &= ~DrawKnotFlag;
                        group->flag |= TraceFlag;
                        DrawRibbon = True;
                    }
                    break;
                }
        }
}


void SetSurfaceStatus( int enable, int flag, int density )
{    
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    if( flag==DotFlag )
    {   if( enable )
        {    ForEachAtom
                if( ptr->flag & SelectFlag )
                    ptr->surf |= DotFlag;
        } else
        {    ForEachAtom
                if( ptr->flag & SelectFlag )
                    ptr->surf &= ~DotFlag;
        }
        CalculateDotSurface(density);
    }
}

void DisableSurface( int flag)
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    if( flag&DotFlag )
    {   ForEachAtom
            ptr->surf &= ~DotFlag;
        DeleteDotSurface();
    }
}


/*===========================*/
/* Atom Selection Functions! */
/*===========================*/


static void SelectBonds( void )
{
    register Bond __far *bptr;

    if( ZoneBoth )
    {   ForEachBond
           if( (bptr->srcatom->flag&bptr->dstatom->flag) & SelectFlag )
           {   bptr->flag |= SelectFlag;
           } else bptr->flag &= ~SelectFlag;
    } else
        ForEachBond
           if( (bptr->srcatom->flag|bptr->dstatom->flag) & SelectFlag )
           {   bptr->flag |= SelectFlag;
           } else bptr->flag &= ~SelectFlag;
}


void DisplaySelectCount( void )
{
    char buffer[40];

    if( FileDepth == -1 )
    {   InvalidateCmndLine();
        if( SelectCount==0 )
        {   WriteString(MsgStrs(TrnNoAtSel));
        } else if( SelectCount>1 )
        {   sprintf(buffer,"%ld",(long)SelectCount);
            WriteString(buffer);
            WriteString(MsgStrs(TrnManAtSel));
        } else WriteString(MsgStrs(TrnOneAtSel));
    }

    if( DisplayMode )
        ReDrawFlag |= RFRefresh;
    AdviseUpdate(AdvSelectCount);
}


void SelectArea( int mode, int count, int xo, int yo, int x, int y )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int x1,x2,y1,y2;
    register int Cx,Cy;

    if( !Database )
        return;

#ifdef INVERT
    yo = YRange - yo;
    y = YRange - y;
#endif

/*Adjust to cursor display*/
#ifdef IBMPC
    Cx = 0;
    Cy = 1;
#else
    Cx = 0;
    Cy = 0;
#endif
    
    x1 = MinFun(xo,x) - Cx;
    x2 = MaxFun(xo,x) - Cx;
    y1 = MinFun(yo,y) - Cy;
    y2 = MaxFun(yo,y) - Cy;

    if( DrawArea )
    {    AreaX1 = x1;
        AreaX2 = x2;
        AreaY1 = y1;
        AreaY2 = y2;
    }
  
    /* If count, perform a full atom selection */
    if( count )
    {    SelectCount = 0;
        if( mode==0 )
        {    ForEachAtom
               if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                {   ptr->flag |= SelectFlag;
                    SelectCount++;
                } else ptr->flag &= ~SelectFlag;
        } else if( mode==1 )
        {    ForEachAtom
               if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                {   ptr->flag |= SelectFlag;
                    SelectCount++;
                } else if( ptr->flag&SelectFlag )
                {   SelectCount++;
                }
        } else /*mode = -1 */
        {    ForEachAtom
               if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                {   ptr->flag &= ~SelectFlag;
                } else if( ptr->flag&SelectFlag )
                {   SelectCount++;
                }
        }
        DisplaySelectCount();
        SelectBonds();
    } else {    /*Go fast for quick redraw*/
        if( mode==0 )
        {    ForEachAtom
                if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                {   ptr->flag |= SelectFlag;
                } else ptr->flag &= ~SelectFlag;
        } else if( mode==1 )
        {    ForEachAtom
                if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                    ptr->flag |= SelectFlag;
        } else /*mode = -1 */
        {    ForEachAtom
                if( ptr->x>x1 && ptr->x<=x2 && ptr->y>y1 && ptr->y<=y2 )
                    ptr->flag &= ~SelectFlag;
        }
    }
}


void SelectZone( int mask )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    SelectCount = 0;
    ForEachAtom
        if( ptr->flag & mask )
        {   ptr->flag |= SelectFlag;
            SelectCount++;
        } else ptr->flag &= ~SelectFlag;
    DisplaySelectCount();
    SelectBonds();
}

static void RestrictHBonds( HBond __far *list )
{
    register HBond __far *ptr;
    register int flag;

    for( ptr=list; ptr; ptr=ptr->hnext ) {
        /* Ignore ZoneBoth setting! */
        flag = ptr->src->flag & ptr->dst->flag;
        if( ptr->flag & DrawBondFlag )
            if( !(flag & SelectFlag) )
                ptr->flag &= ~DrawBondFlag;
    }
}

static void RestrictSurface( void ) {
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int change;

    if( !Database )
        return;

    /* Dot surface only so far */
    if( !DotPtr )
        return;

    change = False;
    ForEachAtom
        if( (ptr->surf & DotFlag) && !(ptr->flag & SelectFlag) )
            change = True;

    if(change) {
        ForEachAtom
            if( (ptr->surf & DotFlag) && !(ptr->flag & SelectFlag) )
                ptr->surf &= ~DotFlag;
        SetSurfaceStatus(True,DotFlag,DotDensity);
    }

}

void RestrictZone( int mask )
{
    register Bond __far *bptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int flag;

    if( !Database )
        return;

    DrawAtoms = False;   MaxAtomRadius = 0;
    DrawStars = False;
    DrawBonds = False;   MaxBondRadius = 0;
    
    SelectCount = 0;
    ForEachAtom
        if( ptr->flag & mask )
        {   ptr->flag |= SelectFlag;
            SelectCount++;

            if( ptr->flag & SphereFlag )
            {   DrawAtoms = True;
                if( ptr->irad>MaxAtomRadius )
                    MaxAtomRadius = ptr->irad;
            }
            if( ptr->flag & StarFlag )
            {   DrawStars = True;
                if( ptr->irad>MaxAtomRadius )
                    MaxAtomRadius = ptr->irad;
            }
        } else 
        {   ptr->flag &= ~(SelectFlag|SphereFlag|StarFlag);
            if( ptr->label )
            {   DeleteLabel( (Label*)ptr->label );
                ptr->label = (void*)0;
            }
        }
    DisplaySelectCount();
    
    ForEachBond
    {   /* Ignore ZoneBoth setting! */
        flag = bptr->dstatom->flag & bptr->srcatom->flag;
        if( flag & SelectFlag )
        {   bptr->flag |= SelectFlag;
            if( bptr->flag&DrawBondFlag )
            {   DrawBonds = True;
                if( bptr->flag & CylinderFlag )
                    if( bptr->irad>MaxBondRadius )
                        MaxBondRadius = bptr->irad;
            } 
        } else bptr->flag &= ~(SelectFlag|DrawBondFlag);
    }

    ForEachBack
    {   /* Ignore ZoneBoth setting! */
        flag = bptr->dstatom->flag & bptr->srcatom->flag;
        if( !(flag&SelectFlag) )
            bptr->flag &= ~(SelectFlag|DrawBondFlag);
    }

    RestrictHBonds( Database->hlist);
    RestrictHBonds( Database->slist);

    if( DrawRibbon )
    {   DrawRibbon = False;
        for( chain=Database->clist; chain; chain=chain->cnext )
            for( group=chain->glist; group; group=group->gnext )
                if( group->flag & DrawKnotFlag )
                {   for( ptr=group->alist; ptr; ptr=ptr->anext )
                        if( IsAlphaCarbon(ptr->refno) ||
                            IsSugarPhosphate(ptr->refno) )
                        {   if( !(ptr->flag&SelectFlag) )
                                group->flag &= ~DrawKnotFlag;
                            break;
                        }
                    if( group->flag & DrawKnotFlag ) 
                        DrawRibbon = True;
                }
    }

    RestrictSurface();
    RestrictMonitors();

    DetermineClipping();
    VoxelsClean = False;
    BucketFlag = False;
}


void SelectZoneExpr( Expr *expr )
{
    register AtomSet __far *pset;
    register int i;

    if( !Database )
        return;

    SelectCount = 0;
    /*Shortcut for defined atomsets*/
    if( expr->type==OpMember )
    {    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
            for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
                for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                    QAtom->flag &= ~SelectFlag;

        pset = expr->rgt.set;
        while( pset )
        {   for( i=0; i<pset->count; i++ )
            {   QAtom = pset->data[i];
                QAtom->flag |= SelectFlag;
                SelectCount++;
            }
            pset = pset->next;
        }
    } else
    {    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
           for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
                for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                    if( EvaluateExpr(expr) )
                    {   QAtom->flag |= SelectFlag;
                        SelectCount++;
                    } else QAtom->flag &= ~SelectFlag;
    }

    DisplaySelectCount();
    SelectBonds();
}


void RestrictZoneExpr( Expr *expr )
{
    register Bond __far *bptr;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int flag;

    if( !Database )
        return;

    DrawAtoms = False;   MaxAtomRadius = 0;
    DrawStars = False;
    DrawBonds = False;   MaxBondRadius = 0;

    SelectCount = 0;
    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
        for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
            for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                if( EvaluateExpr(expr) )
                {   QAtom->flag |= SelectFlag;
                    SelectCount++;

                    if( QAtom->flag & SphereFlag )
                    {   DrawAtoms = True;
                        if( QAtom->irad>MaxAtomRadius )
                            MaxAtomRadius = QAtom->irad;
                    }
                    if( QAtom->flag & StarFlag )
                    {   DrawStars = True;
                        if( QAtom->irad>MaxAtomRadius )
                            MaxAtomRadius = QAtom->irad;
                    }
                }  else 
                {   QAtom->flag &= ~(SelectFlag|SphereFlag|StarFlag);
                    if( QAtom->label )
                    {   DeleteLabel( (Label*)QAtom->label );
                        QAtom->label = (void*)0;
                    }
                }
    DisplaySelectCount();

    ForEachBond
    {   /* Ignore ZoneBoth setting! */
        flag = bptr->dstatom->flag & bptr->srcatom->flag;
        if( flag & SelectFlag )
        {   bptr->flag |= SelectFlag;
            if( bptr->flag & CylinderFlag )
            {   DrawBonds = True;
                if( bptr->irad>MaxBondRadius )
                    MaxBondRadius = bptr->irad;
            } else if( bptr->flag&WireFlag )
                DrawBonds = True;
        } else bptr->flag &= ~(SelectFlag|DrawBondFlag);
    }

    ForEachBack
    {   /* Ignore ZoneBoth setting! */
        flag = bptr->dstatom->flag & bptr->srcatom->flag;
        if( !(flag&SelectFlag) )
            bptr->flag &= ~(SelectFlag|DrawBondFlag);
    }

    RestrictHBonds( Database->hlist);
    RestrictHBonds( Database->slist);

    if( DrawRibbon )
    {   DrawRibbon = False;
        for( chain=Database->clist; chain; chain=chain->cnext )
            for( group=chain->glist; group; group=group->gnext )
                if( group->flag & DrawKnotFlag )
                {   for( ptr=group->alist; ptr; ptr=ptr->anext )
                        if( IsAlphaCarbon(ptr->refno) ||
                            IsSugarPhosphate(ptr->refno) )
                        {   if( !(ptr->flag&SelectFlag) )
                                group->flag &= ~DrawKnotFlag;
                            break;
                        }
                    if( group->flag & DrawKnotFlag )
                        DrawRibbon = True;
                }
    }


    RestrictSurface();
    RestrictMonitors();

    DetermineClipping();
    VoxelsClean = False;
    BucketFlag = False;

}

void SelectAtom( int shift, RAtom __far *PAtom, Group __far *PGroup )
{        
    SelectCount = 0;
	for( QChain=Database->clist; QChain; QChain=QChain->cnext )
		for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
			for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
				if( QAtom->serno == PAtom->serno || 
					(ModelInclude && 
					 QGroup->serno == PGroup->serno &&
					 QAtom->serno - QGroup->alist->serno == 
                     PAtom->serno - PGroup->alist->serno) )
	                 {	if( shift == -1 )
					    {	QAtom->flag &= ~SelectFlag;
                        } else
					    {	QAtom->flag |= SelectFlag;
                            SelectCount++;
                        }
                        } else 
				        {	if( !shift ) 
					        {	QAtom->flag &= ~SelectFlag;
                            } else
					        {	if( QAtom->flag & SelectFlag)
                                    SelectCount++;
                            }
                        }

    DisplaySelectCount();
    SelectBonds();
}


void SelectGroup( int shift, Group __far *PGroup )
{    
    SelectCount = 0;
	for( QChain=Database->clist; QChain; QChain=QChain->cnext )
		for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
			for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
				if( QGroup->serno == PGroup->serno && 
					(ModelInclude || QGroup->model == PGroup->model) )
	            {	if( shift == -1 )
					{	QAtom->flag &= ~SelectFlag;
                    } else
					{	QAtom->flag |= SelectFlag;
                        SelectCount++;
                    }
                    } else 
				    {	if( !shift ) 
					{	QAtom->flag &= ~SelectFlag;
                    } else
					{	if( QAtom->flag & SelectFlag)
                            SelectCount++;
                    }
                }

    DisplaySelectCount();
    SelectBonds();
}


void SelectChain( int shift, Chain __far *PChain )
{    

    SelectCount = 0;
	for( QChain=Database->clist; QChain; QChain=QChain->cnext )
		for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
			for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
				if( QChain->ident == PChain->ident &&
					(ModelInclude || QChain->model == PChain->model) )
                {    if( shift == -1)
					{	QAtom->flag &= ~SelectFlag;
                    } else
					{	QAtom->flag |= SelectFlag;
                        SelectCount++;
                    }
                    } else 
				    {	if( !shift  ) 
					    {	QAtom->flag &= ~SelectFlag;
                        } else
					    {	if( QAtom->flag & SelectFlag)
                                SelectCount++;
                        }
                    }

    DisplaySelectCount();
    SelectBonds();
}


void ResetPrevSelection( void )
{   register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    ForEachAtom
        ptr->flag2 |= SelectFlag;
}


void StorePrevSelection( void )
{   
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    ForEachAtom
        if(ptr->flag&SelectFlag)
            ptr->flag2 |= SelectFlag;
        else
            ptr->flag2 &= ~SelectFlag;
}


void SelectPrevSelection( int save )
{
    register Byte temp;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database )
        return;

    SelectCount = 0;

    if( save )
    {   ForEachAtom
            if( ptr->flag2&SelectFlag )
            {   temp = (ptr->flag&SelectFlag);
                ptr->flag |= SelectFlag;
                SelectCount++;
                if( temp&SelectFlag )
                    ptr->flag2 |= SelectFlag;
                else
                    ptr->flag2 &= ~SelectFlag;
            } else
            {   temp = (ptr->flag&SelectFlag);
                ptr->flag &= ~SelectFlag;
                if( temp&SelectFlag )
                    ptr->flag2 |= SelectFlag;
                else
                    ptr->flag2 &= ~SelectFlag;
            }
        DisplaySelectCount();
    } else
    {   ForEachAtom
            if( ptr->flag2&SelectFlag )
            {   ptr->flag |= SelectFlag;
                SelectCount++;
            } else
                ptr->flag &= ~SelectFlag;
    }

    SelectBonds();
}


/*============================*/
/* Light and Colour Functions */
/*============================*/

#define AXELIGHT    12

#define Norm     nor = LiX*LiX + LiY*LiY + LiZ*LiZ;            \
                nor = sqrt( nor );                            \
                LightX = (int)((double)LiX/nor*LightLength);    \
                LightY = (int)((double)LiY/nor*LightLength);    \
                LightZ = (int)((double)LiZ/nor*LightLength);

void ChangeLight( int Default, int LiX, int LiY, int LiZ )
{    double nor;

    if( Default )
    {    LightX = (int)((double)1/sqrt(6)*LightLength);
        LightY = LightX;
        LightZ = (int)((double)2/sqrt(6)*LightLength);
        return;
    }

    Norm;

    if( (AbsFun(LightX) > 0) && (AbsFun(LightX) < AXELIGHT) )
    {    LightX = 0;
        Norm;
    } 

    if( (AbsFun(LightY) > 0) && (AbsFun(LightY) < AXELIGHT) )
    {    LightY = 0;
        Norm;
    } 

    if( (AbsFun(LightZ) > 0) && (AbsFun(LightZ) < AXELIGHT) )
    {    LightZ = 0;
        Norm;
    }

    if( AbsFun((AbsFun(LightX)-LightLength)) < AXELIGHT )
    {    if( LightX > 0 )
            LightX = LightLength;
         else
            LightX = -LightLength;
        LightY = LightZ = 0;
    } 

    if( AbsFun((AbsFun(LightY)-LightLength)) < AXELIGHT )
    {    if( LightY > 0 )
            LightY = LightLength;
         else
            LightY = -LightLength;
        LightX = LightZ = 0;
    } 

    if( AbsFun((AbsFun(LightZ)-LightLength)) < AXELIGHT )
    {    if( LightZ > 0 )
            LightZ = LightLength;
         else
            LightZ = -LightLength;
        LightY = LightX = 0;
    } 
}


int DefineShade( int r, int g, int b )
{
    register int d,dr,dg,db;
    register int dist,best;
    register int i;

    /* Already defined! */
    for( i=0; i<LastShade; i++ )
        if( Shade[i].refcount )
            if( (Shade[i].r==r)&&(Shade[i].g==g)&&(Shade[i].b==b) )
                return i;

    /* Allocate request */
    for( i=0; i<LastShade; i++ )
         if( !Shade[i].refcount )
         {   Shade[i].r = r;
             Shade[i].g = g;
             Shade[i].b = b;
             Shade[i].refcount = 0;
             return i;
         }

    InvalidateCmndLine();
    WriteString(MsgStrs(TrnNoShade));

    /* To avoid lint warning! */
    best = dist = 0;

    /* Nearest match */
    for( i=0; i<LastShade; i++ )
    {   dr = Shade[i].r - r;
        dg = Shade[i].g - g;
        db = Shade[i].b - b;
        d = dr*dr + dg*dg + db*db;
        if( !i || (d<dist) )
        {   dist = d;
            best = i;
        }
    }
    return best;
}


void ScaleColourMap( int count )
{
    register int i, r, g, b;
    register int fract;

    ScaleCount=0;
    for( i=0; i<LastShade; i++ )
        if( !Shade[i].refcount )
            ScaleCount++;

    /* If there are no shades free! */
    if( !ScaleCount ) ScaleCount = LastShade;

    if( count && (count<ScaleCount) )
        ScaleCount = count;

    if( ScaleCount == 1 )
    {   ScaleRef[i].r = 0;
        ScaleRef[i].g = 0;
        ScaleRef[i].b = 255;
        ScaleRef[i].shade = 0;
        ScaleRef[i].col = 0;
        return;
    }
    
    for( i=0; i<ScaleCount; i++ )
    {   fract = (int)((1023*i)/(ScaleCount-1));
        if( fract < 256 )
        {   r = 0;  g = fract;  b = 255;
        } else if( fract < 512 )
        {   r = 0;  g = 255;  b = 511-fract;
        } else if( fract < 768 )
        {   r = fract-512;  g = 255;  b = 0;
        } else /* fract < 1024 */                             
        {   r = 255;  g = 1023-fract;  b = 0;
        }
        ScaleRef[i].r = r;
        ScaleRef[i].g = g;
        ScaleRef[i].b = b;
        ScaleRef[i].shade = 0;
        ScaleRef[i].col = 0;
    }
}


void SetLutEntry( int i, int r, int g, int b )
{
    ULut[i] = True;
    RLut[i] = r;
    GLut[i] = g;
    BLut[i] = b;

#ifdef EIGHTBIT
    Lut[i] = i;
#else
    Lut[i] = ( (Card)((r<<8)|g)<<8 ) | b;
#endif
}


static double Power( double x, int y )
{
    register double result;

    result = x;
    while( y >= 1 )
    {   result *= x;
        y -= 1;
    }
    return result;
}


void DefineColourMap( void )
{
    register double diffuse,fade;
    register double temp,inten;
    register int col,r,g,b;
    register int i,j,k=0;

    for( i=0; i<LutSize; i++ )
        ULut[i] = False;

    if( !DisplayMode )
    {   SetLutEntry(BackCol,BackR,BackG,BackB);
        SetLutEntry(LabelCol,LabR,LabG,LabB);
        SetLutEntry(AxeCol,AxeR,AxeG,AxeB);
    } else SetLutEntry(BackCol,60,60,60);

    diffuse = 1.0 - Ambient;
    if( DisplayMode )
    {   for( i=0; i<ColourDepth; i++ )
        {   temp = (double)i/ColourMask;
            inten = diffuse*temp + Ambient;

            /* Unselected  [40,40,255] */
            /* Selected    [255,160,0] */
            /* Not Current [0,160,160] */
            /* Center      [255,160,255] */
            /* Bond Selected [255,255,255] */

            r = (int)(255*inten);
            g = (int)(160*inten);
            b = (int)(40*inten);

            /* Avoid Borland Compiler Warning! */
            /* Shade2Colour(0) == FirstCol     */
            SetLutEntry( FirstCol+i, b, b, r );
            SetLutEntry( Shade2Colour(1)+i, r, g, 0 );
            SetLutEntry( Shade2Colour(2)+i, 0, g, g );
            SetLutEntry( Shade2Colour(3)+i, r, g, r );
            SetLutEntry( Shade2Colour(4)+i, r, r, r );
        }
    } else
        for( i=0; i<ColourDepth; i++ )
        {   temp = (double)i/ColourMask;
            
            if( (ShadePower-50) )
                temp = pow(temp,exp((double)(ShadePower-50)*0.4/10));

            inten = diffuse*temp + Ambient;
            fade = 1.0-inten;

            if( FakeSpecular )
            {   temp = Power(temp,SpecPower);
                k = (int)(255*temp);
                temp = 1.0 - temp;
                inten *= temp;
                fade *= temp;
            }

            for( j=0; j<LastShade; j++ )
                if( Shade[j].refcount )
                {   col = Shade2Colour(j);
                    if( UseBackFade )
                    {   temp = 1.0-inten;
                        r = (int)(Shade[j].r*inten + fade*BackR); 
                        g = (int)(Shade[j].g*inten + fade*BackG);
                        b = (int)(Shade[j].b*inten + fade*BackB);
                    } else
                    {   r = (int)(Shade[j].r*inten); 
                        g = (int)(Shade[j].g*inten);
                        b = (int)(Shade[j].b*inten);
                    }

                    if( FakeSpecular )
                    {   r += k;
                        g += k;
                        b += k;
                    }
                    SetLutEntry( col+i, r, g, b );
                }
        }

    if( Interactive )
        AllocateColourMap();
}


void ResetColourMap( void )
{
    register int i;

#ifdef EIGHTBIT
    for( i=0; i<256; i++ )
        ULut[i] = False;
#endif

    SpecPower = 8;
    FakeSpecular = False;
    ShadePower = 50;
    Ambient = DefaultAmbient;
    UseBackFade = False;
    UseDotCol = DotColNone;
    LastSpecShade = ColourMask;

    BackR = BackG = BackB = 0;
    AxeR = AxeG = AxeB = 255;
    LabR = LabG = LabB = 255;
    DotR = DotG = DotB = 255;
    BoxCol = 0;
    UseDotCol = DotColNone;

    for( i=0; i<LastShade; i++ )
        Shade[i].refcount = 0;
    ScaleCount = 0;

    for (i=0; i<AltlDepth; i++)
      AltlColours[i] = 0;

}


void ColourBondNone( void )
{
    register Bond __far *bptr;

    if( Database )
        ForEachBond
            if( (bptr->flag&SelectFlag) && bptr->col )
            {   Shade[Colour2Shade(bptr->col)].refcount--;
                bptr->col = 0;
            }
}


void ColourBondAttrib( int r, int g, int b )
{
    register Bond __far *bptr;
    register int shade,col;

    if( Database )
    {   ForEachBond
            if( (bptr->flag&SelectFlag) && bptr->col )
                Shade[Colour2Shade(bptr->col)].refcount--;

        shade = DefineShade(r,g,b);
        col = Shade2Colour(shade);

        ForEachBond
            if( bptr->flag&SelectFlag )
            {   Shade[shade].refcount++;
                bptr->col = col;
            }
    }
}


void ColourBackNone( void )
{
    register Chain __far *chain;
    register Bond __far *bptr;
    register int flag;

    if( Database )
        ForEachBack
        {   flag = ZoneBoth? bptr->dstatom->flag & bptr->srcatom->flag
                           : bptr->dstatom->flag | bptr->srcatom->flag;

            if( flag&SelectFlag )
            {   bptr->flag |= SelectFlag;
                if( bptr->col )
                {   Shade[Colour2Shade(bptr->col)].refcount--;
                    bptr->col = 0;
                }
            } else bptr->flag &= ~SelectFlag;
        }
}


void ColourBackAttrib( int r, int g, int b )
{
    register int shade,col;
    register Chain __far *chain;
    register Bond __far *bptr;

    if( Database )
    {   ColourBackNone();
        shade = DefineShade(r,g,b);
        col = Shade2Colour(shade);

        ForEachBack
            if( bptr->flag&SelectFlag )
            {   Shade[shade].refcount++;
                bptr->col = col;
            }
    }
}


void ColourHBondNone( int hbonds )
{
    register HBond __far *list;
    register HBond __far *ptr;
    register RAtom __far *src;
    register RAtom __far *dst;

    if( !Database )
        return;

    list = hbonds? Database->hlist : Database->slist;

    if( ZoneBoth )
    {   for( ptr=list; ptr; ptr=ptr->hnext )
        {   src = ptr->src;  dst = ptr->dst;

            if( (src->flag&dst->flag) & SelectFlag )
            {   ptr->flag |= SelectFlag;
                if( ptr->col )
                {   Shade[Colour2Shade(ptr->col)].refcount--;
                    ptr->col = 0;
                }
            } else ptr->flag &= ~SelectFlag;
        }
    } else
        for( ptr=list; ptr; ptr=ptr->hnext )
        {   src = ptr->src;  dst = ptr->dst;

            if( (src->flag|dst->flag) & SelectFlag )
            {   ptr->flag |= SelectFlag;
                if( ptr->col )
                {   Shade[Colour2Shade(ptr->col)].refcount--;
                    ptr->col = 0;
                }
            } else ptr->flag &= ~SelectFlag;
        }
}


void ColourHBondType( void )
{
    register HBond __far *ptr;
    register ShadeRef *ref;
    register int i;

    if( !Database ) return;
    for( i=0; i<7; i++ )
        HBondShade[i].col = 0;

    if( Info.hbondcount < 0 )
    {   CalcHydrogenBonds();
    } else ColourHBondNone( True );

    for( ptr=Database->hlist; ptr; ptr=ptr->hnext )
        if( ptr->flag & SelectFlag )
        {   switch( ptr->offset )
            {   case(  2 ):  ref = HBondShade;     break;
                case(  3 ):  ref = HBondShade+1;   break;
                case(  4 ):  ref = HBondShade+2;   break;
                case(  5 ):  ref = HBondShade+3;   break;
                case( -3 ):  ref = HBondShade+4;   break;
                case( -4 ):  ref = HBondShade+5;   break;
                default:     ref = HBondShade+6;   break;
            }

            if( !ref->col )
            {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
            ptr->col = (Byte)ref->col;
        }
}


void ColourHBondAttrib( int hbonds, int r, int g, int b )
{
    register HBond __far *list;
    register HBond __far *ptr;
    register int col,shade;

    if( !Database )
        return;

    if( hbonds )
    {   if( Info.hbondcount < 0 )
        {   CalcHydrogenBonds();
        } else ColourHBondNone(True);
    } else
        if( Info.ssbondcount < 0 )
        {   FindDisulphideBridges();
        } else ColourHBondNone(False);


    shade = DefineShade(r,g,b);
    col = Shade2Colour(shade);

    list = hbonds? Database->hlist : Database->slist;
    for( ptr=list; ptr; ptr=ptr->hnext )
        if( ptr->flag & SelectFlag )
        {   Shade[shade].refcount++;
            ptr->col = col;
        }
}


void ColourRibbonNone( int flag )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    if( !Database )
        return;

    if( Info.helixcount < 0 )
        return;

    for( chain=Database->clist; chain; chain=chain->cnext )
        for( group=chain->glist; group; group=group->gnext )
            for( aptr=group->alist; aptr; aptr=aptr->anext )
                if( (aptr->flag&SelectFlag) && 
                    (IsAlphaCarbon(aptr->refno)||
                     IsSugarPhosphate(aptr->refno)) )
                {   if( (flag&RibColInside) && group->col1 )
                    {   Shade[Colour2Shade(group->col1)].refcount--;
                        group->col1 = 0;
                    }
                    if( (flag&RibColOutside) && group->col2 )
                    {   Shade[Colour2Shade(group->col2)].refcount--;
                        group->col2 = 0;
                    }
                    break;
                }
}


void ColourRibbonAttrib( int flag, int r, int g, int b )
{
    register int shade, col;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    if( Database )
    {   if( Info.helixcount >= 0 )
        {   ColourRibbonNone( flag );
        } else DetermineStructure(False);

        shade = DefineShade(r,g,b);
        col = Shade2Colour(shade);

        for( chain=Database->clist; chain; chain=chain->cnext )
            for( group=chain->glist; group; group=group->gnext )
                for( aptr=group->alist; aptr; aptr=aptr->anext )
                    if( (aptr->flag&SelectFlag) && 
                        (IsAlphaCarbon(aptr->refno)||
                         IsSugarPhosphate(aptr->refno)) )
                    {   if( flag & RibColInside )
                        {   Shade[shade].refcount++;
                            group->col1 = col;
                        }
                        if( flag & RibColOutside )
                        {   Shade[shade].refcount++;
                            group->col2 = col;
                        }
                        break;
                    }
    }
}


void ColourMonitNone( void )
{
    register Monitor *ptr;
    register int flag;

    if( Database )
        for( ptr=MonitList; ptr; ptr=ptr->next )
            if( ptr->col )
            {   flag = ZoneBoth? ptr->src->flag & ptr->dst->flag
                               : ptr->src->flag | ptr->dst->flag;
                if( flag & SelectFlag )
                {   Shade[Colour2Shade(ptr->col)].refcount--;
                    ptr->col = 0;
                }
            }
}


void ColourMonitAttrib( int r, int g, int b )
{
    register Monitor *ptr;
    register int shade,col;
    register int flag;

    if( !Database )
        return;

    ColourMonitNone();
    shade = DefineShade(r,g,b);
    col = Shade2Colour(shade);

    for( ptr=MonitList; ptr; ptr=ptr->next )
    {   flag = ZoneBoth? ptr->src->flag & ptr->dst->flag 
                       : ptr->src->flag | ptr->dst->flag;
        if( flag & SelectFlag )
        {   Shade[shade].refcount++;
            ptr->col = col;
        }
    }
}


void ColourDotsAttrib( int r, int g, int b )
{
    register int shade;

    DotR = r;
    DotG = g;
    DotB = b;

    if( UseDotCol==DotColAttrib )
    {   Shade[Colour2Shade(DotCol)].refcount--;
    }

    UseDotCol = DotColAttrib;

    shade = DefineShade(r,g,b);
    DotCol = Shade2Colour(shade);
    Shade[shade].refcount++;
}


void ColourBoxAttrib( int r, int g, int b )
{
    register int shade;

    if( BoxCol )
    {   Shade[Colour2Shade(BoxCol)].refcount--;
    }
    shade = DefineShade(r,g,b);
    BoxCol = Shade2Colour(shade);
    Shade[shade].refcount++;
}


/* Coulomb's Law */
#define CoulombScale  ((Long)(1<<12))
int CalculatePotential( Long x, Long y, Long z )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register Long dx,dy,dz;
    register double result;
    register Card dist;
    register Card max;


    /* Calculated charges have b-values < 0.0     */
    /* if( MinFun(MinMainTemp,MinHetaTemp) >= 0 ) */
    /*     CalculateCharges();                    */

    /* 8.0 Angstrom Cut Off */
    max = (Long)2000*2000;

    result = 0;
    ForEachAtom
    {   dx = ptr->xorg + ptr->fxorg - x;
        if( (dist=dx*dx) < max )
        {   dy = ptr->yorg + ptr->fyorg - y;
            if( (dist+=dy*dy) < max )
            {   dz = ptr->zorg + ptr->fzorg - z;
                if( (dist+=dz*dz) < max )
                    result += (double)(CoulombScale*ptr->temp) / isqrt(dist);
            }
        }
    }
    /* Dielectric Constant = 10.0 */
    /* (332.0*250.0)/(10.0*100.0) */
    result = Rint((result*83)/CoulombScale);
    return (int)result;
}


void ColourDotsPotential( void )
{
    register DotStruct __far *ptr;
    register int i,shade,result;
    register ShadeRef *ref;

    if( MinFun(MinMainTemp,MinHetaTemp) >= 0 )
    {   WriteString(MsgStrs(TrnNoPot));
        return;
    }

    UseDotCol = DotColPot;

    if( Database )
    {   for( i=0; i<8; i++ )
            PotentialShade[i].col = 0;

        /* Colour Dots None! */
        for( ptr=DotPtr; ptr; ptr=ptr->next )
            for( i=0; i<ptr->count; i++ )
            {    shade = Colour2Shade(ptr->col[i]);
                 Shade[shade].refcount--;
            }

        for( ptr=DotPtr; ptr; ptr=ptr->next )
            for( i=0; i<ptr->count; i++ )
            {   result = CalculatePotential( ptr->xpos[i],
                                             ptr->ypos[i],
                                             ptr->zpos[i] );

                /* Determine Colour Bucket */
                if( result >= 0 )
                {   if( result > 10 )
                    {      if( result > 24 )
                           {      ref = PotentialShade + 0;
                           } else ref = PotentialShade + 1;
                    } else if( result > 3 )
                           {      ref = PotentialShade + 2;
                           } else ref = PotentialShade + 3;
                } else 
                    if( result > -10 )
                    {      if( result > -3 )
                           {      ref = PotentialShade + 4;
                           } else ref = PotentialShade + 5;
                    } else if( result > -24 )
                           {      ref = PotentialShade + 6;
                           } else ref = PotentialShade + 7;

                if( !ref->col )
                {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                    ref->col = Shade2Colour(ref->shade);
                }
                Shade[ref->shade].refcount++;
                ptr->col[i] = ref->col;
            }
    }
}


static void ResetColourAttrib( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    ForEachAtom
        if( (ptr->flag&SelectFlag) && ptr->col )
            Shade[Colour2Shade(ptr->col)].refcount--;
}


void MonoColourAttrib( int r, int g, int b )
{
    register int shade,col;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( Database )
    {   ResetColourAttrib();
        shade = DefineShade(r,g,b);
        col = Shade2Colour(shade);

        ForEachAtom
            if( ptr->flag&SelectFlag )
            {   Shade[shade].refcount++;
                ptr->col = col;
            }
    }
}

void AddAltlColours( void )
{
    int i, ic;
    register ShadeRef *ref;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    /* Add colours for any alternate conformations used */    

    ForEachAtom
        if( ptr->flag&SelectFlag )
      {   if(! (ptr->altl == '\0' || ptr->altl == ' ') ){
            ic = (((int)(ptr->altl))&(AltlDepth-1))+1;
            i = (int)((Long)ScaleCount*(ic--)/(AltlDepth));

            if( i >= ScaleCount )
            {   ref = ScaleRef + (ScaleCount-1);
            } else if( i >= 0 )
            {   ref = ScaleRef + i;
            } else ref = ScaleRef;

            if( !(ref->col && Shade[ref->shade].refcount) )
            {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
             AltlColours[ic] = ref->col;
           }
    }
 }

void ScaleColourAttrib( int attr )
{
    register ShadeRef *ref;
    register int count, attrno, factor;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register Long temp;
    register int i;

    if( !Database ) return;

    switch( attr )
    {   case(ChainAttr):   attrno = Info.chaincount;   
                           factor = 1;
                           break;

        case(GroupAttr):   factor = MinMainRes;
                           attrno = MaxMainRes;
                           if( HetaGroups && HetaGroupCount )
                           {   if( MinHetaRes < factor )
                                   factor = MinHetaRes;
                               if( MaxHetaRes > attrno )
                                   attrno = MaxHetaRes;
                           } 
                           attrno -= (factor-1);
                           break;

        case(ModelAttr):   factor = MinModel;
                           attrno = MaxModel-MinModel+1;
                           break;

        case(AltAttr):     factor = 1;
                           attrno = AltlDepth;
                           break;

        case(ChargeAttr):
        case(TempAttr):    factor = MinMainTemp;
                           attrno = MaxMainTemp;
                           if( HetaGroups && HetaGroupCount )
                           {   if( MinHetaTemp < factor )
                                   factor = MinHetaTemp;
                               if( MaxHetaTemp > attrno )
                                   attrno = MaxHetaTemp;
                           }
                           attrno -= (factor-1);
                           break;

        default:           return;
    }

    if( attrno<2 )
    {   MonoColourAttrib(255,255,255);
        return;
    } 

    ResetColourAttrib();
    ScaleColourMap(attrno);

        switch( attr )
        {    case(ChainAttr):
                 count = 0;
                 for( chain=Database->clist; chain; chain=chain->cnext )
                 {   ref = &(ScaleRef[(count*ScaleCount)/attrno]);
                     if( !(ref->col && Shade[ref->shade].refcount) )
                     {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                         ref->col = Shade2Colour(ref->shade);
                     }
                     for( group=chain->glist; group; group=group->gnext )
                         for( ptr=group->alist; ptr; ptr=ptr->anext )
                             if( ptr->flag&SelectFlag )
                             {   Shade[ref->shade].refcount++;
                                 ptr->col = ref->col;
                             }
                     count++;
                 }
                 break;


         case(GroupAttr):
                 for( chain=Database->clist; chain; chain=chain->cnext )
                     for( group=chain->glist; group; group=group->gnext )
                     {   temp = (Long)ScaleCount*(group->serno-factor);
                         i = (int)(temp/attrno);

                         if( i >= ScaleCount )
                         {   ref = ScaleRef + (ScaleCount-1);
                         } else if( i >= 0 )
                         {   ref = ScaleRef + i;
                         } else ref = ScaleRef;

                         if( !(ref->col && Shade[ref->shade].refcount) )
                         {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                             ref->col = Shade2Colour(ref->shade);
                         }

                         for( ptr=group->alist; ptr; ptr=ptr->anext )
                             if( ptr->flag&SelectFlag )
                             {   Shade[ref->shade].refcount++;
                                 ptr->col = ref->col;
                             }
                     }
                 break;

         case(ModelAttr):
                 for( chain=Database->clist; chain; chain=chain->cnext )
                     for( group=chain->glist; group; group=group->gnext )
                     {   temp = (Long)ScaleCount*(group->model-factor);
                         i = (int)(temp/attrno);

                         if( i >= ScaleCount )
                         {   ref = ScaleRef + (ScaleCount-1);
                         } else if( i >= 0 )
                         {   ref = ScaleRef + i;
                         } else ref = ScaleRef;

                         if( !(ref->col && Shade[ref->shade].refcount) )
                         {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                             ref->col = Shade2Colour(ref->shade);
                         }

                         for( ptr=group->alist; ptr; ptr=ptr->anext )
                             if( ptr->flag&SelectFlag )
                             {   Shade[ref->shade].refcount++;
                                 ptr->col = ref->col;
                             }
                     }
                 break;


         case(AltAttr):
                 ForEachAtom
                     if( ptr->flag&SelectFlag )
             {   if (ptr->altl == '\0' || ptr->altl == ' ') i=0;
                         else i = (((int)(ptr->altl))&(AltlDepth-1))+1;
                         i = (int)((Long)ScaleCount*i/attrno);

                         if( i >= ScaleCount )
                         {   ref = ScaleRef + (ScaleCount-1);
                         } else if( i >= 0 )
                         {   ref = ScaleRef + i;
                         } else ref = ScaleRef;

                         if( !(ref->col && Shade[ref->shade].refcount) )
                         {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                             ref->col = Shade2Colour(ref->shade);
                         }
                         Shade[ref->shade].refcount++;
                         ptr->col = ref->col;
                     }
                 break;


                 
         case(TempAttr):
                 ForEachAtom
                     if( ptr->flag&SelectFlag )
                     {   i = (int)(((Long)ScaleCount*(ptr->temp-factor))
                                    /attrno);

                         if( i >= ScaleCount )
                         {   ref = ScaleRef + (ScaleCount-1);
                         } else if( i >= 0 )
                         {   ref = ScaleRef + i;
                         } else ref = ScaleRef;

                         if( !(ref->col && Shade[ref->shade].refcount) )
                         {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                             ref->col = Shade2Colour(ref->shade);
                         }
                         Shade[ref->shade].refcount++;
                         ptr->col = ref->col;
                     }
                 break;

        case(ChargeAttr):
                ForEachAtom
                     if( ptr->flag&SelectFlag )
                     {   i = (int)(((Long)ScaleCount*(ptr->temp-factor))
                                    /attrno);

                         if( i <= 0 )
                         {   ref = ScaleRef + (ScaleCount-1);
                         } else if( i < ScaleCount )
                         {   ref = ScaleRef + ((ScaleCount-1)-i);
                         } else ref = ScaleRef;

                         if( !(ref->col && Shade[ref->shade].refcount) )
                         {   ref->shade = DefineShade(ref->r,ref->g,ref->b);
                             ref->col = Shade2Colour(ref->shade);
                         }
                         Shade[ref->shade].refcount++;
                         ptr->col = ref->col;
                     }
                 break;
        }
           
    /* Now add colours for any alternate conformations used */    

    AddAltlColours();
    return;      

}



/*====================================*/
/*  Raster3D Color Record Processing  */
/*====================================*/

static int MatchNumber( int len, int value, char *mask )
{
    register char digit, template;
    register int result;
    register int i;

    result = True;
    for( i=0; i<len; i++ )
    {   digit = (value%10) + '0';
        template = mask[len-i];
        if( template==' ' )
        {   if( value ) result = False;
        } else if( !MatchChar(template,digit) )
            result = False;
        value /= 10;
    }
    return result;
}


void UserMaskAttrib( int fields )
{
    register MaskDesc *mptr;
    register char *temp, *name;
    register int shade, change;
    register int i, rad, match;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database ) return;

    if( !MaskCount )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(TrnNoCol));
        return;
    }

    /* Avoid Compiler Warning! */
    mptr = (MaskDesc*)0;
    match = False;

    change = False;
    ResetColourAttrib();
    if( fields & MaskColourFlag )
        for( i=0; i<MaskCount; i++ )
            MaskShade[i] = -1;

    if( fields & MaskRadiusFlag )
    {   MaxAtomRadius = 0;
        DrawAtoms = False;
        DrawStars = False;
    }

    ForEachAtom
    if( ptr->flag & SelectFlag )
    {   for( i=0; i<MaskCount; i++ )
        {   mptr = UserMask+i;
            temp = mptr->mask;
            match = True;

            if( !MatchChar(temp[13],chain->ident) ) match=False;
            if( !MatchChar(temp[9],ptr->altl) )     match=False;

            /* Atom Name */
            if( match )
            {   name = ElemDesc[ptr->refno];
                if( !MatchChar(temp[5],name[0]) ) match=False;
                if( !MatchChar(temp[6],name[1]) ) match=False;
                if( !MatchChar(temp[7],name[2]) ) match=False;
                if( !MatchChar(temp[8],name[3]) ) match=False;
            }

            /* Group Name */
            if( match )
            {   name = Residue[group->refno];
                if( !MatchChar(temp[10],name[0]) ) match=False;
                if( !MatchChar(temp[11],name[1]) ) match=False;
                if( !MatchChar(temp[12],name[2]) ) match=False;
            }

            if( match && (mptr->flags&SerNoFlag) )
                match = MatchNumber(4,ptr->serno,&temp[0]);
            if( match && (mptr->flags&ResNoFlag) )
                match = MatchNumber(3,group->serno,&temp[14]);
            if( match ) break;
        }

        if( fields & MaskColourFlag )
        {   if( match )
            {   if( MaskShade[i] == -1 )
                {   MaskShade[i] = DefineShade(mptr->r,mptr->g,mptr->b);
                    MaskColour[i] = Shade2Colour(MaskShade[i]);
                }
                Shade[MaskShade[i]].refcount++;
                ptr->col = MaskColour[i];
            } else
            {   shade = DefineShade(255,255,255);
                ptr->col = Shade2Colour(shade);
                Shade[shade].refcount++;
            }
        }

        if( fields & MaskRadiusFlag )
        {   rad = match? mptr->radius : 375;
            ptr->irad = (int)(Scale*rad);
            ptr->flag |= SphereFlag;
            ptr->radius = rad;

            if( ptr->irad>MaxAtomRadius )
                MaxAtomRadius = ptr->irad;
            change = True;
        }
    } else 
    {   if( ptr->flag & SphereFlag )
        {   DrawAtoms = True;
        if( ptr->irad>MaxAtomRadius )
            MaxAtomRadius = ptr->irad;
        }
        if( ptr->flag & StarFlag )
        {   DrawStars = True;
        if( ptr->irad>MaxAtomRadius )
            MaxAtomRadius = ptr->irad;
        }
     }

    if( change )
    {   DrawAtoms = True;
        DetermineClipping();
        VoxelsClean = False;
        BucketFlag = False;
    }
}


void CPKColourAttrib( void )
{
    register ShadeRef *ref;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int i;

    if( !Database ) return;
    for( i=0; i<CPKMAX; i++ )
        CPKShade[i].col = 0;
    ResetColourAttrib();
    ScaleColourMap(CPKMAX);

    ForEachAtom
        if( ptr->flag&SelectFlag )
        {   if( (ptr->elemno == 6 /*carbon*/) && !HasHydrogen )
                ref = CPKShade + 4; /*carbons white like hydrogens*/
            else
                ref = CPKShade + Element[ptr->elemno].cpkcol;

            if( !ref->col )
            {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
            ptr->col = ref->col;
        }

    AddAltlColours();

}


void AminoColourAttrib( void )
{
    register ShadeRef *ref;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int i;

    if( !Database ) return;
    for( i=0; i<13; i++ )
        AminoShade[i].col = 0;
    ResetColourAttrib();
    ScaleColourMap(13);

    ForEachAtom
        if( ptr->flag&SelectFlag )
        {   if( IsAmino(group->refno) )
            {   ref = AminoShade + AminoIndex[group->refno];
            } else ref = AminoShade+12;

            if( !ref->col )
            {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
            ptr->col = ref->col;
        }

    AddAltlColours();

}


void ShapelyColourAttrib( void )
{
    register ShadeRef *ref;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int i;

    if( !Database ) return;
    for( i=0; i<30; i++ )
        Shapely[i].col = 0;
    ResetColourAttrib();
    ScaleColourMap(30);

    ForEachAtom
        if( ptr->flag&SelectFlag )
        {   if( IsAminoNucleo(group->refno) )
            {   ref = Shapely + group->refno;
            } else ref = Shapely+30;

/*  Original Colour Scheme
 *
 *  ref = &(Shapely[26]);
 *  if( IsNucleo(group->refno) )
 *  {   ref = Shapely + group->refno;
 *  } else if( IsShapelyBackbone(ptr->refno) )
 *  {   ref = &(Shapely[24]);
 *  } else if( IsShapelySpecial(ptr->refno) )
 *  {   ref = &(Shapely[25]);
 *  } else if( IsAmino(group->refno) )
 *      ref = Shapely + group->refno;
 */

            if( !ref->col )
            {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
            ptr->col = ref->col;
        }

    AddAltlColours();

}


void StructColourAttrib( void )
{
    register ShadeRef *ref;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register int i;

    if( !Database )
        return;

    if( Info.helixcount < 0 )
        DetermineStructure(False);

    for( i=0; i<4; i++ )
        StructShade[i].col = 0;
    ResetColourAttrib();

    ForEachAtom
        if( ptr->flag&SelectFlag )
        {   if( group->struc & HelixFlag )
            {   ref = StructShade+1;
            } else if( group->struc & SheetFlag )
            {   ref = StructShade+2;
            } else if( group->struc & TurnFlag )
            {   ref = StructShade+3;
            } else ref = StructShade;

            if( !ref->col )
            {   ref->shade = DefineShade( ref->r, ref->g, ref->b );
                ref->col = Shade2Colour(ref->shade);
            }
            Shade[ref->shade].refcount++;
            ptr->col = ref->col;
        }

    AddAltlColours();

}


int IsCPKColour( RAtom __far *ptr )
{
    register ShadeRef *cpk;
    register ShadeDesc *col;

    cpk = CPKShade + Element[ptr->elemno].cpkcol;
    col = Shade + Colour2Shade(ptr->col);
    return( (col->r==cpk->r) && 
            (col->g==cpk->g) && 
            (col->b==cpk->b) );
}


int IsVDWRadius( RAtom __far *ptr )
{
    register int rad;

    if( ptr->flag & SphereFlag )
    {   rad = ElemVDWRadius( ptr->elemno );
        return( ptr->radius == rad );
    } else return False;
}


void DefaultRepresentation( void )
{   FILE *script;

    if( !Database )
        return;

    /* Ensure Drawing */
    ReDrawFlag |= RFRefresh | RFColour;
    if( Info.bondcount < 1 )
    {   EnableBackbone(CylinderFlag,80,64);
    } else EnableWireframe(WireFlag,0,0);
    CPKColourAttrib();

    /* Run Default Script */
    if( (ScriptFileFormat==DataFormat) && *DefaultScriptName && useDefaultScript)
    {   script = fopen(DefaultScriptName,"rb");
        LoadScriptFile(script,DefaultScriptName);
    }
}


void SetWorldSize( Card size )
{   register double newzoom;
    register Card oldworldsize;

    if( !MoleculeList )
        return;

    oldworldsize = WorldSize;
    WorldSize = size;
    WorldRadius = WorldSize>>1;
    DScale = 1.0/WorldSize;


    if( WorldSize!=oldworldsize )
    {   
        /* MaxZoom*DScale*Range*750 == 252 */
        MaxZoom = 0.336*WorldSize/Range;
        if( MaxZoom < 1.0 )
        {   DScale *= MaxZoom;
            MaxZoom = 1.0;
        }
        ZoomRange = Range;
        MaxZoom -= 1.0;

        /* Adjust Zoom if defined */
        if( oldworldsize!=1 )
        {   
            /* Get new DialValue to conserve Scale */
            newzoom = Zoom*WorldSize/oldworldsize;
            if( (newzoom-1.0) <= 0.0 )
            {   DialValue[3] = newzoom - 1.0;
                if( DialValue[3] < -1.0 ) DialValue[3] = -1.0;
            } else
            {   DialValue[3] = (newzoom - 1.0)/MaxZoom;
                if( DialValue[3] > 1.0 ) DialValue[3] = 1.0;
            }

            ReDrawFlag |= RFZoom;
            PrepareWorldZoom(0);
        } else
        {   /* Initialize Scale */
            Zoom = DialValue[3]+1.0; /* 1.0 */;
            Scale = Zoom*DScale*Range;
        }
    }
}


int GetWorldSize( void )
{   register MolSet __far *mptr, *mcur;
    register Card max;

    max = 1;

    if( !MoleculeList )
    {   /* do nothing */
    } else if( MoleculeList->next )
    {   mcur = CurMolSet;
        mptr = MoleculeList;
        while( mptr )
        {   SwitchMolecule(mptr);
            if( (LocalRadius<<1)>max )
                max = (LocalRadius<<1);
            mptr = mptr->next;
        }
        SwitchMolecule(mcur);
    } else
        max = LocalRadius<<1;

    return (int)max;
}


void InitialTransform( void )
{
    register Card dist,max;
    register double fdist,fmax;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;
    register Card ax, ay, az;
    register Long dx, dy, dz;

    dx = MaxX-MinX;   OrigCX = (dx>>1)+MinX;
    dy = MaxY-MinY;   OrigCY = (dy>>1)+MinY;
    dz = MaxZ-MinZ;   OrigCZ = (dz>>1)+MinZ;

    MaxX -= OrigCX;   MinX -= OrigCX;
    MaxY -= OrigCY;   MinY -= OrigCY;
    MaxZ -= OrigCZ;   MinZ -= OrigCZ;

    SideLen = MaxFun(dx,dy);
    if( dz>SideLen ) SideLen = dz;
    SideLen += 1500;  Offset = SideLen>>1;

    XOffset = 0;
    YOffset = 0;
    ZOffset = 0;

    MOffset[0] = 0.0;
    MOffset[1] = 0.0;
    MOffset[2] = 0.0;

    /* Inititalize World Translation */
    WLastTX = WTransValue[0];
    WLastTY = WTransValue[1];
    WLastTZ = WTransValue[2];

    ForEachAtom
    {   ptr->xorg -= OrigCX;
        ptr->yorg -= OrigCY;
        ptr->zorg -= OrigCZ;
        ptr->fxorg = 0;
        ptr->fyorg = 0;
        ptr->fzorg = 0;
    }

    if( Offset > 37836 )
    {   fmax = 0.0;
        ForEachAtom
        {   ax = (Card)AbsFun(ptr->xorg);
            ay = (Card)AbsFun(ptr->yorg);
            az = (Card)AbsFun(ptr->zorg);
            fdist = (double)ax*ax + 
                    (double)ay*ay + 
                    (double)az*az;
            if( fdist > fmax )
                fmax = fdist;
        }
    } else
    {   max = 1;
        ForEachAtom
        {   ax = (Card)AbsFun(ptr->xorg);
            ay = (Card)AbsFun(ptr->yorg);
            az = (Card)AbsFun(ptr->zorg);
            dist = ax*ax + ay*ay + az*az;
            if( dist > max )
                max = dist;
        }
        fmax = (double)max;
    }

    LocalRadius = ((Card)sqrt(fmax))+750;
    SetWorldSize(LocalRadius<<1);
}


void PrepareBondRotate( void )
{
    register Long   x, y, z;
    register Card dist,max,size;
    register double fdist,fmax;
    register Card ax, ay, az;
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *ptr;

    if( !Database ) return;

    /* Reset H-Bonds and SS-Bonds */
    ReclaimAllBonds();

    /* if( !FastDraw )
    {   FindDisulphideBridges();
        CalcHydrogenBonds();
    }*/

    /* Reset Surfaces */
    DisableSurface(AllSurfaceFlag);

    BondRotate();

    MinX = MinY = MinZ = -1;
    MaxX = MaxY = MaxZ = 1;
    ForEachAtom
    {   x = ptr->xorg + ptr->fxorg;
        if( x < MinX ) MinX = x;
        else if( x > MaxX ) MaxX = x;

        y = ptr->yorg + ptr->fyorg;
        if( y < MinY ) MinY = y;
        else if( y > MaxY ) MaxY = y;

        z = ptr->zorg + ptr->fzorg;
        if( z < MinZ ) MinZ = z;
        else if( z > MaxZ ) MaxZ = z;
    }

    if( -MinX > MaxX ) MaxX = -MinX;
        else MinX = -MaxX;
    if( -MinY > MaxY ) MaxY = -MinY;
        else MinY = -MaxY;
    if( -MinZ > MaxZ ) MaxZ = -MinZ;
        else MinZ = -MaxZ;

    x = MaxX-MinX;
    y = MaxY-MinY;
    z = MaxZ-MinZ;

    SideLen = MaxFun(x,y);
    if( z>SideLen ) SideLen = z;
    SideLen += 1500;  Offset = SideLen>>1;

    if( Offset > 37836 )
    {   fmax = 0.0;
        ForEachAtom
        {   ax = (Card)AbsFun(ptr->xorg + ptr->fxorg);
            ay = (Card)AbsFun(ptr->yorg + ptr->fyorg);
            az = (Card)AbsFun(ptr->zorg + ptr->fzorg);
            fdist = (double)ax*ax + (double)ay*ay + (double)az*az;
            if( fdist > fmax )
                fmax = fdist;
        }
    } else
    {   max = 1;
        ForEachAtom
        {   ax = (Card)AbsFun(ptr->xorg + ptr->fxorg);
            ay = (Card)AbsFun(ptr->yorg + ptr->fyorg);
            az = (Card)AbsFun(ptr->zorg + ptr->fzorg);
            dist = ax*ax + ay*ay + az*az;
            if( dist > max )
                max = dist;
        }
        fmax = (double)max;
    }

    size = ((Card)sqrt(fmax))+750;
    if( LocalRadius != size )
    {   double rc;
        LocalRadius = size;
        if( !MoleculeList->next || (MoleculeList->next&&(LocalRadius>WorldRadius)) )
            SetWorldSize(LocalRadius<<1);

        rc = sqrt((double)(CenX*CenX + CenY*CenY + CenZ*CenZ));
        if( rc>=(double)LocalRadius )
            CentreTransform(CurMolSet,0,0,0,False);
    }
}


void ReviseInvMatrix( void )
{
    /* The inverse of a rotation matrix
     * is its transpose, and the inverse
     * of Scale is 1.0/Scale [IScale]!
     */
    InvX[0] = IScale*RotX[0];
    InvX[1] = IScale*RotY[0];
    InvX[2] = IScale*RotZ[0];

    InvY[0] = IScale*RotX[1];
    InvY[1] = IScale*RotY[1];
    InvY[2] = IScale*RotZ[1];

    InvZ[0] = IScale*RotX[2];
    InvZ[1] = IScale*RotY[2];
    InvZ[2] = IScale*RotZ[2];
    ShadowTransform();
}



void RMat2RV( double *RX, double *RY, double *RZ, 
  double RMX[3], double RMY[3], double RMZ[3]  ) {
  
  double SRX, SRY, SRZ, TRX, TRY, TRZ;
  double NSum;
  double TSum;

  if (RMZ[0] < 1. ) {
    if (RMZ[0] > -1.) {
      SRY = asin(-RMZ[0])/PI;
    } else {
      SRY = .5;
    }
  } else {
    SRY = -.5;
  } 
  TRY = 1.-SRY;
  if ( TRY > 2. ) TRY -= 2.;
  TRZ = 1.;
  if (RMZ[0] > .99999999999995) {
    SRX = atan2(-RMX[1],RMY[1])/PI;
    TRX = SRX;
    SRZ = 0;
  } else {
    if (RMZ[0] < -.99999999999995 ) {
    SRX = atan2(RMX[1],RMY[1])/PI;
    TRX = SRX;
    SRZ = 0;
    } else {
      SRX = atan2(-RMZ[1],RMZ[2])/PI;
      TRX = 1.+SRX;
      if ( TRX > 2. ) TRX -= 2.;
      SRZ = atan2(RMY[0],RMX[0])/PI;
      TRZ = 1.+SRZ;
      if ( TRZ > 2. ) TRZ -= 2.;
    }
  }
  
  NSum = 0;
  TSum = 0;
  NSum += fabs(cos(SRX*PI)-cos((*RX)*PI)) + fabs(sin(SRX*PI)-sin((*RX)*PI))
    + fabs(cos(SRY*PI)-cos((*RY)*PI)) + fabs(sin(SRY*PI)-sin((*RY)*PI))
    + fabs(cos(SRZ*PI)-cos((*RZ)*PI)) + fabs(sin(SRZ*PI)-sin((*RZ)*PI));
  TSum += fabs(cos(TRX*PI)-cos((*RX)*PI)) + fabs(sin(TRX*PI)-sin((*RX)*PI))
    + fabs(cos(TRY*PI)-cos((*RY)*PI)) + fabs(sin(TRY*PI)-sin((*RY)*PI))
    + fabs(cos(TRZ*PI)-cos((*RZ)*PI)) + fabs(sin(TRZ*PI)-sin((*RZ)*PI));
  
  if (NSum < TSum) {
    *RX = SRX; *RY = SRY; *RZ = SRZ;
  } else {
    *RX = TRX; *RY = TRY; *RZ = TRZ;
  }
  
}


/* To use on CenterSet only */
static PrepareCenter( void )
{   
    RCenx = Scale*(RotX[0]*CenX + RotX[1]*CenY + RotX[2]*CenZ);
    RCeny = Scale*(RotY[0]*CenX + RotY[1]*CenY + RotY[2]*CenZ);
    RCenz = Scale*(RotZ[0]*CenX + RotZ[1]*CenY + RotZ[2]*CenZ);
}


void PrepareWorldZoom( int mode )
{   register MolSet __far *mptr, *mcur;
    register double orig;
    static double Zo[3];
    
    if( !MoleculeList )
        return;

    if( !mode )
    {   Zo[0] = Zo[1] = 0.0;
        Zo[2] = Wo[2];
    } else if( mode == 1 )
    {   if( !CenterSet )
        {   Zo[0] = Wo[0]; Zo[1] = Wo[1]; Zo[2] = Wo[2];
        } else
        {   mcur = CurMolSet;
            SwitchMolecule(CenterSet);
            PrepareCenter();
            Zo[0] = MOffset[0] + RCenx;
            Zo[1] = MOffset[1] + RCeny; 
            Zo[2] = MOffset[2] + RCenz;
            SwitchMolecule(mcur);
        }
    } else if( mode == 2 )
    {   Zo[0] = Wo[0]; Zo[1] = Wo[1]; Zo[2] = Wo[2];
    }

    ScaleFactor = 1.0;

    if( ReDrawFlag & RFReSize )
    {   if( Range != ZoomRange )
        {   orig = MaxZoom;

            /* MaxZoom*DScale*Range*750 == 252 */
            MaxZoom = 0.336*WorldSize/Range;
            ZoomRange = Range;
            if( MaxZoom < 1.0 )
            {   DScale *= MaxZoom;
                MaxZoom = 1.0;
            }
            MaxZoom -= 1.0;

            /* Handle Change in MaxZoom */
            if( DialValue[3]>0.0 && (int)MaxZoom )
            {   DialValue[3] *= orig/MaxZoom;
                if( DialValue[3]>1.0 )
                    DialValue[3] = 1.0;
            }
            /* Define Rotation Center on origin */ 
            Zo[0] = Zo[1] = Zo[2] = 0.0;
        }
    }

    if( ReDrawFlag & RFMagnify )
    {   double OldScale;

        OldScale = Scale;

        if( DialValue[3] <= 0.0 )
        {   Zoom = DialValue[3]+1.0;
            if( Zoom<0.1 ) Zoom=0.1;
        } else Zoom = (DialValue[3]*MaxZoom) + 1.0;

        Scale = Zoom*DScale*Range;
        ImageSize = (int)(Scale*WorldSize);
        if( ImageSize < 2 )
        {   ImageRadius = 1;
            ImageSize = 2;
        } else 
            ImageRadius = ImageSize>>1;
        IScale = 1.0/Scale;

        if( OldScale )
            ScaleFactor = Scale/OldScale;
    }

    if( ScaleFactor != 1.0 )
    {   
        Wo[0] = (Wo[0]-Zo[0])*ScaleFactor + Zo[0];
        Wo[1] = (Wo[1]-Zo[1])*ScaleFactor + Zo[1];
        Wo[2] = (Wo[2]-Zo[2])*ScaleFactor + Zo[2];

        if( WClip[0] != -999999. )
            WClip[0] = (WClip[0] - Zo[2])*ScaleFactor + Zo[2];
        if( WClip[1] != -999999. )
            WClip[1] = (WClip[1] - Zo[2])*ScaleFactor + Zo[2];
      
        mptr = MoleculeList;
        while( mptr )
        {   double M[3];
            GetMOffset(mptr,&M);
            M[0] = (M[0] - Zo[0])*ScaleFactor + Zo[0];
            M[1] = (M[1] - Zo[1])*ScaleFactor + Zo[1];
            M[2] = (M[2] - Zo[2])*ScaleFactor + Zo[2];
            SetMOffset(mptr,&M);
            MaxAtomRadius = 0;
            MaxBondRadius = 0;
            mptr = mptr->next;
       }
    }
}


void PrepareWorldRotate( int mode )
{   register MolSet __far *mptr, *mcur;
    double A[4][4], R[4][4], B[4][4];
    static double Ro[3];
    int i,j;
    
    if( !MoleculeList )
        return;

    mcur = CurMolSet;

    if( (WRotValue[0]==WLastRX) && 
        (WRotValue[1]==WLastRY) && 
        (WRotValue[2]==WLastRZ) )
        return;
    
    if( !mode )
    {   Ro[0] = Ro[1] = 0.0;
        Ro[2] = Wo[2];
    } else if( mode == 1 )
    {   if( !CenterSet )
        {   Ro[0] = Wo[0]; Ro[1] = Wo[1]; Ro[2] = Wo[2];
        } else
        {   SwitchMolecule(CenterSet);
            PrepareCenter();
            Ro[0] = MOffset[0] + RCenx;
            Ro[1] = MOffset[1] + RCeny; 
            Ro[2] = MOffset[2] + RCenz;
            /*SwitchMolecule(mcur);*/
        }
    } else if( mode == 2 )
    {   Ro[0] = Wo[0]; Ro[1] = Wo[1]; Ro[2] = Wo[2];
    } else if( mode == 3 )
    {   /* Do nothing */
    }


    /* Rotate World Axes */
    IdentityMatrix(A);
    for( i=0; i<3; i++ )
    {   for( j=0; j<3; j++ )
            A[i][j] = WAxes[i][j];
        A[i][3] = Wo[i];
    }

    A[0][3] -= Ro[0];
    A[1][3] -= Ro[1];
    A[2][3] -= Ro[2];
    
    if( ReDrawFlag&RFRotateX )
    {   RotXMatrix(R, PI*(WRotValue[0]-WLastRX));
        MultMatrix(R, A, B);
    } else
        CopyMatrix4(B, A);
    if( ReDrawFlag&RFRotateY )
    {   RotYMatrix(R, PI*(WRotValue[1]-WLastRY));
        MultMatrix(R, B, A);
    } else
        CopyMatrix4(A, B);
    if( ReDrawFlag&RFRotateZ )
    {   RotZMatrix(R, PI*(-WRotValue[2]+WLastRZ));
        MultMatrix(R, A, B);
    } else
        CopyMatrix4(B, A);

    for( i=0; i<3; i++ )
    {   for( j=0; j<3; j++ )
            WAxes[i][j] = B[i][j];
        Wo[i] = B[i][3];
    }

    Wo[0] = B[0][3] + Ro[0];
    Wo[1] = B[1][3] + Ro[1];
    Wo[2] = B[2][3] + Ro[2];
    
    /* Rotate each Molecule */
    mptr = MoleculeList;
    while( mptr )
    {   SwitchMolecule(mptr);

        IdentityMatrix(A);
        for( i=0; i<3; i++ )
        {   A[0][i] = RotX[i];
            A[1][i] = RotY[i];
            A[2][i] = RotZ[i];
            A[i][3] = MOffset[i];
        }

        A[0][3] -= Ro[0];
        A[1][3] -= Ro[1];
        A[2][3] -= Ro[2];
         
        if( ReDrawFlag&RFRotateX )
        {   RotXMatrix(R, PI*(WRotValue[0]-WLastRX));
            MultMatrix(R, A, B);
        } else
            CopyMatrix4(B, A);
        if( ReDrawFlag&RFRotateY )
        {   RotYMatrix(R, PI*(WRotValue[1]-WLastRY));
            MultMatrix(R, B, A);
        } else
            CopyMatrix4(A, B);
        /* Inversion Z rotation */          
        if( ReDrawFlag&RFRotateZ )
        {   RotZMatrix(R, PI*(-WRotValue[2]+WLastRZ));
            MultMatrix(R, A, B);
        } else
            CopyMatrix4(B, A);
    
        for( i=0; i<3; i++ )
        {   RotX[i] = B[0][i];
            RotY[i] = B[1][i];
            RotZ[i] = B[2][i];
        }

        MOffset[0] = B[0][3] + Ro[0];
        MOffset[1] = B[1][3] + Ro[1];
        MOffset[2] = B[2][3] + Ro[2];

        mptr = mptr->next;
    }
    SwitchMolecule(mcur);

    WLastRX = WRotValue[0];
    WLastRY = WRotValue[1];
    WLastRZ = WRotValue[2];
}


/* On CurMolSet only */
void PrepareMoleculeRotate( void )
{
    register double theta;
    register double cost, sint;
    register double x, y, z;
    register double oldcenx,oldceny,oldcenz;

    if( !(ReDrawFlag&(RFRotateX|RFRotateY|RFRotateZ)) )
        return;
    
    if( DialValue[0]!=LastRX )
    {   theta = PI*(DialValue[0]-LastRX);
        cost = cos(theta);  sint = sin(theta);
        LastRX = DialValue[0];

        if( CenX || CenY || CenZ )
        {   oldceny = Scale*(RotY[0]*CenX+RotY[1]*CenY+RotY[2]*CenZ);
            oldcenz = Scale*(RotZ[0]*CenX+RotZ[1]*CenY+RotZ[2]*CenZ);
        }

        y=RotY[0]; z=RotZ[0];
        RotY[0]=cost*y+sint*z; 
        RotZ[0]=cost*z-sint*y;

        y=RotY[1]; z=RotZ[1];
        RotY[1]=cost*y+sint*z;
        RotZ[1]=cost*z-sint*y;

        y=RotY[2]; z=RotZ[2];
        RotY[2]=cost*y+sint*z;
        RotZ[2]=cost*z-sint*y;

        if( CenX || CenY || CenZ )
        {   MOffset[1] -= Scale*(RotY[0]*CenX+RotY[1]*CenY+RotY[2]*CenZ)-oldceny;
            MOffset[2] -= Scale*(RotZ[0]*CenX+RotZ[1]*CenY+RotZ[2]*CenZ)-oldcenz;
        }
    }

    if( DialValue[1]!=LastRY )
    {   theta = PI*(DialValue[1]-LastRY);
        cost = cos(theta);  sint = sin(theta);
        LastRY = DialValue[1];

        if( CenX || CenY || CenZ )
        {   oldcenx = Scale*(RotX[0]*CenX+RotX[1]*CenY+RotX[2]*CenZ);
            oldcenz = Scale*(RotZ[0]*CenX+RotZ[1]*CenY+RotZ[2]*CenZ);
        }
        
        x=RotX[0]; z=RotZ[0];
        RotX[0]=cost*x+sint*z;
        RotZ[0]=cost*z-sint*x;

        x=RotX[1]; z=RotZ[1];
        RotX[1]=cost*x+sint*z;
        RotZ[1]=cost*z-sint*x;

        x=RotX[2]; z=RotZ[2];
        RotX[2]=cost*x+sint*z;
        RotZ[2]=cost*z-sint*x;

        if( CenX || CenY || CenZ )
        {   MOffset[0] -= Scale*(RotX[0]*CenX+RotX[1]*CenY+RotX[2]*CenZ)-oldcenx;
            MOffset[2] -= Scale*(RotZ[0]*CenX+RotZ[1]*CenY+RotZ[2]*CenZ)-oldcenz;
        }
    }

    if( DialValue[2]!=LastRZ )
    {   theta = PI*(DialValue[2]-LastRZ);
        cost = cos(theta);  sint = sin(theta);
        LastRZ = DialValue[2];

        if( CenX || CenY || CenZ )
        {   oldcenx = Scale*(RotX[0]*CenX+RotX[1]*CenY+RotX[2]*CenZ);
            oldceny = Scale*(RotY[0]*CenX+RotY[1]*CenY+RotY[2]*CenZ);
        }

        x=RotX[0]; y=RotY[0];
        RotX[0]=cost*x-sint*y;
        RotY[0]=cost*y+sint*x;

        x=RotX[1]; y=RotY[1];
        RotX[1]=cost*x-sint*y;
        RotY[1]=cost*y+sint*x;

        x=RotX[2]; y=RotY[2];
        RotX[2]=cost*x-sint*y;
        RotY[2]=cost*y+sint*x;

        if( CenX || CenY || CenZ )
        {   MOffset[0] -= Scale*(RotX[0]*CenX+RotX[1]*CenY+RotX[2]*CenZ)-oldcenx;
            MOffset[1] -= Scale*(RotY[0]*CenX+RotY[1]*CenY+RotY[2]*CenZ)-oldceny;
        }
    }
}


void PrepareWorldTranslate( void )
{   register MolSet __far *mptr;
    
    if( !MoleculeList )
        return;

    /* Translate World Origin */
    if( ReDrawFlag&RFTransX )
        Wo[0] += (WTransValue[0]-WLastTX)*Range;
    if( ReDrawFlag&RFTransY )
        Wo[1] += (WTransValue[1]-WLastTY)*Range;
    if( ReDrawFlag&RFTransZ )
        Wo[2] += (WTransValue[2]-WLastTZ)*Range;

    mptr = MoleculeList;
    while( mptr )
    {   double M[3];
        GetMOffset(mptr,&M);
        if( ReDrawFlag&RFTransX )
            M[0] += (WTransValue[0]-WLastTX)*Range;
        if( ReDrawFlag&RFTransY )
            M[1] += (WTransValue[1]-WLastTY)*Range;
        if( ReDrawFlag&RFTransZ )
            M[2] += (WTransValue[2]-WLastTZ)*Range;
        SetMOffset(mptr,&M);
        mptr = mptr->next;
    }

    WLastTX = WTransValue[0];
    WLastTY = WTransValue[1];
    WLastTZ = WTransValue[2];
}


void PrepareMoleculeTranslate( void )
{
    if( ReDrawFlag&RFTransX )
    {   MOffset[0] += (DialValue[4]-LastTX)*Range;
        LastTX = DialValue[4];
    }

    if( ReDrawFlag&RFTransY )
    {   MOffset[1] += (DialValue[5]-LastTY)*Range;
        LastTY = DialValue[5];
    }

    if( ReDrawFlag&RFTransZ )
    {   MOffset[2] += (DialValue[6]-LastTZ)*Range;
        LastTZ = DialValue[6];
    }
}


void ApplyTransform( void )
{
    register int temp;
    register double x, y, z;
    register int oldx,oldy,oldz;
    register Chain __far *chain;
    register Group __far *group;
    register HBond __far *hptr;
    register Bond __far *bptr;
    register RAtom __far *ptr;

    oldx = XOffset;
    oldy = YOffset;
    oldz = ZOffset;
         
    if( ReDrawFlag & RFRotate )
        if( UseShadow )
            ShadowTransform();

    /* Transform */
    if( ReDrawFlag & (RFRotate|RFMagnify|RFZoom) )
    {      
        MatX[0] = Scale*RotX[0]; 
        MatX[1] = Scale*RotX[1];
        MatX[2] = Scale*RotX[2];

        MatY[0] = Scale*RotY[0];
        MatY[1] = Scale*RotY[1];
        MatY[2] = Scale*RotY[2];

        MatZ[0] = Scale*RotZ[0];
        MatZ[1] = Scale*RotZ[1];
        MatZ[2] = Scale*RotZ[2];
        
        /* The inverse of a rotation matrix
         * is its transpose, and the inverse
         * of Scale is 1.0/Scale [IScale]!
         */
        if( UseShadow )
        {   InvX[0] = IScale*RotX[0]; 
            InvX[1] = IScale*RotY[0];
            InvX[2] = IScale*RotZ[0];

            InvY[0] = IScale*RotX[1];
            InvY[1] = IScale*RotY[1];
            InvY[2] = IScale*RotZ[1];

            InvZ[0] = IScale*RotX[2];
            InvZ[1] = IScale*RotY[2];
            InvZ[2] = IScale*RotZ[2];
        }

        RCenx = (double)CenX*MatX[0]+(double)CenY*MatX[1]+(double)CenZ*MatX[2];
        RCeny = (double)CenX*MatY[0]+(double)CenY*MatY[1]+(double)CenZ*MatY[2];
        RCenz = (double)CenX*MatZ[0]+(double)CenY*MatZ[1]+(double)CenZ*MatZ[2];
    }


    /* Get Screen Offsets */
    XOffset = Rint(MOffset[0]) + SRange;
    YOffset = Rint(MOffset[1]) + HRange;
    ZOffset = Rint(MOffset[2] + DRange);

    switch( ReDrawFlag )
    {   case(RFTransX):
                if( XOffset != oldx ) 
                {   temp = XOffset - oldx;
                    ForEachAtom ptr->x += temp;
                }
                break;

        case(RFTransY):
               if( YOffset != oldy ) 
               {   temp = YOffset - oldy;
                   ForEachAtom ptr->y += temp;
               }
               break;

        case(RFTransZ):
                if( ZOffset != oldz ) 
                {   temp = ZOffset - oldz;
                    ForEachAtom ptr->z += temp;
                }
                break;

        case(RFRotateX):
            ForEachAtom
            {   x = ptr->xorg + ptr->fxorg; 
                y = ptr->yorg + ptr->fyorg; 
                z = ptr->zorg + ptr->fzorg;
                ptr->y = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2]+MOffset[1])+HRange;
                ptr->z = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2]+MOffset[2]+DRange);
            }
            break;

        case(RFRotateY):
            ForEachAtom
            {   x = ptr->xorg + ptr->fxorg; 
                y = ptr->yorg + ptr->fyorg; 
                z = ptr->zorg + ptr->fzorg;
                ptr->x = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2]+MOffset[0])+SRange;
                ptr->z = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2]+MOffset[2]+DRange);
            }
            break;

        case(RFRotateZ):
            ForEachAtom
            {   x = ptr->xorg + ptr->fxorg; 
                y = ptr->yorg + ptr->fyorg; 
                z = ptr->zorg + ptr->fzorg;
                ptr->x = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2]+MOffset[0])+SRange;
                ptr->y = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2]+MOffset[1])+HRange;
            }
            break;

        default:
            /* This condition scales atomic radii! */
            if( (DrawAtoms || DrawStars) && (ReDrawFlag&(RFMagnify | RFZoom)) )
            {   ForEachAtom 
                {   x = ptr->xorg + ptr->fxorg; 
                    y = ptr->yorg + ptr->fyorg; 
                    z = ptr->zorg + ptr->fzorg;
                    ptr->x = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2]+MOffset[0])+SRange;
                    ptr->y = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2]+MOffset[1])+HRange;
                    ptr->z = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2]+MOffset[2]+DRange);
                    if( ptr->flag&SphereFlag )
                    {   ptr->irad = (int)(Scale*ptr->radius);
                        if( ptr->irad>MaxAtomRadius )
                            MaxAtomRadius = ptr->irad;
                    }
                }
            } else
                ForEachAtom 
                {   x = ptr->xorg + ptr->fxorg; 
                    y = ptr->yorg + ptr->fyorg; 
                    z = ptr->zorg + ptr->fzorg;
                    ptr->x = (int)(x*MatX[0]+y*MatX[1]+z*MatX[2]+MOffset[0])+SRange;
                    ptr->y = (int)(x*MatY[0]+y*MatY[1]+z*MatY[2]+MOffset[1])+HRange;
                    ptr->z = (int)(x*MatZ[0]+y*MatZ[1]+z*MatZ[2]+MOffset[2]+DRange);
                }

            if( ReDrawFlag & ( RFMagnify | RFZoom ) )
            {   if( DrawBonds )
                    ForEachBond
                        if( bptr->flag&CylinderFlag )
                        {   bptr->irad = (int)(Scale*bptr->radius);
                            bptr->iarad = (int)(Scale*bptr->aradius);
                            if( bptr->irad>MaxBondRadius )
                            MaxBondRadius = bptr->irad;
                        }

                for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
                    if( hptr->flag&CylinderFlag ) 
                    {   hptr->irad = (int)(Scale*hptr->radius);
                        hptr->iarad = (int)(Scale*hptr->aradius);
                    }

                for( hptr=Database->slist; hptr; hptr=hptr->hnext )
                    if( hptr->flag&CylinderFlag ) 
                    {   hptr->irad = (int)(Scale*hptr->radius);
                        hptr->iarad = (int)(Scale*hptr->aradius);
                    }

                ForEachBack
                    if( bptr->flag&CylinderFlag ) 
                    {   bptr->irad = (int)(Scale*bptr->radius);
                        bptr->iarad = (int)(Scale*bptr->aradius);
                    }
            }
    }

    DetermineClipping();
    if( UseScreenClip || ReDrawFlag!=RFRotateY )
        BucketFlag = False;
}


static double FrontWorld( void )
{   register MolSet __far *mptr;
    register double s;
          
    s = MOffset[2];

    if( MoleculeList->next )
    {   mptr = MoleculeList;
        while( mptr )
        {   double M[3];
            GetMOffset(mptr,&M);
            if( M[2] > s )
                s = M[2];
            mptr = mptr->next;
        }
    }
    return s + (int)(Scale*(WorldRadius-750));
}

/*==============*/
/* RasTop World */
/*==============*/

/* 
 * World Scaled Variables:
 * 
 * MOffset:    Molecule (MolSet) coordinates
 * Wo:         World Origin
 * Uo          Universe Origin - Not used -
 * Ro:         Rotation Origin
 * Zo:         Zoom Origin
 * WClip[0]    Slab Plane
 * WClip[1]    Depth Plane
 * 
 * After transformation, Offsets (X Y Z offset) are calculated by adding
 * the Screen Range (Width, High, Depth - W H D Range) to MOffset. 
 * The Width Range is corrected for stereo if necessary (SRange).
 */

void DrawWorld( void )
{
    register MolSet __far *mptr;
    register int FD_FontStroke;
    register int FD_UseShadow;
    register int SaveRD;
    register double QClip;
    
    if( !MoleculeList )
        return;

    ClearBuffers();
        
    /* Fast Draw */
    if( FastDraw )
    {    FD_FontStroke = FontStroke;
        FontStroke = 0;
        FD_UseShadow = UseShadow;
        UseShadow = 0;
    }
    
    /* Stereo */
    if( UseStereo )
        SRange = WRange>>1;
    else
        SRange = WRange;

    /* Correction for light on vectors */
    if( FakeSpecular )
        LastSpecShade = (int)(ColourMask*pow(0.60,(1/(double)(SpecPower+1))));
    else
        LastSpecShade = ColourMask;

    /* BackClipping */
    if( UseDepthCue )
    {   if( UseSlabPlane )
            QClip = WClip[0];
        else
            QClip = FrontWorld();
        DRange = -QClip + ImageSize;
    } else
    {   QClip = WClip[0];
        DRange = 10000.0 - Wo[2];
    }

    if( UseDepthPlane && (WClip[1]>(-DRange)) )
        DRange = - WClip[1];

    CurRenderSet = CurMolSet;
    mptr = MoleculeList;
    SaveRD = ReDrawFlag;
    while( mptr )
    {        
        /* See Hack in SwichtMolecule to keep DialValue[3] */
        SwitchMolecule(mptr);
        ReDrawFlag = SaveRD;

        if( ReDrawFlag & RFApply ) 
           ApplyTransform();

        if( UseDepthCue || UseSlabPlane )
            ShiftS = (Long)(MOffset[2]+ImageRadius-QClip);
        else
            ShiftS = 0;

        /* Get Clipping DialValues*/
        if( UseSlabPlane )
        {   DialValue[DialSlab] = (WClip[0]-MOffset[2])/(Scale*LocalRadius);
            if( DialValue[DialSlab] < -1.0 ) DialValue[DialSlab] = -1.0;
            if( DialValue[DialSlab] >  1.0 ) DialValue[DialSlab] =  1.0;
        }
        if( UseDepthPlane )
        {   DialValue[DialBClip] = (WClip[1] - MOffset[2])/(Scale*LocalRadius);
            if( DialValue[DialBClip] < -1.0 ) DialValue[DialBClip] = -1.0;
            if( DialValue[DialBClip] >  1.0 ) DialValue[DialBClip] =  1.0;
        }

        if( RefreshState )
             DrawFrame();
        
        mptr = mptr->next;
    }
    SwitchMolecule(CurRenderSet);
    CurRenderSet = (void __far*)0;

    /* Fast Draw */
    if( FastDraw )
    {    FontStroke = FD_FontStroke;
        UseShadow = FD_UseShadow;
    }
}


void SetWorldBack( void )
{
    /* No zoom change */

    if( WRotValueBack[0]!=0.0 )
    {   WRotValue[0] -= WRotValueBack[0];
        WRotValueBack[0] = 0.0;
        ReDrawFlag |= RFRotateX;
    }

    if( WRotValueBack[1]!=0.0 )
    {   WRotValue[1] -= WRotValueBack[1];
        WRotValueBack[1] = 0.0;
        ReDrawFlag |= RFRotateY;
    }

    if( WRotValueBack[2]!=0.0 )
    {   WRotValue[2] -= WRotValueBack[2];
        WRotValueBack[2] = 0.0;
        ReDrawFlag |= RFRotateZ;
    }

    PrepareWorldRotate(2);

    if( WTransValueBack[0]!=0.0 )
    {   WTransValue[0] = WTransValueBack[0];
        WLastTX = 0.0;
        WTransValueBack[0] = 0.0;
        ReDrawFlag |= RFTransX;
    }

    if( WTransValueBack[1]!=0.0 )
    {   WTransValue[1] = WTransValueBack[1];
        WLastTY = 0.0;
        WTransValueBack[1] = 0.0;
        ReDrawFlag |= RFTransY;
    }

    if( WTransValueBack[2]!=0.0 )
    {   WTransValue[2] = WTransValueBack[2];
        WLastTZ = 0.0;
        WTransValueBack[2] = 0.0;
        ReDrawFlag |= RFTransZ;
    }

    PrepareWorldTranslate();

    /* Keep FileModified False on following refresh */
    ReDrawFlag = 0;
}


void ResetWorldAxes( int save )
{   double theta;

    /* No zoom reset for file saving */
    if( !save && (DialValue[3] != 0.0) )
    {   DialValue[3] = 0.0;
        ReDrawFlag |= RFZoom;
        PrepareWorldZoom(2);
    }

    theta = atan2(WAxes[1][0],WAxes[0][0])/PI;
    /* Inversion Z Rotation! */
    if( theta!=0.0 )
    {   WRotValue[2] -= theta;
        while( WRotValue[2] < -1.0 )  WRotValue[2] += 2.0;
        while( WRotValue[2] > 1.0 )   WRotValue[2] -= 2.0;
        if( save )
            WRotValueBack[2] = -theta;
        else
            WRotValueBack[2] = 0.0;
        ReDrawFlag |= RFRotateZ;
        PrepareWorldRotate(2);
    } else
        WRotValueBack[2] = 0.0;


    theta = atan2(WAxes[2][0],WAxes[0][0])/PI;
    if( theta!=0.0 )
    {   WRotValue[1] += theta;
        while( WRotValue[1] < -1.0 )  WRotValue[1] += 2.0;
        while( WRotValue[1] > 1.0 )   WRotValue[1] -= 2.0;
        if( save )
            WRotValueBack[1] = theta;
        else
            WRotValueBack[1] = 0.0;
        ReDrawFlag |= RFRotateY;
        PrepareWorldRotate(2);
    } else
        WRotValueBack[1] = 0.0;

    theta = atan2(WAxes[2][1],WAxes[1][1])/PI;
    if( theta!=0.0 )
    {   WRotValue[0] += theta;
        while( WRotValue[0] < -1.0 )  WRotValue[0] += 2.0;
        while( WRotValue[0] > 1.0 )   WRotValue[0] -= 2.0;
        if( save )
            WRotValueBack[0] = theta;
        else
            WRotValueBack[0] = 0.0;
        ReDrawFlag |= RFRotateX;
        PrepareWorldRotate(2);
    } else
        WRotValueBack[0] = 0.0;


    /* Switch World Origin to Origin and Reset TransValues */
    WTransValue[0] = -Wo[0]/Range;
    WTransValue[1] = -Wo[1]/Range;
    WTransValue[2] = -Wo[2]/Range;
    WLastTX = 0.0;
    WLastTY = 0.0;
    WLastTZ = 0.0;

    if( save )
    {   WTransValueBack[0] = -WTransValue[0];
        WTransValueBack[1] = -WTransValue[1];
        WTransValueBack[2] = -WTransValue[2];
    } else
    {   WTransValueBack[0] = 0.0;
        WTransValueBack[1] = 0.0;
        WTransValueBack[2] = 0.0;
    }

    if( WTransValue[0]!=0.0 )
        ReDrawFlag |= RFTransX;
    if( WTransValue[1]!=0.0 )
        ReDrawFlag |= RFTransY;
    if( WTransValue[2]!=0.0 )
        ReDrawFlag |= RFTransZ;

    PrepareWorldTranslate();

    /* Reset Cursors */
    if( !save )
    {   WRotValue[0] = 0.0;
        WRotValue[1] = 0.0;
        WRotValue[2] = 0.0;
        WLastRX = 0.0;
        WLastRY = 0.0; 
        WLastRZ = 0.0;  
        WTransValue[0] = 0.0;
        WTransValue[1] = 0.0;
        WTransValue[2] = 0.0;
        WLastTX = 0.0;
        WLastTY = 0.0;
        WLastTZ = 0.0;
    }
}


void ResetMolSet( void )
{
    MOffset[0] = MOffset[1] = MOffset[2] = 0;
    RotX[0] = 1.0;  RotX[1] = 0.0;  RotX[2] = 0.0;
    RotY[0] = 0.0;  RotY[1] = 1.0;  RotY[2] = 0.0;
    RotZ[0] = 0.0;  RotZ[1] = 0.0;  RotZ[2] = 1.0;
    LastRX = LastRY = LastRZ = 0.0;
    LastTX = LastTY = LastTZ = 0.0;
    CenX = CenY = CenZ = ShiftS = 0;
    RCenx = RCeny = RCenz = 0.0;
    BLastRot = -999999.;
}


void ResetTransform( void )
{
    ResetMolSet();

    IdentityMatrix3(WAxes);
    Wo[0] = Wo[1] = Wo[2] = 0.0;

    WRotValue[0] = 0.0;
    WRotValue[1] = 0.0;
    WRotValue[2] = 0.0;
    WLastRX = WLastRY = WLastRZ = 0.0;
    WRotValueBack[0] = 0.0;
    WRotValueBack[1] = 0.0;
    WRotValueBack[2] = 0.0;

    WTransValue[0] = 0.0;
    WTransValue[1] = 0.0;
    WTransValue[2] = 0.0;
    WLastTX = WLastTY = WLastTZ = 0.0;
    WTransValueBack[0] = 0.0;
    WTransValueBack[1] = 0.0;
    WTransValueBack[2] = 0.0;

    ZRange = Range;
    DRange = 10000.0;

}


void InitialiseTransform( void )
{
    ResetColourMap();
    ResetTransform();

    ZoneBoth = True;
    HetaGroups = True;
    Hydrogens = True;
    MarkAtoms = 0;
    ModelInclude = False;

    Scale = 0.0;
    WorldSize = 1;
    Zoom = 1.0;
}

