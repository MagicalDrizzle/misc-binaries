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


/* script.c
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
 

#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define SCRIPTS
#include "script.h"
#include "molecule.h"
#include "command.h"
#include "cmndline.h"
#include "abstree.h"
#include "transfor.h"
#include "render.h"
#include "repres.h"
#include "graphics.h"
#include "pixutils.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"


#ifdef INVERT
#define InvertY(y) (y)
#else
#define InvertY(y) (-(y))
#endif

#ifdef INVERT
#define RestoreY(y) (-(y))
#else
#define RestoreY(y) (y)
#endif


/* Should match script precision */
#define IsAlmostZero(x) (((x)>-0.005)&&((x)<=0.005))

/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(aptr=group->alist;aptr;aptr=aptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext)
#define ForEachBack  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(bptr=chain->blist;bptr;bptr=bptr->bnext)


/* Special thanks to Profs Jane and David Richardson
 * for the following Kinemage colour lookup table */

#define MAXMAGECOL 20
static struct {
        int r, g, b;
        char *name;
        } KinemageCol[MAXMAGECOL] = {
    { 255,   0,   0, "red"        },  /*  1 */
    {  23, 255,   0, "green"      },  /*  2 */
    {  62,  62, 255, "blue"       },  /*  3 */
    {   0, 242, 226, "cyan"       },  /*  4 */
    { 255, 246,   0, "yellow"     },  /*  5 */
    { 255,   0, 234, "magenta"    },  /*  6 */
    { 255, 255, 255, "white"      },  /*  7 */
    { 255, 101, 117, "pink"       },  /*  8 */
    { 255,  93,   0, "orange"     },  /*  9 */
    { 140,  54, 255, "purple"     },  /* 10 */
    {  58, 144, 255, "skyblue"    },  /* 11 */
    { 175, 117,  89, "brown"      },  /* 12 */
    { 125, 125, 125, "gray"       },  /* 13 */
    { 255, 156,   0, "gold"       },  /* 15 */
    { 246, 246, 117, "yellowtint" },  /* 16 */
    {   0, 250, 109, "seagreen"   },  /* 17 */
    { 255, 171, 187, "pinktint"   },  /* 18 */
    { 175, 214, 255, "bluetint"   },  /* 19 */
    { 152, 255, 179, "greentint"  },  /* 20 */
    { 255,   0, 101, "hotpink"    }   /* 21 */
        };


#define DatWirFlag  (Long)0x10000
#define DatDasFlag  (Long)0x20000
#define DatCylFlag  (Long)0x40000

typedef struct {
        Long datum;
        Long count;
    } FreqEntry;

#define FREQSIZE  8
static FreqEntry Freq[FREQSIZE];

static RAtom __far *MagePrev;
static char *MageCol;
static FILE *OutFile;
static int SelectAll;



/*=======================*/
/*  Function Prototypes  */
/*=======================*/

static void IncFreqTable( Long );
static Long GetBondDatum( Bond __far* );
static Long GetHBondDatum( HBond __far* );
static int FetchBondInfo( RAtom __far*, RAtom __far* );

static void WriteMolScriptAtomSel( Chain __far*, Group __far*, RAtom __far* );

static void WriteScriptDatum( char*, Long );
static void WriteScriptSelectBond( RAtom __far*, RAtom __far* );
static void WriteScriptHBonds( char*, HBond __far* );

static int CheckKinemageChain( RAtom __far*, RAtom __far*, Chain __far* );
static void OutputKinemageVector( RAtom __far*, RAtom __far*, int );
static void WriteKinemageBonds( Chain __far* );
static void WriteKinemageSpheres( Chain __far* );
static void WriteKinemageLabels( Chain __far* );



static void FatalScriptError( char *ptr )
{
    InvalidateCmndLine();
    WriteString(MsgStrs(ScrNoFile));
    WriteString( ptr );  WriteString("'!\n");
}


static void ResetFreqTable( void )
{
    register int i;

    for( i=0; i<FREQSIZE; i++ )
        Freq[i].count = 0;
}


static void IncFreqTable( Long datum )
{
    register Long count;
    register int i;

    for( i=0; i<FREQSIZE; i++ )
        if( !Freq[i].count )
        {   Freq[i].datum = datum;
            Freq[i].count = 1;
            return;
        } else if( Freq[i].datum == datum )
        {   count = Freq[i].count+1;
            while( i && (Freq[i-1].count<=count) )
            {   Freq[i] = Freq[i-1];  
                i--;
            }
            Freq[i].datum = datum;
            Freq[i].count = count;
            return;
        }

    /* Replace Singletons! */
    if( Freq[FREQSIZE-1].count == 1 )
        Freq[FREQSIZE-1].datum = datum;
}


static Long GetBondDatum( Bond __far *bptr )
{
    if( bptr->flag & CylinderFlag )
    {   return( DatCylFlag | bptr->radius );
    } else if( bptr->flag & WireFlag )
    {   return DatWirFlag;
    } else if( bptr->flag & DashFlag )
    {   return DatDasFlag;
    } else return (Long)0;
}

static Long GetHBondDatum( HBond __far *bptr )
{
    if( bptr->flag & CylinderFlag )
    {   return( DatCylFlag | bptr->radius );
    } else if( bptr->flag & WireFlag )
    {   return DatWirFlag;
    } else return (Long)0;
}


/* Used by FetchBondInfo! */
static Chain __far *schn;
static Chain __far *dchn;
static Group __far *sgrp;
static Group __far *dgrp;

static int FetchBondInfo( RAtom __far *src, RAtom __far *dst )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    /* Determine Chain and Groups */
    schn = dchn = (Chain __far*)0;
    sgrp = dgrp = (Group __far*)0;

    for( chain=Database->clist; chain; chain=chain->cnext )
    {   for( group=chain->glist; group; group=group->gnext )
        {   for( aptr=group->alist; aptr; aptr=aptr->anext )
            {   if( aptr == src )
                {   schn = chain;
                    sgrp = group;
                }
                if( aptr == dst ) 
                {   dchn = chain;
                    dgrp = group;
                }
            }
            if( sgrp && dgrp ) 
                return True;
        }
    }
    return False;
}



/*==============================*/
/*  Molscript Input Generation  */
/*==============================*/

#ifdef UNUSED
static void WriteMolScriptColour( int r, int g, int b )
{
    fprintf(OutFile," rgb %#g %#g %#g",r/255.0,g/255.0,b/255.0);
}
#endif


static void WriteMolScriptAtomSel( Chain __far *chain,
                                   Group __far *group,
                                   RAtom __far *aptr )
{
    register char *ptr;
    register int i;

    fputs("require atom ",OutFile);
    ptr = ElemDesc[aptr->refno];
    for( i=0; ptr[i]&& i<4; i++ )
        if( ptr[i]=='*' )
        {   fputc('\'',OutFile);
        } else if( ptr[i]!=' ' )
            fputc(ptr[i],OutFile);

    fputs(" and in residue ",OutFile);
    if( chain->ident!=' ' && !isdigit(chain->ident) )
        fputc(chain->ident,OutFile);
    fprintf(OutFile,"%d",group->serno);
}


static void WriteMolScriptAtoms( void )
{

    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register int count,vdw;
    register long total;

#ifdef UNUSED
    register ShadeDesc *shade;

    ForEachAtom
        if( aptr->flag &SphereFlag )
        {   fputs("set atomcolour ",OutFile);
            WriteMolScriptAtomSel(chain,group,aptr);
            shade = Shade + Colour2Shade(aptr->col);
            WriteMolScriptColour(shade->r,shade->g,shade->b);
            fputs(";\n",OutFile);

            fputs("set atomradius ",OutFile);
            WriteMolScriptAtomSel(chain,group,aptr);
            fprintf(OutFile," %#g",aptr->radius/250.0);
            fputs(";\n",OutFile);
        }
#endif

    vdw = True;
    total = 0;
    count = 0;
    ForEachAtom
        if( aptr->flag & SphereFlag )
        {   if( vdw ) vdw = IsVDWRadius(aptr);
            total += aptr->radius;
            count++;
        }

    if( !count )
        return;

    if( !vdw )
    {   /* Average sphere radius! */
        fprintf(OutFile,"set atomradius atom * %#g;\n",
                (double)total/(250.0*count));
    }

    ForEachAtom
        if( aptr->flag & SphereFlag )
        {   fputs("cpk ",OutFile);
            WriteMolScriptAtomSel(chain,group,aptr);
            fputs(";\n",OutFile);
        }
}


static void WriteMolScriptBonds( void )
{
    register Bond __far *bptr;

    ForEachBond
    {    if( bptr->flag & WireFlag )
         {   FetchBondInfo(bptr->srcatom,bptr->dstatom);
             fputs("bonds ",OutFile);
             WriteMolScriptAtomSel(schn,sgrp,bptr->srcatom);
             fputs("\n      ",OutFile);
             WriteMolScriptAtomSel(dchn,dgrp,bptr->dstatom);
             fputs(";\n",OutFile);
         } else if( bptr->flag & CylinderFlag )
         {   FetchBondInfo(bptr->srcatom,bptr->dstatom);
             fputs("ball-and-stick ",OutFile);
             WriteMolScriptAtomSel(schn,sgrp,bptr->srcatom);
             fputs("\n               ",OutFile);
             WriteMolScriptAtomSel(dchn,dgrp,bptr->dstatom);
             fputs(";\n",OutFile);
         }
    }
}


static void WriteMolScriptLabels( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *label;
    register unsigned char *ptr;
    unsigned char buffer[80];

    ForEachAtom
    {   if( aptr->label )
        {   /* Atom Label */
            label = (Label*)aptr->label;
            FormatLabel(chain,group,aptr,label->label,buffer);

            fputs("label ",OutFile);
            WriteMolScriptAtomSel(chain,group,aptr);
            fputs(" \"",OutFile);
            for( ptr=buffer; *ptr; ptr++ )
                if( *ptr!='%' ) fputc(*ptr,OutFile);
            fputs("\";\n",OutFile);
        }
    }
}


static void MolScriptSegment( char *ptr, int src, int dst, int chain )
{   
    if( (chain!=' ') && !isdigit(chain) ) 
    {   fprintf(OutFile,"  %s from %c%d to %c%d;\n",ptr,chain,src,chain,dst);
    } else fprintf(OutFile,"  %s from %d to %d;\n",ptr,src,dst);
}


int WriteMolScriptFile( char *name )
{
    register double temp;
    double psi, phi, theta;
    register Chain __far *chain;
    register Group __far *group;
    register Group __far *next;
    register Group __far *prev;
    register int flag,len;
    register char *ptr;

    if( !Database )
        return(False);

    OutFile = fopen(name,"w");
    if( !OutFile )
    {   FatalScriptError(name);
        return(False);
    }
    fprintf(OutFile,"! File: %s\n",name);
    fprintf(OutFile,"! Creator: %s Version %s\n\n", VER_NAME, VERSION);
    fputs("! Version: MolScript v1.3\n\n",OutFile);

    fputs("plot\n",OutFile);
    if( BackR || BackG || BackB )
    {   fputs("  background rgb ",OutFile);
        fprintf(OutFile,"%#g ",  BackR/255.0);
        fprintf(OutFile,"%#g ",  BackG/255.0);
        fprintf(OutFile,"%#g;\n",BackB/255.0);
    }
    temp = 0.004/Scale;
    fprintf(OutFile,"  window %g;\n",temp*Range);
    if( UseSlabPlane )
        fprintf(OutFile,"  slab %g;\n",SideLen/250.0);
    fputc('\n',OutFile);

    fprintf(OutFile,"  read mol \"%s\";\n",Info.filename);
    fputs("  transform atom *\n",OutFile);
    fputs("    by centre position atom *\n",OutFile);
    fputs("    by rotation x 180.0",OutFile);

    /*
    phi = Rad2Deg*asin(RotX[2]);
    if( (int)phi == 90 )
    {   theta = -Rad2Deg*atan2(RotY[0],RotY[1]);
        psi = 0;
    } else if( (int)phi == -90 )
    {   theta = Rad2Deg*atan2(RotY[0],RotY[1]);
        psi = 0;
    } else 
    {   theta = Rad2Deg*atan2(RotY[2],RotZ[2]);
        psi =  -Rad2Deg*atan2(RotX[1],RotX[0]);
    }
    */
    
    phi = psi = theta = 0.0;
    RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
    theta *= 180.;
    phi *= 180.;
    psi *= 180.;


    if( (int)theta ) fprintf(OutFile,"\n    by rotation x %#g",InvertY(-theta));
    if( (int)phi )   fprintf(OutFile,"\n    by rotation y %#g",phi);
    if( (int)psi )   fprintf(OutFile,"\n    by rotation z %#g",InvertY(psi));

    if( UseSlabPlane || (XOffset!=WRange) || (YOffset!=HRange) )
    {   fputs("\n    by translation ",OutFile);
        fprintf(OutFile,"%#g ",(XOffset-WRange)*temp);
        fprintf(OutFile,"%#g ",-(YOffset-HRange)*temp);
        if( UseSlabPlane )
        {   temp = (1.0-DialValue[DialSlab])/500.0;
            fprintf(OutFile,"%#g",SideLen*temp);
        } else fputs("0.0",OutFile);
    }
    fputs(";\n\n",OutFile);

    /* fputs("  trace amino-acids;\n",OutFile); */

    if( Database->clist )
    {   if( Info.helixcount < 0 )
            DetermineStructure( False );

        for( chain=Database->clist; chain; chain=chain->cnext )
        {   prev = (Group __far*)0;
            for( group=chain->glist; group && group->gnext; group=next )
            {   next = group->gnext;
                if( next->serno < group->serno )
                {   if( prev && prev!=group )
                        MolScriptSegment("coil",prev->serno,group->serno,
                                                chain->ident);
                    prev = (Group __far*)0;
                    continue;
                }
                flag = group->struc & next->struc;

                if( flag&HelixFlag )
                {   flag = HelixFlag;
                    ptr = "helix";
                } else if( flag&SheetFlag )
                {   flag = SheetFlag;
                    ptr = "strand";
                } else 
                {   if( flag&TurnFlag )
                    {   fputs("  turn residue ",OutFile);
                        if( chain->ident != ' ' )
                            fputc(chain->ident,OutFile);
                        fprintf(OutFile,"%d;\n",group->serno);
                    }
                    if( !prev ) prev = group;
                    continue;
                }

                len = 2;  /* Determine Structure Length */
                while( next->gnext && (next->gnext->struc&flag)
                           && (next->serno<=next->gnext->serno) )
                {   next = next->gnext;
                    len++;
                }

                if( len>2 )
                {   if( prev && prev!=group ) /* MolScript coil or turn? */
                       MolScriptSegment("coil",prev->serno,group->serno,
                                              chain->ident);
                    MolScriptSegment(ptr,group->serno,next->serno,
                                         chain->ident);
                    prev = next;
                } 
            }

            if( prev && prev!=group )  /* C-terminal coil/turn */
                MolScriptSegment("coil",prev->serno,group->serno,chain->ident);
        }
    }


    WriteMolScriptAtoms();
    WriteMolScriptBonds();
    WriteMolScriptLabels();

    fputs("end_plot\n",OutFile);
    fclose(OutFile);
#ifdef APPLEMAC
    SetFileInfo(name,'ttxt','TEXT',133);
#endif
    return True;
}



