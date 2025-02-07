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
 *  Isabel Serv�n Mart�nez,                                                *
 *  Jos� Miguel Fern�ndez Fern�ndez      2.6   Manual              Spanish *
 *  Jos� Miguel Fern�ndez Fern�ndez      2.7.1 Manual              Spanish *
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

/* rasmol.h
 */


/*===========================*/
/*  User Definable Options!  */
/*===========================*/

/* Platform             */

#define IBMPC
#define MSWIN
//#define APPLEMAC
//#define X11WIN
//#define UNIX

/* Projects             */

#define RASTOPWIN

/* Graphics             */

/* Please - Define one */
#define THIRTYTWOBIT
//#define SIXTEENBIT
//#define EIGHTBIT
#if !defined(EIGHTBIT) && !defined(SIXTEENBIT) && !defined(THIRTYTWOBIT)
#define THIRTYTWOBIT
#endif

#define HICOLOR
//#define DIALBOX

/* Preferences          */

#define PREFERENCES

/* Others               */

#define SOCKETS 
//#define TERMIOS
//#define PROFILE
//#define MITSHM


/*===================*/
/*  Default Options! */
/*===================*/

#if !defined(INVERT) && defined(IBMPC)
#define INVERT
#endif

#if !defined(TIME) && defined(VMS)
#define TIME
#endif


#ifdef APPLEMAC
  #define DirChar ':'
#else
  #ifndef VMS
    #ifdef IBMPC
      #define DirChar  '\\'
    #else
      #define DirChar  '/'
    #endif
  #else
    #define DirChar ']'
  #endif
#endif

/*===============================*/
/*  Fix for acos/asin problems   */
/*===============================*/

#if !defined RASTOPWIN
#ifdef asin
#undef asin
#endif
#ifdef acos
#undef acos
#endif

#define acos(x) (atan2(sqrt(1.-(x)*(x)),(x)))
#define asin(x) (atan2((x),sqrt(1.-(x)*(x))))
#endif

/*==============================*/
/*  Application-wide Constants  */
/*==============================*/

#define VER_NAME "RasTop"
#define VERSION "2.1.0"
#define VER_DATE "Jun 2004"

#ifndef True
#define True  1
#define False 0
#endif

#ifndef PI   /* Avoid Linux Warnings! */
#define PI   3.14159265358979323846
#endif


typedef unsigned char Byte;

#ifdef __STDC__
typedef signed char Char;
#else
typedef char Char;
#endif

typedef unsigned long Card;
typedef long Long;

#ifdef EIGHTBIT
typedef unsigned char Pixel;
#else
#ifdef THIRTYTWOBIT
typedef Long Pixel;
#else
typedef short Pixel;
#endif
#endif


#if defined(__sgi)
#define UnusedArgument(x)  ((x)=(x))
#else
#define UnusedArgument(x)
#endif


#define Rad2Deg      (180.0/PI)
#define Deg2Rad      (PI/180.0)
#define AbsFun(a)    (((a)<0)? -(a) : (a))
#define MinFun(a,b)  (((a)<(b))? (a) : (b) )
#define MaxFun(a,b)  (((a)>(b))? (a) : (b) )

#if defined(__STDC__) || defined(IBMPC)  || defined(APPLEMAC) || defined(__sgi)
#define ToUpper(x)   (toupper((x)))
#else
#define ToUpper(x)   (islower((x))?toupper((x)):(x))
#endif

// Function rint() not available on visual studio
#define Rint(x)          ((int)(fabs(x)+0.5))*(((x<0.0)? -1:+1))

#if !defined(IBMPC) || defined(_WIN32)
#ifdef APPLEMAC
#define _fmalloc   NewPtrSys
#define _ffree(x)  DisposePtr((Ptr)(x))
#else
#define _fmalloc   malloc
#define _ffree     free
#endif
#define _fstrnicmp strnicmp
#define _fstrcmp   strcmp
#define _fmemset   memset
#define __huge
#define __far
#endif


#define ItemCount       8
#define AdvPickAtom     0
#define AdvPickNumber   1
#define AdvSelectCount  2
#define AdvName         3
#define AdvIdent        4
#define AdvClass        5
#define AdvImage        6
#define AdvPickCoord    7



int InitialiseRasmol( void );
void ExitRasmol( void );
void RefreshScreen( void );


/*===============*
 Project Specific
 *===============*/
void WriteChar( int );
void WriteString( char* );
void AdviseUpdate( int );
void RefreshView( void );

void RasMolFatalExit( char* );
void RasMolExit( void );

void ResetUserSetBox( void );

void SendforDebug( char* );
