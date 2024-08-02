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

/* pixutils.c
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
#ifdef sun386
#include <stdlib.h>
#endif

#include <stdio.h>
#include <math.h>

#define PIXUTILS
#include "pixutils.h"
#include "graphics.h"
#include "molecule.h"
#include "abstree.h"
#include "transfor.h"
#include "repres.h"
#include "render.h"
#include "font.h"

/* Sutherland-Cohen Line Clipping Macros */
#define BitAbove    0x01
#define BitBelow    0x02
#define BitRight    0x04
#define BitLeft     0x08
#define BitFront    0x10

#define Reject(x,y)   ((x)&(y))
#define Accept(x,y)   (!((x)|(y)))


#ifdef INVERT
#define InvertY(y) (y)
#else
#define InvertY(y) (-(y))
#endif

typedef struct {
                Long dx,dz,di;
                Long x,z,i;
               } Edge;

typedef struct {
                double x;
               } ArcEntry;

#define ARCSIZE (4*MAXRAD)

#if defined(IBMPC) || defined(APPLEMAC)
static ArcEntry __far *ArcTable;
#else
static ArcEntry ArcTable[ARCSIZE];
#endif

static int CurrentRad;
static char FontDimen[23];
static int FontWid[97];
static int ClipStatus;

static int DepthTable[ColourDepth][ColourDepth];


#define SETPIXEL(dptr,fptr,d,c)    if( (d) > *(dptr) )              \
                                   {   *(dptr) = (d);               \
                                       *(fptr) = (c);               \
                                   }

#define SETPIXELP(dptr,fptr,d,c,ca,p) if( (d) > *(dptr))            \
                                   {   *(dptr) = (d);               \
                                       if(!p) {*(fptr) = (c); }     \
                                       else  {*(fptr) = (ca);}      \
                                   }

#define OutCode(res,x,y,z)            \
    {   if( (y)<0 )                   \
        {   (res) = BitAbove;         \
        } else if( (y) >= View.ymax ) \
        {   (res) = BitBelow;         \
        } else (res) = 0;             \
                                      \
        if( (x) < 0 )                 \
        {   (res) |= BitLeft;         \
        } else if( (x) >= View.xmax ) \
            (res) |= BitRight;        \
                                      \
        if( !ZValid((z)) )            \
            (res) |= BitFront;        \
    }



/*=======================*/
/*  Function Prototypes  */
/*=======================*/

#ifdef UNUSED
static int OutCode( int x, int y, int z )
{
    register int result;

    if( y < 0 )
    {   result = BitAbove;
    } else if( y >= View.ymax )
    {   result = BitBelow;
    } else result = 0;

    if( x < 0 )
    {   result |= BitLeft;
    } else if( x >= View.xmax )
        result |= BitRight;

    if( !ZValid(z) )
        result |= BitFront;
    return result;
}
#endif


void PlotPoint( int x, int y, int z, int col )
{
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register Long offset;

    /* SETPIXEL(dptr,fptr,z,Lut[col]); */

    offset = (Long)y*View.yskip+x;
    dptr = View.dbuf+offset;
    if( z > *dptr )
    {   fptr = View.fbuf+offset;
        *fptr = Lut[col];
        *dptr = z;
    }
}


void ClipPoint( int x, int y, int z, int col )
{
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register Long offset;

    if( XValid(x) && YValid(y) && ZValid(z) )
    {   /* PlotPoint(x,y,z,col); */
        offset = (Long)y*View.yskip+x;
        dptr = View.dbuf+offset;
        if( z > *dptr )
        {   fptr = View.fbuf+offset;
            *fptr = Lut[col];
            *dptr = z;
        }
    }
}


void PlotDeepPoint( int x, int y, int z, int col )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int inten;

    offset = (Long)y*View.yskip+x;
    dptr = View.dbuf+offset;

    if( z > *dptr )
    {  fptr = View.fbuf+offset;
       inten = (LastSpecShade*(z+ImageRadius-ZOffset+ShiftS))/ImageSize;
       if( inten>LastSpecShade ) inten = LastSpecShade;
       if( inten > 0 )
       {   *fptr = Lut[col+(inten&ColourMask)];
       } else *fptr = Lut[col];
       *dptr = z;
    }
}


void ClipDeepPoint( int x, int y, int z, int col )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int inten;

    if( XValid(x) && YValid(y) && ZValid(z) )
    {   /* PlotDeepPoint(x,y,z,col); */
        offset = (Long)y*View.yskip+x;
        dptr = View.dbuf+offset;

        if( z > *dptr )
        {  fptr = View.fbuf+offset;
           inten = (ColourDepth*(z+ImageRadius-ZOffset))/ImageSize;
           if( inten<0 ) inten = 0; if( inten>ColourMask ) inten = ColourMask;
           *fptr = Lut[col+inten];
           *dptr = z;
        }
    }
}



/*================================================*/
/*  Macros for Bresenhams Line Drawing Algorithm  */
/*================================================*/

#define CommonStep(s)  z1 += zrate; SETPIXELP(dptr,fptr,z1,c,ca,p);     \
                       if( (zerr+=dz)>0 ) { zerr-=(s); z1+=iz; }

#define XStep  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                 fptr+=ix; dptr+=ix; x1+=ix;                            \
                 p =  altc && (x1-mid<(dx/4)) && (mid-x1<(dx/4));       \
                 CommonStep(dx); }

#define YStep  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                 fptr+=ystep; dptr+=ystep; y1+=iy;                \
                 p =  altc && (y1-mid<(dy/4)) && (mid-y1<(dy/4)); \
                 CommonStep(dy); }
                     

void DrawTwinLine( int x1, int y1, int z1,
                   int x2, int y2, int z2,
                   int col1, int col2, char altl )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int zrate, zerr;
    register int ystep,err;
    register int ix,iy,iz;
    register int dx,dy,dz;
    register int mid=0;
    register Pixel c, ca;
    register int p, altc;

    c = Lut[col1];
    altc = 0;
    ca = c;
    if ( altl != '\0' && altl != ' ') {
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
      ca = Lut[altc];
    }
    

    offset = (Long)y1*View.yskip + x1;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;

    SETPIXEL(dptr,fptr,z1,c);

    dx = x2-x1;  dy = y2-y1; 
    if( !dx && !dy ) return;
    dz = z2-z1;

    if( dy<0 ) 
    {   ystep = -View.yskip;
        dy = -dy; 
        iy = -1;
    } else
    {   ystep = View.yskip;
        iy = 1;
    }

    if( dx<0 ) 
    {   dx = -dx;
        ix = -1;
    } else ix = 1;

    if( dz<0 ) 
    {   dz = -dz;
        iz = -1;
    } else iz = 1;

    if( dx>dy )
    {   if( dz >= dx )
        {   zrate = dz/dx;
            dz -= dx*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;
        err = zerr = -(dx>>1);

        if( col1 != col2 )
        {   mid = (x1+x2)>>1;
            while( x1!=mid ) XStep;
            c = Lut[col2];
        }
        while( x1!=x2 ) XStep;

    } else
    {   if( dz >= dy )
        {   zrate = dz/dy;
            dz -= dy*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;
        err = zerr = -(dy>>1);

        if( col1 != col2 )
        {   mid = (y1+y2)>>1;
            while( y1!=mid ) YStep;
            c = Lut[col2];
        }
        while( y1!=y2 ) YStep;
    }
}


void ClipLine( int x1, int y1, int z1,
               int x2, int y2, int z2,
               int col,  char altl )
{
    register int code1,code2;
    register int delta,rest;
    register int temp;

    OutCode(code1,x1,y1,z1);
    OutCode(code2,x2,y2,z2);
    if( Reject(code1,code2) )
        return;
  
    while( !Accept(code1,code2) )
    {  if( !code1 )
        {   temp=x1; x1=x2; x2=temp;
            temp=y1; y1=y2; y2=temp;
            temp=z1; z1=z2; z2=temp;
            code1 = code2;
            code2 = 0;
        }

        if( code1 & BitAbove )
        {   delta = y2-y1;
            x1 += (int)(((Long)y1*(x1-x2))/delta);  
            z1 += (int)(((Long)y1*(z1-z2))/delta);
            y1 = 0;
        } else if( code1 & BitLeft )
        {   delta = x2-x1;
            y1 += (int)(((Long)x1*(y1-y2))/delta);
            z1 += (int)(((Long)x1*(z1-z2))/delta);
            x1 = 0;
        } else if( code1 & BitRight )
        {   delta = x2-x1;
            temp=View.xmax-1; rest=temp-x1;
            y1 += (int)(((Long)rest*(y2-y1))/delta);
            z1 += (int)(((Long)rest*(z2-z1))/delta);
            x1 = temp;
        } else if( code1 & BitBelow )
        {   delta = y2-y1;
            temp=View.ymax-1; rest=temp-y1;
            x1 += (int)(((Long)rest*(x2-x1))/delta);
            z1 += (int)(((Long)rest*(z2-z1))/delta);
            y1 = temp;
        } else /* SLAB */
        {   delta = z2-z1;
            rest = (SlabValue-1)-z1;
            x1 += (int)(((Long)rest*(x2-x1))/delta);
            y1 += (int)(((Long)rest*(y2-y1))/delta);
            z1 = SlabValue-1;
        }

        OutCode(code1,x1,y1,z1);
        if( Reject(code1,code2) )
            return;

    }
    DrawTwinLine(x1,y1,z1,x2,y2,z2,col,col,altl);
}


void ClipTwinLine( int x1, int y1, int z1,
                   int x2, int y2, int z2,
                   int col1, int col2, char altl )
{
    register int xmid,ymid,zmid;
    register int code1,code2;


    if( col1!=col2 )
    {   OutCode(code1,x1,y1,z1);
        OutCode(code2,x2,y2,z2);
        if( !Reject(code1,code2) )
        {   if( !Accept(code1,code2) )
            {  xmid = (x1+x2)/2;
               ymid = (y1+y2)/2;
               zmid = (z1+z2)/2;
               ClipLine(x1,y1,z1,xmid,ymid,zmid,col1,altl);
               ClipLine(xmid,ymid,zmid,x2,y2,z2,col2,altl);
            } else
               DrawTwinLine(x1,y1,z1,x2,y2,z2,col1,col2,altl);
        }
    } else ClipLine(x1,y1,z1,x2,y2,z2,col1,altl);
}

void ClipDashLine( int x1, int y1, int z1,
                     int x2, int y2, int z2,
                     int col1, int col2, char altl )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int ix,iy,iz;
    register int dx,dy,dz;
    register int zrate, zerr;
    register int ystep,err;
    register int co;
    register Pixel c, ca, mid;
    register int count;
    register int altc;

    if( (x1==x2) && (y1==y2) )
         return;

    /* Reject(OutCode(x1,y1,z1),OutCode(x2,y2,z2)) */
    if( (x1<0) && (x2<0) ) return;
    if( (y1<0) && (y2<0) ) return;
    if( (x1>=View.xmax) && (x2>=View.xmax) ) return;
    if( (y1>=View.ymax) && (y2>=View.ymax) ) return;

    c = Lut[col1];
    altc = 0;
    ca = c;
    if ( altl != '\0' && altl != ' ') {
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
      ca = Lut[altc];
    }

    dx = x2 - x1;  
    dy = y2 - y1;
    dz = z2 - z1;  

    offset = (Long)y1*View.yskip + x1;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;
    count = 0;

    ystep = View.yskip;
    ix = iy = iz = 1;
    if( dy<0 ) { dy = -dy; iy = -1; ystep = -ystep; }
    if( dx<0 ) { dx = -dx; ix = -1; }
    if( dz<0 ) { dz = -dz; iz = -1; }

    if( dx>dy )
    {   if( x2<x1 )
        {   mid = col1;
            col1 = col2;
            col2 = mid;
        }
        if( dz >= dx )
        {   zrate = dz/dx;
            dz -= dx*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        err = zerr = -(dx>>1);
        mid = (x1+x2)/2;

        while( x1!=x2 )
        {   if( XValid(x1) && YValid(y1) && ZValid(z1) )
            {   if( count<2 )
                {   co = (x1<mid)? col1 : col2;
                    c = Lut[co];
                    SETPIXEL(dptr,fptr,z1,c);
                    count++;
                } else if( count==3 )
                {   count = 0;
                } else count++;
            }

            if( (err+=dy)>0 )
            {   err -= dx;
                fptr+=ystep;
                dptr+=ystep;
                y1+=iy;
            }

            if( (zerr+=dz)>0 )
            {   zerr -= dx;
                z1 += iz;
            }

            fptr+=ix; dptr+=ix; x1+=ix;
            z1 += zrate;
        }
    } else
    {   if( y1>y2 )
        {   mid = col1;
            col1 = col2;
            col2 = mid;
        }

        if( dz >= dy )
        {   zrate = dz/dy;
            dz -= dy*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        err = zerr = -(dy>>1);
        mid = (y1+y2)/2;

        
        while( y1!=y2 )
        {   if( XValid(x1) && YValid(y1) && ZValid(z1) )
            {   if( count<2 )
                {   co = (y1<mid)? col1 : col2;
                    c = Lut[co];
                    SETPIXEL(dptr,fptr,z1,c);
                    count++;
                } else if( count==3 )
                {   count = 0;
                } else count++;
            }

            if( (err+=dx)>0 )
            {   err-=dy;
                fptr+=ix;
                dptr+=ix;
                x1+=ix;
            }

            if( (zerr+=dz)>0 )
            {   zerr -= dy;
                z1 += iz;
            }

            fptr+=ystep; dptr+=ystep; y1+=iy;
            z1 += zrate; 
        }
    }
}


/*=============================================*/
/*  Macros for 3D Bresenhams Vector Algorithm  */
/*=============================================*/

#define CommonVectStep(s)  z1 += zrate;   c1 += crate; c2 -= crate;       \
                 SETPIXELP(dptr,fptr,z1,Lut[col+c1],Lut[cola],p);       \
                 if( (zerr+=dz)>0 ) while ( zerr > 0 ) { zerr -= (s); z1 += iz; }  \
                 if( (cerr+=dc)>0 ) while ( cerr > 0 ) { cerr -= (s); c1 += iz; }

#define XVectStep  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                     fptr+=ix; dptr+=ix; x1+=ix;                            \
                     p =  altc && (x1-mid<(dx/4)) && (mid-x1<(dx/4));      \
                     CommonVectStep(dx); }

#define YVectStep  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                     fptr+=ystep; dptr+=ystep; y1+=iy;                \
                     p =  altc && (y1-mid<(dy/4)) && (mid-y1<(dy/4));\
                     CommonVectStep(dy); }