/*============================*/
/*  RasMol Script Generation  */
/*============================*/
  
static void WriteScriptAll( void )
{
    if( !SelectAll )
    {   fputs("select all\n",OutFile);
        SelectAll = True;
    }
}

static void WriteScriptColour( char *ptr, int col )
{
    register ShadeDesc *shade;
    
    if( col )
    {   shade = Shade + Colour2Shade(col);
        fprintf(OutFile,"colour %s [%d,%d,%d]\n",ptr,
                shade->r,shade->g,shade->b);
    } else fprintf(OutFile,"colour %s none\n",ptr);
}


static void WriteScriptBetween( int lo, int hi )
{
    if( lo != hi )
    {   fprintf(OutFile,"select (atomno>=%d) and (atomno<=%d)\n",lo,hi);
    } else fprintf(OutFile,"select atomno=%d\n",lo);
    SelectAll = False;
}


static void WriteScriptSelectBond( RAtom __far *src, RAtom __far *dst )
{
    fprintf(OutFile,"select (atomno=%d) or (atomno==%d)\n",
                    src->serno, dst->serno);
    SelectAll = False;
}


/* PV: This function was differently corrected in rastop 1.3 */
static void WriteScriptAtoms( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Long first=0,last=0;
    register int same,init;
    register int cpk=0,vdw=0;
    register int col=0,rad=0;
    register int prevflag,staron,sphereon;

    fputs("\n# Atoms\n",OutFile);

    same = True;
    init = False;

    ForEachAtom
        if( !init )
        {   first = last = aptr->serno;
            cpk = IsCPKColour( aptr );
            col = aptr->col;
            init = True;
        } else if( cpk && IsCPKColour(aptr) )
        {   last = aptr->serno;
            if( aptr->col != col )
                col = 0;
        } else if( aptr->col == col )
        {   last = aptr->serno;
            cpk = False;
        } else if( aptr->col != col )
        {   WriteScriptBetween( first, last );
            if( !col )
            {   fputs("colour atoms cpk\n",OutFile);
            } else WriteScriptColour("atoms",col);
                
            first = last = aptr->serno;
            cpk = IsCPKColour( aptr );
            col = aptr->col;
            same = False;
        } else last = aptr->serno; 
        
    if( init )
    {   if( !same )
        {   WriteScriptBetween(first,last);
        } else WriteScriptAll();

        if( !col )
        {   fputs("colour atoms cpk\n",OutFile);
        } else WriteScriptColour("atoms",col);
    }

    staron = False;
    sphereon = False;
    if( DrawAtoms || DrawStars )
    {   same = True;
        init = False;
        prevflag = 0;
        ForEachAtom
        {
            if( !init )
            {   rad = (aptr->flag&(SphereFlag | StarFlag)) ? aptr->radius : 0;
                first = last = aptr->serno;
                vdw = IsVDWRadius( aptr );
                if (vdw) rad = -1;
                init = True;
                prevflag = aptr->flag&(SphereFlag | StarFlag);
            } else 
            { if( (prevflag == (aptr->flag&(SphereFlag | StarFlag))) &&
                ((vdw && IsVDWRadius(aptr)) ||
                (rad == (prevflag ? aptr->radius : 0))))
              { last = aptr->serno;
              } else
              { if( prevflag )
                { WriteScriptBetween(first,last);
                  if( rad == -1 )
                  { if( prevflag&SphereFlag ) 
                    { sphereon = True; 
                      fputs("spacefill on\n",OutFile);
                    }
                    if( prevflag&StarFlag ) 
                    { staron = True;
                      fputs("star on\n",OutFile);
                    }
                  } else 
                  { if( rad )
                    { if( prevflag&SphereFlag )
                      { sphereon = True; 
                        fprintf(OutFile,"spacefill %d\n",rad);
                      }
                      if( prevflag&StarFlag)
                      { staron = True;
                        fprintf(OutFile,"star %d\n",rad);
                      }
                    } else 
                    { if( sphereon )
                      { sphereon = False;
                        fputs("spacefill off\n",OutFile); 
                      }
                      if( staron )
                      { staron = False;
                        fputs("star off\n",OutFile);
                      }
                    }
                  }
                }
                prevflag = aptr->flag&(SphereFlag | StarFlag);
                rad = prevflag? aptr->radius : 0;
                first = last = aptr->serno;
                vdw = IsVDWRadius( aptr );
                if( vdw ) rad = -1;
                same = False;
              }
            }
        }

        if( !same )
        {   WriteScriptBetween(first,last);
        } else WriteScriptAll();

        if( rad == -1 )
        {   if( prevflag&SphereFlag )
              fputs("spacefill on\n",OutFile);
            if( prevflag&StarFlag )
              fputs("star on\n",OutFile);
        } else if( rad )
        {   if( prevflag&SphereFlag )
              fprintf(OutFile,"spacefill %d\n",rad);
            if( prevflag&StarFlag )
              fprintf(OutFile,"star %d\n",rad);
        } else {
          if (sphereon) fputs("spacefill off\n",OutFile); 
          if (staron) fputs("star off\n",OutFile);
        }

        if( UseShadow )
        {   fputs("set shadow on\n",OutFile);
        } else fputs("set shadow off\n",OutFile);

    } else
    {   WriteScriptAll();
        fputs("spacefill off\n",OutFile);
    }
        
}


static void WriteScriptDatum( char *ptr, Long datum )
{
    if( datum & DatCylFlag )
    {   fprintf(OutFile,"%s %d\n",ptr,(int)(datum-DatCylFlag));
    } else if( datum & DatWirFlag )
    {   fprintf(OutFile,"%s on\n",ptr);
    } else if( datum & DatDasFlag )
    {   fprintf(OutFile,"%s dash\n",ptr);
    } else fprintf(OutFile,"%s off\n",ptr);
}


static void WriteScriptBonds( void )
{
    register Bond __far *bptr;
    register Long defdat;
    register Long datum;
    register int col;

    fputs("\n# Bonds\n",OutFile);

    ResetFreqTable();
    ForEachBond
        IncFreqTable(GetBondDatum(bptr));

    WriteScriptAll();
    defdat = Freq[0].datum;
    WriteScriptDatum("wireframe",defdat);

    if( Freq[1].count )
    {   ForEachBond
        {   datum = GetBondDatum(bptr);
            if( datum != defdat )
            {    WriteScriptSelectBond(bptr->srcatom,bptr->dstatom);
                 WriteScriptDatum("wireframe",datum);
            }
        }
    } else if( !defdat )
        return;

    ResetFreqTable();
    ForEachBond
        IncFreqTable(bptr->col);

    col = (int)Freq[0].datum;
    if( col )
    {   WriteScriptAll();
        WriteScriptColour("bonds",col);
    }

    if( Freq[1].count )
        ForEachBond
            if( bptr->col != col )
            {   WriteScriptSelectBond(bptr->srcatom,bptr->dstatom);
                WriteScriptColour("bonds",bptr->col);
            }
}


static void WriteScriptBackbone( void )
{
    register Chain __far *chain;
    register Bond __far *bptr;

    register Long defdat;
    register Long datum;
    register int col;

    fputs("\n# Backbone\n",OutFile);

    ResetFreqTable();
    ForEachBack
        IncFreqTable(GetBondDatum(bptr));

    WriteScriptAll();
    defdat = Freq[0].datum;
    WriteScriptDatum("backbone",defdat);

    if( Freq[1].count )
    {   ForEachBack
        {   datum = GetBondDatum(bptr);
            if( datum != defdat )
            {    WriteScriptSelectBond(bptr->srcatom,bptr->dstatom);
                 WriteScriptDatum("backbone",datum);
            }
        }
    } else if( !defdat )
        return;

    ResetFreqTable();
    ForEachBack
        IncFreqTable(bptr->col);

    col = (int)Freq[0].datum;
    if( col )
    {   WriteScriptAll();
        WriteScriptColour("backbone",col);
    }

    if( Freq[1].count )
        ForEachBack
            if( bptr->col != col )
            {   WriteScriptSelectBond(bptr->srcatom,bptr->dstatom);
                WriteScriptColour("backbone",bptr->col);
            }
}


static void WriteScriptRibbons( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;

    fputs("\n# Ribbons\n",OutFile);

    if( DrawRibbon )
    {   if( Info.structsource == SourceCalc )
            fputs("structure\n",OutFile);
        fprintf(OutFile,"set strands %d\n",SplineCount);
        fprintf(OutFile,"set cartoon %s\n",DrawBetaArrows?"on":"off");
        fprintf(OutFile,"set cartoon %d\n",CartoonHeight);

        for( chain=Database->clist; chain; chain=chain->cnext )
            for( group=chain->glist; group; group=group->gnext )
            {    if( IsAmino(group->refno) )
                 {   aptr = FindGroupAtom(group,1);
                 } else aptr = FindGroupAtom(group,7);
                 if( !aptr ) continue;

                 fprintf(OutFile,"select atomno=%d\n",aptr->serno);
                 SelectAll = False;

                 if( group->flag & RibbonFlag )
                 {   fprintf(OutFile,"ribbons %d\n",group->width);
                 } else if( group->flag & CartoonFlag )
                 {   fprintf(OutFile,"cartoon %d\n",group->width);
                 } else if( group->flag & StrandFlag )
                 {   fprintf(OutFile,"strands %d\n",group->width);
                 } else if( group->flag & DashStrandFlag )
                 {   fprintf(OutFile,"strands dash %d\n",group->width);
                 } else if( group->flag & TraceFlag )
                 {   fprintf(OutFile,"trace %d\n",group->width);
                 } else if( group->flag & DotsFlag )
                 {   fprintf(OutFile,"trace dots %d\n",group->width);
                 } else fputs("ribbons off\n",OutFile);

                 if( group->col1 != group->col2 )
                 {   if( group->col1 )
                         WriteScriptColour("ribbon1",group->col1);
                     if( group->col2 )
                         WriteScriptColour("ribbon2",group->col2);
                 } else if( group->col1 )
                     WriteScriptColour("ribbons",group->col1);
            }
    } else
    {   WriteScriptAll();
        fputs("ribbons off\n",OutFile);
    }
}


static void WriteScriptHBonds( char *obj, HBond __far *list )
{
    register HBond __far *ptr;
    register Long defdat;
    register Long datum;
    register int col;

    fprintf(OutFile,"\n# %s\n",obj);

    ResetFreqTable();
    for( ptr=list; ptr; ptr=ptr->hnext )
        IncFreqTable(GetHBondDatum(ptr));

    WriteScriptAll();
    defdat = Freq[0].datum;
    WriteScriptDatum(obj,defdat);

    if( Freq[1].count )
    {   for( ptr=list; ptr; ptr=ptr->hnext )
        {   datum = GetHBondDatum(ptr);
            if( datum != defdat )
            {    WriteScriptSelectBond(ptr->src,ptr->dst);
                 WriteScriptDatum(obj,datum);
            }
        }
    } else if( !defdat )
        return;

    ResetFreqTable();
    for( ptr=list; ptr; ptr=ptr->hnext )
        IncFreqTable(ptr->col);

    col = (int)Freq[0].datum;
    if( col )
    {   WriteScriptAll();
        WriteScriptColour(obj,col);
    }

    if( Freq[1].count )
        for( ptr=list; ptr; ptr=ptr->hnext )
            if( ptr->col != col )
            {   WriteScriptSelectBond(ptr->src,ptr->dst);
                WriteScriptColour(obj,ptr->col);
            }
}


static void WriteScriptLabels( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Long first = 0,last = 0;
    register Label *label;

    fputs("\n# Labels\n",OutFile);
    WriteScriptAll();
    fputs("labels off\n",OutFile);
    if( !LabelList ) return;

    if( UseLabelCol )
    {   fprintf(OutFile,"colour labels [%d,%d,%d]\n",LabR,LabG,LabB);
    } else fputs("colour labels none\n",OutFile);

    if ( FontPS )
    { fprintf(OutFile,"set fontsize %d PS\n",FontSize);
    } else fprintf(OutFile,"set fontsize %d\n",FontSize);

    if ( FontStroke )
      fprintf(OutFile,"set fontstroke %d\n",FontStroke);

    label = (Label*)0;
    ForEachAtom
        if( aptr->label != label )
        {   if( label )
            {   WriteScriptBetween(first,last);
                fprintf(OutFile,"label \"%s\"\n",label->label);
            }
            label = (Label*)aptr->label;
            first = last = aptr->serno;
        } else last = aptr->serno;

    if( label )
    {   WriteScriptBetween(first,last);
        fprintf(OutFile,"label \"%s\"",label->label);
    }
}


static void WriteScriptMonitors( void )
{
    register Monitor *ptr;
    register int col;

    fputs("\n# Monitors\n",OutFile);
    if( !MonitList )
    {   fputs("monitors off\n",OutFile);
        return;
    }

    fprintf(OutFile,"set monitors %s\n",DrawMonitDistance?"on":"off");

    ResetFreqTable();
    for( ptr=MonitList; ptr; ptr=ptr->next )
    {   fprintf(OutFile,"monitor %d %d\n",ptr->src->serno,ptr->dst->serno);
        IncFreqTable(ptr->col);
    }

    col = (int)Freq[0].datum;
    if( col )
    {   WriteScriptAll();
        WriteScriptColour("monitors",col);
    }

    if( Freq[1].count )
        for( ptr=MonitList; ptr; ptr=ptr->next )
            if( ptr->col != col )
            {   WriteScriptSelectBond(ptr->src,ptr->dst);
                WriteScriptColour("monitor",ptr->col);
            }
}


