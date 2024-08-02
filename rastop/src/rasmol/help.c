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



#define HELP
#include "help.h"
//#include "prefer.h"
//#include "command.h"
//#include "tokens.h"
#include "molecule.h"
//#include "infile.h"
//#include "abstree.h"
//#include "transfor.h"
#include "cmndline.h"
//#include "render.h"
//#include "pixutils.h"
//#include "repres.h"
//#include "graphics.h"
//#include "outfile.h"
//#include "script.h"
//#include "multiple.h"
//#include "vector.h"
//#include "brotate.h"
#include "langsel.h"

//#include <math.h>


typedef struct _HlpEntry {
                struct _HlpEntry __far *next;
                struct _HlpEntry __far *info;
                char __far *keyword;
                Long fpos;
                } HlpEntry;

#define HelpPool   16
static char HelpFileName[256];
static char HelpFileBuf[80];
static HlpEntry __far *FreeInfo;
static HlpEntry __far *HelpInfo;


/*====================================*/
/*  Command Line On-Line Help System  */
/*====================================*/

static int PrefixString( char __far *str1, char  __far *str2 )
{
    while( *str1 )
        if( *str1++ != *str2++ )
            return False;
    return True;
}

char __far *xfgets( char __far* s, int n,  FILE __far *fp )
{
    register int i;
    register int c;
    register char __far *cs;
    cs = s;
    c = '\0';
    for (i = 0; i < n-1; i++)
    {
      if ((c = fgetc(fp)) == EOF)
      {
        *cs++ = '\0';
        return NULL;
      }
      if (c == '\r' || c == '\n' || c == '\0') break;
      *cs++ = c;
    }
    if (c == '\r') {
      c = getc(fp);
      if ( c != '\n' )
        ungetc(c,fp);
    }
    *cs++ = '\n';
    *cs++ = '\0';
    return s;             
}



static HlpEntry __far *EnterHelpInfo( char *text )
{
    register HlpEntry __far * __far *tmp;
    register HlpEntry __far *ptr;
    register int res,len,i;
    register char ch;
    char keyword[32];

    ptr = (void __far*)0;
    while( *text && (*text!='\n') )
    {   while( *text && (*text!='\n') && (*text==' ') )
            text++;

        len = 0;
        while( *text && (*text!='\n') && (*text!=' ') )
            if( len<31 )
            {   ch = *text++;
                keyword[len++] = ToUpper(ch);
            } else text++;
        keyword[len]='\0';

        if( ptr )
        {   tmp = &ptr->info;
            ptr = (void __far*)0;
        } else tmp = &HelpInfo;

        while( *tmp )
        {   res = _fstrcmp(keyword,(*tmp)->keyword);
            if( res==0 ) /* Exact Match */
            {   ptr = *tmp;
                break;
            } else if( res<0 )
                break;
            tmp = &(*tmp)->next;
        }

        if( !ptr )
        {   if( !FreeInfo )
            {   ptr = (HlpEntry __far*)_fmalloc(HelpPool*sizeof(HlpEntry));
                RegisterAlloc( ptr );
                if( !ptr ) 
                    RasMolFatalExit(MsgStrs(RfeCmdMem));
                for( i=1; i<HelpPool; i++ )
                {   ptr->next = FreeInfo;
                    FreeInfo = ptr++;
                }
            } else
            {   ptr = FreeInfo;
                FreeInfo = ptr->next;
            }

            ptr->keyword = (char __far*)_fmalloc(len+1);
            if( !(ptr->keyword) ) 
                    RasMolFatalExit(MsgStrs(RfeCmdMem));
                RegisterAlloc( (ptr->keyword) );
            for( i=0; i<=len; i++ )
                ptr->keyword[i] = keyword[i];

            ptr->info = (void __far*)0;
            ptr->next = *tmp;
            ptr->fpos = 0;
            *tmp = ptr;
        }
    }
    return ptr;
}