static void DrawTwinVector( int x1, int y1, int z1,
                     int x2, int y2, int z2,
                     int col1, int col2, char altl )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int dx,dy,dz,dc;
    register int crate, cerr;
    register int zrate, zerr;
    register int ystep,err;
    register int ix,iy,iz;
    register int col, cola,  mid;
    register int c1, c2;
    register int p, altc;

    c1 = (LastSpecShade*(z1+ImageRadius-ZOffset+ShiftS))/ImageSize;
    if( c1<0 ) c1 = 0; if( c1>LastSpecShade ) c1 = LastSpecShade;
    c2 = (LastSpecShade*(z2+ImageRadius-ZOffset+ShiftS))/ImageSize;
    if( c2<0 ) c2 = 0; if( c2>LastSpecShade ) c2 = LastSpecShade;

    altc = 0;
    if ( altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
    cola = altc;

    offset = (Long)y1*View.yskip + x1;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;

    SETPIXEL(dptr,fptr,z1,Lut[col1+c1]);

    dx = x2 - x1;  dy = y2 - y1;
    dz = z2 - z1;  dc = c2 - c1;
    if( !dx && !dy ) return;

    if( dy<0 ) 
    {   ystep = -View.yskip;
        dy = -dy; 
        iy = -1; 
    } else
    {   ystep = View.yskip;
        iy = 1;
    }

    if( dx<0 ) 
    {   dx = -dx; 
        ix = -1; 
    } else ix = 1;

    iz = (dz<0)? -1 : 1;

    if( dx>dy )
    {   if( dz >= dx )
        {   zrate = dz/dx;
            dz -= dx*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        if( dc >= dx )
        {   crate = dc/dx;
            dc -= dx*crate;
            if( iz < 0 )
                crate = -crate;
        } else crate = 0;

        err = zerr = cerr = -(dx>>1);
        col = col1;

        if( dz<0 )
        {   dz = -dz;
            dc = -dc;
        }
        
        if( col1 != col2 )
        {   mid = (x1+x2)>>1;
            while( x1!=mid ) XVectStep;
            col = col2;
        }
        while( x1!=x2 ) XVectStep;
    } else
    {   if( dz >= dy )
        {   zrate = dz/dy;
            dz -= dy*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        if( dc >= dy )
        {   crate = dc/dy;
            dc -= dy*crate;
            if( iz < 0 )
                crate = -crate;
        } else crate = 0;

        err = zerr = cerr = -(dy>>1);
        col = col1;

        if( dz<0 )
        {   dz = -dz;
            dc = -dc;
        }

        if( col1 != col2 )
        {   mid = (y1+y2)>>1;
            while( y1!=mid ) YVectStep;
            col = col2;
        }
        while( y1!=y2 ) YVectStep;
    }
}


static void ClipVector( int x1, int y1, int z1,
                        int x2, int y2, int z2,
                        int col, char altl )
{
    register int code1,code2;
    register int delta,rest;
    register int temp;

    OutCode(code1,x1,y1,z1);
    OutCode(code2,x2,y2,z2);
    if( Reject(code1,code2) )
        return;

    while( !Accept(code1,code2) )
    {   if( !code1 )
        {   temp=x1; x1=x2; x2=temp;
            temp=y1; y1=y2; y2=temp;
            temp=z1; z1=z2; z2=temp;
            code1 = code2;
            code2 = 0;
        }

        if( code1 & BitAbove )
        {   delta = y2-y1;
            x1 += (int)(((Long)y1*(x1-x2))/delta);  
            z1 += (int)(((Long)y1*(z1-z2))/delta);
            y1 = 0;
        } else if( code1 & BitLeft )
        {   delta = x2-x1;
            y1 += (int)(((Long)x1*(y1-y2))/delta);
            z1 += (int)(((Long)x1*(z1-z2))/delta);
            x1 = 0;
        } else if( code1 & BitRight )
        {   delta = x2-x1;
            temp=View.xmax-1; rest=temp-x1;
            y1 += (int)(((Long)rest*(y2-y1))/delta);
            z1 += (int)(((Long)rest*(z2-z1))/delta);
            x1 = temp;
        } else if( code1 & BitBelow )
        {   delta = y2-y1;
            temp=View.ymax-1; rest=temp-y1;
            x1 += (int)(((Long)rest*(x2-x1))/delta);
            z1 += (int)(((Long)rest*(z2-z1))/delta);
            y1 = temp;
        } else /* SLAB */
        {   delta = z2-z1;
            rest = (SlabValue-1)-z1;
            x1 += (int)(((Long)rest*(x2-x1))/delta);
            y1 += (int)(((Long)rest*(y2-y1))/delta);
            z1 = SlabValue-1;
        }
        OutCode(code1,x1,y1,z1);
        if( Reject(code1,code2) )
            return;
    }
    DrawTwinVector(x1,y1,z1,x2,y2,z2,col,col,altl);
}


void ClipTwinVector( int x1, int y1, int z1,
                     int x2, int y2, int z2,
                     int col1, int col2, char altl )
{
    register int xmid,ymid,zmid;
    register int code1,code2;

    if( col1!=col2 )
    {   OutCode(code1,x1,y1,z1);
        OutCode(code2,x2,y2,z2);
        if( !Reject(code1,code2) )
        {   if( !Accept(code1,code2) )
            {  xmid = (x1+x2)/2;
               ymid = (y1+y2)/2;
               zmid = (z1+z2)/2;
               ClipVector(x1,y1,z1,xmid,ymid,zmid,col1,altl);
               ClipVector(xmid,ymid,zmid,x2,y2,z2,col2,altl);
            } else
               DrawTwinVector(x1,y1,z1,x2,y2,z2,col1,col2,altl);
        }
    } else
        ClipVector(x1,y1,z1,x2,y2,z2,col1,altl);
}


/*==================================*/
/*  Monochrome Depth-Cued Vectors!  */
/*==================================*/

void DrawTwinVector2( int x1, int y1, int z1,
                      int x2, int y2, int z2,
                      int col1, int col2 )
{
    register int inten;
    register int midz;

    midz = ((z1+z2)/2)+ImageRadius-ZOffset;
    if( midz >= ImageSize )
    {   inten = ColourMask;
    } else if( midz > 0 )
    {   inten = (LastSpecShade*(midz+ShiftS))/ImageSize;
        if( inten<0 ) inten = 0; if( inten>LastSpecShade ) inten = LastSpecShade;
    } else inten = 0;
    DrawTwinLine(x1,y1,z1,x2,y2,z2,col1+inten,col2+inten,' ');
}


void ClipTwinVector2( int x1, int y1, int z1,
                      int x2, int y2, int z2,
                      int col1, int col2 )
{
    register int inten;
    register int midz;

    midz = ((z1+z2)/2)+ImageRadius-ZOffset;
    if( midz >= ImageSize )
    {   inten = ColourMask;
    } else if( midz > 0 )
    {   inten = (LastSpecShade*(midz+ShiftS))/ImageSize;
        if( inten<0 ) inten = 0; if( inten>LastSpecShade ) inten = LastSpecShade;
    } else inten = 0;
    ClipTwinLine(x1,y1,z1,x2,y2,z2,col1+inten,col2+inten,' ');
}


void ClipDashVector( int x1, int y1, int z1,
                     int x2, int y2, int z2,
                     int col1, int col2, char altl )
{
    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int ix,iy,iz,ic;
    register int dx,dy,dz,dc;
    register int crate, cerr;
    register int zrate, zerr;
    register int ystep,err;
    register int col, cola, mid;
    register int c1, c2;
    register int count;
    register int p, altc;

    if( (x1==x2) && (y1==y2) )
         return;

    /* Reject(OutCode(x1,y1,z1),OutCode(x2,y2,z2)) */
    if( (x1<0) && (x2<0) ) return;
    if( (y1<0) && (y2<0) ) return;
    if( (x1>=View.xmax) && (x2>=View.xmax) ) return;
    if( (y1>=View.ymax) && (y2>=View.ymax) ) return;
    if( UseSlabPlane && (z1>=SlabValue) && (z2>=SlabValue) )
        return;

    c1 = (LastSpecShade*(z1+ImageRadius-ZOffset+ShiftS))/ImageSize;
    if( c1<0 ) c1 = 0; if( c1>LastSpecShade ) c1 = LastSpecShade;
    c2 = (LastSpecShade*(z2+ImageRadius-ZOffset+ShiftS))/ImageSize;
    if( c2<0 ) c2 = 0; if( c2>LastSpecShade ) c2 = LastSpecShade;

    altc = 0;
    if ( altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
    cola = altc;

    dx = x2 - x1;  dy = y2 - y1;
    dz = z2 - z1;  dc = c2 - c1;

    offset = (Long)y1*View.yskip + x1;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;
    count = 0;

    ystep = View.yskip;
    ix = iy = iz = ic = 1;
    if( dy<0 ) { dy = -dy; iy = -1; ystep = -ystep; }
    if( dx<0 ) { dx = -dx; ix = -1; }
    if( dz<0 ) { dz = -dz; iz = -1; }
    if( dc<0 ) { dc = -dc; ic = -1; }

    if( dx>dy )
    {   if( x2<x1 )
        {   mid = col1;
            col1 = col2;
            col2 = mid;
        }
        if( dz >= dx )
        {   zrate = dz/dx;
            dz -= dx*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        if( dc >= dx )
        {   crate = dc/dx;
            dc -= dx*crate;
        } else crate = 0;

        err = zerr = cerr = -(dx>>1);
        mid = (x1+x2)/2;

        while( x1!=x2 )
        {   if( XValid(x1) && YValid(y1) && ZValid(z1) )
            {   if( count<2 )
                {   col = (x1<mid)? col1 : col2;
                    cola = (x1<mid)? col2 : col1;
                    p =  altc&&(abs(x1-mid)<abs(dx)/4);
                    SETPIXELP(dptr,fptr,z1,Lut[col+c1], Lut[cola],p);
                    count++;
                } else if( count==3 )
                {   count = 0;
                } else count++;
            }

            if( (err+=dy)>0 )
            {   err -= dx;
                fptr+=ystep;
                dptr+=ystep;
                y1+=iy;
            }

            if( (zerr+=dz)>0 )
            {   zerr -= dx;
                z1 += iz;
            }

            if( (cerr+=dc)>0 )
            {   cerr -= dx;
                c1 += ic;
            }

            fptr+=ix; dptr+=ix; x1+=ix;
            z1 += zrate;   c1 += crate;
        }
    } else
    {   if( y1>y2 )
        {   mid = col1;
            col1 = col2;
            col2 = mid;
        }

        if( dz >= dy )
        {   zrate = dz/dy;
            dz -= dy*zrate;
            if( iz < 0 )
                zrate = -zrate;
        } else zrate = 0;

        if( dc >= dy )
        {   crate = dc/dy;
            dc -= dy*crate;
        } else crate = 0;

        err = zerr = cerr = -(dy>>1);
        mid = (y1+y2)/2;

        
        while( y1!=y2 )
        {   if( XValid(x1) && YValid(y1) && ZValid(z1) )
            {   if( count<2 )
                {   col = (y1<mid)? col1 : col2;
                    p =  altc&&(abs(y1-mid)<abs(dy)/4);
                    SETPIXELP(dptr,fptr,z1,Lut[col+c1],Lut[cola],p);
                    count++;
                } else if( count==3 )
                {   count = 0;
                } else count++;
            }

            if( (err+=dx)>0 )
            {   err-=dy;
                fptr+=ix;
                dptr+=ix;
                x1+=ix;
            }

            if( (zerr+=dz)>0 )
            {   zerr -= dy;
                z1 += iz;
            }

            if( (cerr+=dc)>0 )
            {   cerr -= dy;
                c1 += ic;
            }

            fptr+=ystep; dptr+=ystep; y1+=iy;
            z1 += zrate;   c1 += crate; c2 -=crate;
        }
    }
}


/* SplineCount is either 1, 2, 3, 4, 5 or 9! */
void StrandRibbon( Knot __far *src, Knot __far *dst, int col1, int col2 )
{
    register double hsx, hsy, hsz;
    register double hdx, hdy, hdz;
    register double qsx, qsy, qsz;
    register double qdx, qdy, qdz;
    register int col;

    if( SplineCount != 4 )
    {   if( SplineCount == 1 ) 
        {   ClipVector( Rint(src->px), Rint(src->py), Rint(src->pz),
                        Rint(dst->px), Rint(dst->py), Rint(dst->pz), col2, ' ' );
            return;
        } else if( SplineCount != 2 )
            ClipVector( Rint(src->px), Rint(src->py), Rint(src->pz),
                        Rint(dst->px), Rint(dst->py), Rint(dst->pz), col1, ' ' );

        ClipVector( Rint(src->px+src->wx), Rint(src->py+src->wy), Rint(src->pz+src->wz),
                    Rint(dst->px+dst->wx), Rint(dst->py+dst->wy), Rint(dst->pz+dst->wz), 
                    col2, ' ' );
        ClipVector( Rint(src->px-src->wx), Rint(src->py-src->wy), Rint(src->pz-src->wz),
                    Rint(dst->px-dst->wx), Rint(dst->py-dst->wy), Rint(dst->pz-dst->wz), 
                    col2, ' ' );
        if( SplineCount<=3 ) return;

        hsx = src->wx/2;  hsy = src->wy/2;  hsz = src->wz/2;
        hdx = dst->wx/2;  hdy = dst->wy/2;  hdz = dst->wz/2;

        ClipVector( Rint(src->px+hsx), Rint(src->py+hsy), Rint(src->pz+hsz),
                    Rint(dst->px+hdx), Rint(dst->py+hdy), Rint(dst->pz+hdz), col1, ' ' );
        ClipVector( Rint(src->px-hsx), Rint(src->py-hsy), Rint(src->pz-hsz),
                    Rint(dst->px-hdx), Rint(dst->py-hdy), Rint(dst->pz-hdz), col1, ' ' );
        if( SplineCount==5 ) 
            return;
        col = col1;
    } else /* SplineCount == 4 */
    {   hsx = src->wx/2;  hsy = src->wy/2;  hsz = src->wz/2;
        hdx = dst->wx/2;  hdy = dst->wy/2;  hdz = dst->wz/2;
        col = col2;
    }

    qsx = hsx/2;  qsy = hsy/2;  qsz = hsz/2;
    qdx = hdx/2;  qdy = hdy/2;  qdz = hdz/2;

    ClipVector( Rint(src->px+hsx+qsx), Rint(src->py+hsy+qsy), Rint(src->pz+hsz+qsz),
                Rint(dst->px+hdx+qdx), Rint(dst->py+hdy+qdy), Rint(dst->pz+hdz+qdz), 
                col, ' ' );
    ClipVector( Rint(src->px+hsx-qsx), Rint(src->py+hsy-qsy), Rint(src->pz+hsz-qsz),
                Rint(dst->px+hdx-qdx), Rint(dst->py+hdy-qdy), Rint(dst->pz+hdz-qdz), 
                col1, ' ' );
    ClipVector( Rint(src->px-hsx+qsx), Rint(src->py-hsy+qsy), Rint(src->pz-hsz+qsz),
                Rint(dst->px-hdx+qdx), Rint(dst->py-hdy+qdy), Rint(dst->pz-hdz+qdz), 
                col1, ' ' );
    ClipVector( Rint(src->px-hsx-qsx), Rint(src->py-hsy-qsy), Rint(src->pz-hsz-qsz),
                Rint(dst->px-hdx-qdx), Rint(dst->py-hdy-qdy), Rint(dst->pz-hdz-qdz), 
                col, ' ' );
}


void DashRibbon( Knot __far *src, Knot __far *dst, int col1, int col2 )
{
    register double hsx, hsy, hsz;
    register double hdx, hdy, hdz;
    register double qsx, qsy, qsz;
    register double qdx, qdy, qdz;
    register int col;

    if( SplineCount != 4 )
    {   if( SplineCount == 1 ) 
        {   ClipDashVector( Rint(src->px), Rint(src->py), Rint(src->pz),
                            Rint(dst->px), Rint(dst->py), Rint(dst->pz), col2, col2, ' ' );
            return;
        } else if( SplineCount != 2 )
            ClipDashVector( Rint(src->px), Rint(src->py), Rint(src->pz),
                            Rint(dst->px), Rint(dst->py), Rint(dst->pz), col1, col1, ' ' );

        ClipDashVector(Rint(src->px+src->wx),Rint(src->py+src->wy),Rint(src->pz+src->wz),
                       Rint(dst->px+dst->wx),Rint(dst->py+dst->wy),Rint(dst->pz+dst->wz),
                       col2,col2, ' ');
        ClipDashVector(Rint(src->px-src->wx),Rint(src->py-src->wy),Rint(src->pz-src->wz),
                       Rint(dst->px-dst->wx),Rint(dst->py-dst->wy),Rint(dst->pz-dst->wz),
                       col2,col2, ' ');
        if( SplineCount<=3 ) return;

        hsx = src->wx/2;  hsy = src->wy/2;  hsz = src->wz/2;
        hdx = dst->wx/2;  hdy = dst->wy/2;  hdz = dst->wz/2;

        ClipDashVector( Rint(src->px+hsx), Rint(src->py+hsy), Rint(src->pz+hsz),
                        Rint(dst->px+hdx), Rint(dst->py+hdy), Rint(dst->pz+hdz), 
                        col1, col1, ' ' );
        ClipDashVector( Rint(src->px-hsx), Rint(src->py-hsy), Rint(src->pz-hsz),
                        Rint(dst->px-hdx), Rint(dst->py-hdy), Rint(dst->pz-hdz), 
                        col1, col1, ' ' );
        if( SplineCount==5 ) 
            return;
        col = col1;
    } else /* SplineCount == 4 */
    {   hsx = src->wx/2;  hsy = src->wy/2;  hsz = src->wz/2;
        hdx = dst->wx/2;  hdy = dst->wy/2;  hdz = dst->wz/2;
        col = col2;
    }

    qsx = hsx/2;  qsy = hsy/2;  qsz = hsz/2;
    qdx = hdx/2;  qdy = hdy/2;  qdz = hdz/2;

    ClipDashVector(Rint(src->px+hsx+qsx),Rint(src->py+hsy+qsy),Rint(src->pz+hsz+qsz),
                   Rint(dst->px+hdx+qdx),Rint(dst->py+hdy+qdy),Rint(dst->pz+hdz+qdz),
                   col,col, ' ');
    ClipDashVector(Rint(src->px+hsx-qsx),Rint(src->py+hsy-qsy),Rint(src->pz+hsz-qsz),
                   Rint(dst->px+hdx-qdx),Rint(dst->py+hdy-qdy),Rint(dst->pz+hdz-qdz),
                   col1,col1, ' ');
    ClipDashVector(Rint(src->px-hsx+qsx),Rint(src->py-hsy+qsy),Rint(src->pz-hsz+qsz),
                   Rint(dst->px-hdx+qdx),Rint(dst->py-hdy+qdy),Rint(dst->pz-hdz+qdz),
                   col1,col1, ' ');
    ClipDashVector(Rint(src->px-hsx-qsx),Rint(src->py-hsy-qsy),Rint(src->pz-hsz-qsz),
                   Rint(dst->px-hdx-qdx),Rint(dst->py-hdy-qdy),Rint(dst->pz-hdz-qdz),
                   col,col, ' ');
}


#ifdef UNUSED /* Unused Function */
static void OutLinePolygon( Poly *p )
{
    register int i;

    for( i=0; i<p->count-1; i++ )
         ClipLine( p->v[i].x, p->v[i].y, p->v[i].z, 
                   p->v[i+1].x, p->v[i+1].y, p->v[i+1].z,
                   p->v[i].inten);
    ClipLine( p->v[i].x, p->v[i].y, p->v[i].z,
              p->v[0].x, p->v[0].y, p->v[0].z,
              p->v[i].inten);
}
#endif


#ifdef UNUSED
static void DrawPolygon(  Poly *p )
{
    static Edge lft, rgt;
    register Edge *pmin, *pmax;
    register Pixel __huge *fbase;
    register short __huge *dbase;
    register short __huge *dptr;
    register Long offset;

    register Long dz,di;
    register Long z,inten;
    register int ri,li,ry,ly;
    register int xmin,xmax;
    register int dy,ymin;
    register int top,rem;
    register int x,y,i;

    /* Find top vertex */
    top = 0;  
    ymin = p->v[0].y;
    for( i=1; i<p->count; i++ )
       if( p->v[i].y < ymin )
       {   ymin = p->v[i].y;
           top = i;
       }

    rem = p->count;
    ly = ry = y = ymin;
    li = ri = top;

    offset = (Long)y*View.yskip;
    fbase = View.fbuf+offset;
    dbase = View.dbuf+offset;

    while( rem )
    {   while( ly<=y && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ly;
                lft.di = (((Long)(p->v[i].inten - p->v[li].inten))<<16)/dy;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.i = ((Long)p->v[li].inten)<<16;
                lft.x = ((Long)p->v[li].x)<<16;
                lft.z = ((Long)p->v[li].z)<<16;
            }
            ly = p->v[i].y;
            rem--;  li = i;
        }

        while( ry<=y && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ry;
                rgt.di = (((Long)(p->v[i].inten - p->v[ri].inten))<<16)/dy;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.i = ((Long)p->v[ri].inten)<<16;
                rgt.x = ((Long)p->v[ri].x)<<16;
                rgt.z = ((Long)p->v[ri].z)<<16;
            }
            ry = p->v[i].y;
            rem--; ri = i;
        }

        ymin = MinFun(ly,ry);
        
        while( y<ymin )
        {   if( lft.x < rgt.x )
            {   pmin = &lft;
                pmax = &rgt;
            } else
            {   pmin = &rgt;
                pmax = &lft;
            }

            xmax = (int)(pmax->x>>16)+1;
            xmin = (int)(pmin->x>>16);

            di = (Long)((pmax->i-pmin->i)/(xmax-xmin));
            dz = (Long)((pmax->z-pmin->z)/(xmax-xmin));
            inten = pmin->i;  
            z = pmin->z;

            dptr = dbase+xmin;
            for( x=xmin; x<xmax; x++ )
            {   if( (int)(z>>16) > *dptr )
                {   fbase[x] = Lut[(int)(inten>>16)];
                    *dptr = (int)(z>>16);
                }
                inten += di;
                z += dz;
                dptr++;
            }

            lft.x += lft.dx;  rgt.x += rgt.dx;
            lft.z += lft.dz;  rgt.z += rgt.dz;
            lft.i += lft.di;  rgt.i += rgt.di;
            dbase += View.yskip;
            fbase += View.yskip;
            y++;
        }
    }
}
#endif


#ifdef UNUSED
static void DrawFlatPolygon( Poly *p )
{
    static Edge lft, rgt;
    register Edge *pmin, *pmax;
    register Pixel __huge *fbase;
    register short __huge *dbase;
    register short __huge *dptr;
    register Long offset;

    register Long z,dz;
    register int ri,li,ry,ly;
    register int xmin,xmax;
    register int dy,ymin;
    register int top,rem;
    register int x,y,i;

    /* Find top vertex */
    top = 0;  
    ymin = p->v[0].y;
    for( i=1; i<p->count; i++ )
       if( p->v[i].y < ymin )
       {   ymin = p->v[i].y;
           top = i;
       }

    rem = p->count;
    ly = ry = y = ymin;
    li = ri = top;

    offset = (Long)y*View.yskip;
    fbase = View.fbuf+offset;
    dbase = View.dbuf+offset;

    while( rem )
    {   while( ly<=y && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ly;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.x = ((Long)p->v[li].x)<<16;
                lft.z = ((Long)p->v[li].z)<<16;
            }
            ly = p->v[i].y;
            rem--;  li = i;
        }

        while( ry<=y && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ry;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.x = ((Long)p->v[ri].x)<<16;
                rgt.z = ((Long)p->v[ri].z)<<16;
            }
            ry = p->v[i].y;
            rem--; ri = i;
        }


        ymin = MinFun(ly,ry);
        
        while( y<ymin )
        {   if( lft.x < rgt.x )
            {   pmin = &lft;
                pmax = &rgt;
            } else
            {   pmin = &rgt;
                pmax = &lft;
            }

            xmax = (int)(pmax->x>>16)+1;
            xmin = (int)(pmin->x>>16);

            dz = (Long)((pmax->z-pmin->z)/(xmax-xmin));
            z = pmin->z;

            dptr = dbase+xmin;
            for( x=xmin; x<xmax; x++ )
            {   if( (int)(z>>16) > *dptr )
                {   fbase[x] = Lut[p->v[0].inten];
                    *dptr = (int)(z>>16);
                }
                z += dz;
                dptr++;
            }

            lft.x += lft.dx;  rgt.x += rgt.dx;
            lft.z += lft.dz;  rgt.z += rgt.dz;
            dbase += View.yskip;
            fbase += View.yskip;
            y++;
        }
    }
}
#endif