static void WriteScriptSurface( int flag )
{    
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    char buffer[MAXBUFFLEN];
    register int i;

    ForEachAtom
        if( aptr->surf & flag )
            aptr->flag |= SelectFlag;
        else
            aptr->flag &= ~SelectFlag;
            
    do
    {    i = WriteAtomSelection(buffer,0);
        fprintf(OutFile,buffer);
    } while( i );

}


static void WriteScriptSet( SymEntry __far *ptr )
{    register AtomSet __far *pset;
    char buffer[MAXBUFFLEN];
    register int i;

    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
        for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
            for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                QAtom->flag &= ~SelectFlag;

    pset = ptr->defn;
    while( pset )
    {   for( i=0; i<pset->count; i++ )
        {   QAtom = pset->data[i];
            QAtom->flag |= SelectFlag;
        }
        pset = pset->next;
    }
            
    do
    {    i = WriteAtomSelection(buffer,0);
        fprintf(OutFile,buffer);
    } while( i );

}


static void WriteScriptAtomSets( void )
{    register SymEntry __far * __far *prev;
    register SymEntry __far *ptr;
    register SymEntry __far *cur;
    register SymEntry __far *temp;
    register int i,j;
    char buffer[MAXBUFFLEN];

    fputs("\n# AtomSets\n",OutFile);

    cur = SymbolTable;
    if( !cur )
    {    fputs("# no sets\n",OutFile);
        return;
    }
    while( cur->lft )
        cur = cur->lft;    

    while( cur )
    {   if( cur->defn)
        {    WriteScriptSet(cur);
            sprintf(buffer,"define %s selected\n",cur->ident);
            for(i=0;i<MAXBUFFLEN;i++)
                buffer[i] = tolower(buffer[i]);
            fprintf(OutFile,buffer);
        }

        prev = &SymbolTable;
        temp = *prev;
        while( temp->rgt )
            temp = temp->rgt;    
        while( *prev )
        {   ptr = *prev;
            i = strcmp(temp->ident,ptr->ident);
            j = strcmp(cur->ident,ptr->ident);
            if( i>0&&j<0 ) temp = ptr;
            prev = (j<0)? &(ptr->lft) : &(ptr->rgt);
        }
        if( cur==temp )
            cur = NULL;
        else
            cur = temp;
    }
    SelectAll = False;
    WriteScriptAll();
}


static void WriteScriptDots( void )
{    
    fputs("\n# Dots\n",OutFile);

    if( SolventDots )
        fputs("set solvent true\n",OutFile);
    else
        fputs("set solvent false\n",OutFile);
    fprintf(OutFile,"set radius %d\n",ProbeRadius);
    fprintf(OutFile,"set dots %d\n",DotSize);

    if( DotPtr )
    {   WriteScriptSurface(DotFlag);
        if( DotDensity!=50 )
            fprintf(OutFile,"dots %d\n",DotDensity);
        else
            fprintf(OutFile,"dots on\n");
        if( UseDotCol==DotColPot )
            fprintf(OutFile,"colour dots potential\n");
        else if( UseDotCol==DotColAttrib )
            fprintf(OutFile,"colour dots [%d,%d,%d]\n", DotR,DotG,DotB);
        else if( UseDotCol==DotColNone )
            fprintf(OutFile,"colour dots none\n");
    } else
        fprintf(OutFile,"dots off\n");
}


int WriteScriptFile( char *name, int append, int world )
{    
    double theta,phi,psi;
    register char *ptr;
    register double temp,x,y,z;

    if( !append )
        OutFile = fopen(name,"w");
    else
        OutFile = fopen(name,"a");

    if( !OutFile )
    {   FatalScriptError(name);
        return(False);
    }

    /*Standard source <.scr> file*/
    if( !append )
    {    fprintf(OutFile,"#!rasmol -script\n# File: %s\n",name);
    } else
    /*Standard rasmol <.rsm> file*/
    {   fputs("\n\n",OutFile);
        fprintf(OutFile,"#!rasmol -rsm file\n");
    }

    fprintf(OutFile,"# Creator: %s Version %s", VER_NAME, VERSION);
    fputc('\n',OutFile);

    fputs("zap\n",OutFile);

    if( !Database&&OutFile&&!append )
    {   /* No Molecule! */
        fclose(OutFile);
#ifdef APPLEMAC
        SetFileInfo(name,'RSML','RSML',133);
#endif
        return True;
    }

    /* Molecule File Name */
    switch( CurMolSet->format )
    {   default:
        case(FormatPDB):      ptr = "pdb";      break;
        case(FormatAlchemy):  ptr = "alchemy";  break;
        case(FormatCharmm):   ptr = "charmm";   break;
        case(FormatMol2):     ptr = "mol2";     break;
        case(FormatMDL):      ptr = "mdl";      break;
        case(FormatXYZ):      ptr = "xyz";      break;
        case(FormatCIF):      ptr = "cif";      break;
    }

    /*Standard source <.scr> file*/
    if( !append )
    {    if ( !strcmp(Info.filename,"inline") ) 
        {   fprintf(OutFile,
            "\n#*** WARNING Data loaded inline not included in this script\n");
            fprintf(OutFile,"load %s %s\n",ptr,Info.filename);
        } else {
            fprintf(OutFile,"load %s \"%s\"\n",ptr,Info.filename);
        }
    } else
    /*Standard rasmol <.rsm> file*/
    {    if( CalcBondsFlag==True )
            fputs("set connect on\n",OutFile);
        else
            fputs("set connect off\n",OutFile);
        fprintf(OutFile,"load %s inline\n",ptr);
        fprintf(OutFile,"set title %s\n",CurMolSet->title);
        fputc('\n',OutFile);
        if( !Database )
        {   fprintf(OutFile,"background [%d,%d,%d]\n",BackR,BackG,BackB);
            fputc('\n',OutFile);
            fprintf(OutFile,"exit\n");
            fclose(OutFile);
#ifdef APPLEMAC
            SetFileInfo(name,'RSML','RSML',133);
#endif
            return(True);
        }
    }

    /* Colour Details */
    if( !append || !world || (CurMolSet==MoleculeList) )
    {   fputs("# Colour details\n",OutFile);
        fprintf(OutFile,"background [%d,%d,%d]\n",BackR,BackG,BackB);
        fputs("set backfade ",OutFile);
        if( UseBackFade )
           fputs("on\n",OutFile);
        else 
            fputs("off\n",OutFile);
        if( (LightZ==2*LightX) && (LightZ==2*LightY) )
        {    fputs("set headlight default\n",OutFile);
        } else
        {    fprintf(OutFile,"set headlight [%d,%d,%d]\n",
                (int)(100*(double)LightX/LightLength),
                (int)(100*(double)LightY/LightLength),
                (int)(-100*(double)LightZ/LightLength) );
        }
        fprintf(OutFile,"set ambient %d\n", (int)(100*Ambient) );
        fputs("set specular ",OutFile);
        if( FakeSpecular )
        {   fprintf(OutFile,"on\nset specpower %d\n",SpecPower);
        } else fputs("off\n",OutFile);
        fprintf(OutFile,"set shadepower %d\n",ShadePower);
        if( UseDepthCue )
        {   fprintf(OutFile,"set depthcue on\n");
            fprintf(OutFile,"set depthcue %d\n",DepthCueMode);
        } else
            fprintf(OutFile,"set depthcue off\n");
        fputs("set shadow ",OutFile);
        if( UseShadow )
           fputs("on\n",OutFile);
        else 
            fputs("off\n",OutFile);
        if( UseStereo )
        {   if( StereoAngle == 6.0 )
                fputs("stereo on\n",OutFile);
            else
                fprintf(OutFile,"set stereo %.1f\n",StereoAngle);
        } else 
            fputs("stereo off\n",OutFile);
        putc('\n',OutFile);
    }

    /* Bond Modifications */
    if( BondOrderList || RotBondList )
        fputs("# Bond modifications\n",OutFile);

    if( BondOrderList )
    {   BondOrder __far *boptr;
        boptr = BondOrderList;
        while( boptr )
        {   if( boptr->order!=boptr->neworder )
            {   if( boptr->neworder==0 )
                    fprintf(OutFile,"bond %d %d unbond\n", 
                    boptr->BSrcAtom->serno, boptr->BDstAtom->serno);
                else if( boptr->neworder==1 )
                    fprintf(OutFile,"bond %d %d single\n", 
                    boptr->BSrcAtom->serno, boptr->BDstAtom->serno);
                else if( boptr->neworder==2 )
                    fprintf(OutFile,"bond %d %d double\n", 
                    boptr->BSrcAtom->serno, boptr->BDstAtom->serno);
                else if( boptr->neworder==3 )
                    fprintf(OutFile,"bond %d %d triple\n", 
                    boptr->BSrcAtom->serno, boptr->BDstAtom->serno);
            }
            boptr = boptr->next;
        }
    }

    if( RotBondList ) 
    {    BondRot __far *brptr;
        brptr = RotBondList;
        while( brptr )
        {   int angle;
            angle = Rint(brptr->BRotValue*180.);
            if( angle )
            {   fprintf(OutFile,"bond %d %d pick\n", 
                brptr->BSrcAtom->serno, brptr->BDstAtom->serno);
                fprintf(OutFile,"rotate bond %d\n", angle);
            }
            brptr = brptr->brnext;
        }
    }

    if( BondOrderList || RotBondList )
        putc('\n',OutFile);

    /* Transformation */
    fputs("# Transformation\n",OutFile);
    fputs("reset\n",OutFile);
    fputs("rotate molecule\n",OutFile);

    /* Prevent pickmode dependent script */
    fputs("set picking ident\n",OutFile);

    if( !CurMolSet->visible )
        fputs("molecule hide\n",OutFile);

    /* WorldDepth */
    if( !append || !world || (CurMolSet==MoleculeList) )
        fprintf(OutFile,"set worlddepth %d\n",WorldSize);
    else
        fprintf(OutFile,"# set worlddepth %d\n",WorldSize);

    /* Zoom */
    if( !append )
    {   if( DialValue[DialZoom]<0.0 )
        {   temp = 100*DialValue[DialZoom];
        } else temp = 100*MaxZoom*DialValue[DialZoom];
        fprintf(OutFile,"zoom %.2f\n",temp+100);
    } else
    {   if( DialValue[DialZoom]<0.0 )
        {   temp = 100*DialValue[DialZoom];
        } else temp = 100*MaxZoom*DialValue[DialZoom];
        fprintf(OutFile,"# zoom %.2f\n",temp+100);
        fprintf(OutFile,"scale %.2f\n",Range/(Scale*250));
    }

    phi = psi = theta = 0.0;
    RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
    theta *= 180.;
    phi *= 180.;
    psi *= 180.;

    if( !IsAlmostZero(theta) ) fprintf(OutFile,"rotate x %.2f\n",InvertY(-theta));
    if( !IsAlmostZero(phi) )   fprintf(OutFile,"rotate y %.2f\n",phi);
    if( !IsAlmostZero(psi) )   fprintf(OutFile,"rotate z %.2f\n",InvertY(-psi));

    /* Get absolute values */
    x = 100.0*MOffset[0]/Range;
    if( !append ) /* Get Old RasMol Values */
    {   if( x<-100.0 ) x = -100.0;
        if( x>100.0 ) x = 100.0;
        if( !IsAlmostZero(x) )
            fprintf(OutFile,"translate x %.2f\n",x);
    } else
        if( !IsAlmostZero(x) )
            fprintf(OutFile,"# translate x %.2f\n",x);

    y = 100.0*MOffset[1]/Range;
    if( !append ) /* Get Old RasMol Values */
    {   if( y<-100.0 ) y = -100.0;
        if( y>100.0 ) y = 100.0;
        if( !IsAlmostZero(y) )
            fprintf(OutFile,"translate y %.2f\n",InvertY(-y));
    } else
        if( !IsAlmostZero(y) )
            fprintf(OutFile,"# translate y %.2f\n",InvertY(-y));
            
    z = 100.0*MOffset[2]/Range;
    if( !IsAlmostZero(z) && append )
        fprintf(OutFile,"# translate z %.2f\n",-z);

    if( append )
    {   x = MOffset[0]/(Scale*250.0);
        y = MOffset[1]/(Scale*250.0);
#ifdef INVERT
        y = -y;
#endif
        z = -MOffset[2]/(Scale*250.0);
        fprintf(OutFile,"position x %.3f y %.3f z %.3f\n",x,y,z);
    }

    if( !append || !IsWorldActive() )
    {
        if( CenterSet )
        {   if( CenX || CenY || CenZ )
            {
#ifdef INVERT
            fprintf(OutFile,"centre [%ld,%ld,%ld] translate\n",CenX,-CenY,-CenZ);
#else
            fprintf(OutFile,"centre [%ld,%ld,%ld] translate\n",CenX,CenY,-CenZ);
#endif
            }
        }
    }

    if( !append || !IsWorldActive() )
    {   if( UseSlabPlane )
        {   temp = 50.0*DialValue[7];
            if( temp )
            {   fprintf(OutFile,"slab %.2f\n",temp+50);
            } else fputs("slab on\n",OutFile);

            fputs("set slabmode ",OutFile);
            switch( SlabMode )
            {   default:            
                case(SlabClose):    ptr = "solid";    break;
                case(SlabReject):   ptr = "reject";   break;
                case(SlabHalf):     ptr = "half";     break;
                case(SlabHollow):   ptr = "hollow";   break;
                case(SlabSection):  ptr = "section";
            }
            fputs(ptr,OutFile);
            putc('\n',OutFile);
        } else fputs("slab off\n",OutFile);

        if( UseDepthPlane )
        {   temp = 50.0*DialValue[8];
            if( temp )
            {   fprintf(OutFile,"depth %.2f\n",temp+50);
            } else fputs("depth on\n",OutFile);
        } else fputs("depth off\n",OutFile);
    }
    putc('\n',OutFile);
        
    /* Rendering */
    fputs("# Rendering\n",OutFile);
    if( DrawAxes || DrawBoundBox || DrawUnitCell )
        WriteScriptColour("axes",BoxCol);
    fprintf(OutFile,"set axes %s\n", DrawAxes? "on":"off" );
    fprintf(OutFile,"set boundingbox %s\n", DrawBoundBox? "on":"off" );
    fprintf(OutFile,"set unitcell %s\n", DrawUnitCell? "on":"off" );

    if( DrawBonds )
        fprintf(OutFile,"set bonds %s\n", DrawDoubleBonds? "on":"off" );

    if( append && !IsWorldActive() && DrawWorldAxes )
    {   fputs("set axes world on\n",OutFile);
        if( (AxeR!=255) || (AxeG!=255) || (AxeB!=255) )
            fprintf(OutFile,"colour world axes [%d,%d,%d]\n",AxeR,AxeG,AxeB);
    }

    if( Database->hlist )
    {   fputs("set hbond ",OutFile);
        fputs(HBondMode?"backbone":"sidechain",OutFile);
        fputc('\n',OutFile);
    }

    if( Database->slist )
    {   fputs("set ssbond ",OutFile);
        fputs(SSBondMode?"backbone":"sidechain",OutFile);
        fputc('\n',OutFile);
    }

    fputs("set bondmode and\ndots off\n",OutFile); 
    fputs("\n# Avoid Colour Problems!\nselect all\n",OutFile);
    fputs("colour bonds none\ncolour backbone none\n",OutFile);
    fputs("colour hbonds none\ncolour ssbonds none\n",OutFile);
    fputs("colour ribbons none\ncolour white\n",OutFile);
    SelectAll = True;

    WriteScriptAtoms();
    if( !(UseSlabPlane && (SlabMode==SlabSection)) )
    {   WriteScriptBonds();
        WriteScriptRibbons();
        WriteScriptBackbone();
        WriteScriptLabels();
        WriteScriptMonitors();
        WriteScriptHBonds("ssbonds",Database->slist);
        WriteScriptHBonds("hbonds",Database->hlist);
        WriteScriptDots();
    }

    WriteScriptAtomSets();

    fputc('\n',OutFile);
    SelectAll = False;
    WriteScriptAll();

    fputc('\n',OutFile);

    /* Other Molecules */
    if( world && (CurMolSet==MoleculeList) && MoleculeList->next )
    {   int i;
        MolSet *mptr;

        fputs("# Molecules\n",OutFile);
        mptr = MoleculeList->next;
        i = 2;
        while( mptr )
        {   fprintf(OutFile,"add %sw%d\n",SpaceName,i);
            i++;
            mptr = mptr->next;
        }
        putc('\n',OutFile);
    }

    /* World Transformation */
    if( append && IsWorldActive() &&
        ((world && CurMolSet==MoleculeList) || !world) )
    {   
        fputs("# World Transformation\n",OutFile);
        fputs("rotate world\n",OutFile);
        fputs("centre origin\n",OutFile);

        fprintf(OutFile,"set axes world %s\n", DrawWorldAxes? "on":"off" );
        if( DrawWorldAxes )
            if( !(AxeR!=255) || (AxeG!=255) || (AxeB!=255) )
                fprintf(OutFile,"colour world axes [%d,%d,%d]\n",AxeR,AxeG,AxeB);

        /* Repeat WorldDepth */
        if( MoleculeList->next )
            fprintf(OutFile,"set worlddepth %d\n",WorldSize);

        theta = -180.0*WRotValueBack[0];
        phi =   -180.0*WRotValueBack[1];
        psi =   -180.0*WRotValueBack[2];
        if( !IsAlmostZero(theta) )
            fprintf(OutFile,"rotate world x %.2f\n",InvertY(-theta));
        if( !IsAlmostZero(phi))
            fprintf(OutFile,"rotate world y %.2f\n",phi);
        if( !IsAlmostZero(psi) )
            fprintf(OutFile,"rotate world z %.2f\n",InvertY(-psi));
        
        temp = 100.0*WTransValueBack[0];
        if( !IsAlmostZero(temp) )
            fprintf(OutFile,"# translate world x %.2f\n",temp);
        temp = 100.0*WTransValueBack[1];
        if( !IsAlmostZero(temp) )
            fprintf(OutFile,"# translate world y %.2f\n",InvertY(-temp));
        /*temp = 100.0*WTransValueBack[2];
        if( !IsAlmostZero(temp) )
            fprintf(OutFile,"# translate world z %.2f\n",-temp);*/

        x = WTransValueBack[0]*Range/(Scale*250.0);
        y = WTransValueBack[1]*Range/(Scale*250.0);
#ifdef INVERT
        y = -y;
#endif
        z = -WTransValueBack[2]*Range/(Scale*250.0);
        fprintf(OutFile,"position world x %.3f y %.3f\n",x,y);

        if( UseSlabPlane )
        {   fputs("slab world origin\n",OutFile);
            /* temp = -(WClip[0]-Wo[2])/(Scale*250); */
            temp = -(WClip[0]-WTransValueBack[2]*ZRange)/(Scale*250);
            fprintf(OutFile,"slab world %.2f\n",temp);

            fputs("set slabmode ",OutFile);
            switch( SlabMode )
            {   default:            
                case(SlabClose):    ptr = "solid";    break;
                case(SlabReject):   ptr = "reject";   break;
                case(SlabHalf):     ptr = "half";     break;
                case(SlabHollow):   ptr = "hollow";   break;
                case(SlabSection):  ptr = "section";
            }
            fputs(ptr,OutFile);
            putc('\n',OutFile);
        } else 
        {   fputs("reset slab\n",OutFile);
            fputs("slab off\n",OutFile);
        }

        if( UseDepthPlane )
        {   fputs("depth world origin\n",OutFile);
            /* temp = -(WClip[1]-Wo[2])/(Scale*250); */
            temp = -(WClip[1]-WTransValueBack[2]*ZRange)/(Scale*250);
            fprintf(OutFile,"depth world %.2f\n",temp);
        } else
        {   fputs("reset depth\n",OutFile);
            fputs("depth off\n",OutFile);
        }

        if( world )
        {   if( CenterSet )
            {   int count;
                count = GetMolsetIndex(CenterSet);
                if( count )
                    fprintf(OutFile,"molecule %d\n",count);

            if( CenX || CenY || CenZ )
            {
#ifdef INVERT
            fprintf(OutFile,"centre [%ld,%ld,%ld] translate\n",CenX,-CenY,-CenZ);
#else
            fprintf(OutFile,"centre [%ld,%ld,%ld] translate\n",CenX,CenY,-CenZ);
#endif
            } else
                fputs("centre molecule\n",OutFile);
            } else
                fputs("centre origin\n",OutFile);

            if( CenterSet != MoleculeList )
                fputs("molecule 1\n",OutFile);
        } else
        {   fputs("molecule 1\n",OutFile);
            fputs("centre molecule\n",OutFile);
        }

        putc('\n',OutFile);
    } else
        fputs("rotate world\n\n",OutFile);
    

    fprintf(OutFile,"exit\n");

    if(OutFile)
    {  fclose(OutFile);
#ifdef APPLEMAC
        SetFileInfo(name,'RSML','RSML',133);
#endif
    }

    return True;
}