static void InitHelpFile( void )
{
    register char *dst;
    register HlpEntry __far *fix;
    register HlpEntry __far *ptr;
    register FILE *fp;
    register Long pos;
    char buffer[82];

    if(!*RasMolPath)
        return;

    dst = RasMolPath;
    dst = strcpy(HelpFileName,dst);
    while( *dst++ ) {};
    dst--;
    dst = strcpy(dst,"help");
    dst += 4;
    *dst++ = DirChar;
    strcpy(dst,"rasmol.hlp");

    fp=fopen(HelpFileName,"rb");
    if( !fp )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(HlpHFil));
        return;
    }


    pos = 0;
    xfgets(buffer,80,fp);
    while( !feof(fp) )
    {    fix = (void __far*)0;
         while( *buffer=='?' )
         {   ptr = EnterHelpInfo(buffer+1);
             if( ptr )
             {   ptr->info = fix;
                 fix = ptr;
             }

             pos = ftell(fp);
             if( !xfgets(buffer,80,fp) )
                 break;
         }

         while( fix )
         {   ptr = fix->info;
             fix->info = (void __far*)0;
             fix->fpos = pos;
             fix = ptr;
         }

         while( xfgets(buffer,80,fp) )
             if( *buffer=='?' )
                 break;
    }
    fclose(fp);
}


void FindHelpInfo( char *top, char *subtop )
{
    register HlpEntry __far * __far *tmp;
    register HlpEntry __far *ptr;
    register int res;
    register Long pos;
    register FILE *fp;
    char buffer[82];
    int found;

    if( !HelpInfo )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(HlpHFil));
        return;
    }

    found = 0;
    if( *top )
    {   ptr = NULL;
        tmp = &HelpInfo;

        while( *tmp )
        {   res = _fstrcmp(top,(*tmp)->keyword);
            if( res<0 )
            {   if( PrefixString(top,(*tmp)->keyword) )
                {   ptr = *tmp;
                    if( ptr->next && 
                        PrefixString(top,ptr->next->keyword) )
                    {   InvalidateCmndLine();
                        WriteString(MsgStrs(HlpHTop));
                        return;
                    } else break;
                } else break;
            } else if( res==0 ) 
            {   ptr = *tmp;
                found = 1;
                break;
            }
            tmp = &(*tmp)->next;
        }

        if( !found )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(HlpHNone));
            return;
        }

        tmp = &ptr->info;

        if( *subtop ) {
            ptr = (void __far*)0;
            found = 0;
            while( *tmp )
            {   res = _fstrcmp(subtop,(*tmp)->keyword);
                if( res<0 )
                {   if( PrefixString(subtop,(*tmp)->keyword) )
                    {   ptr = *tmp;
                        if( ptr->next && 
                            PrefixString(subtop,ptr->next->keyword) )
                        {   InvalidateCmndLine();
                            WriteString(MsgStrs(HlpHTop));
                            return;
                        } else break;
                    } else break;
                } else if( res==0 ) 
                {   ptr = *tmp;
                found = 1;
                    break;
                }
                tmp = &(*tmp)->next;
            }
            if( !found )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(HlpHNone));
                return;
            }
        }

        if( !ptr->fpos )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(HlpHNone));
            return;
        } else pos=ptr->fpos;

    } else return;


    if( !(fp=fopen(HelpFileName,"rb")) )
        RasMolFatalExit(MsgStrs(RfeHROpn));

    InvalidateCmndLine();

    fseek(fp,pos,SEEK_SET);
    while( xfgets(buffer,80,fp) )
        if( *buffer!='?' )
        {   InvalidateCmndLine();
            WriteString(buffer);
        } else break;
    fclose(fp);
}



void StartHelpHTML( void )
{
    if( !*HelpHTMLSource )
    {   InvalidateCmndLine();
		WriteString(MsgStrs(HlpHTMLHelp));
        return;
    } 

#ifdef MSWIN
	ShellExecute(NULL, NULL, HelpHTMLSource, 
			NULL,"C:\\",SW_SHOWNORMAL);
#endif
}

/* Initiate default path in rasmol/help subdiectory */
static void InitHelpHTML( void ) {
    register char *dst;
    register FILE *fp;

    if(!*RasMolPath)
        return;

    dst = RasMolPath;
    dst = strcpy(HelpHTMLSource,dst);
    while( *dst++ ) {};
    dst--;
    dst = strcpy(dst,"help");
    dst += 4;
    *dst++ = DirChar;
    strcpy(dst,"index.htm");
    if( !(fp=fopen(HelpHTMLSource,"rb")) )
        *HelpHTMLSource = '\0';
    else
        fclose(fp);
}

void InitializeHelp( void )
{
    *HelpFileName = '\0';
    *HelpHTMLSource = '\0';

    FreeInfo = (void __far*)0;
    HelpInfo = (void __far*)0;

    InitHelpFile();
    InitHelpHTML();


}