static void ClipPolygon( Poly *p, int Slabbing )
{
    static Edge lft, rgt;
    register Edge *pmin, *pmax;
    register Pixel __huge *fbase;
    register short __huge *dbase;
    register short __huge *dptr;
    register Long offset;

    register Long dz,di;
    register Long z,inten;
    register int ri,li,ry,ly;
    register int xmin,xmax;
    register int dy,ymin;
    register int top,rem;
    register int x,y,i;
    register int rnd,temp,pix;

    if( !Slabbing )
    {    /* Reject Clip Polygon */
        if( UseSlabPlane )
            for( i=0; i<p->count; i++ )
                if( p->v[i].z >= SlabValue )
                    return;
    }

    /*seed random*/
    rnd = temp = 0;

    /* Find top vertex */
    top = 0;  
    ymin = p->v[0].y;
    for( i=1; i<p->count; i++ )
       if( p->v[i].y < ymin )
       {   ymin = p->v[i].y;
           top = i;
       }

    rem = p->count;
    ly = ry = y = ymin;
    li = ri = top;

    if( y<0 )
    {   rem--;

        while( ly<=0 && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > 0 )
            {   dy = p->v[i].y - ly;
                lft.di = (((Long)(p->v[i].inten - p->v[li].inten))<<16)/dy;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.i = (((Long)p->v[li].inten)<<16) - (Long)ly*lft.di;
                lft.x = (((Long)p->v[li].x)<<16) - (Long)ly*lft.dx;
                lft.z = (((Long)p->v[li].z)<<16) - (Long)ly*lft.dz;
            } else rem--;
            ly = p->v[i].y;
            li = i;
        }

        while( ry<=0 && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > 0 )
            {   dy = p->v[i].y - ry;
                rgt.di = (((Long)(p->v[i].inten - p->v[ri].inten))<<16)/dy;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.i = (((Long)p->v[ri].inten)<<16) - (Long)ry*rgt.di;
                rgt.x = (((Long)p->v[ri].x)<<16) - (Long)ry*rgt.dx;
                rgt.z = (((Long)p->v[ri].z)<<16) - (Long)ry*rgt.dz;
            } else rem--;
            ry = p->v[i].y;
            ri = i;
        }

        fbase = View.fbuf;
        dbase = View.dbuf;
        y = 0;
    } else /* y >= 0 */
    {   offset = (Long)y*View.yskip;
        fbase = View.fbuf+offset;
        dbase = View.dbuf+offset;
    }

    while( rem )
    {   while( ly<=y && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ly;
                lft.di = (((Long)(p->v[i].inten - p->v[li].inten))<<16)/dy;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.i = ((Long)p->v[li].inten)<<16;
                lft.x = ((Long)p->v[li].x)<<16;
                lft.z = ((Long)p->v[li].z)<<16;
            }
            ly = p->v[i].y;
            rem--;  li = i;
        }

        while( ry<=y && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ry;
                rgt.di = (((Long)(p->v[i].inten - p->v[ri].inten))<<16)/dy;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.i = ((Long)p->v[ri].inten)<<16;
                rgt.x = ((Long)p->v[ri].x)<<16;
                rgt.z = ((Long)p->v[ri].z)<<16;
            }
            ry = p->v[i].y;
            rem--; ri = i;
        }


        ymin = MinFun(ly,ry);
        if( ymin>View.ymax )
        {   ymin = View.ymax;
            rem = 0;
        }
        
        while( y<ymin )
        {   if( lft.x < rgt.x )
            {   pmin = &lft;
                pmax = &rgt;
            } else
            {   pmin = &rgt;
                pmax = &lft;
            }

            xmax = (int)(pmax->x>>16)+1;
            xmin = (int)(pmin->x>>16);

            if( (xmin<View.xmax) && (xmax>=0) )
            {   di = (Long)((pmax->i-pmin->i)/(xmax-xmin));
                dz = (Long)((pmax->z-pmin->z)/(xmax-xmin));
                if( xmin<0 )
                {   inten = pmin->i - xmin*di;
                    z = pmin->z - xmin*dz;
                    xmin = 0;
                } else /* xmin >= 0 */
                {   inten = pmin->i;  
                    z = pmin->z;
                }

                if( xmax>=View.xmax )
                    xmax = View.xmax;

                dptr = dbase+xmin;
                for( x=xmin; x<xmax; x++ )
                {   if( (int)(z>>16) > *dptr )
                    {   
                        /* Pseudo random generator - Philippe Valadon 2001 */
                        pix = inten;
                        for( i=0;i<4;i++ )
                        {    temp += pix&15;
                            temp = temp<<4;
                            pix = pix>>4;
                        }
                        rnd = (rnd + temp)&0xFFFF;
                        pix = (inten&0xFFFF);

                        if( rnd<pix )
                            fbase[x] = Lut[(int)((inten)>>16)+1];
                        else
                            fbase[x] = Lut[(int)(inten>>16)];

                        *dptr = (int)(z>>16);

                        temp = (rnd&1)<<15;
                        rnd = (rnd>>1)+temp;

                    }
                    inten += di;
                    z += dz;
                    dptr++;
                }
            }

            lft.x += lft.dx;  rgt.x += rgt.dx;
            lft.z += lft.dz;  rgt.z += rgt.dz;
            lft.i += lft.di;  rgt.i += rgt.di;
            dbase += View.yskip;
            fbase += View.yskip;
            y++;
        }
    }
}

  
#ifdef UNUSED
static void ClipFlatPolygon( Poly *p )
{
    static Edge lft, rgt;
    register Edge *pmin, *pmax;
    register Pixel __huge *fbase;
    register short __huge *dbase;
    register short __huge *dptr;
    register Long offset;

    register Long z,dz;
    register int ri,li,ry,ly;
    register int xmin,xmax;
    register int dy,ymin;
    register int top,rem;
    register int x,y,i;

    /* Reject Clip Polygon */
    if( UseSlabPlane )
        for( i=0; i<p->count; i++ )
            if( p->v[i].z >= SlabValue )
                return;

    /* Find top vertex */
    top = 0;  
    ymin = p->v[0].y;
    for( i=1; i<p->count; i++ )
       if( p->v[i].y < ymin )
       {   ymin = p->v[i].y;
           top = i;
       }

    rem = p->count;
    ly = ry = y = ymin;
    li = ri = top;

    if( y<0 )
    {   rem--;

        while( ly<=0 && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > 0 )
            {   dy = p->v[i].y - ly;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.x = ((Long)p->v[li].x)<<16;
                lft.z = ((Long)p->v[li].z)<<16;
            } else rem--;
            ly = p->v[i].y;
            li = i;
        }

        while( ry<=0 && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > 0 )
            {   dy = p->v[i].y - ry;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.x = ((Long)p->v[ri].x)<<16;
                rgt.z = ((Long)p->v[ri].z)<<16;
            } else rem--;
            ry = p->v[i].y;
            ri = i;
        }

        fbase = View.fbuf;
        dbase = View.dbuf;
        y = 0;
    } else /* y >= 0 */
    {   offset = (Long)y*View.yskip;
        fbase = View.fbuf+offset;
        dbase = View.dbuf+offset;
    }

    while( rem )
    {   while( ly<=y && rem )
        {   i = li-1; if( i<0 ) i=p->count-1;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ly;
                lft.dx = (((Long)(p->v[i].x - p->v[li].x))<<16)/dy;
                lft.dz = (((Long)(p->v[i].z - p->v[li].z))<<16)/dy;

                lft.x = ((Long)p->v[li].x)<<16;
                lft.z = ((Long)p->v[li].z)<<16;
            }
            ly = p->v[i].y;
            rem--;  li = i;
        }

        while( ry<=y && rem )
        {   i = ri+1; if( i>=p->count ) i = 0;
            if( p->v[i].y > y )
            {   dy = p->v[i].y - ry;
                rgt.dx = (((Long)(p->v[i].x - p->v[ri].x))<<16)/dy;
                rgt.dz = (((Long)(p->v[i].z - p->v[ri].z))<<16)/dy;

                rgt.x = ((Long)p->v[ri].x)<<16;
                rgt.z = ((Long)p->v[ri].z)<<16;
            }
            ry = p->v[i].y;
            rem--; ri = i;
        }

        ymin = MinFun(ly,ry);
        if( ymin>View.ymax )
        {   ymin = View.ymax;
            rem = 0;
        }
        
        while( y<ymin )
        {   if( lft.x < rgt.x )
            {   pmin = &lft;
                pmax = &rgt;
            } else
            {   pmin = &rgt;
                pmax = &lft;
            }

            xmax = (int)(pmax->x>>16)+1;
            xmin = (int)(pmin->x>>16);

            if( (xmin<View.xmax) && (xmax>=0) )
            {   dz = (Long)((pmax->z-pmin->z)/(xmax-xmin));
                if( xmin<0 )
                {   z = pmin->z - xmin*dz;
                    xmin = 0;
                } else /* xmin >= 0 */
                    z = pmin->z;

                if( xmax>=View.xmax )
                    xmax = View.xmax;

                dptr = dbase+xmin;
                for( x=xmin; x<xmax; x++ )
                {   if( (int)(z>>16) > *dptr )
                    {   fbase[x] = Lut[p->v[0].inten];
                        *dptr = (int)(z>>16);
                    }
                    z += dz;
                    dptr++;
                }
            }

            lft.x += lft.dx;  rgt.x += rgt.dx;
            lft.z += lft.dz;  rgt.z += rgt.dz;
            dbase += View.yskip;
            fbase += View.yskip;
            y++;
        }
    }
}
#endif