#define MAXBUFFLEN2 80

/*This function write the current atom selection*/
int WriteAtomSelection( char* ptr, int out )
{    static int cont = 0;
    static Long cur = 0;
    static char sbloc[32];

    register char *src;
    register Long ori, end;
    register int bloc;

    src = ptr;
    bloc = 0;
    ori = end = 0;

    /*Allow external reset*/
    if( out==-1 )
    {    cont = 0;
        return True;
    }

    if( !cont && out )
        ptr += sprintf(ptr,"#!rasmol -selection\n");
    if( !cont )
        ptr += sprintf(ptr,"select <");
    else
    {    ptr += sprintf(ptr,"select >");
        ptr += sprintf(ptr,sbloc);
    }

    if( cont==2 )
    {    ptr--;
        *ptr++ = '>';
        *ptr++ = '\n';
        *ptr++ = '\0';
        cont = 0;
        return(cont);
    }
        
    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
        for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
            for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                {    if( cont )
                        if( QAtom->serno == cur )
                        {    ori = end = cur;
                            cont = 0;
                        } else
                            continue;
                
                    if( QAtom->flag & SelectFlag )
                    {    if( bloc==0 )
                        {    ori = end = QAtom->serno;
                            bloc = 1;
                            continue;
                        } else if( QAtom->serno == end + 1 )
                        {    end += 1;
                            continue;
                        }
                    } else if( !bloc )
                        continue;

                    if( ori!=end )
                        sprintf(sbloc,"%d-%d,",ori,end);
                    else
                        sprintf(sbloc,"%d,",ori);
                    if( (strlen(src)+strlen(sbloc)+1)<MAXBUFFLEN2-1 )
                    {    ptr += sprintf(ptr,sbloc);
                        ori = end = QAtom->serno;
                        bloc = 0;
                    } else
                        goto stop;
                }

    stop: if( QAtom ) cur = QAtom->serno;
    cont = bloc;

    if( cont )
    {    if( ori!=end )
            sprintf(sbloc,"%d-%d,",ori,end);
        else
            sprintf(sbloc,"%d,",ori);

        if( (strlen(src)+strlen(sbloc)+2)<MAXBUFFLEN2 )
        {    ptr += sprintf(ptr,sbloc);
            cont = 0;
        } else
        {    if( QAtom )
            {    cont = 1;
            } else
                cont = 2;
        }
    }

    ptr--;
    if( *ptr=='>'||*ptr=='<' )
        ptr++;
    if( cont )
        *ptr++ = '<';
    else
        *ptr++ = '>';
    *ptr++ = '\n';
    *ptr = '\0';

    return cont;
}


/*This function writes the molecule position*/
int WriteScriptPosition( char *ptr, int out )
{   register double temp;
    char *scr;
    double theta,phi,psi;
    double x,y,z;
    double WRotX[3], WRotY[3], WRotZ[3];

    if( out )
        ptr += sprintf(ptr,"#!rasmol -position\n");

    ptr += sprintf(ptr,"rotate world\n");
    ptr += sprintf(ptr,"reset\n");
    ptr += sprintf(ptr,"centre origin\n");

    ptr += sprintf(ptr,"# set worlddepth %d\n",WorldSize);
    if( DialValue[DialZoom]<=0.0 )
    {   temp = (int)(100*DialValue[DialZoom]);
    } else temp = (int)(100*MaxZoom*DialValue[DialZoom]);
    ptr += sprintf(ptr,"# zoom %.2f\n",temp+100);

    ptr += sprintf(ptr,"scale %.2f\n",Range/(Scale*250));

    WRotX[0] = WAxes[0][0]; WRotX[1] = WAxes[0][1]; WRotX[2] = WAxes[0][2]; 
    WRotY[0] = WAxes[1][0]; WRotY[1] = WAxes[1][1]; WRotY[2] = WAxes[1][2]; 
    WRotZ[0] = WAxes[2][0]; WRotZ[1] = WAxes[2][1]; WRotZ[2] = WAxes[2][2]; 
    RMat2RV(&theta, &phi, &psi, WRotX, WRotY, WRotZ);
    theta *= 180.;
    phi *= 180.;
    psi *= 180.;
    if( !IsAlmostZero(theta) ) 
        ptr += sprintf(ptr,"rotate world x %.2f\n",InvertY(-theta));
    if( !IsAlmostZero(phi) )
        ptr += sprintf(ptr,"rotate world y %.2f\n",phi);
    if( !IsAlmostZero(psi) )
        ptr += sprintf(ptr,"rotate world z %.2f\n",InvertY(-psi));

    x = Wo[0]/Range;
    y = Wo[1]/Range;
#ifdef INVERT
    y = -y;
#endif
    z = -Wo[2]/Range;

    if( !IsAlmostZero(x) ) 
        ptr += sprintf(ptr,"# translate world x %.2f\n",100.0*x);
    if( !IsAlmostZero(y) ) 
        ptr += sprintf(ptr,"# translate world y %.2f\n",100.0*y);
    /*if( !IsAlmostZero(z) ) 
        ptr += sprintf(ptr,"# translate world z %.2f\n",100.0*z);*/

    x = Wo[0]/(Scale*250.0);
    y = Wo[1]/(Scale*250.0);
#ifdef INVERT
    y = -y;
#endif
    z = -Wo[2]/(Scale*250.0);
    ptr += sprintf(ptr,"position world x %.3f y %.3f\n",x,y);

    if( UseSlabPlane )
    {   ptr += sprintf(ptr,"slab world origin\n");
        temp = GetClipCoord(WorldCoor,CurMolSet,0);
        ptr += sprintf(ptr,"slab world %.2f\n",temp);
        ptr += sprintf(ptr,"set slabmode ");
        switch( SlabMode )
        {   default:            
            case(SlabClose):    scr = "solid\n";    break;
            case(SlabReject):   scr = "reject\n";   break;
            case(SlabHalf):     scr = "half\n";     break;
            case(SlabHollow):   scr = "hollow\n";   break;
            case(SlabSection):  scr = "section\n";
        }
        ptr += sprintf(ptr, scr);
    } else ptr += sprintf(ptr,"slab off\n");


    if( UseDepthPlane )
    {   ptr += sprintf(ptr,"depth world origin\n");
        temp = GetClipCoord(WorldCoor,CurMolSet,1);
        ptr += sprintf(ptr,"depth world %.2f\n",temp);
    } else ptr += sprintf(ptr,"depth off\n");

    ptr = '\0';

    return( False );
}

/*=======================*/
/*  Kinemage Generation  */
/*=======================*/

static char *FindKinemageCol( int r, int g, int b )
{
    register Long dist,best;
    register int dr,dg,db;
    register int i,res;

    res = 0;
    dr = KinemageCol[0].r - r;
    dg = KinemageCol[0].g - g;
    db = KinemageCol[0].b - b;
    best = (Long)dr*dr + (Long)dg*dg + (Long)db*db;

    for( i=1; i<MAXMAGECOL; i++ )
    {   dr = KinemageCol[i].r - r;
        dg = KinemageCol[i].g - g;
        db = KinemageCol[i].b - b;
        dist = (Long)dr*dr + (Long)dg*dg + (Long)db*db;  

        if( dist < best ) 
        {   best = dist;
            res = i;
        }
    }
    return( KinemageCol[res].name );
}