void SolidRibbon( Knot __far *src, Knot __far *dst, int col )
{
    static Poly p;
    register int inten;
    register int s;

    inten = src->vinten;
    if( UseDepthCue )
    {   s = (int)(ColourDepth*(ImageRadius-src->pz+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
        inten -= DepthTable[inten][s];
        if( inten<0 ) inten = 0;
    }

    p.v[0].x = Rint(src->px+src->wx);  
    p.v[0].y = Rint(src->py+src->wy);  
    p.v[0].z = Rint(src->pz+src->wz);
    p.v[0].inten = inten+col;

    p.v[3].x = Rint(src->px-src->wx);  
    p.v[3].y = Rint(src->py-src->wy);  
    p.v[3].z = Rint(src->pz-src->wz);
    p.v[3].inten = inten+col;

    inten = dst->vinten;
    if( UseDepthCue )
    {   s = (int)(ColourDepth*(ImageRadius-dst->pz+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
        inten -= DepthTable[inten][s];
        if( inten<0 ) inten = 0;
    }

    p.v[1].x = Rint(dst->px+dst->wx);  
    p.v[1].y = Rint(dst->py+dst->wy);  
    p.v[1].z = Rint(dst->pz+dst->wz);
    p.v[1].inten = inten+col;

    p.v[2].x = Rint(dst->px-dst->wx);
    p.v[2].y = Rint(dst->py-dst->wy);  
    p.v[2].z = Rint(dst->pz-dst->wz);
    p.v[2].inten = inten+col;

    p.count = 4;
    // OutLinePolygon( &p );
    ClipPolygon( &p, 0 );
}


void SolidRibbon2( Knot __far *src, Knot __far *dst, int col1, int col2 )
{
    register int dx,dy;
    register int col,inten1,inten2;
    static Poly p;
    register int s;

    inten1 = src->vinten;
    inten2 = dst->vinten;

    if( UseDepthCue )
    {   s = (int)(ColourDepth*(ImageRadius-src->pz+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        s = s&ColourMask;
        if( s>ColourMask ) s = ColourMask;
        inten1 -= DepthTable[inten1][s];
        if( inten1<0 ) inten1 = 0;

        s = (int)(ColourDepth*(ImageRadius-dst->pz+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
        inten2 -= DepthTable[inten2][s];
        if( inten2<0 ) inten2 = 0;
    }

    p.count = 3;
    p.v[0].x = Rint(src->px+src->wx);  
    p.v[0].y = Rint(src->py+src->wy);  
    p.v[0].z = Rint(src->pz+src->wz);
    p.v[1].x = Rint(dst->px-dst->wx);  
    p.v[1].y = Rint(dst->py-dst->wy);  
    p.v[1].z = Rint(dst->pz-dst->wz);

    dx = p.v[1].x - p.v[0].x;
    dy = p.v[1].y - p.v[0].y;

    p.v[2].x = Rint(dst->px+dst->wx);
    p.v[2].y = Rint(dst->py+dst->wy);  
    p.v[2].z = Rint(dst->pz+dst->wz);

#ifdef INVERT
    col = ( dst->wx*dy > dst->wy*dx )? col2 : col1;
#else
    col = ( dst->wx*dy < dst->wy*dx )? col2 : col1;
#endif

    p.v[0].inten = inten1+col;
    p.v[1].inten = inten2+col;
    p.v[2].inten = inten2+col;

    /* OutLinePolygon( &p ); */
    ClipPolygon( &p, 0 );

    p.v[2].x = Rint(src->px-src->wx);  
    p.v[2].y = Rint(src->py-src->wy);  
    p.v[2].z = Rint(src->pz-src->wz);

#ifdef INVERT
    col = ( src->wx*dy > src->wy*dx )? col2 : col1;
#else
    col = ( src->wx*dy < src->wy*dx )? col2 : col1;
#endif

    p.v[0].inten = inten1+col;
    p.v[1].inten = inten2+col;
    p.v[2].inten = inten1+col;
    /* OutLinePolygon( &p ); */
    ClipPolygon( &p, 0 );
}
 
 
int CalculateTInten( Knot *ptr )
{
    register double inten;
    register double tsize;
 
    tsize = sqrt( ptr->tx*ptr->tx +
                  ptr->ty*ptr->ty +
                  ptr->tz*ptr->tz ) + 1;
 
#ifdef ORIGINAL
    inten = LightDot(ptr->tx,ptr->ty,ptr->tz);
    inten /= tsize*LightLength;
#else
    inten = ptr->tz/ptr->tsize;
#endif
 
    if( ptr->tz < 0 ) inten = -inten;

    if( inten > 0.0 )
    {   return (int)(ColourMask*inten);
    } else return 0;
}


void RectRibbon( Knot __far *src, Knot __far *dst, int col, 
                 int side, double wx, double wy, double wz )
{
    static Poly p;
    register int overlap, inten1, inten2;
    register int s1,s2;

    /* 0x0000    no back no front
       0x0001    front side
       0x0010    back side
       0x0100    back side with previous size
       0x1000   left and right
    */

    /*hidden faces not drawn;
      side normal vectors are not calculated;
      overlap tries to correct defects
    */

    overlap = 10;

    /*slabplane*/
    if( UseSlabPlane )
    {    if( src->pz>=SlabValue && dst->pz>=SlabValue )
        {    return;
        } else if( !(src->pz<SlabValue && dst->pz<SlabValue) )
        {    side = 0x0011;
        }
    }
        
    p.count = 4;

    inten1 = src->vinten;
    inten2 = dst->vinten;
    if( UseDepthCue )
    {   s1 = (int)(ColourDepth*(ImageRadius-src->pz+ZOffset-ShiftS))/ImageSize;
        if( s1<0 ) s1 = 0;
        if( s1>ColourMask ) s1 = ColourMask;
        inten1 -= DepthTable[inten1][s1];
        if( inten1<0 ) inten1 = 0;

        s2 = (int)(ColourDepth*(ImageRadius-dst->pz+ZOffset-ShiftS))/ImageSize;
        if( s2<0 ) s2 = 0;
        if( s2>ColourMask ) s2 = ColourMask;
        inten2 -= DepthTable[inten2][s2];
        if( inten2<0 ) inten2 = 0;
    }
    p.v[0].inten = inten1+col;
    p.v[1].inten = inten2+col;
    p.v[2].inten = inten2+col;
    p.v[3].inten = inten1+col;

    /* Top Surface */
    if( src->vnz>0 || dst->vnz>0  )
    {   
        p.v[0].x = Rint(src->px+src->wx+src->dx);  
        p.v[0].y = Rint(src->py+src->wy+src->dy);  
        p.v[0].z = Rint(src->pz+src->wz+src->dz);

        p.v[1].x = Rint(dst->px+dst->wx+dst->dx);  
        p.v[1].y = Rint(dst->py+dst->wy+dst->dy);  
        p.v[1].z = Rint(dst->pz+dst->wz+dst->dz);

        p.v[2].x = Rint(dst->px-dst->wx+dst->dx);
        p.v[2].y = Rint(dst->py-dst->wy+dst->dy);  
        p.v[2].z = Rint(dst->pz-dst->wz+dst->dz);

        p.v[3].x = Rint(src->px-src->wx+src->dx);  
        p.v[3].y = Rint(src->py-src->wy+src->dy);  
        p.v[3].z = Rint(src->pz-src->wz+src->dz);
        ClipPolygon( &p, 1 );
    } 

    /* Bottom Surface */
    if( src->vnz<0 || dst->vnz<0  )
    {   p.v[0].x = Rint(src->px+src->wx-src->dx);  
        p.v[0].y = Rint(src->py+src->wy-src->dy);  
        p.v[0].z = Rint(src->pz+src->wz-src->dz);

        p.v[1].x = Rint(dst->px+dst->wx-dst->dx);  
        p.v[1].y = Rint(dst->py+dst->wy-dst->dy);  
        p.v[1].z = Rint(dst->pz+dst->wz-dst->dz);

        p.v[2].x = Rint(dst->px-dst->wx-dst->dx);
        p.v[2].y = Rint(dst->py-dst->wy-dst->dy);  
        p.v[2].z = Rint(dst->pz-dst->wz-dst->dz);

        p.v[3].x = Rint(src->px-src->wx-src->dx);  
        p.v[3].y = Rint(src->py-src->wy-src->dy);  
        p.v[3].z = Rint(src->pz-src->wz-src->dz);
        ClipPolygon( &p, 1 );
    }

    inten1 = src->hinten;
    inten2 = dst->hinten;
    if( UseDepthCue )
    {   inten1 -= DepthTable[inten1][s1];
        if( inten1<0 ) inten1 = 0;
        inten2 -= DepthTable[inten2][s2];
        if( inten2<0 ) inten2 = 0;
    }
    p.v[0].inten = inten1+col;
    p.v[1].inten = inten2+col;
    p.v[2].inten = inten2+col;
    p.v[3].inten = inten1+col;

    /* Left Surface */
    if( src->wz>-overlap || dst->wz>-overlap || (side&0x1000))
    {   p.v[0].x = Rint(src->px+src->wx+src->dx);  
        p.v[0].y = Rint(src->py+src->wy+src->dy);  
        p.v[0].z = Rint(src->pz+src->wz+src->dz);

        p.v[1].x = Rint(dst->px+dst->wx+dst->dx);  
        p.v[1].y = Rint(dst->py+dst->wy+dst->dy);  
        p.v[1].z = Rint(dst->pz+dst->wz+dst->dz);

        p.v[2].x = Rint(dst->px+dst->wx-dst->dx);
        p.v[2].y = Rint(dst->py+dst->wy-dst->dy);  
        p.v[2].z = Rint(dst->pz+dst->wz-dst->dz);

        p.v[3].x = Rint(src->px+src->wx-src->dx);  
        p.v[3].y = Rint(src->py+src->wy-src->dy);  
        p.v[3].z = Rint(src->pz+src->wz-src->dz);
        ClipPolygon( &p, 1 );
    }

    /* Right Surface */
    if( src->wz<overlap || dst->wz<overlap || (side&0x1000) )
    {   p.v[0].x = Rint(src->px-src->wx+src->dx);  
        p.v[0].y = Rint(src->py-src->wy+src->dy);  
        p.v[0].z = Rint(src->pz-src->wz+src->dz);

        p.v[1].x = Rint(dst->px-dst->wx+dst->dx);  
        p.v[1].y = Rint(dst->py-dst->wy+dst->dy);  
        p.v[1].z = Rint(dst->pz-dst->wz+dst->dz);

        p.v[2].x = Rint(dst->px-dst->wx-dst->dx);
        p.v[2].y = Rint(dst->py-dst->wy-dst->dy);  
        p.v[2].z = Rint(dst->pz-dst->wz-dst->dz);
    
        p.v[3].x = Rint(src->px-src->wx-src->dx);  
        p.v[3].y = Rint(src->py-src->wy-src->dy);  
        p.v[3].z = Rint(src->pz-src->wz-src->dz);
        ClipPolygon( &p, 1 );
    }

    
    if( side==0 )
        return;

    /*front*/
    if( side&0x0001 )
    {   inten1 = CalculateTInten(dst);
        if( UseDepthCue )
        {   inten1 -= DepthTable[inten1][s2];
            if( inten1<0 ) inten1 = 0;
        }
        p.v[0].inten = inten1+col;
        p.v[1].inten = inten1+col;
        p.v[2].inten = inten1+col;
        p.v[3].inten = inten1+col;
    
        p.v[0].x = Rint(dst->px+dst->wx+dst->dx);  
        p.v[0].y = Rint(dst->py+dst->wy+dst->dy);  
        p.v[0].z = Rint(dst->pz+dst->wz+dst->dz);

        p.v[1].x = Rint(dst->px-dst->wx+dst->dx);  
        p.v[1].y = Rint(dst->py-dst->wy+dst->dy);  
        p.v[1].z = Rint(dst->pz-dst->wz+dst->dz);

        p.v[2].x = Rint(dst->px-dst->wx-dst->dx);  
        p.v[2].y = Rint(dst->py-dst->wy-dst->dy);  
        p.v[2].z = Rint(dst->pz-dst->wz-dst->dz);

        p.v[3].x = Rint(dst->px+dst->wx-dst->dx);  
        p.v[3].y = Rint(dst->py+dst->wy-dst->dy);  
        p.v[3].z = Rint(dst->pz+dst->wz-dst->dz);
        ClipPolygon( &p, 1 );

    } 
    
    /*back*/
    if( side&0x0010 )
    {   inten1 = CalculateTInten(src);
        if( UseDepthCue )
        {   inten1 -= DepthTable[inten1][s1];
            if( inten1<0 ) inten1 = 0;
        }
        p.v[0].inten = inten1+col;
        p.v[1].inten = inten1+col;
        p.v[2].inten = inten1+col;
        p.v[3].inten = inten1+col;
    
        p.v[0].x = Rint(src->px+src->wx+src->dx);  
        p.v[0].y = Rint(src->py+src->wy+src->dy);  
        p.v[0].z = Rint(src->pz+src->wz+src->dz);

        p.v[1].x = Rint(src->px+src->wx-src->dx);  
        p.v[1].y = Rint(src->py+src->wy-src->dy);  
        p.v[1].z = Rint(src->pz+src->wz-src->dz);

        p.v[2].x = Rint(src->px-src->wx-src->dx);  
        p.v[2].y = Rint(src->py-src->wy-src->dy);  
        p.v[2].z = Rint(src->pz-src->wz-src->dz);

        p.v[3].x = Rint(src->px-src->wx+src->dx);  
        p.v[3].y = Rint(src->py-src->wy+src->dy);  
        p.v[3].z = Rint(src->pz-src->wz+src->dz);
        ClipPolygon( &p, 1 );

    } else if( side&0x0100 )
    {    /*back with previous size*/
        inten1 = CalculateTInten(src);
        if( UseDepthCue )
        {   inten1 -= DepthTable[inten1][s1];
            if( inten1<0 ) inten1 = 0;
        }
        p.v[0].inten = inten1+col;
        p.v[1].inten = inten1+col;
        p.v[2].inten = inten1+col;
        p.v[3].inten = inten1+col;
    
        p.v[0].x = Rint(src->px+wx+src->dx);  
        p.v[0].y = Rint(src->py+wy+src->dy);  
        p.v[0].z = Rint(src->pz+wz+src->dz);

        p.v[1].x = Rint(src->px+wx-src->dx);  
        p.v[1].y = Rint(src->py+wy-src->dy);  
        p.v[1].z = Rint(src->pz+wz-src->dz);

        p.v[2].x = Rint(src->px-wx-src->dx);  
        p.v[2].y = Rint(src->py-wy-src->dy);  
        p.v[2].z = Rint(src->pz-wz-src->dz);

        p.v[3].x = Rint(src->px-wx+src->dx);  
        p.v[3].y = Rint(src->py-wy+src->dy);  
        p.v[3].z = Rint(src->pz-wz+src->dz);
        ClipPolygon( &p, 1 );
    }

}


static int TestSphere( int x, int y, int z, int rad )
{
    register int temp;

    ClipStatus = 0;

    if( UseSlabPlane )
    {   if( z-rad>=SlabValue )
            return( False );

        if( z+rad>=SlabValue )
        {   if( SlabMode )
            {   ClipStatus |= BitFront;
            } else return( False );
        } else if( SlabMode==SlabSection )
            return( False );

        if( UseDepthPlane )
        {    if( WClip[1]>WClip[0] )
                return( False );
        }
    }

    temp = x+rad;
    if( temp<0 ) return( False );
    if( temp>=View.xmax ) ClipStatus |= BitRight;

    temp = x-rad;
    if( temp>=View.xmax ) return( False );
    if( temp<0 ) ClipStatus |= BitLeft;

    temp = y+rad;
    if( temp<0 ) return( False );
    if( temp>=View.ymax ) ClipStatus |= BitBelow;

    temp = y-rad;
    if( temp>=View.ymax ) return( False );
    if( temp<0 ) ClipStatus |= BitAbove;

    return True;
}



/*===========================*/
/*  Sphere Rendering Macros  */
/*===========================*/

// inten = (int)((double)ColourDepth*LightDot(dx,dy,dz)/(LightLength*(double)(rad  + 0.5 + (double)rad/70) + 4));

#define UpdateAcross(dz)    \
        depth = (dz)+z;                    \
        if( depth > *dptr )                \
        {   *dptr = depth;                 \
            fptr = fold+dx;                \
            inten = (int)LightDot(dx,dy,dz);    \
            if( inten>0 )                  \
            {      inten = (int)((inten*ColConst[rad])>>ColBits); \
				   if( inten>ColourMask ) inten = ColourMask; \
                   if( UseDepthCue )       \
                   {   inten -= DepthTable[inten][s];\
                       if( inten<0 ) inten = 0;      \
                   }                       \
                   *fptr = Lut[col+inten]; \
            } else *fptr = Lut[col];       \
        }                                  \
        dptr++;  dx++;

#define UpdateLine  \
        dx = -wide;                   \
        dptr = dold-wide;             \
        tptr = LookUp[wide]+wide;     \
        while( dx<0 ) { UpdateAcross(*tptr); tptr--; }       \
        do { UpdateAcross(*tptr); tptr++; } while(dx<=wide); \
        dold += View.yskip;  fold += View.yskip;             \
        dy++;


void DrawSphere( int x, int y, int z, int rad, int col )
{
    register Pixel __huge *fptr, __huge *fold;
    register short __huge *dptr, __huge *dold;
    register Byte __far *tptr;

    register Long offset;
    register int depth,wide,inten,s;
    register int dx,dy;

    /* Avoid Lookup Table Overflow! */
    if( rad > MAXRAD ) rad = MAXRAD;

    if( UseDepthCue )
    {   s = (ColourDepth*(ImageRadius-z+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
    } else
        s = 0;

    offset = (Long)(y-rad)*View.yskip + x;
    fold=View.fbuf+offset;  
    dold=View.dbuf+offset;

    dy = -rad;
    while( dy<0 ) 
    {   wide = LookUp[rad][-dy]; 
        UpdateLine;
    }

    do { 
        wide = LookUp[rad][dy];  
        UpdateLine; 
    } while( dy<=rad );
}


void ClipSphere( int x, int y, int z, int rad, int col )
{
    register Pixel __huge *fptr, __huge *fold;
    register short __huge *dptr, __huge *dold;

    register int lastx,lasty,dx,dy,dz;
    register int depth,wide,inten,side,s;
    register int crad,cwide,temp;
    register Long offset;

    /* Avoid Lookup Table Overflow! */
    if( rad > MAXRAD ) rad = MAXRAD;

    /* Visibility Tests */
    if( !TestSphere(x,y,z,rad) )
        return;

    if( !ClipStatus )
    {   DrawSphere(x,y,z,rad,col);
        return;
    }

    if( UseDepthCue )
    {   s = (ColourDepth*(ImageRadius-z+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
    } else
        s = 0;

    if( ClipStatus&BitAbove )
    {   dy = -y;
        fold = View.fbuf + x;
        dold = View.dbuf + x;
    } else
    {   dy = -rad;
        offset = (Long)(y+dy)*View.yskip+x;
        fold = View.fbuf + offset;
        dold = View.dbuf + offset;
    }

    if( ClipStatus&BitBelow )
    {   lasty = (View.ymax-1)-y;
    } else lasty = rad;

    side = (View.xmax-1)-x;
    /* No Slab Plane Clipping */
    if( !(ClipStatus&BitFront) )
    {   while( dy<=lasty )
        {   wide = LookUp[rad][AbsFun(dy)];
            lastx = MinFun(wide,side);
            dx = - MinFun(wide,x);
            dptr = dold + dx;

            while( dx<=lastx )
            {   dz = LookUp[wide][AbsFun(dx)];
                UpdateAcross(dz);
            }
            dold += View.yskip;
            fold += View.yskip;
            dy++;
        }
        return;
    }

    dz = SlabValue-z;
    crad = LookUp[rad][AbsFun(dz)];
 
    if( (z>SlabValue) || (SlabMode==SlabSection) )
    {   if( crad<lasty ) lasty = crad;
        if( -crad>dy ) 
        {   dy = -crad;
            offset = (Long)(y+dy)*View.yskip+x;
            fold = View.fbuf + offset;
            dold = View.dbuf + offset;
        }
    }

    while( dy<=lasty )
    {   temp = AbsFun(dy);
        wide = LookUp[rad][temp];
        lastx = MinFun(wide,side);
        dx = - MinFun(x,wide);
        dptr = dold + dx;

        if( temp<=crad )
        {   cwide = LookUp[crad][temp];
            while( dx<=lastx )
            {   temp = AbsFun(dx);
                if( temp<=cwide )
                {    /* Slab Plane Clipping Modes */
                    switch( SlabMode )
                    {   case( SlabFinal ):
                                fold[dx] = Lut[col+SlabInten];
                                *dptr = SliceValue;
                                break;

                        case( SlabHollow ):
                                dz = LookUp[wide][temp];
                                depth = z - dz;
                                if( depth>*dptr )
                                {   *dptr = depth;
                                    inten = (int)LightDot(-dx,-dy,dz);
                                    if( inten>0 )
                                    {   inten=(int)( (inten*ColConst[rad])
                                                     >>(ColBits+1));
                                        fold[dx] = Lut[col+inten];
                                    } else fold[dx] = Lut[col];
                                }
                                break;

                        case( SlabSection ):
                        case( SlabClose ):
                                dz = SlabValue-z;
                                depth = dx*dx+dy*dy+dz*dz+SliceValue;
                                if( (*dptr<SliceValue) || (depth<*dptr) )
                                {   fold[dx] = Lut[col+SlabInten];
                                    *dptr = depth;
                                }
                                break;
                    }
                    dptr++;  dx++;
                } else if( (z<SlabValue) && (SlabMode!=SlabSection) )
                {    dz = LookUp[wide][temp];
                     UpdateAcross(dz);
                } else
                {   dptr++;  dx++;
                }
            }
        } else /* Slabless ScanLine */
            while( dx<=lastx )
            {   dz = LookUp[wide][AbsFun(dx)];
                UpdateAcross(dz);
            }

        dold += View.yskip;
        fold += View.yskip;
        dy++;
    }
}


void DrawStar( int x, int y, int z, int rad, int col )
{
    DrawTwinVector((int)(x-rad*MatX[0]),(int)(y-rad*MatY[0]),(int)(z-rad*MatZ[0]),
    (int)(x+rad*MatX[0]),(int)(y+rad*MatY[0]),(int)(z+rad*MatZ[0]), col,col,' ');
    DrawTwinVector((int)(x-rad*MatX[1]),(int)(y-rad*MatY[1]),(int)(z-rad*MatZ[1]),
    (int)(x+rad*MatX[1]),(int)(y+rad*MatY[1]),(int)(z+rad*MatZ[1]), col,col,' ');
    DrawTwinVector((int)(x-rad*MatX[2]),(int)(y-rad*MatY[2]),(int)(z-rad*MatZ[2]),
    (int)(x+rad*MatX[2]),(int)(y+rad*MatY[2]),(int)(z+rad*MatZ[2]), col,col,' ');
}


void ClipStar( int x, int y, int z, int rad, int col )
{
    ClipTwinVector((int)(x-rad*MatX[0]),(int)(y-rad*MatY[0]),(int)(z-rad*MatZ[0]),
      (int)(x+rad*MatX[0]),(int)(y+rad*MatY[0]),(int)(z+rad*MatZ[0]),col,col,' ');
    ClipTwinVector((int)(x-rad*MatX[1]),(int)(y-rad*MatY[1]),(int)(z-rad*MatZ[1]),
      (int)(x+rad*MatX[1]),(int)(y+rad*MatY[1]),(int)(z+rad*MatZ[1]),col,col,' ');
    ClipTwinVector((int)(x-rad*MatX[2]),(int)(y-rad*MatY[2]),(int)(z-rad*MatZ[2]),
      (int)(x+rad*MatX[2]),(int)(y+rad*MatY[2]),(int)(z+rad*MatZ[2]),col,col,' ');
}


/*=============================*/
/*  Cylinder Rendering Macros  */
/*=============================*/

/* Philippe Valadon 06/09/2001 */

static double CapInten = 0.0;

#define RoundEdges                                   \
            if( rx>=0.5 )                            \
                rx2 = (int)((rx+0.5));               \
            else                                     \
                rx2 = (int)((rx-0.5));               \
            if( ry>=0.5 )                            \
                ry2 = (int)((ry+0.5));               \
            else                                     \
                ry2 = (int)((ry-0.5));               \
            if( rz>=0.5 )                            \
                rz2 = (int)((rz+0.5));               \
            else                                     \
                rz2 = (int)((rz-0.5));               \


#define    LineInten                                       \
        inten = LightDot(rx,ry,rz);                        \
        inten /= lightfactor;                              \
        if( inten > 0.0 )                                  \
        {   inten = (char)(ColourMask*inten + 0.1);        \
            if( inten>ColourMask ) inten = ColourMask;     \
            if( UseDepthCue )                              \
            {   inten -= DepthTable[(char)inten][s];       \
                if( inten<0 ) inten = 0;                   \
            }                                              \
        } else inten = 0;                                  \


#define OutCodeLine(res,x,y)              \
        {   if( (y)<0 )                   \
            {   (res) = BitAbove;         \
            } else if( (y) >= View.ymax ) \
            {   (res) = BitBelow;         \
            } else (res) = 0;             \
                                          \
            if( (x) < 0 )                 \
            {   (res) |= BitLeft;         \
            } else if( (x) >= View.xmax ) \
                (res) |= BitRight;        \
        }


/* 3D Bresenhams Vector Algorithm - Macros for cylinders */

#define SETPIXELE(dptr,fptr,d,c)    if( (d) >= *(dptr) )            \
                                   {   *(dptr) = (d);               \
                                       *(fptr) = (c);               \
                                   }

#define SETPIXELPE(dptr,fptr,d,c,ca,p) if( (d) >= *(dptr))          \
                                   {   *(dptr) = (d);               \
                                       if(!p) {*(fptr) = (c); }     \
                                       else  {*(fptr) = (ca);}      \
                                   }

#define CommonZStep(s)  if( (zerr-=dz)>0 ) { \
                        while ( zerr > 0 ) { zerr -= (s); az -= 1; } } \

#define CommonLineStep(s)  SETPIXELPE(dptr,fptr,az,Lut[col+(char)inten],Lut[cola],p);       \
                           SETPIXELPE(dptr-iy,fptr-iy,az,Lut[col+(char)inten],Lut[cola],p); \
                           CommonZStep(s); \


#define XLineStep  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                     fptr+=ix; dptr+=ix; ax+=ix;                            \
                     p =  altc && (ax-mid<(dx/4)) && (mid-ax<(dx/4));      \
                     CommonLineStep(dx); }


#define YLineStep  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                     fptr+=ystep; dptr+=ystep; ay+=iy;                \
                     p =  altc && (ay-mid<(dy/4)) && (mid-ay<(dy/4));\
                     CommonLineStep(dy); }


#define VALIDPIXEL(dptr) ( ((int)dptr>=dmin) && ((int)dptr<dmax) )

#define XLineStep3  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                     fptr+=ix; dptr+=ix; ax+=ix;                            \
                     if( VALIDPIXEL(dptr) ) \
                     { SETPIXELE(dptr,fptr,az,Lut[col+(char)inten]); \
                       SETPIXELE(dptr-ix,fptr-ix,az,Lut[col+(char)inten]);}\
                     CommonZStep(dx); }


#define YLineStep3  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                     fptr+=ystep; dptr+=ystep; ay+=iy;                \
                     if( VALIDPIXEL(dptr) ) \
                     { SETPIXELE(dptr,fptr,az,Lut[col+(char)inten]); \
                       SETPIXELE(dptr-iy,fptr-iy,az,Lut[col+(char)inten]);}\
                     CommonZStep(dy); }

#define XLineStep2  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                     fptr+=ix; dptr+=ix; ax+=ix;                            \
                     if( VALIDPIXEL(dptr) ) \
                     { p =  altc && (ax-mid<(dx/4)) && (mid-ax<(dx/4));      \
                     SETPIXELPE(dptr,fptr,az,Lut[col+(char)inten],Lut[cola],p); \
                     SETPIXELPE(dptr-ix,fptr-ix,az,Lut[col+(char)inten],Lut[cola],p);}\
                     CommonZStep(dx); }


#define YLineStep2  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                     fptr+=ystep; dptr+=ystep; ay+=iy;                \
                     if( VALIDPIXEL(dptr) ) \
                     { p =  altc && (ay-mid<(dy/4)) && (mid-ay<(dy/4));\
                     SETPIXELPE(dptr,fptr,az,Lut[col+(char)inten],Lut[cola],p); \
                     SETPIXELPE(dptr-iy,fptr-iy,az,Lut[col+(char)inten],Lut[cola],p);}\
                     CommonZStep(dy); }

#define XClipStep  { if((err+=dy)>0) { fptr+=ystep; dptr+=ystep; err-=dx; } \
                     fptr+=ix; dptr+=ix; ax+=ix;                            \
                     CommonZStep(dx); }

#define YClipStep  { if((err+=dx)>0) { fptr+=ix; dptr+=ix; err-=dy; } \
                     fptr+=ystep; dptr+=ystep; ay+=iy;                \
                     CommonZStep(dy); }


static void DrawCylinderCaps( int x1, int y1, int z1,
                              int x2, int y2, int z2,
                              int c1, int c2, int rad, char altl )
{
    register short __huge *dold, __huge *dptr;
    register Pixel __huge *fold;
    register Long offset,temp,end;
    register int inten,inten1,inten2,s1,s2,absx;
    register int wide,depth;
    register int dx,dy,dz;
    register int lx,ly;
    register int p, alts, altc;

    altc=0;
    if (altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];

    lx = x2-x1;
    ly = y2-y1;

    if( UseDepthCue )
    {   s1 = (ColourDepth*(ImageRadius-z1+ZOffset-ShiftS))/ImageSize;
        if( s1<0 ) s1 = 0;
        if( s1>ColourMask ) s1 = ColourMask;
        s2 = (ColourDepth*(ImageRadius-z2+ZOffset-ShiftS))/ImageSize;
        if( s2<0 ) s2 = 0;
        if( s2>ColourMask ) s2 = ColourMask;
    } else
        s1 = s2 = 0;

    end = (Long)ly*View.yskip+lx;
    temp = (Long)y1*View.yskip+x1;
    fold = View.fbuf+temp;
    dold = View.dbuf+temp;

    temp = (Long)-(rad*View.yskip);
    for( dy= -rad; dy<=rad; dy++ )
    {   wide = LookUp[rad][AbsFun(dy)];
        alts = 0;

        for( dx= -wide; dx<=wide; dx++ )
        {   absx = AbsFun(dx);
            dz = LookUp[wide][absx];
            inten = (int)LightDot(dx,dy,dz);
            if( inten>0 )
            {   inten = (int)((inten*ColConst[rad])>>ColBits);
				if( inten>ColourMask ) inten = ColourMask; 
            } else inten = 0;

            if( UseDepthCue )
            {   inten1 = inten - DepthTable[inten][s1];
                if( inten1<0 ) inten1 = 0;
                inten2 = inten - DepthTable[inten][s2];
                if( inten2<0 ) inten2 = 0;
            } else
                inten2 = inten1 = inten;   

            offset = temp+dx;

            dptr = dold+offset; depth = z1+dz;
            p = altc&&(4*dx*dx*rad*rad + 4*dy*dy*wide*wide < rad*rad*wide*wide );
            SETPIXELP(dptr,fold+offset,depth,Lut[c1+inten1], \
            Lut[altc+inten1],p);

            dptr = dold+(offset+end); depth = z2+dz;
            p = altc&&(4*dx*dx*rad*rad + 4*dy*dy*wide*wide < rad*rad*wide*wide );
            SETPIXELP(dptr,fold+(offset+end),depth,Lut[c2+inten2], \
            Lut[altc+inten2],p);
        }
        temp += View.yskip;
    }
}


static void ClipCylinderCaps( int x1, int y1, int z1,
                              int x2, int y2, int z2,
                              int c1, int c2, int rad, char altl, short cap)
{
    register short __huge *dold, __huge *dptr;
    register Pixel __huge *fold;
    register Long offset,temp,end;
    register int inten,inten1,inten2,s1,s2,absx;
    register int wide,depth;
    register int dx,dy,dz;
    register int lx,ly;
    register int p, alts, altc;

    altc=0;
    if (altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];

    lx = x2-x1;
    ly = y2-y1;

    if( UseDepthCue )
    {   s1 = (ColourDepth*(ImageRadius-z1+ZOffset-ShiftS))/ImageSize;
        if( s1<0 ) s1 = 0;
        if( s1>ColourMask ) s1 = ColourMask;
        s2 = (ColourDepth*(ImageRadius-z2+ZOffset-ShiftS))/ImageSize;
        if( s2<0 ) s2 = 0;
        if( s2>ColourMask ) s2 = ColourMask;
    } else
        s1 = s2 = 0;

    end = (Long)ly*View.yskip+lx;
    temp = (Long)y1*View.yskip+x1;
    fold = View.fbuf+temp;
    dold = View.dbuf+temp;

    temp = (Long)-(rad*View.yskip);
    for( dy= -rad; dy<=rad; dy++ )
    {   wide = LookUp[rad][AbsFun(dy)];
        alts = 0;

        for( dx= -wide; dx<=wide; dx++ )
        {   absx = AbsFun(dx);
            dz = LookUp[wide][absx];
            inten = (int)LightDot(dx,dy,dz);
            if( inten>0 )
            {   inten = (int)((inten*ColConst[rad])>>ColBits);
				if( inten>ColourMask ) inten = ColourMask; 
            } else inten = 0;

            if( UseDepthCue )
            {   inten1 = inten - DepthTable[inten][s1];
                if( inten1<0 ) inten1 = 0;
                inten2 = inten - DepthTable[inten][s2];
                if( inten2<0 ) inten2 = 0;
            } else
                inten2 = inten1 = inten;
                 
            offset = temp+dx;

            if( XValid(x1+dx) && YValid(y1+dy) && (cap!=1) )
            {   dptr = dold+offset; depth = z1+dz;
                p = altc&&
                  (4*dx*dx*rad*rad + 4*dy*dy*wide*wide < rad*rad*wide*wide );
                SETPIXELP(dptr,fold+offset,depth,Lut[c1+inten1], \
                   Lut[altc+inten1],p);
            }

            if( XValid(x2+dx) && YValid(y2+dy) && (cap!=2) )
            {   dptr = dold+(offset+end); depth = z2+dz;
                p = altc&&
                  (4*dx*dx*rad*rad + 4*dy*dy*wide*wide < rad*rad*wide*wide );
                SETPIXELP(dptr,fold+(offset+end),depth,Lut[c2+inten2], \
                  Lut[altc+inten2],p);
            }
        }
        temp += View.yskip;
    }
}


static void ClipCapLine( int ax, int ay, int az,
                         int bx, int by, int bz,
                         int c, char inten)
{
    register int tmp;
    register int lx,ly,lz,dx,dy,dz;
    register int ix,iy;

    register int lax,lay,laz,lbx,lby,lbz;

    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int zerr,ystep,err;
    register int col;

    register int code1,code2;
    register int delta,rest;
    register Long dmin, dmax;

    if( az<bz )
    {   tmp=ax; ax=bx; bx=tmp;
        tmp=ay; ay=by; by=tmp;
        tmp=az; az=bz; bz=tmp;
    }

    lx = bx-ax;
    ly = by-ay;
    lz = bz-az;

    ix = (lx<0)? -1:1;
    iy = (ly<0)? -1:1;

    dx = ix*lx; 
    dy = iy*ly; 
    ystep = iy*View.yskip;
    dz = lz;
    
    /* PV: Check implementation on other platforms */
    dmin = (int) View.dbuf;
    dmax = dmin + 2*XRange*YRange;

    lax = ax;
    lay = ay;
    laz = az;

    lbx = bx;
    lby = by;
    lbz = bz;

    OutCodeLine(code1,ax,ay);
    OutCodeLine(code2,bx,by);

    if( Reject(code1,code2) )
        return;
  
    while( !Accept(code1,code2) )
    {  if( !code1 )
        {   tmp=lax; lax=lbx; lbx=tmp;
            tmp=lay; lay=lby; lby=tmp;
            tmp=laz; laz=lbz; lbz=tmp;
            code1 = code2;
            code2 = 0;
        }

        if( code1 & BitAbove )
        {   delta = lby-lay;
            lax += (int)(((Long)lay*(lax-lbx))/delta);  
            laz += (int)(((Long)lay*(laz-lbz))/delta);
            lay = 0;
        } else if( code1 & BitLeft )
        {   delta = lbx-lax;
            lay += (int)(((Long)lax*(lay-lby))/delta);
            laz += (int)(((Long)lax*(laz-lbz))/delta);
            lax = 0;
        } else if( code1 & BitRight )
        {   delta = lbx-lax;
            tmp=View.xmax-1; rest=tmp-lax;
            lay += (int)(((Long)rest*(lby-lay))/delta);
            laz += (int)(((Long)rest*(lbz-laz))/delta);
            lax = tmp;
        } else if( code1 & BitBelow )
        {   delta = lby-lay;
            tmp=View.ymax-1; rest=tmp-lay;
            lax += (int)(((Long)rest*(lbx-lax))/delta);
            laz += (int)(((Long)rest*(lbz-laz))/delta);
            lay = tmp;
        }

        OutCodeLine(code1,lax,lay);
        if( Reject(code1,code2) )
            break;
    }
    if( Reject(code1,code2) )
        return;

    col = c;        
    offset = (Long)ay*View.yskip + ax;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;

    if( dx>dy )
    {   if( lbx<lax )
        {   int tmp;
            tmp = lax;
            lax = lbx;
            lbx = tmp;
        }
        err = zerr = -(dx>>1);
        if( ix>0 )
        {   while( ax<lax ) XClipStep;
            while( ax<lbx ) XLineStep3;
        } else
        {   while( ax>lbx ) XClipStep;
            while( ax>lax ) XLineStep3;
        }
    } else
    {   if( lby<lay )
        {   int tmp;
            tmp = lay;
            lay = lby;
            lby = tmp;
        }

        err = zerr = -(dy>>1);
        if( iy>0 )
        {   while( ay<lay ) YClipStep;
            while( ay<lby ) YLineStep3;
        } else
        {   while( ay>lby ) YClipStep;
            while( ay>lay ) YLineStep3;
        }
    }
}


static void ClipCylinderLine( int ax, int ay, int az,
                               int bx, int by, int bz,
                               int altc, int cola,
                               int c1, int c2, char inten)
{
    register int tmp;
    register int lx,ly,lz,dx,dy,dz;
    register int ix,iy;

    register int lax,lay,laz,lbx,lby,lbz;

    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int zerr,ystep,err;
    register int col,mid,p;

    register int code1,code2;
    register int delta,rest;
    register Long dmin, dmax;

    if( az<bz )
    {   tmp=ax; ax=bx; bx=tmp;
        tmp=ay; ay=by; by=tmp;
        tmp=az; az=bz; bz=tmp;
    }

    lx = bx-ax;
    ly = by-ay;
    lz = bz-az;

    ix = (lx<0)? -1:1;
    iy = (ly<0)? -1:1;

    dx = ix*lx; 
    dy = iy*ly; 
    ystep = iy*View.yskip;
    dz = lz;
    
    /* PV: Check implementation on other platforms */
    dmin = (int) View.dbuf;
    dmax = dmin + 2*XRange*YRange;

    lax = ax;
    lay = ay;
    laz = az;

    lbx = bx;
    lby = by;
    lbz = bz;

    OutCodeLine(code1,ax,ay);
    OutCodeLine(code2,bx,by);

    if( Reject(code1,code2) )
        return;
  
    while( !Accept(code1,code2) )
    {  if( !code1 )
        {   tmp=lax; lax=lbx; lbx=tmp;
            tmp=lay; lay=lby; lby=tmp;
            tmp=laz; laz=lbz; lbz=tmp;
            code1 = code2;
            code2 = 0;
        }

        if( code1 & BitAbove )
        {   delta = lby-lay;
            lax += (int)(((Long)lay*(lax-lbx))/delta);  
            laz += (int)(((Long)lay*(laz-lbz))/delta);
            lay = 0;
        } else if( code1 & BitLeft )
        {   delta = lbx-lax;
            lay += (int)(((Long)lax*(lay-lby))/delta);
            laz += (int)(((Long)lax*(laz-lbz))/delta);
            lax = 0;
        } else if( code1 & BitRight )
        {   delta = lbx-lax;
            tmp=View.xmax-1; rest=tmp-lax;
            lay += (int)(((Long)rest*(lby-lay))/delta);
            laz += (int)(((Long)rest*(lbz-laz))/delta);
            lax = tmp;
        } else if( code1 & BitBelow )
        {   delta = lby-lay;
            tmp=View.ymax-1; rest=tmp-lay;
            lax += (int)(((Long)rest*(lbx-lax))/delta);
            laz += (int)(((Long)rest*(lbz-laz))/delta);
            lay = tmp;
        }

        OutCodeLine(code1,lax,lay);
        if( Reject(code1,code2) )
            break;
    }
    if( Reject(code1,code2) )
        return;

    col = c1;        
    offset = (Long)ay*View.yskip + ax;
    fptr = View.fbuf+offset;
    dptr = View.dbuf+offset;

    if( XValid(ax) )
    {   if( YValid(ay) )
        {   SETPIXELE(dptr,fptr,az,Lut[c1+(char)CapInten]);
            if( dx>dy )
            {    SETPIXELE(dptr-ix,fptr-ix,az,Lut[c1+(char)CapInten]);
            } else
            {    SETPIXELE(dptr-iy,fptr-iy,az,Lut[c1+(char)CapInten]);
            }
        }
    }

    if( dx>dy )
    {   mid = (ax+bx)>>1;
        if( lbx<lax )
        {   int tmp;
            tmp = lax;
            lax = lbx;
            lbx = tmp;
        }
        err = zerr = -(dx>>1);
        if( c1 != c2 )
        {   if( ix>0 )
            {   while( ax<lax ) XClipStep;
                if( ax>=mid )       col = c2;
                while( (ax<lbx )&&(ax!=mid) ) XLineStep2;
                if( ax==mid )       col = c2;
                while( ax<lbx ) XLineStep2;
            } else
            {   while( ax>lbx ) XClipStep;
                if( ax<=mid )       col = c2;
                while( (ax>lax )&&(ax!=mid) ) XLineStep2;
                if( ax==mid )       col = c2;
                while( ax>lax ) XLineStep2;
            }
        } else
        {    if( ix>0 )
            {   while( ax<lax ) XClipStep;
                while( ax<lbx ) XLineStep2;
            } else
            {   while( ax>lbx ) XClipStep;
                while( ax>lax ) XLineStep2;
            }
        }
    } else
    {   mid = (ay+by)>>1;
        if( lby<lay )
        {   int tmp;
            tmp = lay;
            lay = lby;
            lby = tmp;
        }

        err = zerr = -(dy>>1);
        if( c1 != c2 )
        {   if( iy>0 )
            {   while( ay<lay ) YClipStep;
                if( ay>=mid )       col = c2;
                while( (ay<lby)&&(ay!=mid) ) YLineStep2;
                if( ay==mid )       col = c2;
                while( ay<lby ) YLineStep2;
            } else
            {   while( ay>lby ) YClipStep;
                if( ay<=mid )       col = c2;
                while( (ay>lay)&&(ay!=mid) ) YLineStep2;
                if( ay==mid )       col = c2;
                while( ay>lay ) YLineStep2;
            }
        } else
        {    if( iy>0 )
            {   while( ay<lay ) YClipStep;
                while( ay<lby ) YLineStep2;
            } else
            {   while( ay>lby ) YClipStep;
                while( ay>lay ) YLineStep2;
            }
        }
    }
}

                                                            
static void DrawCylinderOnAxe( int x1, int y1, int z1,
                   int x2, int y2, int z2,
                   int c1, int c2, int rad,  char altl )
{
    register int rads;
    register int lx,ly,lz,dx,dy,dz;
    register int ix,iy;
    register double rx,ry,rz;
    register int rx2,ry2,rz2;
    register double nor,cos,sin,temp;
    register int ax,ay,az,bx,by,bz;
    register double inten, lightfactor;
    register ArcEntry *ptr;

    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int zerr,ystep,err;
    register int col, mid,altc,cola,p,s;

    lx = x2-x1;
    ly = y2-y1;
    lz = z2-z1;

    ix = (lx<0)? -1:1;
    iy = (ly<0)? -1:1;

    DrawCylinderCaps(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl);

    altc = 0;
    if ( altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
    cola = altc;
    lightfactor = (double)(rad)*LightLength;
    if( UseDepthCue )
    {   s = (ColourDepth*(ImageRadius-z1+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
    }

    rads = rad*rad;
    if( rad != CurrentRad )
    {    ptr = ArcTable;
        for( rx2=-2*rad; rx2<2*rad; rx2++ )
        {    ptr->x = sqrt(rads-((double)rx2*rx2)/4);
            ptr++;
        }
        CurrentRad = rad;
    }

    if( lx==0 )
    {    nor = sqrt(lz*lz+ly*ly);
        cos = (double)lz/nor;
        sin = (double)ly/nor;

        ptr = ArcTable;
        for( rx2=-rad;rx2<rad;rx2++ )
        {    /*spatial rotation along y*/
            rx = rx2;
            temp = iy*ptr->x;
            ry = -temp*cos;
            rz =  temp*sin;
            ptr++;
            ptr++;

            RoundEdges

            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;

            LineInten;
            
            offset = (Long)ay*View.yskip + ax;
            fptr = View.fbuf+offset;
            dptr = View.dbuf+offset;
            SETPIXEL(dptr,fptr,az,Lut[c1+(char)inten]);

            dx = 0;
            dy = iy*ly; 
            ystep = iy*View.yskip;
            dz = lz;
            col = c1;

            mid = (ay+by)>>1;
            err = zerr = -(dy>>1);
            if( c1 != c2 )
            {   while( ay!=mid ) YLineStep;
                col = c2;
            }
            while( ay!=by ) YLineStep;
        }
    } else if( ly==0 )
    {
        nor = (int)sqrt(lx*lx+lz*lz);
        cos = (double)lz/nor;
        sin = (double)lx/nor;

        ptr = ArcTable;
        for( ry2=-rad;ry2<rad;ry2++ )
        {    temp = ix*ptr->x;
            rx = -temp*cos;
            ry = ry2;
            rz =  temp*sin;
            ptr++;
            ptr++;

            RoundEdges

            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;

            LineInten;
            
            offset = (Long)ay*View.yskip + ax;
            fptr = View.fbuf+offset;
            dptr = View.dbuf+offset;

            SETPIXEL(dptr,fptr,az,Lut[c1+(char)inten]);

            dx = ix*lx; 
            dy = 0; 
            ystep = iy*View.yskip;
            dz = lz;
            col = c1;

            mid = (ax+bx)>>1;
            err = zerr = -(dx>>1);
            if( c1 != c2 )
            {   while( ax!=mid ) XLineStep;
                col = c2;
            }
            while( ax!=bx ) XLineStep;
        }
    }
}


void DrawCylinder( int x1, int y1, int z1,
                   int x2, int y2, int z2,
                   int c1, int c2, int rad,
                   char altl, int arad, short cap )
{
    register int wide,rads,tmp;
    register int lx,ly,lz,dx,dy,dz;
    register double rx,ry,rz;
    register int rx2,ry2,rz2;
    register double ynor,cosy,siny,znor,cosz,sinz,temp;
    register ArcEntry *ptr;

    register int ax,ay,az,bx,by,bz;
    register double inten,lightfactor;

    register Long offset;
    register Pixel __huge *fptr;
    register short __huge *dptr;
    register int zerr,ystep,err;
    register int ix,iy;
    register int col,mid;
    register int altc,cola,p,s;

    if( cap )
    {   ClipCylinder(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl,arad,cap);
        return;
    }

    /* Avoid ArcTable Overflow! */
    if( rad > MAXRAD ) rad = MAXRAD;

  /* Trivial Case */
    if( (x1==x2) && (y1==y2) )
    {   if( z1>z2 )
        {      DrawSphere(x1,y1,z1,rad,c1);
        } else DrawSphere(x2,y2,z2,rad,c2);
        return;
    }

    if( z1<z2 )
    {   tmp=x1; x1=x2; x2=tmp;
        tmp=y1; y1=y2; y2=tmp;
        tmp=z1; z1=z2; z2=tmp;
        tmp=c1; c1=c2; c2=tmp;
    }

    lx = x2-x1;
    ly = y2-y1;
    lz = z2-z1;

    if( lx==0 || ly==0 )
    {    DrawCylinderOnAxe(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl);
        return;
    }
    DrawCylinderCaps(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl);

    altc = 0;
    if ( altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
    cola = altc;
    lightfactor = (double)(rad)*LightLength;
    if( UseDepthCue )
    {   s = (ColourDepth*(ImageRadius-z1+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
    }

    ix = (lx<0)? -1:1;
    iy = (ly<0)? -1:1;

    dx = ix*lx; 
    dy = iy*ly; 
    ystep = iy*View.yskip;
    dz = lz;

    rads = rad*rad;
    tmp = lx*lx+ly*ly;
    znor = sqrt(tmp);
    cosz = (double)lx/znor;
    sinz = (double)ly/znor;

    ynor = sqrt(tmp+lz*lz);
    cosy = (double)lz/ynor;
    siny = (double)znor/ynor;
    

    if( rad != CurrentRad )
    {    ptr = ArcTable;
        for( wide=-2*rad; wide<2*rad; wide++ )
        {    ptr->x = sqrt(rads-((double)wide*wide)/4);
            ptr++;
        }
        CurrentRad = rad;
    }

    ptr = ArcTable;
    for( wide=0;wide<(rad*4);wide++ )
    {    /*spatial rotation along y*/
        rx = - ptr->x*cosy;
        ry = (double)ix*((double)wide/2-rad);
        rz = ptr->x*siny;
        ptr++;

        /*spatial rotation along z*/
        temp = rx*cosz - ry*sinz;
        ry = rx*sinz + ry*cosz;
        rx = temp;
        /*rz = rz*/

        /*round up*/
        RoundEdges

        LineInten;
        col = c1;
        
        ax = x1 + rx2; bx = ax + lx;
        ay = y1 + ry2; by = ay + ly;
        az = z1 + rz2; bz = az + lz;

        mid = (ax+bx)>>1;

        offset = (Long)ay*View.yskip + ax;
        fptr = View.fbuf+offset;
        dptr = View.dbuf+offset;

        SETPIXEL(dptr,fptr,az,Lut[c1+(char)inten]);

        if( dx>dy )
        {   err = zerr = -(dx>>1);
            if( c1 != c2 )
            {   mid = (ax+bx)>>1;
                while( ax!=mid ) XLineStep;
                col = c2;
            }
            while( ax!=bx ) XLineStep;
        } else
        {   err = zerr = -(dy>>1);
            if( c1 != c2 )
            {   mid = (ay+by)>>1;
                while( ay!=mid ) YLineStep;
                col = c2;
            }
            while( ay!=by ) YLineStep;
        }
    }
}


static int TestCylinder( int x1, int y1, int z1,
                         int x2, int y2, int z2,
                         int rad )
{
    register int tmp1, tmp2;

    if( UseSlabPlane )
        if( (z1+rad>SlabValue) || (z2+rad>SlabValue) )
            return(False);

    ClipStatus = False;

    tmp1 = x1+rad;  tmp2 = x2+rad;
    if( (tmp1<0) && (tmp2<0) )
        return( False );
    if( (tmp1>=View.xmax) || (tmp2>=View.xmax) )
        ClipStatus = True;

    tmp1 = x1-rad;  tmp2 = x2-rad;
    if( (tmp1>=View.xmax) && (tmp2>=View.xmax) )
        return( False );
    if( (tmp1<0) || (tmp2<0) )
        ClipStatus = True;

    tmp1 = y1+rad;  tmp2 = y2+rad;
    if( (tmp1<0) && (tmp2<0) )
        return( False );
    if( (tmp1>=View.ymax) || (tmp2>=View.ymax) )
        ClipStatus = True;

    tmp1 = y1-rad;  tmp2 = y2-rad;
    if( (tmp1>=View.ymax) && (tmp2>=View.ymax) )
        return( False );
    if( (tmp1<0) || (tmp2<0) )
        ClipStatus = True;

    return True;
}


void ClipCylinder( int x1, int y1, int z1,
                   int x2, int y2, int z2,
                   int c1, int c2, int rad,
                   char altl, int arad, short cap)
{
    register int wide,rads,tmp;
    register int lx,ly,lz;
    register int ix,iy;
    register double rx,ry,rz;
    register double rx1,ry1,p1,p2,p3,p4;
    register int rx2,ry2,rz2;
    register double ynor,cosy,siny,znor,cosz,sinz,temp;
    register int ax,ay,az,bx,by,bz;
    register int crx,cry,crz;

    register double inten,lightfactor;
    register ArcEntry *ptr;
    register int altc,cola,s;


    /* Avoid ArcTable Overflow! */
    if( rad > MAXRAD ) rad = MAXRAD;

    /* Visibility Tests */
    if( !TestCylinder(x1,y1,z1,x2,y2,z2,rad) )
        return;

  /* Trivial Case */
    if( (x1==x2) && (y1==y2) )
    {   if( z1>z2 )
        {      ClipSphere(x1,y1,z1,rad,c1);
        } else ClipSphere(x2,y2,z2,rad,c2);
        return;
    }

    if( z1<z2 )
    {   tmp=x1; x1=x2; x2=tmp;
        tmp=y1; y1=y2; y2=tmp;
        tmp=z1; z1=z2; z2=tmp;
        tmp=c1; c1=c2; c2=tmp;
        if( cap )
        {   if( cap == 1 ) cap = 2;
            else if( cap == 2 ) cap = 1;
        }
    }

    if( !ClipStatus && !cap )
    {   DrawCylinder(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl,arad,False);
        return;
    }

    if( cap<3 )
        ClipCylinderCaps(x1,y1,z1,x2,y2,z2,c1,c2,rad,altl,cap);

    /* Reduce to front flat cap only */
    if( cap!=1 ) cap = 0;

    altc = 0;
    if ( altl != '\0' && altl != ' ')
      altc = AltlColours[((int)altl)&(AltlDepth-1)];
    cola = altc;
    lightfactor = (double)(rad)*LightLength;
    if( UseDepthCue )
    {   s = (ColourDepth*(ImageRadius-z1+ZOffset-ShiftS))/ImageSize;
        if( s<0 ) s = 0;
        if( s>ColourMask ) s = ColourMask;
    }

    rads = rad*rad;

    if( rad != CurrentRad )
    {    ptr = ArcTable;
        for( wide=-2*rad; wide<2*rad; wide++ )
        {    ptr->x = sqrt(rads-((double)wide*wide)/4);
            ptr++;
        }
        CurrentRad = rad;
    }

    lx = x2-x1;
    ly = y2-y1;
    lz = z2-z1;

    ix = (lx<0)? -1:1;
    iy = (ly<0)? -1:1;

    tmp = lx*lx+ly*ly;
    ynor = sqrt(tmp+lz*lz);

    if( cap )
    {   CapInten = -LightDot(lx,ly,lz);
        CapInten /= (ynor*LightLength);
        if( CapInten > 0.0 )
        {   CapInten = (char)(ColourMask*CapInten);
            if( CapInten>ColourMask ) CapInten = ColourMask;
        } else CapInten = 0;
    }

    if( lx==0 )
    {    cosy = (double)lz/ynor;
        sinz = (double)ly/ynor;

        ptr = ArcTable;
        for( wide=-rad;wide<=0;wide++ )
        {    rx = wide;
            temp = iy*ptr->x;
            ry = -temp*cosy;
            rz =  temp*sinz;

            RoundEdges
            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;
            LineInten;

            if( !cap )
                CapInten = inten;

            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            crx = rx2;
            cry = ry2;
            crz = rz2;

            rx = -rx;
            ry = ry;
            rz = rz;

            RoundEdges
            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;
            LineInten;

            if( !cap )
                CapInten = inten;

            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            if( cap )
            {   ClipCapLine(x1+crx,y1+cry,z1+crz,x1-rx2,y1-ry2,z1-rz2,c1,(char)CapInten);
                ClipCapLine(ax,ay,az,x1-crx,y1-cry,z1-crz,c1,(char)CapInten);
            }

            ptr++;
            ptr++;
        }
    } else if( ly==0 )
    {
        cosy = (double)lz/ynor;
        siny = (double)lx/ynor;

        ptr = ArcTable;
        for( wide=-rad;wide<=0;wide++ )
        {    temp = ix*ptr->x;
            rx = -temp*cosy;
            ry = wide;
            rz =  temp*siny;

            RoundEdges

            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;

            if( wide == 0 )
                wide = wide;

            LineInten;

            if( !cap )
                CapInten = inten;
     
            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            crx = rx2;
            cry = ry2;
            crz = rz2;

            rx = rx;
            ry = -ry;
            rz = rz;

            RoundEdges
            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;
            LineInten;

            if( !cap )
                CapInten = inten;

            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            if( cap )
            {   if( wide == 0 )
                    wide = wide;

                ClipCapLine(x1+crx,y1+cry,z1+crz,x1-rx2,y1-ry2,z1-rz2,c1,(char)CapInten);
                ClipCapLine(ax,ay,az,x1-crx,y1-cry,z1-crz,c1,(char)CapInten);
            }

            ptr++;
            ptr++;
        }
    } else
    {   
        znor = sqrt(tmp);
        cosz = (double)lx/znor;
        sinz = (double)ly/znor;
        
        cosy = (double)lz/ynor;
        siny = (double)znor/ynor;

        ptr = ArcTable;
        for( wide=0;wide<=(rad*2);wide++ )
        {    /*spatial rotation along y*/
            rx1 = - ptr->x*cosy;
            ry1 = (double)ix*((double)wide/2-rad);
            rz = ptr->x*siny;

            /*spatial rotation along z*/
            p1 = rx1*cosz;
            p2 = ry1*sinz;
            p3 = rx1*sinz;
            p4 = ry1*cosz;

            rx = p1 - p2;
            ry = p3 + p4 ;
            RoundEdges

            crx = rx2;
            cry = ry2;
            crz = rz2;

            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;
            LineInten;

            if( !cap )
                CapInten = inten;

            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            /* symetric */
            rx = p1 + p2;
            ry = p3 - p4 ;

            RoundEdges

            ax = x1 + rx2; bx = ax + lx;
            ay = y1 + ry2; by = ay + ly;
            az = z1 + rz2; bz = az + lz;

            LineInten;

            if( !cap )
                CapInten = inten;

            ClipCylinderLine(ax,ay,az,bx,by,bz,altc,cola,c1,c2,(char)inten);

            if( cap )
            {   ClipCapLine(x1+crx,y1+cry,z1+crz,x1-rx2,y1-ry2,z1-rz2,c1,(char)CapInten);
                ClipCapLine(ax,ay,az,x1-crx,y1-cry,z1-crz,c1,(char)CapInten);
            }

            ptr++;
        }
    }
}



/*================================*/
/*  Character Rendering Routines  */
/*================================*/

void SetFontSize( int size )
{
    register int count;
    register char *ptr;
    register int i;

    if( LabelList || (MonitList && DrawMonitDistance) )
      ReDrawFlag |= RFRefresh;

    FontSize = abs(size);
    FontPS = False;
    if ( size < 0 ) FontPS = True;
    count = 0;
    for( i=0; i<23; i++ )
    {   FontDimen[i] = count>>4;
        count += FontSize;
    }

    for ( i=0; i<97; i++ )
    { if ( FontPS )
      { ptr = VectFont[i];
        FontWid[i] = 0;
        while( *ptr )
        { if( ptr[0] < 'a' )
        { if( FontDimen[ptr[0]-'A'] > FontWid[i] ) 
              FontWid[i] = FontDimen[ptr[0]-'A'];
      } else {
            if( FontDimen[ptr[0]-'a'] > FontWid[i] ) 
              FontWid[i] = FontDimen[ptr[0]-'a'];
          }
          ptr += 2;
        }
        FontWid[i] += FontSize/4 + 1;
      } else {
        FontWid[i] = FontSize;
      }
    } 
}

void SetFontStroke( int width )
{
    FontStroke = width;
}

static void ClipCharacter( int x, int y,int z, int glyph, int col )
{
    register char *ptr;
    register int sx,sy;
    register int ex=0,ey=0;

    ptr = VectFont[glyph];
    while( *ptr )
    {   /* Uppercase test */
        if( ptr[0] < 'a' )
        {   sx = x + FontDimen[ptr[0]-'A'];
            sy = y + InvertY(FontDimen[ptr[1]-'a']);
            ptr += 2;
        } else
        {   sx = ex;
            sy = ey;
        }

        ex = x + FontDimen[ptr[0]-'a'];
        ey = y + InvertY(FontDimen[ptr[1]-'a']);
        if (FontStroke < 1 ) {
          if( (ex!=sx) || (ey!=sy) )
          {   ClipLine(sx,sy,z,ex,ey,z,col,' ');
          } else ClipPoint(ex,ey,z,col);
        } else {
          if( (ex!=sx) || (ey!=sy) )
          {   ClipCylinder(sx,sy,z,ex,ey,z,col,col,FontStroke,' ',FontStroke,False);
          } /* else ClipSphere(ex,ey,z,FontStroke,col); */
        }
        ptr += 2;
    }
}


void DisplayRasString( int x, int y, int z, unsigned char *label,  int col )
{
    register int clip,high,max;
    register char *ptr;
    register int sx,sy;
    register int ex=0,ey=0;

    high = (FontSize*3)>>1;
#ifdef INVERT
    if( ((y+high)<0) || (y>=View.ymax) ) return;
    clip = (y<0) || (y+high>=View.ymax);
#else
    if( (y<0) || ((y-high)>=View.ymax) ) return;
    clip = (y-high<0) || (y>=View.ymax);
#endif

    if( x < 0 )
    {   while( *label && (x<=-FontSize) )
        {   x +=  FontWid[(*label-32)]+FontStroke;
            label++;
        }

        if( *label )
        {   ClipCharacter(x,y,z,(*label-32),col);
            x += FontWid[(*label-32)]+FontStroke;
            label++;
        } else return;
    }

    if( !clip )
    {   max = View.xmax-FontSize;
        while( *label && (x<max) )
        {  ptr = VectFont[*label-32];
           while( *ptr )
           {   /* Uppercase test */
               if( ptr[0] < 'a' )
               {   sx = x + FontDimen[ptr[0]-'A'];
                   sy = y + InvertY(FontDimen[ptr[1]-'a']);
                   ptr += 2;
               } else
               {   sx = ex;
                   sy = ey;
               }

               ex = x + FontDimen[ptr[0]-'a'];
               ey = y + InvertY(FontDimen[ptr[1]-'a']);
               if (FontStroke < 1 )
               {   if( (ex!=sx) || (ey!=sy) )
                   {   DrawTwinLine(sx,sy,z,ex,ey,z,col,col,' ');
                   } else PlotPoint(ex,ey,z,col);
               } else 
               {   if( (ex!=sx) || (ey!=sy) )
                   {   DrawCylinder(sx,sy,z,ex,ey,z,col,col,
                                  FontStroke,' ',FontStroke,False);
                   } /* else DrawSphere(ex,ey,z,FontStroke,col); */
               }
               ptr += 2;
           }

           x += FontWid[(*label-32)]+FontStroke;
           label++;
        }

        if( *label )
            ClipCharacter(x,y,z,(*label-32),col);
    } else /* Always Clip! */
        while( *label && (x<View.xmax) )
        {   ClipCharacter(x,y,z,(*label-32),col);
            x += FontWid[(*label-32)]+FontStroke;
            label++;
        }
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


void SetDepthTable( void )
{   register int i,j;
    register double temp;

    if( !UseDepthCue )
    {   for( i=0; i<ColourDepth; i++ )
            for( j=0; j<ColourDepth; j++ )
                DepthTable[i][j] = 0;
        return;
    }
    
    for( i=0; i<ColourDepth; i++ )
        for( j=0; j<ColourDepth; j++ )
        {   if( DepthCueMode == 1 )
                DepthTable[i][j] = j;
            else if( DepthCueMode == 2 )
                DepthTable[i][j] = (int)(j*i/ColourMask);
            else if( DepthCueMode == 3 )
                DepthTable[i][j] = (int)(j*j/ColourMask);
            else if( DepthCueMode == 4 )
                DepthTable[i][j] = (int)(j*j*i/(ColourMask*ColourMask));
        }

    if( FakeSpecular )
    {   for( i=0; i<ColourDepth; i++ )
        {   temp = (double)i/ColourMask;
            temp = Power(temp,SpecPower);
            temp = 1.0 - temp;
            for( j=0; j<ColourDepth; j++ )
                DepthTable[i][j] = (int)(temp*DepthTable[i][j]);
        }
    }
}


void InitialisePixUtils( void )
{   

#if defined(IBMPC) || defined(APPLEMAC)
    ArcTable = (ArcEntry __far*)_fmalloc(ARCSIZE*sizeof(ArcEntry));
#endif
    CurrentRad = 0;
    SplineCount = 5;
    SetFontSize(8);
    SetFontStroke(0);
    SetDepthTable();
}

#if defined RASTOPWIN
void UpdatePixutils()
{    if(ArcTable) free(ArcTable);
}
#endif