static char *GetKinemageCol( int col )
{
    register ShadeDesc *ptr;

    ptr = Shade + Colour2Shade(col);
    return( FindKinemageCol(ptr->r,ptr->g,ptr->b) );
}


static int CheckKinemageChain( RAtom __far *src,  RAtom __far *dst, 
                               Chain __far *chain )
{
    register Group __far *group;
    register RAtom __far *aptr;

    /* Determine Chain and Groups */
    schn = dchn = (Chain __far*)0;
    sgrp = dgrp = (Group __far*)0;

    if( chain )
    {   for( group=chain->glist; group; group=group->gnext )
        {   for( aptr=group->alist; aptr; aptr=aptr->anext )
            {   if( aptr == src ) sgrp = group;
                if( aptr == dst ) dgrp = group;
            }
            if( sgrp && dgrp ) 
                return True;
        }
    }
    return False;
}


static void OutputKinemageVector( RAtom __far *src,  RAtom __far *dst, int col )
{
    register RAtom __far *aptr;
    register double x, y, z;
    register char *col1;
    register char *col2;

    if( !col )
    {   col1 = GetKinemageCol(src->col);
        col2 = GetKinemageCol(dst->col);
    } else col1 = col2 = GetKinemageCol(col);

    if( (col1!=MageCol) && (col2==MageCol) )
    {   aptr = src;  src = dst;  dst = aptr;
        col2 = col1;  col1 = MageCol;
    }

    if( col1 != MageCol )
    {   fprintf(OutFile,"@vectorlist {} color= %s\n",col1);
        MagePrev = (RAtom __far*)0;
    }

    if( src != MagePrev )
    {   if( MainGroupCount>1 )
        {   fprintf(OutFile,"{%.4s %.3s %d}", ElemDesc[src->refno], 
                    Residue[sgrp->refno], sgrp->serno );
        } else fprintf(OutFile,"{%.4s %ld}",ElemDesc[src->refno],src->serno);
        fprintf(OutFile," P %g %g %g\n", 
          (src->xorg + src->fxorg + src->xtrl/40. + OrigCX)/250.0, 
          RestoreY((src->yorg + src->fyorg + src->ytrl/40. + OrigCY))/250.0,
          -(src->zorg + src->fzorg - src->ztrl/40. + OrigCZ)/250.0 );
    }

    if( col1 != col2 )
    {   x = (src->xorg + dst->xorg + src->fxorg + dst->fxorg + 
          (src->xtrl + dst->xtrl)/40. + OrigCX + OrigCX)/500.0;
        y = (src->yorg + dst->yorg + src->fyorg + dst->fyorg +
          (src->ytrl + dst->ytrl)/40. + OrigCY + OrigCY)/500.0;
        z = (src->zorg + dst->zorg + src->fzorg + dst->fzorg -
          (src->ztrl + dst->ztrl)/40. + OrigCZ + OrigCZ)/500.0;

        fprintf(OutFile,"{} L %g %g %g\n", x, RestoreY(y), -z );
        fprintf(OutFile,"@vectorlist {} color= %s\n",col2);
        fprintf(OutFile,"{} P %g %g %g\n", x, RestoreY(y), -z );
    }

    if( MainGroupCount>1 )
    {   fprintf(OutFile,"{%.4s %.3s %d}", ElemDesc[dst->refno],
                Residue[dgrp->refno], dgrp->serno );
    } else fprintf(OutFile,"{%.4s %ld}",ElemDesc[dst->refno],dst->serno);
    fprintf(OutFile," L %g %g %g\n", 
      (dst->xorg + dst->fxorg + dst->xtrl/40. + OrigCX)/250.0,
      RestoreY((dst->yorg + dst->fyorg + dst->ytrl/40. + OrigCY))/250.0,
       -(dst->zorg + dst->fzorg - dst->ztrl/40. + OrigCZ)/250.0 );

    MagePrev = dst;
    MageCol = col2;
}


static void WriteKinemageBonds( Chain __far *chain )
{
    register Bond __far *bptr;
    register Bond __far *flag;

    MagePrev = (RAtom __far*)0;  
    MageCol = (char*)0;

    ForEachBond
        if( KinemageFlag || (bptr->flag&DrawBondFlag) )
        {   if( CheckKinemageChain(bptr->srcatom,bptr->dstatom,chain) )
            {   if( !MagePrev ) 
                    fputs("@subgroup {wireframe} dominant\n",OutFile);
                OutputKinemageVector(bptr->srcatom,bptr->dstatom,bptr->col);
            }
        }

    if( !chain->blist ) 
        return;

    /* Test for displayed backbone */
    for( flag=chain->blist; flag; flag=flag->bnext )
        if( flag->flag & DrawBondFlag ) break;
    if( !KinemageFlag && !flag ) return;

    MagePrev = (RAtom __far*)0;  
    MageCol = (char*)0;

    for( bptr=chain->blist; bptr; bptr=bptr->bnext )
        if( KinemageFlag || (bptr->flag&DrawBondFlag) )
        {   if( CheckKinemageChain(bptr->srcatom,bptr->dstatom,chain) )
            {   if( !MagePrev )
                {   fputs("@subgroup {alpha trace} dominant",OutFile);
                    fputs( (KinemageFlag && !flag)? " off\n":"\n",OutFile);
                }
                OutputKinemageVector(bptr->srcatom,bptr->dstatom,bptr->col);
            }
        }
}


static void WriteKinemageSpheres( Chain __far *chain )
{
    register Group __far *group;
    register RAtom __far *aptr;
    register char *col;
    register int radius = 0;

    MageCol = (char*)0;
    for( group=chain->glist; group; group=group->gnext )
        for( aptr=group->alist; aptr; aptr=aptr->anext )
            if( aptr->flag & SphereFlag )
            {   if( !MageCol )
                    fputs("@subgroup {CPK spheres} dominant\n",OutFile);

                col = GetKinemageCol(aptr->col);
                if( (col!=MageCol) || (aptr->radius!=radius) )
                {   fprintf(OutFile,"@balllist {} color= %s radius= %g\n",
                                    col, aptr->radius/250.0);
                    radius = aptr->radius;
                    MageCol = col;
                }

                if( MainGroupCount>1 )
                {   fprintf(OutFile,"{%.4s %.3s %d}", ElemDesc[aptr->refno],
                                    Residue[group->refno], group->serno );
                } else fprintf(OutFile,"{%.4s %ld}",ElemDesc[aptr->refno],
                                    aptr->serno);
                fprintf(OutFile," %g %g %g\n", 
                  (aptr->xorg + aptr->fxorg 
                    + (aptr->xtrl)/40. + OrigCX)/250.0,
                  RestoreY((aptr->yorg + aptr->fyorg
                    + (aptr->ytrl)/40. + OrigCY))/250.0, 
                  -(aptr->zorg + aptr->fzorg 
                    + (aptr->ztrl)/40. + OrigCZ)/250.0 );
            }
}



static void WriteKinemageLabels( Chain __far *chain )
{
    register Group __far *group;
    register RAtom __far *aptr;
    register Label *label;
    register char *col;

    auto unsigned char buffer[256];

    MageCol = (char*)0;
    for( group=chain->glist; group; group=group->gnext )
        for( aptr=group->alist; aptr; aptr=aptr->anext )
            if( aptr->label )
            {   if( !MageCol )
                    fputs("@subgroup {labels} dominant\n",OutFile);
                if( UseLabelCol )
                {   col = FindKinemageCol(LabR,LabG,LabB);
                } else col = GetKinemageCol(aptr->col);

                if( col != MageCol )
                    fprintf(OutFile,"@labellist {} color= %s\n",col);
                label = (Label*)aptr->label;
                FormatLabel(chain,group,aptr,label->label,buffer);
                fprintf(OutFile,"{%s} %g %g %g\n", buffer, 
                  (aptr->xorg + aptr->fxorg 
                  + (aptr->xtrl)/40. + OrigCX)/250.0, 
                  RestoreY((aptr->yorg + aptr->fyorg
                    + (aptr->ytrl)/40. + OrigCY))/250.0, 
                  -(aptr->zorg + aptr->fzorg 
                    + (aptr->ztrl)/40.  + OrigCZ)/250.0);
                MageCol = col;
            }
}


#ifdef UNUSED
static void WriteKinemageUnitCell( void )
{
}
#endif


static void WriteKinemageDots( void )
{
    auto int status[LastShade];
    register DotStruct __far *ptr;
    register ShadeDesc *shade;
    register int flag;
    register int i,j;

    fputs("@group {dot surface} dominant\n",OutFile);
    MageCol = (char*)0;

    for( i=0; i<LastShade; i++ )
        if( Shade[i].refcount )
        {      status[i] = 0;
        } else status[i] = 2;

    for( i=0; i<LastShade; i++ )
        if( status[i] == 0 )
        {   shade = &Shade[i];
            MageCol = FindKinemageCol(shade->r,shade->g,shade->b);
            shade++;

            status[i] = 1;
            for( j=i+1; j<LastShade; j++ )
            {   if( MageCol == FindKinemageCol(shade->r,shade->g,shade->b) )
                    status[j] = 1;
                shade++;
            }

            flag = False;
            for( ptr=DotPtr; ptr; ptr=ptr->next )
                for( j=0; j<ptr->count; j++ )
                    if( status[Colour2Shade(ptr->col[j])] == 1 )
                    {   if( !flag )
                        {   fprintf(OutFile,"@dotlist {} color= %s\n",MageCol);
                            flag = True;
                        }
                        fprintf(OutFile, "{} %g %g %g\n", 
                             (ptr->xpos[j] + OrigCX)/250.0,
                             RestoreY((ptr->ypos[j] + OrigCY))/250.0, 
                             -(ptr->zpos[j] + OrigCZ)/250.0 );
                    }

            for( j=i; j<LastShade; j++ )
                if( status[j] == 1 )
                    status[j] = 2;
        }
}


static void WriteKinemageData( void )
{
    register HBond __far *hptr;
    register RAtom __far *src;
    register RAtom __far *dst;
    register double dx, dy, dz, cx, cy, cz;

    /* Hydrogen Bonds */
    for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
        if( hptr->flag & DrawBondFlag ) break;

    if( KinemageFlag && Database->hlist )
    {   fputs("@group {h-bonds}",OutFile);
        fputs( hptr? "\n" : "off", OutFile );

        fputs("@subgroup {sidechain} dominant",OutFile);
        fputs( HBondMode? " off\n" : "\n", OutFile);
        for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
        {   CheckKinemageChain( hptr->src, hptr->dst, (Chain __far*)0 );
            OutputKinemageVector( hptr->src, hptr->dst, hptr->col);
        }

        fputs("@subgroup {mainchain} dominant",OutFile);
        fputs( HBondMode? "\n" : " off\n", OutFile);
        for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
        {   FetchBondInfo( hptr->src, hptr->dst );
            OutputKinemageVector( hptr->srcCA, hptr->dstCA, hptr->col);
        }
    } else if( hptr )
    {   fputs("@group {h-bonds} dominant\n",OutFile);
        for( hptr=Database->hlist; hptr; hptr=hptr->hnext )
            if( hptr->flag&DrawBondFlag )
            {   src = HBondMode? hptr->srcCA : hptr->src;
                dst = HBondMode? hptr->dstCA : hptr->dst;

                if( src && dst )
                {   FetchBondInfo( src, dst );
                    OutputKinemageVector( src, dst, hptr->col);
                }
            }
    }


    /* Disulphide Bridges */
    for( hptr=Database->slist; hptr; hptr=hptr->hnext )
        if( hptr->flag & DrawBondFlag ) break;

    if( KinemageFlag && Database->slist )
    {   fputs("@group {S-S bridges}",OutFile);
        fputs( hptr? "\n" : "off", OutFile );

        fputs("@subgroup {sidechain} dominant",OutFile);
        fputs( SSBondMode? " off\n" : "\n", OutFile);
        for( hptr=Database->slist; hptr; hptr=hptr->hnext )
        {   FetchBondInfo( hptr->src, hptr->dst );
            OutputKinemageVector(hptr->src,hptr->dst,hptr->col);
        }

        fputs("@subgroup {mainchain} dominant",OutFile);
        fputs( SSBondMode? "\n" : " off\n", OutFile);
        for( hptr=Database->slist; hptr; hptr=hptr->hnext )
        {   FetchBondInfo( hptr->srcCA, hptr->dstCA );
            OutputKinemageVector( hptr->srcCA, hptr->dstCA, hptr->col );
        }
    } else if( hptr )
    {   fputs("@group {S-S bridges} dominant\n",OutFile);
        for( hptr=Database->slist; hptr; hptr=hptr->hnext )
            if( hptr->flag&DrawBondFlag )
            {   FetchBondInfo( hptr->src, hptr->dst );
                OutputKinemageVector( SSBondMode?hptr->srcCA:hptr->src,
                                      SSBondMode?hptr->dstCA:hptr->dst,
                                      hptr->col );
            }
    }


    /* Dot Surfaces */
    if( DotPtr )
        WriteKinemageDots();

    /* Draw `Background' Objects */
    if( !KinemageFlag && !DrawAxes && 
        !DrawBoundBox && !DrawUnitCell )
        return;

    dx = MaxX/250.0;  dy = MaxY/250.0;  dz = MaxZ/250.0;
    cx = OrigCX/250.0; cy = OrigCY/250.0; cz = OrigCZ/250.0;
    MageCol = FindKinemageCol( AxeR,AxeG,AxeB );

    if( DrawAxes || KinemageFlag )
    {   fputs("@group {coord axes} dominant",OutFile);
        fputs( (DrawAxes?"\n":" off\n"), OutFile );
        fprintf(OutFile,"@vectorlist {} color= %s\n",MageCol);

        fprintf(OutFile,"{} P %g 0 0\n{} L %g 0 0\n",-dx+cx,dx+cx);
        fprintf(OutFile,"{} P 0 %g 0\n{} L 0 %g 0\n",-dy+cy,dy+cy);
        fprintf(OutFile,"{} P 0 0 %g\n{} L 0 0 %g\n",-dz+cz,dz+cz);
    }

    if( DrawBoundBox || KinemageFlag )
    {   fputs("@group {bound box} dominant",OutFile);
        fputs( (DrawAxes?"\n":" off\n"), OutFile );
        fprintf(OutFile,"@vectorlist {} color= %s\n",MageCol);

        fprintf(OutFile,"{} P %g %g %g\n",-dx+cx,-dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx,-dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx, dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx, dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx,-dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx,-dy+cy, dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx,-dy+cy, dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx, dy+cy, dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx, dy+cy, dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx,-dy+cy, dz+cz);

        fprintf(OutFile,"{} P %g %g %g\n", dx+cx,-dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx,-dy+cy, dz+cz);
        fprintf(OutFile,"{} P %g %g %g\n", dx+cx, dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n", dx+cx, dy+cy, dz+cz);
        fprintf(OutFile,"{} P %g %g %g\n",-dx+cx, dy+cy,-dz+cz);
        fprintf(OutFile,"{} L %g %g %g\n",-dx+cx, dy+cy, dz+cz);
    }

    if( *Info.spacegroup && (DrawUnitCell || KinemageFlag) )
    {   fputs("@group {unit cell} dominant",OutFile);
        fputs( (DrawAxes?"\n":" off\n"), OutFile );
        fprintf(OutFile,"@vectorlist {} color= %s\n",MageCol);
        /* WriteKinemageUnitCell(); */
    }
}


int WriteKinemageFile( char *name )
{
    register Chain __far *chain;
    register double zoom;

    if( !Database )
        return(True);

    OutFile = fopen(name,"w");
    if( !OutFile )
    {   FatalScriptError(name);
        return(False);
    }

    fprintf(OutFile,"@text\n%s v%s generated Kinemage\n \n", VER_NAME, VERSION);
    if( *Info.moleculename )
        fprintf(OutFile,"Molecule name ....... %s\n",Info.moleculename);
    if( *Info.classification )
        fprintf(OutFile,"Classification ...... %s\n",Info.classification);
    if( *Info.identcode )
        fprintf(OutFile,"Database Code ....... %s\n",Info.identcode);

    fprintf(OutFile,
      "@kinemage 1\n@caption %s v%s generated Kinemage\n",VER_NAME,VERSION);
    fputs("@onewidth\n",OutFile);

    if( DialValue[DialZoom] != 0.0 )
    {   if( DialValue[DialZoom]<0.0 )
        {   zoom = DialValue[DialZoom];
        } else zoom = MaxZoom*DialValue[DialZoom];
        fprintf(OutFile,"zoom %g\n",zoom+1.0);
    }

    if( Info.chaincount > 1 )
    {   for( chain=Database->clist; chain; chain=chain->cnext )
        {   fprintf(OutFile,"@group {chain %c}\n",chain->ident);
            WriteKinemageSpheres( chain );
            WriteKinemageBonds( chain );
            WriteKinemageLabels( chain );
        }
    } else
    {   fputs("@group {molecule}\n",OutFile);
        chain = Database->clist;
        WriteKinemageSpheres( chain );
        WriteKinemageBonds( chain );
        WriteKinemageLabels( chain );
    }

    WriteKinemageData();
    fclose(OutFile);
#ifdef APPLEMAC
    SetFileInfo(name,'MAGE','TEXT',135);
#endif
    return True;
}




/*===========================================*/
/*  VRML Virtual Reality Modelling Language  */
/*===========================================*/

static void WriteVRMLTriple( double x, double y, double z )
{
#ifdef __STDC__
    fprintf(OutFile,"%.3f %.3f %.3f",x,y,z);
#else
    fprintf(OutFile,"%.3lf %.3lf %.3lf",x,y,z);
#endif
}


static void WriteVRMLColour( int indent, int shade )
{
    register int i;

    for( i=0; i<indent; i++ )
        fputc(' ',OutFile);
    fputs("Material { diffuseColor ",OutFile);
    WriteVRMLTriple(Shade[shade].r/255.0,
                    Shade[shade].g/255.0,
                    Shade[shade].b/255.0);
    fputs(" }\n",OutFile);
}


static void WriteVRMLAtoms( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register double ox,oy,oz;
    register double x,y,z;
    register int i,flag;

    for( i=0; i<LastShade; i++ )
        if( Shade[i].refcount )
        {   flag = False;
            ox = 0.0;  oy = 0.0;  oz = 0.0;
            ForEachAtom
                if( (aptr->flag&SphereFlag)&&(i==Colour2Shade(aptr->col)) )
                {   if( !flag )
                    {   WriteVRMLColour(2,i);
                        fputs("  Separator {\n",OutFile);
                        flag = True;
                    }

                    x = (double)(aptr->xorg + aptr->fxorg)/250.0
                      + (double)(aptr->xtrl)/10000.;
                    y = (double)(aptr->yorg + aptr->fyorg)/250.0
                      + (double)(aptr->ytrl)/10000.;
                    z = -(double)(aptr->zorg + aptr->fzorg)/250.0
                      + (double)(aptr->ztrl)/10000.;

                    fputs("    Translation { translation ",OutFile);
                    WriteVRMLTriple(x-ox,RestoreY(y-oy),z-oz);
                    ox = x;  oy = y;  oz = z;
                    fputs(" }\n",OutFile);

                    fputs("    Sphere { radius ",OutFile);
#ifdef __STDC__
                    fprintf(OutFile,"%.3f }\n",(double)aptr->radius/250.0);
#else
                    fprintf(OutFile,"%.3f }\n",(double)aptr->radius/250.0);
#endif
                }

            if( flag )
                fputs("  }\n",OutFile);
        }
}
                            

static void WriteVRMLLine( int src, int dst, int shade, int *flag )
{
    if( !*flag )
    {   WriteVRMLColour(4,shade);
        fputs("    IndexedLineSet {\n",OutFile);
        fputs("      coordIndex [\n",OutFile);
        *flag = True;
    }
    fprintf(OutFile,"        %5d, %5d, -1,\n",src-1,dst-1);
}


static void WriteVRMLWireframe( void )
{
    register Chain __far *chain;
    register Group __far *group;
    register RAtom __far *aptr;
    register Bond __far *bptr;
    register RAtom __far *src;
    register RAtom __far *dst;
    register double x,y,z;
    register int i,j;
    static int flag;

    ForEachAtom
        aptr->mbox = 0;

    i = 0;
    ForEachBond
        if( bptr->flag & WireFlag )
        {   src = bptr->srcatom;
            dst = bptr->dstatom;
            if( i == 0 )
            {   fputs("  Separator {\n",OutFile);
                fputs("    Coordinate3 {\n",OutFile);
                fputs("      point [\n",OutFile);
            }

            if( !src->mbox )
            {   x = (double)(src->xorg + src->fxorg)/250.0
                  + (double)(src->xtrl)/10000.;
                y = (double)(src->yorg + src->fyorg)/250.0
                  + (double)(src->ytrl)/10000.;
                z = -(double)(src->zorg + src->fzorg)/250.0
                  - (double)(src->ztrl)/10000.;

                fputs("        ",OutFile);
                WriteVRMLTriple(x,RestoreY(y),z);
                fputs(",\n",OutFile);
                src->mbox = ++i;
            }
            
            if( !dst->mbox )
            {   x = (double)(dst->xorg + dst->fxorg)/250.0
                  + (double)(dst->xtrl)/10000.;
                y = (double)(dst->yorg + dst->fyorg)/250.0
                  + (double)(dst->ytrl)/10000.;
                z = -(double)(dst->zorg + dst->fzorg)/250.0
                  + (double)(dst->ztrl)/10000.;

                fputs("        ",OutFile);
                WriteVRMLTriple(x,RestoreY(y),z);
                fputs(",\n",OutFile);
                dst->mbox = ++i;
            }

            if( !bptr->col && (src->col!=dst->col) )
            {   x = (double)(src->xorg + dst->xorg +
                      src->fxorg + dst->fxorg)/500.0
                      +(double)(src->xtrl + dst->xtrl)/20000.;
                y = (double)(src->yorg + dst->yorg +
                      src->fyorg + dst->fyorg)/500.0
                      +(double)(src->ytrl + dst->ytrl)/20000.;
                z = -(double)(src->zorg + dst->zorg +
                      src->fzorg + dst->fzorg)/500.0
                      +(double)(src->ztrl + dst->ztrl)/20000.;
                
                fputs("        ",OutFile);
                WriteVRMLTriple(x,RestoreY(y),z);
                fputs(",\n",OutFile);
                i++;
            }
        }

    /* No wireframe! */
    if( !i )  return;

    fputs("      ]\n",OutFile);
    fputs("    }\n",OutFile);
    
    for( j=0; j<LastShade; j++ )
        if( Shade[j].refcount )
        {   i = 1;
            flag = False;
            ForEachBond
                if( bptr->flag & WireFlag )
                {   src = bptr->srcatom;   
                    dst = bptr->dstatom;

                    if( src->mbox == i ) i++;
                    if( dst->mbox == i ) i++;

                    if( bptr->col )
                    {   if( Colour2Shade(bptr->col) == j )
                            WriteVRMLLine(src->mbox,dst->mbox,j,&flag);
                    } else if( src->col == dst->col )
                    {   if( Colour2Shade(src->col) == j )
                            WriteVRMLLine(src->mbox,dst->mbox,j,&flag);
                    } else /* Two Colour Bond */
                    {   if( Colour2Shade(src->col) == j )
                        {   WriteVRMLLine(src->mbox,i,j,&flag);
                        } else if( Colour2Shade(dst->col) == j )
                            WriteVRMLLine(dst->mbox,i,j,&flag);
                        i++;
                    }
                }

            if( flag )
            {   fputs("      ]\n",OutFile);
                fputs("    }\n",OutFile);
            }
        }
    fputs("  }\n",OutFile);
}


static void WriteVRMLDots( void )
{
    auto int hist[LastShade];
    register DotStruct __far *ptr;
    register double x,y,z;
    register int count;
    register int flag;
    register int i,j;

    flag = False;
    for( i=0; i<LastShade; i++ )
        if( Shade[i].refcount )
        {   count = 0;

            for( ptr=DotPtr; ptr; ptr=ptr->next )
                for( j=0; j<ptr->count; j++ )
                    if( Colour2Shade(ptr->col[j]) == i )
                    {   if( !flag )
                        {   fputs("  Separator {\n",OutFile);
                            fputs("    Coordinate3 {\n",OutFile);
                            fputs("      point [\n",OutFile);
                            flag = True;
                        }

                        x = (double)ptr->xpos[j]/250.0;
                        y = (double)ptr->ypos[j]/250.0;
                        z = -(double)ptr->zpos[j]/250.0;

                        fputs("        ",OutFile);
                        WriteVRMLTriple(x,RestoreY(y),z);
                        fputs(",\n",OutFile);
                        count++;
                    }

            hist[i] = count;
        } else hist[i] = 0;

    if( flag )
    {   fputs("      ]\n",OutFile);
        fputs("    }\n",OutFile);

        count = 0;
        for( i=0; i<LastShade; i++ )
            if( hist[i] )
            {   WriteVRMLColour(4,i);
                fputs("    PointSet {\n      ",OutFile);
                fprintf(OutFile,"startIndex %d numPoints %d\n",count,hist[i]);
                fputs("    }\n",OutFile);
                count += hist[i];
            }

        fputs("  }\n",OutFile);
    }
}


int WriteVRMLFile( char *name )
{
    if( !Database )
        return True;

    OutFile = fopen(name,"w");
    if( !OutFile )
    {   FatalScriptError(name);
        return False;
    }

    fputs("#VRML V1.0 ascii\n",OutFile);
    fprintf(OutFile,"#Created by %s v%s\n\n", VER_NAME, VERSION);

    fputs("DEF Viewer Info { string \"examiner\" }\n",OutFile);
    fprintf(OutFile,"DEF Title Info { string \"%s\" }\n",Info.moleculename);
    fprintf(OutFile,
      "DEF Creator Info { string \"Created by %s v%s\" }\n",VER_NAME,VERSION);
    fputs("DEF BackgroundColor Info { string \"",OutFile);
    WriteVRMLTriple(BackR/255.0,BackG/255.0,BackB/255.0);
    fputs("\" }\n\n",OutFile);
    
    fputs("Separator {\n",OutFile);

#ifdef ORIG
    fputs("  DirectionalLight {\n",OutFile);
    fputs("    direction -1 -1 -2\n",OutFile);
    fputs("  }\n\n",OutFile);
#endif

    WriteVRMLAtoms();
    WriteVRMLWireframe();
    if( DotPtr )
        WriteVRMLDots();

    fputs("}\n",OutFile);
    fclose(OutFile);
#ifdef APPLEMAC
    SetFileInfo(name,'ttxt','TEXT',133);
#endif
    return True;
}

static void WriteBuffer ( char* buffer )
{
   if ( OutFile ) {
     fputs(buffer, OutFile);
   } else {
     WriteString ( buffer );
   }
 }

int WritePhiPsiAngles ( char *name, int ramachan )
{

  register Chain __far *chain;
  register Group __far *group;
  register RAtom __far *aptr;
  register Group __far *prev;
  register char c;
  char pplot[73][73];
  char buffer[130];
  AtomRef current;
  double phi, psi, omega;
  int i,j,iphi,jphi,ipsi,jpsi,model;

  if (ramachan < 0 )
    for (i = 0; i<73; i++)
      for (j = 0; j < 73; j++)
        pplot[i][j] = ' ';

  model = -1;

  if( !Database )
    return(True);

  if (name && !(name[0] == '-' && name[1] == '\0')) {
    OutFile = fopen(name,"w");
    if( !OutFile ){
      FatalScriptError(name);
      return(False);
    }
  } else {
    OutFile = (FILE *)0;
  }   
  /* Note: '#' is comment-sign in gnuplot */ 
  if (OutFile) {
    sprintf(buffer,"# File: %s\n",name);  
    WriteBuffer(buffer);
    sprintf(buffer,"# Creator: %s Version %s\n\n", VER_NAME, VERSION);
    WriteBuffer(buffer);
    sprintf(buffer, "# from Molecule: %s\n", 
      Info.moleculename);
    WriteBuffer(buffer);
    sprintf(buffer, "# Database Code: %s\n", 
      Info.identcode);
    WriteBuffer(buffer);
  }
  if(ramachan > 0 ){
    WriteBuffer("# phi/psi-angles for Ramachandran-Plot\n");
    WriteBuffer("#\n#\t\tPhi \t Psi\n");
  } else {
    if (ramachan < 0 ) {
    WriteBuffer("# Ramachandran Printer Plot\n");
    } else {
      WriteBuffer("# Combinations of backbone-angles\n");
    }
  }

  for(chain=Database->clist;chain;chain=chain->cnext){ 
    prev = (Group __far *)0;
    for(group=chain->glist;group;group=group->gnext){
      for(aptr=group->alist;aptr;aptr=aptr->anext){
    if( !(aptr->flag&SelectFlag) || !IsAmino(group->refno)){
      prev = group;
      break;      
    } 
    if(!aptr->anext){ 
      /* save reference to current group for later information */
      current.chn = chain;
      current.grp = group;
      current.atm = aptr;

      /* calculation of angles (-180.0 <= angle >= 180.0) */
      if(prev && IsAmino(prev->refno)){
         phi   = CalcPhiAngle(prev, group);
          omega =  CalcOmegaAngle(prev, group);
      } else {
        phi   = 360.0;
        omega = 360.0;
      }
     
      if(group->gnext && IsAmino(group->gnext->refno))
        psi   = CalcPsiAngle(group, group->gnext);
      else
        psi   = 360.0;

      if ( !(group->model == model) ) {
        if (model > 0 || group->model > 0 ) {
          sprintf(buffer, "Model: %d\n", group->model);
          WriteBuffer(buffer);
          if ( ( ramachan < 0)  && (model > 0) ) {
            WriteBuffer(
              "    ...........................................................................\n");
            for ( i = 0; i < 73 ; i++ ) {
              sprintf(buffer,"%5d.",180-5*i);
              for ( j = 0; j < 73; j++ ) buffer[5+j] = pplot[i][j];
              buffer[5+73] = '.'; buffer[5+74] = '\n'; buffer[5+75] = '\0';
              WriteBuffer(buffer);
            }
            WriteBuffer(
              "    ...........................................................................\n\n");
            WriteBuffer(
              "P    - - - - - - - - -                                       + + + + + + + + +\n");
            WriteBuffer(
              "s    1 1 1 1 1 1 1 1 1 - - - - - - - - -   + + + + + + + + + 1 1 1 1 1 1 1 1 1\n");
            WriteBuffer(
              "i    8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1   1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8\n");
            WriteBuffer(
              "Phi   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n");
          }
          for (i = 0; i<73; i++)
            for (j = 0; j < 73; j++)
              pplot[i][j] = ' ';
          model = group->model;
        }
      }

      /* writing (current group and) angles */
      if(ramachan){     /* data for Ramachandran-plot */
        if(fabs(phi) <= 180.0 && 180.0 >= fabs(psi)) {
          if (ramachan > 0) {
            sprintf(buffer, "%3s\t%c%d\t%#.1f\t%#.1f\n", 
              Residue[group->refno], 
              chain->ident, 
              group->serno, 
              phi, psi);
            WriteBuffer(buffer);
          }
          iphi = (int)((phi+182.5)/5.);
          ipsi = (int)((psi+182.5)/5.);
          ipsi = 72-ipsi;
          if (iphi < 0 ) iphi = 0;
          if (iphi > 72) iphi = 72;
          if (ipsi < 0 ) ipsi = 0;
          if (ipsi > 72) ipsi = 72;
          jphi = iphi;
          jpsi = ipsi;
          if (iphi == 0 || iphi == 72) {
            iphi = 0;
            jphi = 72;
          }
          if (ipsi == 0 || ipsi == 72) {
            ipsi = 0;
            jpsi = 72;
          }
          for ( i = iphi; i <= jphi; i+= 72 ) {
            for ( j = ipsi; j <= jpsi; j+=72 ) {
              c = pplot[j][i];
              if ( c == ' ' ) {
                c = '1';
              } else if ( c == '9' || c == '+' ) {
                c = '+';
              } else c++;
              pplot[j][i] = c;
            }
          }
        }  
      }
      else 
      {  WriteBuffer( DescribeObj(&current, GRP) ); 
         if( abs((int)phi) < 10.0 )
         {    sprintf(buffer, "    Phi =   ");
             WriteBuffer(buffer);
         } else if( abs((int)phi) < 100.0 )
         {    sprintf(buffer, "    Phi =  ");
            WriteBuffer(buffer);
         } else if(phi != 360.0) 
         {    sprintf(buffer, "    Phi = ");
            WriteBuffer(buffer);
         }

         if(phi != 360.0) 
         {  sprintf(buffer, "%# .1f", phi);    
            WriteBuffer(buffer);
         } else 
         {  sprintf(buffer, "                ");    
           WriteBuffer(buffer);
         }

         if( abs((int)psi) < 10.0 )
         {    sprintf(buffer, "    Psi =   ");
            WriteBuffer(buffer);
         } else if( abs((int)psi) < 100.0 )
         {    sprintf(buffer, "    Psi =  ");
            WriteBuffer(buffer);
         } else if(psi != 360.0) 
         {    sprintf(buffer, "    Psi = ");
            WriteBuffer(buffer);
         }

         if(psi != 360.0) 
         {  sprintf(buffer, "%# .1f", psi);
           WriteBuffer(buffer);
         } else 
         {  sprintf(buffer, "                ");    
           WriteBuffer(buffer);
         }
        /*  only omega-angles of cis-peptidbonds are listed, */
        /*  see 'molecule.h' for definition of 'CIS'          */
        if(fabs(omega) < CisBondCutOff)  {

         if( abs((int)omega) < 10.0 )
         {    sprintf(buffer, "    Omega =   ");
            WriteBuffer(buffer);
         } else if( abs((int)omega) < 100.0 )
         {    sprintf(buffer, "    Omega =  ");
            WriteBuffer(buffer);
         } else
         {    sprintf(buffer, "    Omega = ");
            WriteBuffer(buffer);
         }
          sprintf(buffer, "%# .1f", omega);
          WriteBuffer(buffer);
        }
        WriteBuffer("\n");
      }
      prev = group;
    }
      } /* for atom ... */
    } /* for group ... */ 
  } /* for chain ... */
 
  if (ramachan < 0) {
    WriteBuffer(
      "    ...........................................................................\n");
    for ( i = 0; i < 73 ; i++ ) {
      sprintf(buffer,"%4d.",180-5*i);
      for ( j = 0; j < 73; j++ ) buffer[5+j] = pplot[i][j];
        buffer[5+73] = '.'; buffer[5+74] = '\n'; buffer[5+75] = '\0';
      WriteBuffer(buffer);
    }
    WriteBuffer(
      "    ...........................................................................\n\n");
    WriteBuffer(
      "P    - - - - - - - - -                                       + + + + + + + + +\n");
    WriteBuffer(
      "s    1 1 1 1 1 1 1 1 1 - - - - - - - - -   + + + + + + + + + 1 1 1 1 1 1 1 1 1\n");
    WriteBuffer(
      "i    8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1   1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8\n");
    WriteBuffer(
      "Phi  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n");
  }

  if(OutFile) {
    fclose(OutFile);
#ifdef APPLEMAC
    SetFileInfo(name,'ttxt','TEXT',133);
#endif
  }
  return True;
}


/****************************************************************************\
 * POV-Ray:
 * ========
 *
 * Below you find all you need for the transformation of a RasMol-Image to a 
 * POV-script. Note, check in the help which Povray version is required.
 *
 * 'WritePOVRayFile' is the main function that writes the scriptfile, it's
 * an external function, all other functions used by WritePOVRay3File' are
 * internal (static).
\****************************************************************************/


/*==========================================================================*\
 * datatypes, variables and definitions for POV-work
\*==========================================================================*/

typedef struct _Coord {
  double x;
  double y;
  double z;
} Coord;


/* storing RGB-values */
typedef struct _Color{
  double r;
  double g;
  double b;
} Color;


/*==========================================================================*\
 * Prototypes of internal functions
\*==========================================================================*/
static Coord POVGetMidCoord(RAtom __far *src, RAtom __far *dst);
static Color POVGetColor(int col);
static void  POVWriteTexture(Color color);
static void  POVWriteHeader(void);
static void  POVWriteAtoms(void);
static void  POVWriteLine(RAtom __far *src,RAtom __far *dst,int rad,Byte flag, int cap);
static void  POVWriteBonds(void);
static void  POVWriteBackBone(void);
static void  POVWriteHSBonds(void);
static void  POVWriteDots(void);

/*=========================================================================*\
 * Definitions for the finish of the POV-scene, may me changed by user
 *
 * SPECPOWER: a high value makes a shiny surface, value from 0..1
 * ROUGHNESS: 1..0.005, low values for a small highlight (= very smooth),
 *            bigger values for a rough surface
 * DIFFUSE  : high value causes sharp shadows, objects in a shadow are
 *            very dark, LOW value means that less light comes directly
 *            from the lightsource, most light comes from everywhere. 
 *            Values from 0..1.
 * AMBIENT  : "ambient" together with "diffuse" controlls the scattered 
 *            light, higher values for ambient means that there are many
 *            lightsources everywhere in space. Values from 0..1
 * TRANSMIT : determines the transparence of a surface, values from 0..1,
 *            1.0 is completely transparent
 * DOTFACTOR: bigger values for bigger dots (spheres) of dots surfaces
 * MINWIRE  : because a cylinder in POVRay is not allowed to have radius 0
 *            you have to define a minimal value for this, default is 10,
 *            this creates a very thin cylinder
 * MINDASH  : the same as above only for a dashed wireframe
\*=========================================================================*/
#define SPECPOWER 0.4  
#define ROUGHNESS 0.005
#define DIFFUSE   0.8 
#define AMBIENT   0.2 
#define TRANSMIT  0.0

#define MAXNAME 512  /* maximal length of a filename */

/* to convert the world coordinates into original pdb coordinates */
#define Trans 250.0
#define INVTrans 0.004
#define TransO(o) ((double)(o)*INVTrans)
#define TransX(x) ((double)((x)+OrigCX)*INVTrans)
#define TransY(y) ((double)(RestoreY((y)+OrigCY)*INVTrans))
#define TransZ(z) (-((double)((z)+OrigCZ)*INVTrans))

#define DOTFACTOR (500.0) 
#define MINWIRE   (15)
#define MINDASH   (MINWIRE)

#define NOSHADEDOTS /* define this to avoid shade each dot of dotted surfaces */


/*==========================================================================*\
 * These internal but global variables are shared by internal POV-functions.
 * Because no function should change the values for file handling expect
 * the main function 'WritePOVRayFile' there's no need to use function 
 * parameters instead.
\*==========================================================================*/

static FILE *OutFile_inc = NULL; 
static char INCname[MAXNAME]   ; 
static char POVname[MAXNAME]   ;
static int POVCoor             ;



/*==========================================================================*\
 * Below you find all function definitions ...
\*==========================================================================*/
/****************************************************************************/

/*===========================================================================*\
 * internal function "POVWriteHeader"
\*===========================================================================*/

static void POVWriteHeader( void )
{
    double theta,phi,psi;

    fprintf(OutFile,"// Original image size: H = %d, W = %d\n\n", 
        YRange, XRange);   

    if( POVCoor==MolCoor )
    {   fputs("// Molecular coordinates", OutFile);
        fprintf(OutFile,"// OrigCX = %ld, OrigCY = %ld, OrigCZ = %ld, Scale = %g\n\n", 
          OrigCX, OrigCY, OrigCZ, Scale);
    } else
    {   if( POVCoor==WorldCoor )
            fputs("// World coordinates\n", OutFile);
        else if( POVCoor==ScreenCoor )
            fputs("// Screen coordinates\n", OutFile);
        else if( POVCoor==AbsCoor )
            fputs("// Absolute coordinates\n", OutFile);
        fprintf(OutFile,"// Scale = %g\n\n", Scale);
    }
   
    fputs("// Camera\ncamera {\n   orthographic\n", OutFile);
    fprintf(OutFile,"   location <%g, %g, %g>\n", 
        0.0, 
        0.0, 
        2*TransO(-ZRange));
    fprintf(OutFile,"   look_at <%g, %g, %g>\n", 
        0.0, 
        0.0, 
        TransO(-ZRange));
    
    /*To left-handed space (?)*/
    fprintf(OutFile,"   right <-1.33, 0, 0>\n");
    fputs("}\n\n",OutFile);
    
    /* (-1) factor for compensing 180o Z rotation, see below */
    fprintf(OutFile, "// Light\nlight_source {<%g, %g, %g>", 
        (double)(-1)*LightX/LightLength,
        (double)-(-1)*LightY/LightLength,
        (double)-2.0*LightZ/LightLength);       
    fputs("\n   color rgb <1.0, 1.0, 1.0>",OutFile);
    fputs("\n   parallel",OutFile);
    fputs("\n   point_at <0,0,0>",OutFile);
    fputs("\n}\n\n",OutFile);
    fprintf(OutFile,"background { color rgb <%g, %g, %g> }\n\n", 
            (double)BackR/255.0, (double)BackG/255.0, (double)BackB/255.0);

    fputs("union {   \n// Objects\n",OutFile);
    fprintf(OutFile,"   // union finish\n"
        "   // change \"T\" to a higher value (up to 1.0, e.g. 0.8) and\n"
        "   // uncomment \"hollow\" to make your object transparent\n"
        "   #declare F = finish { specular %g roughness %g\n"
        "                         diffuse %g\n"
        "                         ambient %g };\n"
        "   #declare T = %g;\n",
        SPECPOWER, ROUGHNESS, DIFFUSE, AMBIENT, TRANSMIT);  

    theta = WClip[0];
    theta = Wo[2];
    theta = -(WClip[0]-Wo[2])/(Scale*250);

    /* supports hollow slabbing only at the moment! */
    /* Screen coordinate gives an image identical to rastop
       Absolute coordinates are translated by world center */
    if( UseSlabPlane || UseDepthPlane )
    {   if( POVCoor==AbsCoor || POVCoor==ScreenCoor)
        {   if( UseSlabPlane && UseDepthPlane )
            {   fprintf(OutFile,"   #declare C = difference{\n"
                    "      plane{ z, %g }\n"
                    "      plane{ z, %g }\n"
                    "   }\n",
                    -(WClip[1]-Wo[2])/(Scale*250),
                    -(WClip[0]-Wo[2])/(Scale*250));
            } else if( UseSlabPlane )   
            {   fprintf(OutFile,"   #declare C = difference{\n"
                    "      plane{ z, %d }\n"
                    "      plane{ z, %g }\n"
                    "   }\n",
                    ZRange, 
                    -(WClip[0]-Wo[2])/(Scale*250));
            } else if( UseDepthPlane )
            {   fprintf(OutFile,"   #declare C = intersection{\n"
                    "      plane{ z, %d }\n"
                    "      plane{ z, %g }\n"
                    "   }\n",
                    ZRange, 
                    -(WClip[1]-Wo[2])/(Scale*250));
            }
        } else
            fprintf(OutFile,"   // clipping only supported in absolute and screen coordinates\n",OutFile);
    }

    fprintf(OutFile,"   #include \"%s\"\n",INCname);   

    /*if( !UseShadow )*/
    fputs("   no_shadow\n",OutFile);  // Comment // if shadow
    fputs("   // hollow\n\n",OutFile);  


    if( POVCoor==MolCoor )
    {   /* Retrieve rasmol coordinates */
        fputs("   // retrieve rasmol coordinates from pdb coordinates\n",OutFile);  
        fprintf(OutFile,"   translate \n"
             "        < %g,\t%g,\t%g >\n", 
             -OrigCX*INVTrans,-RestoreY(OrigCY)*INVTrans,OrigCZ*INVTrans);
    }
    /* Resolve orientation on screen - Error?
       Possible missing inversion on y, 
       see left-handed space above, 
       which introduces an inversion on x (-x,-y == 180o) */
    fputs("   // See code\n",OutFile);  
    fprintf(OutFile,"   rotate < 0, 0, 180>\n"), 
    fputs("\n",OutFile);  


    fputs("   // RasTop transformation\n",OutFile);  

    /* Decompose angles */
    phi = psi = theta = 0.0;
    if( POVCoor==MolCoor )
    {   RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
        theta *= 180.;
        phi *= 180.;
        psi *= 180.;

        if( !IsAlmostZero(theta) )
            fprintf(OutFile,"   rotate    < %g,\t0,\t0 >\n", theta); 
        if( !IsAlmostZero(phi) )
            fprintf(OutFile,"   rotate    < 0,\t%g,\t0 >\n", phi); 
        if( !IsAlmostZero(psi) )
            fprintf(OutFile,"   rotate    < 0,\t0,\t%g >\n", -psi); 
        fputs("\n",OutFile);  

        /* (-1) factor for compensing 180o Z rotation */
        fprintf(OutFile,    "   translate < %g,\t%g,\t%g >\n\n", 
          (-1)*MOffset[0]/(Scale*250.0),(-1)*RestoreY(MOffset[1])/(Scale*250.0),-MOffset[2]/(Scale*250.0));
    } else if( POVCoor==WorldCoor )
    {   double WRotX[3], WRotY[3], WRotZ[3];

        WRotX[0] = WAxes[0][0]; WRotX[1] = WAxes[0][1]; WRotX[2] = WAxes[0][2]; 
        WRotY[0] = WAxes[1][0]; WRotY[1] = WAxes[1][1]; WRotY[2] = WAxes[1][2]; 
        WRotZ[0] = WAxes[2][0]; WRotZ[1] = WAxes[2][1]; WRotZ[2] = WAxes[2][2]; 
        RMat2RV(&theta, &phi, &psi, WRotX, WRotY, WRotZ);
        theta *= 180.;
        phi *= 180.;
        psi *= 180.;

        if( !IsAlmostZero(theta) )
            fprintf(OutFile,"   rotate    < %g,\t0,\t0 >\n", theta); 
        if( !IsAlmostZero(phi) )
            fprintf(OutFile,"   rotate    < 0,\t%g,\t0 >\n", phi); 
        if( !IsAlmostZero(psi) )
            fprintf(OutFile,"   rotate    < 0,\t0,\t%g >\n", -psi); 

        /* (-1) factor for compensing 180o Z rotation */
        fprintf(OutFile,    "   translate < %g,\t%g,\t%g >\n\n", 
          (-1)*Wo[0]/(Scale*250.0),(-1)*RestoreY(Wo[1])/(Scale*250.0),-Wo[2]/(Scale*250.0));
    } else if( POVCoor==AbsCoor )
    {   /* (-1) factor for compensing 180o Z rotation */
        fprintf(OutFile,    "   translate < %g,\t%g,\t%g >\n\n", 
          (-1)*Wo[0]/(Scale*250.0),(-1)*RestoreY(Wo[1])/(Scale*250.0),-Wo[2]/(Scale*250.0));

    }
    
    fputs("   // Change scale factor for zooming\n",OutFile);  
    fprintf(OutFile,"   scale       %g\n",  Scale);   

    fputs("}\n", OutFile);
        
}


/*===========================================================================*\
 * returns a POVRAY-Color
\*===========================================================================*/
static Color POVGetColor( int col )
{

  ShadeDesc *shade;
  Color POVcol;

  shade = Shade+Colour2Shade(col);
  POVcol.r = (double)shade->r/255.0;
  POVcol.g = (double)shade->g/255.0;
  POVcol.b = (double)shade->b/255.0;
  
  return POVcol;
}


/*===========================================================================*\
 * writes the color of an object including finish
\*===========================================================================*/
static void POVWriteTexture( Color color )
{
        fputs("    texture {\n",OutFile_inc);
        fprintf(OutFile_inc,"      pigment {color rgbt <%g, %g, %g, T>}\n",
      (color).r, (color).g, (color).b),
        fputs("      finish {F}\n", OutFile_inc);
    fputs("    }\n", OutFile_inc);
}


static void Clip( void )
{   
   if( (UseSlabPlane || UseDepthPlane) && (POVCoor==AbsCoor || POVCoor==ScreenCoor) ) 
      fputs("   clipped_by{C}\n", OutFile_inc);
}


/*==========================================================================*\
 * "POVWriteSphere" writes a sphere with in the atom colour
 * but with an explicit given radius, so you can also use it for rounded
 * cylinder caps. 
\*==========================================================================*/
static void POVWriteSphere(RAtom  __far *aptr, double rad)
{
    double x, y, z;
    GetCurrentCoord(POVCoor,CurMolSet,aptr,&x,&y,&z);

    fprintf(OutFile_inc, "object {\n"        "   sphere { < %g , %g , %g > %g }\n",
    x,y,z,TransO(rad)),
    POVWriteTexture(POVGetColor((int)(aptr->col))), 
    Clip(),
    fputs("}\n", OutFile_inc);
}


/*==========================================================================*\
 * returns the coordinate that is exact between two given coordinates
\*==========================================================================*/
static Coord POVGetMidCoord( RAtom __far *src, RAtom __far *dst )
{
    double sx,sy,sz,dx,dy,dz;
    Coord d;

    GetCurrentCoord(POVCoor,CurMolSet,src,&sx,&sy,&sz);
    GetCurrentCoord(POVCoor,CurMolSet,dst,&dx,&dy,&dz);
    d.x = (sx + dx)/2;
    d.y = (sy + dy)/2; 
    d.z = (sz + dz)/2; 

    return d;
}


static void POVWriteAtoms( void )
{
  
    register Chain __far *chain;
    register Group __far *group;
    register RAtom  __far *aptr;

    ForEachAtom {
        if( !(aptr->flag&SphereFlag) )
            continue;
        POVWriteSphere(aptr, aptr->radius);
    }

}

static void POVWriteLine( RAtom __far *src, RAtom __far *dst,
                           int rad, Byte flag, int cap )
{

    Coord middle;
    double x,y,z;

    if( !(flag&(CylinderFlag|WireFlag|DashFlag) ) ) 
        return;

    middle = POVGetMidCoord(src, dst);
    if( flag&WireFlag)
      rad = MINWIRE;
    else if( flag&DashFlag)
      rad = MINDASH;
  
    GetCurrentCoord(POVCoor,CurMolSet,src,&x,&y,&z);
    fprintf(OutFile_inc,"object {\n"
        "   cylinder {<%g, %g, %g> <%g, %g, %g> %g}\n",
        x,y,z,
        middle.x,
        middle.y,
        middle.z,
        TransO(rad));
    POVWriteTexture(POVGetColor((int)src->col));
    Clip();
    fputs("}\n", OutFile_inc);

    GetCurrentCoord(POVCoor,CurMolSet,dst,&x,&y,&z);
    fprintf(OutFile_inc,"object {\n"
        "   cylinder {<%g, %g, %g> <%g, %g, %g> %g}\n",
        middle.x,
        middle.y,
        middle.z,
        x,y,z,        
        TransO(rad));
    POVWriteTexture(POVGetColor((int)dst->col));
    Clip();
    fputs("}\n", OutFile_inc);

    /* add cylinder caps */
    /* cap = 0  equivalent to cap = 3
       cap = 1  src is flat cap
       cap = 2  des is flat cap
       cap = 3  src and des are both flat cap */
    if( cap==1 )
        POVWriteSphere(dst, rad);
    else if( cap==2 )
        POVWriteSphere(src, rad);
    else if( cap==0 )
    {   if( !(src->flag&SphereFlag) || (src->flag&SphereFlag && rad > src->radius) )
            POVWriteSphere(src, rad);
        if( !(dst->flag&SphereFlag) || (dst->flag&SphereFlag && rad > dst->radius) )
            POVWriteSphere(dst, rad);
    }

}
        

static void POVWriteBonds( void )
{
    register Bond __far *bptr;

    ForEachBond {   
        if( (bptr->flag&DrawBondFlag) )
            POVWriteLine(bptr->srcatom, bptr->dstatom, bptr->radius, bptr->flag, bptr->cap);
    }  
}


static void POVWriteBackBone( void )
{
  register Bond  __far *bptr;
  register Chain __far *chain;

  ForEachBack {
    if( (bptr->flag&DrawBondFlag) )
      POVWriteLine(bptr->srcatom, bptr->dstatom, bptr->radius, bptr->flag,0);
  }  
 
}
  

static void POVWriteHSBonds( void )
{

  register HBond __far *bptr;

  for( bptr=CurMolecule->hlist; bptr; bptr=bptr->hnext ) {
    if( (bptr->flag&DrawBondFlag) )
      POVWriteLine(bptr->src, bptr->dst, bptr->radius, bptr->flag, 0);
  }

  for( bptr=CurMolecule->slist; bptr; bptr=bptr->hnext ) {
    if( (bptr->flag&DrawBondFlag) )
      POVWriteLine(bptr->src, bptr->dst, bptr->radius, bptr->flag, 0);
  }

}


static void POVWriteDots( void )
{

  register DotStruct __far *ptr;
  register int i;
  RAtom tmp;
  RAtom *atom = &tmp;

  ptr = DotPtr;

#ifdef NOSHADEDOTS
  if( UseShadow && ptr )
    fputs("union {\n",OutFile_inc);  
#endif   
  for( ptr=DotPtr; ptr; ptr=ptr->next ) {
    for( i=0; i<ptr->count; i++ ){ 
      tmp.xorg = ptr->xpos[i] ;
      tmp.yorg = ptr->ypos[i] ;
      tmp.zorg = ptr->zpos[i] ;
      tmp.col  = ptr->col[i]  ;
      tmp.fxorg = 0;
      tmp.fyorg = 0;
      tmp.fzorg = 0;
      atom     = &tmp;
      POVWriteSphere(atom, DOTFACTOR/(double)DotPtr->count);    
    }
  }
#ifdef NOSHADEDOTS
  if( UseShadow && ptr ) {
    fputs("no_shadow\n",OutFile_inc);
    fputs("}\n",OutFile_inc);
  }
#endif  

}


int WritePOVRayFile( char *name )
{
    register MolSet __far *mptr,*mcur;
    char *ptr;

    if( !MoleculeList )
        return False;

    POVCoor = SysCoor;

    if( strlen(name) > (MAXNAME-5) )
    {   FatalScriptError(name);
        return(False);
    }
    if( strlen(name) < 4 )
        return False;

    ptr = name + strlen(name) -1;
    if( *ptr=='v')
        if( *(ptr-1)=='o' )
            if( *(ptr-2)=='p' )
                if( *(ptr-3)=='.' )
                    *(ptr-3)='\0';

    strcpy(POVname, name);
    strcpy(INCname, name);
    strcat(POVname, ".pov");
    strcat(INCname, ".inc");

    if( !Database )
    return True;
  
    OutFile = fopen(POVname,"w");
    if( !OutFile ){
    FatalScriptError(POVname);
    return False;
    }

    OutFile_inc = fopen(INCname,"w");
    if( !OutFile ){
    FatalScriptError(INCname);
    return False;
    }

    fprintf(OutFile,"// File: %s\n",POVname);
    fprintf(OutFile,"// Creator: %s Version %s\n\n", VER_NAME, VERSION);
    fputs("// Version: POV-Ray Version 3.5\n",OutFile);
    fprintf(OutFile,"// Needs objectfile \"%s\"\n\n",INCname);

    fprintf(OutFile_inc,"// File: %s\n",INCname);
    fprintf(OutFile_inc,"// Creator: %s Version %s\n\n", VER_NAME, VERSION);
    fputs("// Version: POV-Ray Version 3.5\n",OutFile_inc);
    fputs("// Objects for POV-Ray Version 3.5\n\n",OutFile_inc);
      
    POVWriteHeader();

    if( SysCoor==MolCoor )
    {   if( CurMolSet->visible )     
        {   POVWriteAtoms();
            POVWriteBonds();
            POVWriteBackBone();
            POVWriteHSBonds();
            POVWriteDots();
        }
    } else
    {   mcur = CurMolSet;
        mptr = MoleculeList;
        while( mptr )
        {   SwitchMolecule(mptr);
            if( mptr->visible )     
            {   POVWriteAtoms();
                POVWriteBonds();
                POVWriteBackBone();
                POVWriteHSBonds();
                POVWriteDots();
            }
            mptr = mptr->next;
        }
        SwitchMolecule(mcur);
    }

    fclose(OutFile);
    fclose(OutFile_inc);
#ifdef APPLEMAC
    SetFileInfo(name,'ttxt','TEXT',133);
#endif
    return True;
}

