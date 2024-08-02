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


/* command.c
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

#define COMMAND
#include "prefer.h"
#include "command.h"
#include "tokens.h"
#include "molecule.h"
#include "infile.h"
#include "abstree.h"
#include "transfor.h"
#include "cmndline.h"
#include "render.h"
#include "pixutils.h"
#include "repres.h"
#include "graphics.h"
#include "outfile.h"
#include "script.h"
#include "multiple.h"
#include "vector.h"
#include "brotate.h"
#include "langsel.h"
#include "help.h"

#include <math.h>


#if defined(__sun) && !defined(_XOPEN_SOURCE)
/* SUN doesn't always define popen in stdio! */
extern FILE *popen( const char*, const char* );
#endif

/* Macros for commonly used loops */
#define ForEachAtom  for(chain=Database->clist;chain;chain=chain->cnext) \
                     for(group=chain->glist;group;group=group->gnext)    \
                     for(ptr=group->alist;ptr;ptr=ptr->anext)
#define ForEachBond  for(bptr=Database->blist;bptr;bptr=bptr->bnext)


#define IsIdentChar(x)  ((isalnum(x))||((x)=='_')||((x)=='$'))

#ifdef INVERT
#define InvertY(y) (y)
#else
#define InvertY(y) (-(y))
#endif



#define ErrSyntax        0
#define ErrBigNum        1
#define ErrBadOpt        2
#define ErrParam         3
#define ErrFilNam        4
#define ErrBadLoad       5
#define ErrNotNum        6
#define ErrNotSep        7
#define ErrNotBrac       8
#define ErrNoCol         9
#define ErrColour       10
#define ErrBadArg       11
#define ErrBadExpr      12
#define ErrParen        13
#define ErrScript       14
#define ErrFunc         15
#define ErrSetName      16
#define ErrBadSet       17
#define ErrInScrpt      18
#define ErrOutScrpt     19
#define ErrBadMolDB     20
#define ErrBlocSel      21
#define ErrNoBond       22


#define MAXERROR 20
static int ErrorFlag = 0;
static int ErrorCount = 0;


typedef struct {
        Byte red;
        Byte grn;
        Byte blu;
    } RGBStruct;

static RGBStruct ColourTable[34] = {
    {   0,   0,   0 },  /* Black      */
    {   0,   0, 255 },  /* Blue       */
    { 175, 214, 255 },  /* BlueTint   */
    { 175, 117,  89 },  /* Brown      */
    {   0, 255, 255 },  /* Cyan       */
    { 255, 156,   0 },  /* Gold       */
    { 125, 125, 125 },  /* Gray       */
    {   0, 255,   0 },  /* Green      */
    {  46, 139,  87 },  /* GreenBlue  */
    { 152, 255, 179 },  /* GreenTint  */
    { 255,   0, 101 },  /* HotPink    */
    { 255,   0, 255 },  /* Magenta    */
    { 255, 165,   0 },  /* Orange     */
    { 255, 101, 117 },  /* Pink       */
    { 255, 171, 187 },  /* PinkTint   */
    { 160,  32, 240 },  /* Purple     */
    { 255,   0,   0 },  /* Red        */
    { 255,  69,   0 },  /* RedOrange  */
    {   0, 250, 109 },  /* SeaGreen   */
    {  58, 144, 255 },  /* SkyBlue    */
    { 238, 130, 238 },  /* Violet     */
    { 255, 255, 255 },  /* White      */
    { 255, 255,   0 },  /* Yellow     */
    { 246, 246, 117 }   /* YellowTint */
        };



static char ResidueChar[29] = {
        'A', 'G', 'L', 'S', 'V', 'T', 'K', 'D', 'I', 'N',
        'E', 'P', 'R', 'F', 'Q', 'Y', 'H', 'C', 'M', 'W',
        'B', 'Z', '*', 'P',
        'A', 'C', 'G', 'T',
        'U'
    };


static int TokenLength;
static Long TokenValue;
static char TokenIdent[128];
static char *TokenStart;
static char *TokenPtr;
static int CurToken;
static int ZapToken = False;   /* Prevent some tokens when adding rsm files  */
static int AddRSMDir = False;  /* Indicate that RSM directory is to be added */


static int RVal, GVal, BVal;


/*=======================*/
/*  Function Prototypes  */
/*=======================*/

int ExecuteCommand( void );
int ExecuteIPCCommand( char __huge* );
static int PrefixString( char __far*, char __far* );
static int FetchToken( void );

static void CommandError( char *error )
{
    register char *ptr;
    char buffer[80];
    char TempLine[MAXBUFFLEN];

    if( TokenPtr )
    {   if( FileDepth > -1 )
        {   strcpy(TempLine,CurLine);
            InvalidateCmndLine();
            WriteString(TempLine);
            WriteChar('\n');
        } else 
            WriteString("        ");

        for( ptr=CurLine; ptr<TokenStart; ptr++ )
            WriteChar(' ');
        WriteString("^\n");
    }

    if( FileDepth > -1 )
    {   if( LineStack[FileDepth] )
        {   if( NameStack[FileDepth] )
            {   WriteChar('"');
                ptr = NameStack[FileDepth];
                while( *ptr ) ptr++;
                while( *ptr != '\\' && ptr != NameStack[FileDepth] ) ptr--;
                if( *ptr == '\\' ) ptr++;
                WriteString(ptr);
                WriteString("\", ");
            }
            sprintf(buffer,"line %d: ",LineStack[FileDepth]);
            WriteString(buffer);
        } else
        {   WriteString(NameStack[FileDepth]);
            WriteString(": ");
        }
    }

    if( error ) {
        WriteString(error);
        /* rasmol 2.7.2.1.1
        if (strlen(error)>(size_t)0 && !(error[strlen(error)-(size_t)1]=='\n')) {
          WriteString("\n");
        }*/
    }
    CommandActive = False;
    CurToken = 0;
}



/*==========================*/
/*  File Handling Services  */
/*==========================*/

#ifdef IBMPC
static char *ProcessFileName( char *name )
{
    register char *ptr;

    while( *name==' ' )
        name++;

    ptr = DataFileName;

#if !defined RASTOPWIN
    while( *name )
    {   *ptr++ = ToUpper(*name);
        name++;
    }
#else
    while( *name )
        *ptr++ = *name++;
#endif

    /* Strip trailing spaces! */
    while( (ptr!=DataFileName) && (ptr[-1]==' ') )
        ptr--;
    *ptr = '\0';
    return ptr;
}
#endif

#ifdef APPLEMAC
static char *ProcessFileName( char *name )
{
    register char *ptr;

    while( *name==' ' )
        name++;

    ptr = DataFileName;
    while( *name )
        *ptr++ = *name++;

    /* Strip trailing spaces! */
    while( (ptr!=DataFileName) && (ptr[-1]==' ') )
        ptr--;
    *ptr = '\0';
    return ptr;
}
#endif

#ifdef VMS 
static char *ProcessFileName( char *name )
{
    register char *ptr;

    while( *name==' ' )
        name++;

    ptr = DataFileName;
    while( *name && (*name!=' ') )
    {   *ptr++ = ToUpper(*name);
        name++;
    }
    *ptr = '\0';
    return ptr;
}
#endif


#if !defined(IBMPC) && !defined(APPLEMAC) && !defined(VMS)
static int IsSecure( int ch )
{
    switch( ch )
    {   /* Dangerous characters in UNIX "popen"!  */
        case('<'):  case('>'):  case('('):  case(')'):
        case('{'):  case('}'):  case('['):  case(']'):
        case('\''): case(';'):  case('|'):  case('&'):
            return False;
    }
    return True;
}


static char *ProcessFileName( char *name )
{
    register struct passwd *entry;
    register char *temp;
    register char *ptr;
    char username[64];

    while( *name==' ' )
        name++;

    /* Perform filename globbing */
    if( *name=='~' )
    {   ptr = username;  name++;
        while( *name && (*name!=' ') && (*name!='/') )
            *ptr++ = *name++;
        *ptr = '\0';

        ptr = DataFileName;
        if( *username )
        {   if( (entry=getpwnam(username)) )
            {   temp = entry->pw_dir;
                endpwent();
            } else /* Unknown user! */
            {   temp = username;
                *ptr++ = '~';
            }

        } else if( !(temp=(char*)getenv("HOME")) )
            temp = ".";

        while( *temp )
            *ptr++ = *temp++;
    } else ptr = DataFileName;

    /* Strip dubious characters! */
    while( *name && (*name!=' ') )
        if( IsSecure(*name) )
        {   *ptr++ = *name++;
        } else name++;
    *ptr = '\0';
    return ptr;
}
#endif


#ifdef UNIX

#define MaxFileExt  4
/* UNIX Compressed Filename extensions! */
static char *FileExt[MaxFileExt] = { "", ".Z", ".gz", ".z" };

static FILE *OpenDataFile( char *begin, char *end )
{
    register char *src, *dst;
    register FILE *fp;
    register int i;
    
    for( i=0; i<MaxFileExt; i++ )
    {   dst = end; src = FileExt[i];
        while( (*dst++ = *src++) );
        if( (fp=fopen(begin,"rb")) ) {
          *end = '\0';
          return fp;
    }
    }
    fp = fopen(begin,"rb");
    *end = '\0';
    return fp;
}
#else /* !defined(UNIX) */

static FILE *OpenDataFile( char *begin, char *end )
{
    register FILE *fp;

    fp = fopen(begin,"rb");
    return fp;
}
#endif


int ProcessFile( int format, int info, FILE *fp )
{
    register int done;
    char __far *temp;
    register MolSet __far *mcur;

    if( FileDepth!=-1 )
        BeginWait();

    /* Create new molecule */
    mcur = CurMolSet;
    CreateNewMolSet( );

    temp = getenv("RASMOLCIF");
    if (temp) {
      UseCIF = 0;
      if (!strncasecmp(temp,"cif",3)) 
      	UseCIF = 1;
    }

    /* The Format may be changed in LoadPDB or LoadCIF */
    DataFileFormat = format;

    switch( format )
    {   case(FormatPDB):      done = LoadPDBMolecule(fp,False);  break;
        case(FormatNMRPDB):   done = LoadPDBMolecule(fp,True);   break;
        case(FormatMacroMod): done = LoadMacroModelMolecule(fp); break;
        case(FormatAlchemy):  done = LoadAlchemyMolecule(fp);    break;
        case(FormatCharmm):   done = LoadCharmmMolecule(fp);     break;
        case(FormatBiosym):   done = LoadBiosymMolecule(fp);     break;
        case(FormatMOPAC):    done = LoadMOPACMolecule(fp);      break;
        case(FormatSHELX):    done = LoadSHELXMolecule(fp);      break;
        case(FormatMol2):     done = LoadMol2Molecule(fp);       break;
        case(FormatFDAT):     done = LoadFDATMolecule(fp);       break;
        case(FormatMDL):      done = LoadMDLMolecule(fp);        break;
        case(FormatXYZ):      done = LoadXYZMolecule(fp);        break;
        case(FormatCIF):      done = LoadCIFMolecule(fp);        break;
#ifdef CEXIOLIB
        case(FormatCEX):      done = LoadCEXMolecule(fp);        break;
#endif
        default:              done = False;
    }


    /* The filename might have changed to "inline" in DivertoData */
    strcpy(Info.filename,DataFileName);

    if( FileDepth!=-1 )
        EndWait();

    if( !done || !Database )
    {   ZapMolecule();
        SwitchMolecule(mcur);
        return False;
    }

    CurMolSet->format = DataFileFormat;
    RecordMolFile();

    if( MoleculeList && !MoleculeList->next)
    {   /* Only the first file loaded */
        SetDirectory();
        if( ScriptFileFormat == RSMFormat )
        {   WorldScriptFormat = RSMFormat;
            FileModified = False;
        } else       
        {   WorldScriptFormat = DataFormat;
            FileModified = True;
        }
    } else
    {   /* Not adding a molecule from an RSM file */
        if( (WorldScriptFormat != RSMFormat) ||
            (FileDepth != 1)                 ||
            (ScriptFileFormat != RSMFormat)   )
        {   WorldScriptFormat = DataFormat;
            FileModified = True;
        }
    }

    SetTitle();

    if (info) {
        InvalidateCmndLine();
        WriteString(MsgStrs(StrLoadFile));
        WriteString(CurMolSet->title);
        if( CurMolSet->copynum>1 )
        {   char buffer[80];
            WriteString(" #");
            _itoa(CurMolSet->copynum,buffer,10);
            WriteString(buffer);
        }
        WriteString(":\n");
        DescribeMolecule();
    }

    AdviseUpdate(AdvName);
    AdviseUpdate(AdvClass);
    AdviseUpdate(AdvIdent);

#ifdef X11WIN
    if( Interactive )
       FetchEvent(False);
#endif

    if( CalcBondsFlag )
    {   if( Info.bondcount <= (MainAtomCount+HetaAtomCount)-Info.chaincount )
        {   if( MainAtomCount+HetaAtomCount > 255 )
            {   CreateMoleculeBonds(info,False,False);
            } else CreateMoleculeBonds(info,True,False);
        }
    }
    if (info)
        WriteString("\n");

    /* Explicit Hydrogen Bonds! */
    if( Info.hbondcount > 0 )
        SetHBondStatus(True,True,0,0);

    /* Explicit SSbonds!        */
    if (Info.ssbondcount > 0 ) {
      SetHBondStatus(False,True,0,0);
      SSBondMode = True;
    }

    ResetPrevSelection();
    VoxelsClean = False;
    RotMode = RotAll;
    InitialTransform();
    ReDrawFlag |= RFInitial;
    return True;
}


int FetchFile( int format, int info, char *name )
{
#ifndef APPLEMAC
#ifdef UNIX
    register int comp;
#endif /* UNIX */
    register char *src,*dst;
    register char *tmp;
    char buffer[128];
    char msg[80];
#endif /* APPLEMAC */

    register int done = 0;
    register FILE *fp;

    name = ProcessFileName(name);
    fp = OpenDataFile(DataFileName,name);

#ifndef APPLEMAC
    /* Search for directory specification! */
#ifndef VMS
    if( !fp )
    {   src = DataFileName;
        while( *src && (*src!=DirChar) )
            src++;
        done = !(*src);
    }
#else
    done = True;
#endif

    /* Try using a default file path! */
    if( !fp && done )
    {   switch( format )
        {   case(FormatNMRPDB):
            case(FormatPDB):     src = (char*)getenv("RASMOLPDBPATH");  break;
            case(FormatMol2):    src = (char*)getenv("RASMOLMOL2PATH"); break;
            case(FormatMMDB):    src = (char*)getenv("RASMOLMMDBPATH"); break;
            case(FormatAlchemy): src = (char*)getenv("RASMOLMOLPATH");  break;
            case(FormatMDL):     src = (char*)getenv("RASMOLMDLPATH");  break;
            case(FormatXYZ):     src = (char*)getenv("RASMOLXYZPATH");  break;
            case(FormatCIF):     src = (char*)getenv("RASMOLCIFPATH");  break;
            default:             src = NULL;
        }

        if( src && *src )
        {   
#ifdef VMS
            dst = buffer;
            while( *src ) *dst++ = *src++;

            tmp = DataFileName;
            while( *dst = *tmp++ ) dst++;
            if( fp = OpenDataFile(buffer,dst) )
                strcpy(DataFileName,buffer);
#else
            while( *src )
            {   dst = buffer;
                while( *src && (*src!=':') ) 
                    *dst++ = *src++;
                if( *src == ':' ) 
                    src++;

                if( dst != buffer )
                {   if( *(dst-1) != DirChar )
                        *dst++ = DirChar;
                    tmp = DataFileName;
                    while( (*dst = *tmp++) ) dst++;
                    if( (fp = OpenDataFile(buffer,dst)) )
                    {   strcpy(DataFileName,buffer);
                        break;
                    }
                }
            }
#endif
        }
    }
#endif /* APPLEMAC */


#ifdef CEXIOLIB
    if( !fp && (format==FormatCEX) )
    {   if( ProcessFile(format,info,fp) )
            return True;
    }
#endif

    if( !fp )
    {   *name = '\0';
        InvalidateCmndLine();
        FormatMsg(msg,StrErrFile,DataFileName);
        WriteString(msg);
        CommandActive=False;
        return False;
    }

#ifdef UNIX
    done = getc(fp);
    if( done == 0x1f )
    {   done = getc(fp);
        fclose(fp);

        if( done == 0x9d )
        {   /* Should #include <signal.h> and trap "" SIGPIPE */
            sprintf(buffer,"trap \"\" 13; uncompress -c %s 2> /dev/null\n",
                                                              DataFileName);
        } else if( done == 0x8b )
        {   /* Should #include <signal.h> and trap "" SIGPIPE */
            sprintf(buffer,"trap \"\" 13; gzip -cdq %s 2> /dev/null\n",
                                                          DataFileName);
        } else /* bad magic number! */
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrCFmt));
            return False;
        }
   
        comp = True;
        fp = popen(buffer,"r");
        if( !fp )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrDcmp));
            return False;
        }
    } else /* Uncompressed! */
    {   ungetc(done,fp);
        comp = False;
    }
#endif

    done = ProcessFile(format,info,fp);

#ifdef UNIX
    if( comp )
    {   if( pclose(fp) )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrDcmp));
            return False;
        }
    } else fclose(fp);
#else /* !defined(UNIX) */
    fclose(fp);
#endif
    return done;
}
  

int DivertToData( int format, int info )
{
    register int ch,len,done;
    register Long pos;
    FILE *fp;

    fp = FileStack[FileDepth];
    pos = ftell(fp);

    if( ScriptFileFormat==RSMFormat )
    {    fseek(fp,0,SEEK_SET);
        AcceptData[FileDepth] = 'N';
        done = ProcessFile( format, info, fp );
        fseek(fp,pos,SEEK_SET);
        return done;
    } else
    {   
        do 
        {    len = 0;
            ch = getc(fp);
            while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
            {   if( len<MAXBUFFLEN )
                    CurLine[len++] = ch;
                ch = getc(fp);
            }
    
            if( ch == '\r' )
            {   ch = getc(fp);
                if( ch != '\n' )
                    ungetc(ch,fp);
            }
    
            if( len<MAXBUFFLEN )
            {   CurLine[len] = '\0';
                TokenPtr = CurLine;
                if( FetchToken() ) 
                {    if ( CurToken == QuitTok || CurToken == ExitTok ) 
                    {    ScriptFileFormat = InlineFormat;
                        done = ProcessFile( format, info, fp );
                        fseek(fp,pos,SEEK_SET);
                        strcpy (Info.filename,"inline");
                        return done;
                    } else 
                    {    if ( CurToken == HeaderTok || CurToken == CIFDataTok ) 
                        {    Recycle = &CurLine[0];
                            AcceptData[FileDepth] = 'N';
                            ScriptFileFormat = InlineFormat;
                            done = ProcessFile( format, info, fp );
                            fseek(fp,pos,SEEK_SET);
                            strcpy (Info.filename,"inline");
                            return done;
                        }
                    }
                }
            } else CommandError(MsgStrs(StrSLong));
        } while( ch!=EOF );
    }

    return False;
}


void LoadScriptFile( FILE *fp,  char *name )
{
    register char *ptr;
    register int ch;
    register size_t len;
    register int stat;
    char msg[80];


    if( fp )
    {   FileDepth++;
        len = strlen(name)+1;
        ptr = (char*)_fmalloc(len);
        memcpy(ptr,name,len);
        NameStack[FileDepth] = ptr;
        FileStack[FileDepth] = fp;
        LineStack[FileDepth] = 0;
        AcceptData[FileDepth] = 'Y';

        ScriptFileFormat = ScriptFormat;

        /* Check for RSM files */
        do {
            len = 0;
            ch = getc(fp);
            while( isspace(ch) ) ch =  getc(fp);
            while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
            {   if( len<18 )
                    CurLine[len++] = ch;
                ch = getc(fp);
            }
            CurLine[len] = '\0';

            if( ch == '\r' )
            {   ch = getc(fp);
                if( ch != '\n' )
                    ungetc(ch,fp);
            }

            LineStack[FileDepth]++;
            if( (len==18) && (CurLine[0]=='#') )
                if( !strncmp(CurLine,"#!rasmol -rsm file",18) )
                {    ScriptFileFormat = RSMFormat;
                    break;
                }
        } while( ch!=EOF );

        if( ScriptFileFormat != RSMFormat )
            ZapToken = False;

        /* First level RSM file opening */
        if( (FileDepth==0) && (ScriptFileFormat==RSMFormat) )
            AddRSMDir = True;

        if( ch==EOF )
        {    LineStack[FileDepth] = 0;
            fseek(fp,0,SEEK_SET);
        }

        do {
            len = 0;
            ch = getc(fp);
            while( (ch!='\n') && (ch!='\r') &&  (ch!=EOF) )
            {   if( len<(size_t)MAXBUFFLEN )
                    CurLine[len++] = ch;
                ch = getc(fp);
            }

            if( ch == '\r' )
            {   ch = getc(fp);
                if( ch != '\n' )
                    ungetc(ch,fp);
            }

            LineStack[FileDepth]++;
            if( len<(size_t)MAXBUFFLEN )
            {   CurLine[len] = '\0';
                /* Fast Drawing during Scripts */
                FastDraw = True;
                stat = ExecuteCommand();

                /* Stop reading files with too many unrecognized commands */
                if( ErrorFlag )
                {    ErrorFlag = 0;
                    ErrorCount++;
                    if( ErrorCount>MAXERROR )
                    {   InvalidateCmndLine();
                        WriteString(MsgStrs(StrTooManyErr));
                        WriteString(MsgStrs(StrScrStop));
                        ErrorCount = 0;
                        stat = ExitTok;
                    }
                } else ErrorCount = 0;

                if( stat )
                {   if( stat == QuitTok )
                    {   while( FileDepth >= 0 )
                        {   fclose(FileStack[FileDepth]);
                            free(NameStack[FileDepth]);
                            FileDepth--;
                        }
                        RasMolExit();
                    } else /* ExitTok */
                        break;
                } else if( IsPaused )
                {   ZapToken = False;
                    AddRSMDir = False;
                    return;
                }

            } else CommandError(MsgStrs(StrSLong));
        } while( ch!=EOF );
        free(NameStack[FileDepth]);
        fclose( fp );
        FileDepth--;
        if( FileDepth==-1 )
            AddRSMDir = False;

    } else
    {   CommandError( (char*)NULL );
        FormatMsg(msg,StrSFile,name);
        InvalidateCmndLine();
        WriteString(msg);
    }
    FastDraw = False;
}


/*============================*/
/*  RSM file saving routines  */
/*============================*/


static int SaveSingleMolecule( char *filename, int world )
{   FILE *fp;
    char filepath[1024];
    register char *ptr;
    register int result;

    if( !MoleculeList )
        return False;

    if( !world && !filename )
        return False;

    if( !filename && (FileModified==False) )
        return True;

    BeginWait();

    if( !filename )
    {   ptr = strcpy(filepath,RSMPathName);
        while( *ptr ) ptr++;
        strcpy(ptr,SpaceName);
        while( *ptr ) ptr++;
        strcpy(ptr,".rsm");
    } else
        strcpy(filepath,filename);

    fp = fopen(filepath,"w");

    if( !fp )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrErrNoFile));
        EndWait();
        return False;
    }

    if( fputs(CurMolSet->molfile, fp) == EOF )
    {    fclose(fp);
        InvalidateCmndLine();
        WriteString(MsgStrs(StrErrSave));
        EndWait();
        return False;
    }

    fclose(fp);

    /* Reset World */
    ResetWorldAxes(True);
    StorePrevSelection();

    result = WriteScriptFile(filepath,True,world);

    /* Set World Back */
    SetWorldBack();
    SelectPrevSelection(False);
    
    /* Record result */
    if( !result )
    {   fclose(fp);
        InvalidateCmndLine();
        WriteString(MsgStrs(StrNoScrSave));
        WorldScriptFormat = DataFormat;
        FileModified = True;
    } else if( filename && world )
    {   WorldScriptFormat = RSMFormat;
        FileModified = False;
        SetDirectory();
        SetTitle();
    } else if( !filename && world )
        FileModified = False;

    EndWait();
    return result;
}


static int SaveWorldMolecules( char *filename)
{   FILE *fp;
    char filepath[1024], savedpath[1024], buffer[16];
    register int count, result;
    register MolSet __far *mptr, *mcur;

    if( !MoleculeList )
        return False;

    BeginWait();

    if( filename )
    {   strcpy(savedpath,RSMPathName);
        strcat(savedpath,SpaceName);
        strcat(savedpath,".rsm");
        /* Temporary Directory */
        SetDirectory();
    } else
        *savedpath = '\0';

    /* Reset World */
    ResetWorldAxes(True);
    StorePrevSelection();

    count = 1;
    mcur = CurMolSet;
    mptr=MoleculeList;
    while( mptr )
    {   
        SwitchMolecule(mptr);

        if( count==1 )
        {   strcpy(filepath,RSMPathName);
            strcat(filepath,SpaceName);
            strcat(filepath,".rsm");
        } else
        {   strcpy(filepath,RSMPathName);
            strcat(filepath,SpaceName);
            sprintf(buffer,"w%d",count);
            strcat(filepath,buffer);
        }

        fp = fopen(filepath,"w");

        if( !fp )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrErrNoFile));
            result = False;
            break;
        }

        if( fputs(CurMolSet->molfile,fp) == EOF )
        {    fclose(fp);
            InvalidateCmndLine();
            WriteString(MsgStrs(StrErrSave));
            result = False;
            break;
        }

        fclose(fp);
        result = WriteScriptFile(filepath,True,True);
        mptr = mptr->next;
        count++;
    }

    /* Set World Back */
    SetWorldBack();
    SwitchMolecule(mcur);
    SelectPrevSelection(False);
    
    /* Record result */
    if( !result )
    {   fclose(fp);
        InvalidateCmndLine();
        WriteString(MsgStrs(StrSavWorld));
        WorldScriptFormat = DataFormat;
        FileModified = True;

        /* Set back to initial directory */
        strcpy(DataFileName,savedpath);
        SetDirectory();

    } else if( filename)
    {   WorldScriptFormat = RSMFormat;
        FileModified = False;
        SetDirectory();
        SetTitle();
    } else
    {   FileModified = False;
    }

    EndWait();
    return result;
}


static int SaveRsmMolecule( char *filename, int world )
{    
    if( !Database || !MoleculeList )
        return False;

    if( (WorldScriptFormat!=RSMFormat) && !filename )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrErrNoName));
        return False;
    }

    if( !world && !filename )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrErrNoName));
        return False;
    }

    if( !filename && !FileModified)
        return True;

    if( !CurMolSet->molfile )
    {    InvalidateCmndLine();
        WriteString(MsgStrs(StrNoMolSave));
        return False;
    }

    if( !MoleculeList->next )
    {   return( SaveSingleMolecule(filename, world) );
    } else if( !world )
    {   return( SaveSingleMolecule(filename, world) );
    } else
    {   return( SaveWorldMolecules(filename) );
    }
    return False;
}



/*=================================*/
/*  Command Line Lexical Analysis  */
/*=================================*/

static int FetchToken( void )
{
    register char ch;

    CurToken = 0;
    ch = *TokenPtr++;
    while( ch && (ch!='#') )
    {   if( isspace(ch) )
        {   ch = *TokenPtr++;
            continue;
        }

        TokenStart = TokenPtr-1;
        if( isalpha(ch) )
         {   TokenLength = 1;
             *TokenIdent = ToUpper(ch);
             while( IsIdentChar(*TokenPtr) && (TokenLength<32) )
             {   ch = *TokenPtr++;
                 TokenIdent[TokenLength++] = ToUpper(ch);
             }
             if( TokenLength==32 )
             {   CommandError(MsgStrs(StrSLong));
                 return(0);
             } else TokenIdent[TokenLength] = '\0';
             return( CurToken = LookUpKeyword(TokenIdent) );

         } else if( isdigit(ch) )
         {   TokenValue = ch-'0';
             while( isdigit(*TokenPtr) )
                 TokenValue = 10*TokenValue + (*TokenPtr++)-'0';
             return( CurToken = NumberTok );

         } else if( (ch=='\'') || (ch=='\"') || (ch=='`') )
         {   TokenLength = 0;
             while( *TokenPtr && (TokenLength<128) && (*TokenPtr!=ch) )
                 TokenIdent[TokenLength++] = *TokenPtr++;

             if( ch != *TokenPtr )
             {   if( *TokenPtr )
                 {   CommandError(MsgStrs(StrCTerm));
                 } else CommandError(MsgStrs(StrCLong));
                 return( 0 );
             } else TokenPtr++;

             TokenIdent[TokenLength]='\0';
             return( CurToken = StringTok );
         } else if( ispunct(ch) )
             return( CurToken = ch );

         ch = *TokenPtr++;
    }
    TokenPtr--;
    return 0;
}


static int NextIf( int tok, int err )
{
    if( FetchToken() != tok )
    {   CommandError(MsgStrs(err));
        return True;
    } else return False;
}


static void FetchFloat( Long value, int scale )
{
    register int count;
    register int mant;

    if( !value && !isdigit(*TokenPtr) )
    {   CommandError(MsgStrs(StrFNum));
        TokenValue = 0;
        return;
    }

    mant = 0;
    count = 1;
    while( isdigit(*TokenPtr) )
    {   if( count < scale )
        {   mant = 10*mant + (*TokenPtr-'0');
            count *= 10;
        }
        TokenPtr++;
    }

    mant = (scale*mant)/count;
    TokenValue = value*scale + mant;
}


static int ParseColour( void )
{
    register RGBStruct *rgb;

    if( IsColourToken(CurToken) )
    {   rgb = ColourTable + Token2Colour(CurToken);
        RVal = rgb->red;
        GVal = rgb->grn;
        BVal = rgb->blu;
        return True;

    } else if( CurToken == '[' )
    {   RVal = GVal = BVal = 0;

        if( NextIf(NumberTok,ErrNotNum) ) 
        {   return False;
        } else if( TokenValue>255 )
        {   CommandError(MsgStrs(ErrBigNum)); 
            return False;
        } else RVal = (int)TokenValue;

        if( NextIf(',',ErrNotSep) ) 
            return False;

        if( NextIf(NumberTok,ErrNotNum) ) 
        {   return False;
        } else if( TokenValue>255 )
        {   CommandError(MsgStrs(ErrBigNum)); 
            return False;
        } else GVal = (int)TokenValue;

        if( NextIf(',',ErrNotSep) ) 
            return False;

        if( NextIf(NumberTok,ErrNotNum) ) 
        {   return False;
        } else if( TokenValue>255 )
        {   CommandError(MsgStrs(ErrBigNum));
            return False;
        } else BVal = (int)TokenValue;

        return !NextIf(']',ErrNotBrac);

    } else if( !CurToken && (*TokenPtr=='#') )
    {   RVal = 0;
        GVal = 0;
        BVal = 0;
    
    } else if( CurToken == IdentTok )
        if( Interactive )
            return LookUpColour(TokenIdent,&RVal,&GVal,&BVal);

    return False;
}


static Expr *ParseRange( int neg )
{
    register Expr *tmp1,*tmp2;
    register char ch;

    tmp1 = AllocateNode();
    tmp1->type = OpLftProp|OpRgtVal;
    tmp1->rgt.val = neg? -(int)TokenValue : (int)TokenValue;
    tmp1->lft.val = PropResId;

    if( *TokenPtr == '-' )
    {   TokenPtr++;
        neg = (*TokenPtr=='-');
        if( neg ) TokenPtr++;
        FetchToken();

        if( CurToken != NumberTok )
        {   CommandError(MsgStrs(ErrNotNum));
            DeAllocateExpr( tmp1 );
            return( (Expr*)NULL );
        }

        tmp1->type |= OpMoreEq;
        tmp2 = AllocateNode();
        tmp2->rgt.ptr = tmp1;
        tmp2->type = OpAnd;

        tmp1 = AllocateNode();
        tmp1->type = OpLftProp|OpRgtVal|OpLessEq;
        tmp1->rgt.val = neg? -(int)TokenValue : (int)TokenValue;
        tmp1->lft.val = PropResId;
        tmp2->lft.ptr = tmp1;
        tmp1 = tmp2;
    } else tmp1->type |= OpEqual;

    if( *TokenPtr == ':' )
        TokenPtr++;

    ch = *TokenPtr;
    if( isalnum(ch) )
    {   ch = ToUpper(ch);
        TokenPtr++;

        tmp2 = AllocateNode();
        tmp2->type = OpAnd;
        tmp2->rgt.ptr = tmp1;

        tmp1 = AllocateNode();
        tmp1->type = OpEqual | OpLftProp | OpRgtVal;
        tmp1->lft.val = PropChain;               
        tmp1->rgt.val = ch;

        tmp2->lft.ptr = tmp1;
        tmp1 = tmp2;
    } else if( (ch=='?') || (ch=='%') || (ch=='*') )
        TokenPtr++;

    FetchToken();
    return tmp1;
}


static Expr *ParseExpression( int level )
{
    register Expr *tmp1,*tmp2;
    register int done, pred = 0;
    register int neg;

    switch( level )
    {    case(0): /* Disjunctions */
                  tmp1 = ParseExpression(1);
                  while( (CurToken==OrTok) || (CurToken=='|') ||
                         (CurToken==',') )
                  {   if( CurToken=='|' )
                      {   if( FetchToken()=='|' )
                              FetchToken();
                      } else FetchToken();

                      tmp2 = AllocateNode();
                      tmp2->type = OpOr;
                      tmp2->lft.ptr = tmp1;
                      tmp2->rgt.ptr = NULL;
                      if( !(tmp1=ParseExpression(1)) )
                      {   DeAllocateExpr(tmp2);
                          return( tmp1 );
                      }
                      tmp2->rgt.ptr = tmp1;
                      tmp1 = tmp2;
                  }
                  return( tmp1 );

         case(1): /* Conjunctions */
                  tmp1 = ParseExpression(2);
                  while( (CurToken==AndTok) || (CurToken==YTok) || (CurToken=='&') )
                  {   if( CurToken=='&' )
                      {   if( FetchToken()=='&' )
                              FetchToken();
                      } else FetchToken();

                      tmp2 = AllocateNode();
                      tmp2->type = OpAnd;
                      tmp2->lft.ptr = tmp1;
                      tmp2->rgt.ptr = NULL;
                      if( !(tmp1=ParseExpression(2)) )
                      {   DeAllocateExpr(tmp2);
                          return( tmp1 );
                      }
                      tmp2->rgt.ptr = tmp1;
                      tmp1 = tmp2;
                  }
                  return( tmp1 );

         case(2): /* Primitives */
                  if( IsPredTok(CurToken) || (CurToken==BackboneTok) )
                  {   switch( CurToken )
                      {   case(HelixTok):    if( Info.helixcount < 0 )
                                                 DetermineStructure(False);
                                             pred = PredHelix;
                                             break;
                          case(SheetTok):    if( Info.laddercount < 0 )
                                                 DetermineStructure(False);
                                             pred = PredSheet;
                                             break;
                          case(TurnTok):     if( Info.turncount < 0 )
                                                 DetermineStructure(False);
                                             pred = PredTurn;
                                             break;
                          case(CystineTok):  if( Info.ssbondcount < 0 )
                                                 FindDisulphideBridges();
                                             pred = PredCystine;     
                                             break;
                          case(CisBondedTok):if( Info.cisbondcount<0 )
                                                FindCisBonds();
                                             pred = PredCisBond;   
                                             break;             
                          case(BackboneTok): pred = PredMainChain;   
                                             break;
                          case(SelectedTok): pred = PropSelect;
                                             break;
                          default:  pred = PredAbsChr(PredTokOrd(CurToken));
                      }

                      tmp1 = AllocateNode();
                      tmp1->type = OpConst|OpLftProp|OpRgtVal;
                      tmp1->lft.val = pred;
                      FetchToken();
                      return( tmp1 );

                  } else if( IsPropTok(CurToken) )
                  {   tmp1 = AllocateNode();
                      tmp1->type = OpLftProp|OpRgtVal;
                      switch( CurToken )
                      {   case(TemperatureTok): pred = PropTemp;    break;
                          case(RadiusTok):      pred = PropRad;     break;
                          case(AtomNoTok):      pred = PropIdent;   break;
                          case(ElemNoTok):      pred = PropElemNo;  break;
                          case(ResNoTok):       pred = PropResId;   break;
                          case(ModelTok):       pred = PropModel;   break;
                          case(AltlTok):        pred = PropAltl;    break;
                      }
                      tmp1->lft.val = pred;

                      FetchToken();
                      if( CurToken=='=' )
                      {   tmp1->type |= OpEqual;
                          if( FetchToken()=='=' )
                              FetchToken();
                      } else if( CurToken=='<' )
                      {   FetchToken();
                          if( CurToken=='>' )
                          {   tmp1->type |= OpNotEq;
                              FetchToken();
                          } else if( CurToken=='=' )
                          {   tmp1->type |= OpLessEq;
                              FetchToken();
                          } else tmp1->type |= OpLess;
                      } else if( CurToken=='>' )
                      {   if( FetchToken()=='=' )
                          {   tmp1->type |= OpMoreEq;
                              FetchToken();
                          } else tmp1->type |= OpMore;
                      } else if( (CurToken=='!') || (CurToken=='/') )
                      {   if( NextIf('=',ErrBadExpr) )
                          {   DeAllocateExpr( tmp1 );
                              return( (Expr*)NULL );
                          } else tmp1->type |= OpNotEq;
                          FetchToken();
                      } else
                      {   CommandError(MsgStrs(ErrBadExpr));
                          DeAllocateExpr( tmp1 );
                          return( (Expr*)NULL );
                      }


                      if( CurToken == '-' )
                      {   FetchToken();
                          neg = True;
                      } else neg = False;

                      if( CurToken!=NumberTok )
                      {   CommandError(MsgStrs(ErrNotNum));
                          DeAllocateExpr( tmp1 );
                          return( (Expr*)NULL );
                      } 

                      if( neg )
                      {     tmp1->rgt.val = -(int)TokenValue; 
                      } else tmp1->rgt.val = (int)TokenValue;
                      FetchToken();
                      return( tmp1 );
                      
                  } else switch( CurToken )
                  {   case('('):    FetchToken();
                                    if( !(tmp1=ParseExpression(0)) )
                                        return( (Expr*)NULL );

                                    if( CurToken!=')' )
                                    {   CommandError(MsgStrs(ErrParen));
                                        DeAllocateExpr( tmp1 );
                                        return( (Expr*)NULL );
                                    }
                                    FetchToken();
                                    return(tmp1);

                      case('!'): case('~'):
                      case(NotTok): FetchToken();
                                    if( !(tmp1=ParseExpression(2)) )
                                        return( (Expr*)NULL );

                                    tmp2 = AllocateNode();
                                    tmp2->type = OpNot | OpRgtVal;
                                    tmp2->lft.ptr = tmp1;
                                    return( tmp2 );

                      case('-'):    if( NextIf(NumberTok,ErrNotNum) )
                                        return( (Expr*)NULL );
                                    return( ParseRange(True) );

                      case(NumberTok):
                                    return( ParseRange(False) );

                      case(WithinTok):
                                    if( NextIf('(',ErrFunc) )
                                        return( (Expr*)NULL );

                                    FetchToken();
                                    if( CurToken==NumberTok )
                                    {   if( *TokenPtr=='.' )
                                        {   TokenPtr++;
                                            FetchFloat(TokenValue,250);
                                        }
                                    } else if( CurToken!='.' )
                                    {   CommandError(MsgStrs(ErrNotNum));
                                        return( (Expr*)NULL );
                                    } else FetchFloat(0,250);

                                    if( TokenValue>10000 )
                                    {   CommandError(MsgStrs(ErrBigNum));
                                        return( (Expr*)NULL );
                                    } else pred = (int)TokenValue;
                                    if( NextIf(',',ErrNotSep) )
                                        return( (Expr*)NULL );

                                    FetchToken();
                                    if( !(tmp1=ParseExpression(0)) )
                                        return( (Expr*)NULL );

                                    if( CurToken!=')' )
                                    {   CommandError(MsgStrs(ErrParen));
                                        DeAllocateExpr( tmp1 );
                                        return( (Expr*)NULL );
                                    }

                                    FetchToken();
                                    if( !pred )
                                        return( tmp1 );

                                    tmp2 = AllocateNode();
                                    tmp2->type = OpWithin;
                                    tmp2->lft.limit = (Long)pred*pred;
                                    tmp2->rgt.set = BuildAtomSet(tmp1);
                                    DeAllocateExpr(tmp1);
                                    return( tmp2 );

                      default:      if( CurToken==IdentTok )
                                    {   tmp1 = LookUpSetExpr(TokenIdent);
                                        if( !tmp1 ) 
                                            tmp1 = LookUpElement(TokenIdent);

                                        if( tmp1 )
                                        {   FetchToken();
                                            return(tmp1);
                                        }
                                    }

                                    TokenPtr = TokenStart;
                                    done = ParsePrimitiveExpr(&TokenPtr);
                                    FetchToken();

                                    if( !done )
                                    {   CommandError(MsgStrs(ErrBadExpr));
                                        DeAllocateExpr( QueryExpr );
                                        return( (Expr*)NULL );
                                    } else return( QueryExpr );
                  }
    }
    return (Expr*)NULL;
}



/*======================================*/
/*  RasMol Command Parsing & Execution  */
/*  Commands listed alphabetically      */
/*======================================*/

static void ExecuteAxesCommand( void )
{    register int wflag;

    FetchToken();
    if( CurToken==WorldTok )
    {   if( ZapToken ) return;
        wflag = 1;
        FetchToken();
    } else wflag = 0;
    if( !CurToken || (CurToken==FalseTok) )
    {   ReDrawFlag |= RFRefresh;
        if( wflag )
            DrawWorldAxes = False;
        else
            DrawAxes = False;
    } else if( CurToken == TrueTok )
    {   ReDrawFlag |= RFRefresh;
        if( wflag )
            DrawWorldAxes = True;
        else
            DrawAxes = True;
    } else
    {   CommandError(MsgStrs(ErrBadOpt));
        return;
    }
    if( !BoxCol )
    {   ReDrawFlag |= RFColour;
        ColourBoxAttrib(255,255,255);
    }
}


static void ExecuteBoundBoxCommand( void )
{
    FetchToken();
    if( !CurToken || (CurToken==FalseTok) )
    {   ReDrawFlag |= RFRefresh;
        DrawBoundBox = False;
    } else if( CurToken == TrueTok )
    {   ReDrawFlag |= RFRefresh;
        DrawBoundBox = True;
        if( !BoxCol )
        {   ReDrawFlag |= RFColour;
            ColourBoxAttrib(255,255,255);
        }
    } else CommandError(MsgStrs(ErrBadOpt));
}


static void ExecuteUnitCellCommand( void )
{
    FetchToken();
    if( !CurToken || (CurToken==FalseTok) )
    {   ReDrawFlag |= RFRefresh;
        DrawUnitCell = False;
    } else if( CurToken == TrueTok )
    {   ReDrawFlag |= RFRefresh;
        DrawUnitCell = True;
        if( !BoxCol )
        {   ReDrawFlag |= RFColour;
            ColourBoxAttrib(255,255,255);
        }
    } else CommandError(MsgStrs(ErrBadOpt));
}


static void ExecuteCentreCommand( void )
{
    register double x, y, z;
    register Long count;
    int xlatecen;

    if( ZapToken ) return;

    /* Default */
    xlatecen = True;

    FetchToken();

    /* 'centre' or 'centre all' or centre [0,0,0] */
    /* Translate molecule to center of world      */
    if( !CurToken || (CurToken==AllTok) )
    {   CentreTransform(CurMolSet,0,0,0,True);
        return;

    } else if( CurToken==OriginTok )
    {   FetchToken();
    
        /* 'centre origin'             */
        /* Move Centre to World Origin */
        if( !CurToken )
        {   CentreTransform(NULL,0,0,0,False);

        /* 'centre origin translate'   */
        /* Move Centre to World Origin */
        /* & Translate Molecules with  */
        } else if( CurToken==TranslateTok )
        {   MolSet __far *mcur, *mptr;
            double tx,ty,tz;

            /* Already there */
            if( !CenterSet ) return;

            if( RotMode==RotAll )
            {   mcur = CurMolSet;
                SwitchMolecule(CenterSet);
                tx = MOffset[0] + RCenx - Wo[0];
                ty = MOffset[1] + RCeny - Wo[1];
                tz = MOffset[2] + RCenz - Wo[2];
                mptr = MoleculeList;
                while( mptr )
                {   SwitchMolecule(mptr);
                    DialValue[DialTX] = -tx/Range + LastTX;
                    DialValue[DialTY] = -ty/Range + LastTY;
                    DialValue[DialTZ] = -tz/Range + LastTZ;;
                    ReDrawFlag |= RFTrans;
                    PrepareMoleculeTranslate();
                    mptr = mptr->next;
                }
                SwitchMolecule(mcur);
            } else
            {   mcur = CurMolSet;
                SwitchMolecule(CenterSet);
                tx = MOffset[0] + RCenx - Wo[0];
                ty = MOffset[1] + RCeny - Wo[1];
                tz = MOffset[2] + RCenz - Wo[2];
                SwitchMolecule(mcur);
                DialValue[DialTX] = -tx/Range + LastTX;
                DialValue[DialTY] = -ty/Range + LastTY;
                DialValue[DialTZ] = -tz/Range + LastTZ;;
                ReDrawFlag |= RFTrans;
                PrepareMoleculeTranslate();
            }

        /* 'centre origin centre'      */
        /* Move World Origin to Centre */
        } else if( CurToken==CentreTok )
        {   MolSet __far *mptr;

            /* Already there */
            if( !CenterSet ) return;

            mptr = CurMolSet;
            SwitchMolecule(CenterSet);
            Wo[0] = MOffset[0] + RCenx;
            Wo[1] = MOffset[1] + RCeny;
            Wo[2] = MOffset[2] + RCenz;
            SwitchMolecule(mptr);
            ReDrawFlag |= RFRefresh;
        } else
            CommandError(MsgStrs(ErrSyntax));

        return;
    
    /* 'centre world'                 */
    /* Move World to Center of Screen */
    } else if( CurToken==WorldTok )
    {   WTransValue[0] = -Wo[0]/Range + WLastTX;
        WTransValue[1] = -Wo[1]/Range + WLastTY;
        WTransValue[2] = -Wo[2]/Range + WLastTZ;;
        ReDrawFlag |= RFTrans;
        PrepareWorldTranslate();
        return;

    /* 'centre molecule' or 'centre [0,0,0] translate' */
    /* Translate Center to Centre of Molecule          */
    } else if( CurToken==MoleculeTok )
    {   CentreTransform(CurMolSet,0,0,0,False);
        return;

    /* 'centre [x,y,z]' and 'centre [x,y,z] translate' */
    /* New center with or without translation          */
    } else if ( CurToken == '[' )
    {   Long CenV[3];
        int icen, negcen;
        double rc;

        for (icen = 0; icen < 3; icen++)
        {   FetchToken();
            CenV[icen] = 0;
            if( CurToken == '-' )
            {   FetchToken();
                negcen = True;
            } else negcen = False;
        
            if( CurToken == NumberTok )
            {   if (negcen )
                {   CenV[icen] = -TokenValue;
                } else CenV[icen] = TokenValue;
                FetchToken();
            } 
            if( !(CurToken == ',' && icen < 2) && 
                !(CurToken == ']' && icen == 2 ))
            {   CommandError(MsgStrs(ErrSyntax));
                return;
            }
        }

#ifdef INVERT
        CenV[1] = -CenV[1];
#endif

        rc = (double)(CenV[0])*(double)(CenV[0]);
        rc += (double)(CenV[1])*(double)(CenV[1]);
        rc += (double)(CenV[2])*(double)(CenV[2]);
        rc = sqrt(rc);

        if( rc>=(double)LocalRadius )
        {   CommandError(MsgStrs(ErrBigNum)); 
            return;
        }

        FetchToken();
        if( CurToken )
        {   if( !CurToken || (CurToken==CentreTok) || (CurToken==TranslateTok) )
            {   xlatecen = (CurToken==TranslateTok)?False:True;
            } else
            {   CommandError(MsgStrs(ErrSyntax));
                return;
            }
        }

        CentreTransform(CurMolSet,CenV[0],CenV[1],-CenV[2],xlatecen);
        return;
    }
 
    /* 'centre expression' and 'centre expression translate' */
    /* New Center with or without Translation                */
    QueryExpr = ParseExpression(0);
    if( !QueryExpr ) return;

    if( CurToken && (CurToken!=TranslateTok) && (CurToken!=CentreTok) )
    {   CommandError(MsgStrs(ErrSyntax));
        DeAllocateExpr(QueryExpr);
        return;
    }

    /* CentreZoneExpr(QueryExpr); */
    if( !Database ) return;

    count = 0;
    x = y = z = 0.0;
    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
        for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
            for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                if( EvaluateExpr(QueryExpr) )
                {   x += (double)(QAtom->xorg+QAtom->fxorg);
                    y += (double)(QAtom->yorg+QAtom->fyorg);
                    z += (double)(QAtom->zorg+QAtom->fzorg);
                    count++;
                }

    if( count )
    {   if( CurToken )
        {   if (  (CurToken==CentreTok) || (CurToken==TranslateTok) )
            {   xlatecen = (CurToken==TranslateTok)?True:False;
            } else
            {   CommandError(MsgStrs(ErrSyntax));
                return;
            }
        }
        CentreTransform(CurMolSet,
           (Long)(x/count),(Long)(y/count),(Long)(z/count),xlatecen);
    } else
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrCent));
    }
    DeAllocateExpr(QueryExpr);
}


static void ExecuteClipboardCommand( void )
{
    register int done;

    if( !RefreshState )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrNoClip));
        return;
    }

    done = 0;

    /*paste command shorcut*/
    if( CurToken==PasteTok )
    {    done = ClipboardPaste();
    } else
    {   FetchToken();

        if( !CurToken || CurToken==ImageTok )
        {    done = ClipboardImage();
        } else if( CurToken==PositionTok )
        {    done = ClipboardPosition();
        } else if( CurToken==SelectedTok )
        {    done = ClipboardSelection();
        } else if( CurToken==PasteTok )
        {    done = ClipboardPaste();
        } else if( CurToken == NumberTok )
        {    /* Image enlargement factor during clipboard operations */
            if( ClipBoardFactor != TokenValue )
                PrefsModified = True;
            ClipBoardFactor = TokenValue;
            if( ClipBoardFactor<1 )
                ClipBoardFactor = 1;
            if( ClipBoardFactor> MAXCLIPBOARDFACTOR )
                ClipBoardFactor = MAXCLIPBOARDFACTOR;
            return;
        }
    }

    if( !done )
    {   InvalidateCmndLine();
        if( CurToken==PasteTok )
            WriteString(MsgStrs(StrPClip));
        else
            WriteString(MsgStrs(StrCClip));
    }
}


static void ExecuteLoadCommand( void )
{
    register int format;
    register int info;
    register FILE *fp;
    char msg[80];

    FetchToken();
    format = FormatPDB;
    if( !*TokenPtr || *TokenPtr==' ' )
    {   if( IsMoleculeToken(CurToken) )
        {   format = Tok2Format(CurToken);
            FetchToken();
        } else if( CurToken == DotsTok )
        {   format = FormatDots;
            FetchToken();
        }
    }

    if( !CurToken )
    {   CommandError(MsgStrs(ErrFilNam));
        return;
    }

/*
#ifdef STRICT
    if( (CurToken!=StringTok) && (CurToken!=IdentTok) )
    {   CommandError(MsgStrs[ErrFilNam]);
        return;
    }
#endif
 */
    info = (FileDepth == -1);

    if( IsMoleculeFormat(format) )
    {   if( CurToken==InLineTok )
        {   if( (FileDepth!=-1) && LineStack[FileDepth] )
            {   DivertToData( format, info );
            } else CommandError(MsgStrs(ErrOutScrpt));
        } else if( CurToken==StringTok )
        {   ScriptFileFormat = DataFormat;
            if( !FetchFile(format,info,TokenIdent) )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(StrFailLoad));
            }
        } else 
        {   ScriptFileFormat = DataFormat;
            if( !FetchFile(format,info,TokenStart) )
            {   InvalidateCmndLine();
                WriteString(MsgStrs(StrFailLoad));
            }
        }
        DefaultRepresentation();
    } else /* format == FormatDots */
    {   if( !Database )
        {   CommandError(MsgStrs(ErrBadMolDB));
            return;
        }

        if( CurToken==StringTok )
        {      ProcessFileName(TokenIdent);
        } else ProcessFileName(TokenStart);

        if( !(fp=fopen(DataFileName,"rb")) )
        {   CommandError( (char*)NULL );
            FormatMsg(msg,StrDFile,DataFileName);
            InvalidateCmndLine();
            WriteString(msg);
            return;
        } else
        {   LoadDotsFile(fp,info);
            fclose(fp);
        }
        LoadDotsFile(fp,info);
    }
    CurToken = 0;
}


static void ExecutePauseCommand( void )
{
    if( FileDepth==-1 )
    {   CommandError(MsgStrs(ErrOutScrpt));
        return;
    }

    /* Ignore Pause Commands via IPC! */
    if( LineStack[FileDepth] )
    {   CommandActive = True;
        IsPaused = True;

#if defined(MSWIN) && !defined RASTOPWIN
        /* Disable Drag & Drop! */
        DragAcceptFiles(CanvWin,FALSE);
#endif
    }

    FastDraw = False;
    ReDrawFlag |= RFRefresh;
}


static void ExecutePickingCommand( void )
{
    switch( FetchToken() )
    {   case(TrueTok):     case(0):
        case(IdentifyTok): SetPickMode(PickIdent); break;
        case(FalseTok):
        case(NoneTok):     SetPickMode(PickNone);  break;
        case(LabelTok):    SetPickMode(PickLabel); break;
        case(DistanceTok): SetPickMode(PickDist);  break;
        case(AngleTok):    SetPickMode(PickAngle); break;
        case(TorsionTok):  SetPickMode(PickTorsn); break;
        case(MonitorTok):  SetPickMode(PickMonit); break;
        case(CentreTok):   SetPickMode(PickCentr); break;
        case(OriginTok):   SetPickMode(PickOrign); break;
        case(CoordTok):    SetPickMode(PickCoord); break;
        case(BondTok):     SetPickMode(PickBond);  break;
        case(AtomTok):     SetPickMode(PickAtom);  break;
        case(GroupTok):    SetPickMode(PickGroup); break;
        case(ChainTok):    SetPickMode(PickChain); break;
        case(MoleculeTok): SetPickMode(PickMolSet);break;
        case(RotateTok):   SetPickMode(PickRBond); break;
        case(LightSourceTok): SetPickMode(PickLight); break;
        default:           CommandError(MsgStrs(ErrBadOpt));
    }
}


static void ExecutePrintCommand( void )
{
    if( !RefreshState )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrNoPrint));
        return;
    }
    if( !PrintImage() )
    {   InvalidateCmndLine();
        WriteString(MsgStrs(StrNPrint));
    }
}


static void ExecuteTitleCommand( void )
{
    if( !CurMolSet ) return;
    FetchToken();
    if( CurToken == StringTok )
    {   strcpy(CurMolSet->title,TokenIdent);
        SetTitle();
    } else if( CurToken )
    {    strcpy(CurMolSet->title,TokenStart);
        SetTitle();
    }
    CurToken = 0;
    FileModified = True;
}


static void ExecuteLightSourceCommand ( void )
{
    static int lix, liy, liz;
    static int sign, result;

    FetchToken();
    if( CurToken == DefaultTok )
    {    ChangeLight( 1, 0, 0, 0 );
    } else if( CurToken == '[' )
    {    result = FetchToken();
        if( CurToken == '-' )
        {   result = FetchToken();
            sign = True;
        } else sign = False;
        if( result != NumberTok ) 
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        } else if( TokenValue>100)
        {   CommandError(MsgStrs(ErrBigNum)); 
            return;
        } else
        {    lix = (int)TokenValue;
            if( sign ) lix = -lix;
        }
        
        if( NextIf(',',ErrNotSep) ) 
        {    CommandError(MsgStrs(ErrNotSep));
            return;
        }
        
        result = FetchToken();
        if( CurToken == '-' )
        {   result = FetchToken();
            sign = True;
        } else sign = False;
        if( result != NumberTok ) 
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        } else if( TokenValue>100 )
        {   CommandError(MsgStrs(ErrBigNum)); 
            return;
        } else
        {    liy = (int)TokenValue;
            if( sign ) liy = -liy;
        }
        
        if( NextIf(',',ErrNotSep) ) 
        {    CommandError(MsgStrs(ErrNotSep));
            return;
        }
        
        result = FetchToken();
        if( CurToken == '-' )
        {   result = FetchToken();
            sign = True;
        } else sign = False;
        if( result != NumberTok ) 
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        } else if( TokenValue>100 )
        {   CommandError(MsgStrs(ErrBigNum));
            return;
        } else
        {    liz = (int)TokenValue;
            if( sign ) liz = -liz;
        }
        
        if( NextIf(']',ErrNotBrac) )
        {   CommandError(MsgStrs(ErrNotBrac));
            return;
        }

        if( lix==0 && liy==0 && liz==0 )
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrUnLight));
            ChangeLight( 1, 0, 0, 0 );
        } else
            ChangeLight( 0, lix, liy, -liz );
    }

    ReDrawFlag |= RFColour;
}


/*=======================================*/
/*  Generic Command Parsing & Execution  */
/*=======================================*/

static void ExecuteSetCommand( void )
{
    register int option;
    char buffer[80];

    switch( FetchToken() )
    {   case(SlabTok):
        case(SlabModeTok):
            if( ZapToken ) return;
            option = -1;
            FetchToken();
            if( CurToken==RejectTok )
            {   option = SlabReject;
            } else if( CurToken==HalfTok )
            {   option = SlabHalf;
            } else if( CurToken==HollowTok )
            {   option = SlabHollow;
            } else if( CurToken==SolidTok )
            {   option = SlabClose;
            } else if( CurToken==SectionTok )
                option = SlabSection;

            if( option != -1 )
            {   if( UseSlabPlane && (SlabMode!=option) )
                    ReDrawFlag |= RFSlab;
                SlabMode = option;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(ShadowTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken==TrueTok )
            {   UseShadow = True;
                ReviseInvMatrix();
                VoxelsClean = False;
                UseSlabPlane = False;
                UseDepthPlane = False;
                ReDrawFlag |= RFSlab;
                ReAllocBuffers();
            } else if( CurToken==FalseTok )
            {   ReDrawFlag |= RFRefresh;
                UseShadow = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;
                                  
        case(SpecularTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken==TrueTok )
            {   FakeSpecular = True;
                ReDrawFlag |= RFColour;
                SetDepthTable();
            } else if( CurToken==FalseTok )
            {   FakeSpecular = False;
                ReDrawFlag |= RFColour;
                SetDepthTable();
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(SpecPowerTok):
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken )
            {   SpecPower = 8;
                SetDepthTable();
                ReDrawFlag |= RFColour;
            } else if( CurToken==NumberTok )
            {   if( TokenValue<=100 )
                {   ReDrawFlag |= RFColour;
                    SpecPower = (int)TokenValue;
                    SetDepthTable();
                } else 
                    CommandError(MsgStrs(ErrBigNum));
            } else CommandError(MsgStrs(ErrNotNum));
            break;

        case(ShadePowerTok):
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken )
            {   ShadePower = 0;
                ReDrawFlag |= RFColour;
            } else if( CurToken==NumberTok )
            {   if( TokenValue<=100 )
                {   ReDrawFlag |= RFColour;
                    ShadePower = TokenValue;
                } else 
                    CommandError(MsgStrs(ErrBigNum));
            } else CommandError(MsgStrs(ErrNotNum));
            break;

        case(AmbientTok):
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken )
            {   ReDrawFlag |= RFColour;
                Ambient = DefaultAmbient;
            } else if( CurToken==NumberTok )
            {   if( TokenValue<=100 )
                {   Ambient = TokenValue/100.0;
                    ReDrawFlag |= RFColour;
                } else
                    CommandError(MsgStrs(ErrBigNum)); 
            } else CommandError(MsgStrs(ErrNotNum));
            break;

        case(HeteroTok):
            FetchToken();
            if( CurToken==TrueTok )
            {   HetaGroups = True;
            } else if( CurToken==FalseTok )
            {   HetaGroups = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;
                                  
        case(HydrogenTok):
            FetchToken();
            if( CurToken==TrueTok )
            {   Hydrogens = True;
            } else if( CurToken==FalseTok )
            {   Hydrogens = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;
                                  

        case(BackgroundTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken == TransparentTok )
            {   UseTransparent = True;
            } else if( CurToken == NormalTok )
            {   UseTransparent = False;
            } else if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                BackR = RVal;
                BackG = GVal;
                BackB = BVal;
#ifndef IBMPC
                FBClear = False;
#endif
            } else if( CurToken )
            {   CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(LightSourceTok):    
            if( ZapToken ) return;
            ExecuteLightSourceCommand();
            break;

        case(BondModeTok):
            FetchToken();
            if( !CurToken || (CurToken==AndTok) )
            {   ZoneBoth = True;
            } else if( CurToken==OrTok )
            {   ZoneBoth = False;
            } else if( CurToken==AllTok ) 
            {   MarkAtoms = AllAtomFlag;
            } else if( CurToken==NoneTok)
            {    MarkAtoms = 0;
            } else if( CurToken==NotTok )
            {   FetchToken();
                if( !CurToken || (CurToken==BondedTok) )
                {    MarkAtoms = NonBondFlag;
                } else CommandError(MsgStrs(ErrBadOpt));
            } else CommandError(MsgStrs(ErrBadOpt));
            break;
            
        case(HBondTok):
            FetchToken();
            if( (CurToken==BackboneTok) || (CurToken==MainChainTok) )
            {   ReDrawFlag |= RFRefresh;
                HBondMode = True;
            } else if( !CurToken || (CurToken==SidechainTok) )
            {   ReDrawFlag |= RFRefresh;
                HBondMode = False;
            } else if( CurToken == ChainTok )
            {   FetchToken();
                if( !CurToken || (CurToken==TrueTok) )
                {   if( !HBondChainsFlag && (Info.hbondcount>=0) )
                    {   ReDrawFlag |= RFRefresh;
                        HBondChainsFlag = True;
                        CalcHydrogenBonds();
                    }
                } else if( CurToken == FalseTok )
                {   if( HBondChainsFlag && (Info.hbondcount>=0) )
                    {   ReDrawFlag |= RFRefresh;
                        HBondChainsFlag = False;
                        CalcHydrogenBonds();
                    }
                } else CommandError(MsgStrs(ErrBadOpt));
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(SSBondTok):
            FetchToken();
            if( (CurToken==BackboneTok) || (CurToken==MainChainTok) )
            {   ReDrawFlag |= RFRefresh;
                SSBondMode = True;
            } else if( !CurToken || (CurToken==SidechainTok) )
            {   ReDrawFlag |= RFRefresh;
                SSBondMode = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(HourGlassTok):
            FetchToken();
            if( CurToken==TrueTok )
            {   UseHourGlass = True;
            } else if( CurToken==FalseTok )
            {   UseHourGlass = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(StrandsTok):
            FetchToken();
            if( !CurToken )
            {   ReDrawFlag |= RFRefresh;
                SplineCount = 5;
            } else if( CurToken==NumberTok )
            {   if( (TokenValue>0) && (TokenValue<=5) )
                {   SplineCount = (int)TokenValue;
                    ReDrawFlag |= RFRefresh;
                } else if( TokenValue==9 )
                {   ReDrawFlag |= RFRefresh;
                    SplineCount = 9;
                } else CommandError(MsgStrs(ErrBadOpt));
            } else CommandError(MsgStrs(ErrNotNum));
            break;

        case(MouseTok):
            FetchToken();
            if( !CurToken || (CurToken==RasMolTok) )
            {   if( MouseMode!=MMRasMol )
                    PrefsModified = True;
                if( Interactive )
                    SetMouseMode( MMRasMol );
            } else if( CurToken==InsightTok )
            {   if( MouseMode!=MMInsight )
                    PrefsModified = True;
                if( Interactive )
                    SetMouseMode( MMInsight );
            } else if( CurToken==QuantaTok )
            {   if( MouseMode!=MMQuanta )
                    PrefsModified = True;
                if( Interactive )
                    SetMouseMode( MMQuanta );
            } else if( CurToken==SybylTok )
            {   if( MouseMode!=MMSybyl )
                    PrefsModified = True;
                if( Interactive )
                    SetMouseMode( MMSybyl );
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(DisplayTok):
            FetchToken();
            if( !CurToken || (CurToken==NormalTok) )
            {   ReDrawFlag |= RFRefresh | RFColour;
                DisplayMode = 0;
                if( !FileModified )
                {   RefreshView();
                    FileModified = False;
                }
            } else if( CurToken==SelectedTok )
            {   ReDrawFlag |= RFRefresh | RFColour;
                DisplayMode = 1;
                if( !FileModified )
                {   RefreshView();
                    FileModified = False;
                }
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(VectPSTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   UseOutLine = False;
            } else if( CurToken == TrueTok )
            {   UseOutLine = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(KinemageTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   KinemageFlag = False;
            } else if( CurToken == TrueTok )
            {   KinemageFlag = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(MenusTok):
            FetchToken();
            if( !CurToken || (CurToken==TrueTok) )
            {   EnableMenus(True);
            } else if( CurToken == FalseTok )
            {   EnableMenus(False);
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(RadiusTok):
            FetchToken();
            if( !CurToken )
            {   ProbeRadius = SolventDots? 300 : 0;
            } else if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }

                if( TokenValue>750 )
                {   CommandError(MsgStrs(ErrBigNum));
                } else ProbeRadius = (int)TokenValue;
            } else if( CurToken=='.' )
            {   FetchFloat(0,250);
                if( TokenValue>750 )
                {   CommandError(MsgStrs(ErrBigNum));
                } else ProbeRadius = (int)TokenValue;

            } else CommandError(MsgStrs(ErrNotNum));
            break;

        case(SolventTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   SolventDots = False;
                ProbeRadius = 0;
            } else if( CurToken == TrueTok )
            {   SolventDots = True;
                ProbeRadius = 300;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(FontSizeTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken==NumberTok )
            {   if( TokenValue<=48 )
                {   int fsize;

                    fsize = (int)TokenValue;
                    FetchToken();
                    if ( !CurToken || CurToken==FSTok )
                    { SetFontSize(fsize);
                    } else if ( CurToken ==PSTok ) 
                    { SetFontSize(-fsize);
                    } else CommandError(MsgStrs(ErrBadOpt));
                } else CommandError(MsgStrs(ErrBigNum));
            } else if( !CurToken )
            {   SetFontSize(8);
            } else if (CurToken == FSTok)
            {   SetFontSize(abs(FontSize));
            } else if (CurToken == PSTok)
        {   SetFontSize(-abs(FontSize));
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(FontStrokeTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken==NumberTok )
            {   if( TokenValue<=8 )
                {   if( LabelList || (MonitList && DrawMonitDistance) )
                        ReDrawFlag |= RFRefresh;
                    SetFontStroke((int)TokenValue);
                } else CommandError(MsgStrs(ErrBigNum));
            } else if( !CurToken )
            {   if( LabelList )
                    ReDrawFlag |= RFRefresh;
                SetFontStroke(0);
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(WriteTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   AllowWrite = False;
            } else if( CurToken == TrueTok )
            {   if( (FileDepth!=-1) && LineStack[FileDepth] )
                {   CommandError(MsgStrs(ErrInScrpt));
                } else AllowWrite = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(StereoTok):  
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken )
            {   if (UseStereo) {
                   StereoAngle = -StereoAngle;
                   if (StereoAngle  < 0.0 ) {
                     SetStereoMode(True);
                   } else {
                     SetStereoMode(False);
                   }
                } else SetStereoMode(True);
            } else if( CurToken==TrueTok )
            {   SetStereoMode(True);
            } else if( CurToken==FalseTok )
            {   SetStereoMode(False);
            } else if( CurToken == '-' )
            {   if( !NextIf(NumberTok,ErrNotNum) )
                {   StereoAngle = -TokenValue;
                    SetStereoMode(True);
                    CurToken = 0;
                }
            } else if( CurToken == '+' )
            {   if( !NextIf(NumberTok,ErrNotNum) )
                {   StereoAngle = TokenValue;
                    SetStereoMode(True);
                    CurToken = 0;
                }
            } else if( CurToken==NumberTok )
            {   StereoAngle = TokenValue;
                SetStereoMode(True);
            } else CommandError(MsgStrs(ErrSyntax));
            break;

        case(BondTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   ReDrawFlag |= RFRefresh;
                DrawDoubleBonds = False;
            } else if( CurToken == TrueTok )
            {   ReDrawFlag |= RFRefresh;
                DrawDoubleBonds = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(MonitorTok):
            FetchToken();
            if( !CurToken || (CurToken==TrueTok) )
            {   ReDrawFlag |= RFRefresh;
                DrawMonitDistance = True;
            } else if( CurToken == FalseTok )
            {   ReDrawFlag |= RFRefresh;
                DrawMonitDistance = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(CartoonTok):
            FetchToken();
            if( !CurToken )
            {   ReDrawFlag |= RFRefresh;
                DrawBetaArrows = True;
                CartoonHeight = 120;
            } else if( CurToken==TrueTok )
            {   ReDrawFlag |= RFRefresh;
                DrawBetaArrows = True;
            } else if( CurToken==FalseTok )
            {   ReDrawFlag |= RFRefresh;
                DrawBetaArrows = False;
            } else if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }

                if( TokenValue <= 500 )
                {   CartoonHeight = (int)TokenValue;
                    ReDrawFlag |= RFRefresh;
                } else CommandError(MsgStrs(ErrBigNum));
            } else if( CurToken=='.' )
            {   FetchFloat(0,250);
                if( TokenValue <= 500 )
                {   CartoonHeight = (int)TokenValue;
                    ReDrawFlag |= RFRefresh;
                } else CommandError(MsgStrs(ErrBigNum));

            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(BackFadeTok):
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   ReDrawFlag |= RFColour;
                UseBackFade = False;
            } else if( CurToken == TrueTok )
            {   ReDrawFlag |= RFColour;
                UseBackFade = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(TransparentTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   UseTransparent = False;
            } else if( CurToken == TrueTok )
            {   UseTransparent = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(DepthCueTok):
            if( ZapToken ) return;
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   ReDrawFlag |= RFSlab;
                UseDepthCue = False;
                SetDepthTable();
            } else if( CurToken == TrueTok )
            {   ReDrawFlag |= RFSlab;
                UseDepthCue = True;
                SetDepthTable();
                InvalidateCmndLine();
            } else if( CurToken==NumberTok )
            {   if( (TokenValue>0) && (TokenValue<=4))
                {   if( DepthCueMode != TokenValue )
                    {   DepthCueMode = TokenValue;
                        PrefsModified = True;
                        SetDepthTable();
                        if( UseDepthCue )
                            ReDrawFlag |= RFRefresh;
                    }
                } else
                    CommandError(MsgStrs(ErrBadOpt));
            } else
                CommandError(MsgStrs(ErrBadOpt));
            break;

        case(CisAngleTok):
           FetchToken();
           if( !CurToken )
           {   CisBondCutOff = CIS;         
           } else if( CurToken==NumberTok )
           {   if( TokenValue<=180 )
               {   CisBondCutOff = TokenValue;
                   Info.cisbondcount = -1; /* to recalculate peptide bonds */
               } else
                   CommandError(MsgStrs(ErrBigNum)); 
           } else 
               CommandError(MsgStrs(ErrNotNum));
           sprintf(buffer,"CisBondCutOff = %d\n", CisBondCutOff);
           InvalidateCmndLine();
           WriteString( buffer );
           break;    

        case(SequenceTok):
            FetchToken();
            if( !CurToken || (CurToken==FalseTok) )
            {   if( SeqFormat!=False )
                    PrefsModified = True;
                SeqFormat = False;
            } else if( CurToken == TrueTok )
            {   if( SeqFormat!=True )
                    PrefsModified = True;
                SeqFormat = True;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(ConnectTok):
            FetchToken();
            if( !CurToken || (CurToken==TrueTok) )
            {   CalcBondsFlag = True;
            } else if( CurToken == FalseTok )
            {   CalcBondsFlag = False;
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(AxesTok):     ExecuteAxesCommand();     break;
        case(BoundBoxTok): ExecuteBoundBoxCommand(); break;
        case(PickingTok):  ExecutePickingCommand();  break;
        case(TitleTok):    ExecuteTitleCommand();    break;
        case(UnitCellTok): ExecuteUnitCellCommand(); break;

        case(DotsTok):
            FetchToken();
            if( !CurToken )
            {   SurfaceChainsFlag = False;
                SolventDots = False;
                ProbeRadius = 0;
            } else if( CurToken == SolventTok )
            {   FetchToken();
                if( !CurToken || (CurToken==FalseTok) )
                {   SolventDots = False;
                    ProbeRadius = 0;
                } else if( CurToken == TrueTok )
                {   SolventDots = True;
                    ProbeRadius = 300;
                } else CommandError(MsgStrs(ErrBadOpt));
            } else if( CurToken == ChainTok )
            {   FetchToken();
                if( !CurToken || (CurToken==TrueTok) )
                {   SurfaceChainsFlag = True;
                } else if( CurToken == FalseTok )
                {   SurfaceChainsFlag = False;
                } else CommandError(MsgStrs(ErrBadOpt));
            } else if( CurToken == NumberTok )
            {   if( TokenValue<=20 )
                {   if( TokenValue )
                    {   DotSize = TokenValue;
                        ReDrawFlag |= RFRefresh;
                    }
                } else CommandError(MsgStrs(ErrBigNum));
            } else CommandError(MsgStrs(ErrBadOpt));
            break;

        case(CoordTok):
            FetchToken();
            if( CurToken==MoleculeTok )
            {   if( SysCoor != MolCoor )
                    PrefsModified = True;
                SysCoor = MolCoor;
            } else if( CurToken==WorldTok )
            {   if( SysCoor != WorldCoor )
                    PrefsModified = True;
                SysCoor = WorldCoor;
            } else if( CurToken==ScreenTok )
            {   if( SysCoor != ScreenCoor )
                    PrefsModified = True;
                SysCoor = ScreenCoor;
            } else if( CurToken==AbsoluteTok )
            {   if( SysCoor != AbsCoor )
                    PrefsModified = True;
                SysCoor = AbsCoor;
            } else CommandError(MsgStrs(ErrSyntax));
            break;

        case(WorldDepthTok):
            if( ZapToken ) return;
            FetchToken();
            if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }
                if( (TokenValue<=250000) && (TokenValue>=1500) )
                {   SetWorldSize(TokenValue);
                } else CommandError(MsgStrs(ErrParam));
            } else CommandError(MsgStrs(ErrSyntax));
            break;

        case(LangTok):
            FetchToken();
            if( CurToken==IdentTok)
            {   SwitchLang(TokenIdent);
            } else if (CurToken==DefaultTok) {
                ExitLangSel();
                InitialiseLangSel();
            } else
                CommandError(MsgStrs(ErrBadArg));
            break;

        default:
            CommandError(MsgStrs(ErrParam));
    }
}


static void ExecuteColourCommand( void )
{
    register int flag;

    flag = 0;
    switch( FetchToken() )
    {   case(AtomTok):
            FetchToken();
        default:
            switch( CurToken )
            {   case(CPKTok):         CPKColourAttrib(); 
                                      ReDrawFlag |= RFColour; break;

                case(AminoTok):       AminoColourAttrib();
                                      ReDrawFlag |= RFColour; break;

                case(ShapelyTok):     ShapelyColourAttrib();
                                      ReDrawFlag |= RFColour; break;
                
                case(UserTok):        UserMaskAttrib(MaskColourFlag);
                                      ReDrawFlag |= RFColour; break;

                case(GroupTok):       ScaleColourAttrib(GroupAttr);
                                      ReDrawFlag |= RFColour; break;

                case(ChainTok):       ScaleColourAttrib(ChainAttr);
                                      ReDrawFlag |= RFColour; break;

                case(ModelTok):       ScaleColourAttrib(ModelAttr);
                                      ReDrawFlag |= RFColour; break;

                case(AltlTok):        ScaleColourAttrib(AltAttr);
                                      ReDrawFlag |= RFColour; break;

                case(ChargeTok):      ScaleColourAttrib(ChargeAttr);
                                      ReDrawFlag |= RFColour; break;

                case(TemperatureTok): ScaleColourAttrib(TempAttr);
                                      ReDrawFlag |= RFColour; break;

                case(StructureTok):   StructColourAttrib();
                                      ReDrawFlag |= RFColour; break;

                default:  if( ParseColour() )
                          {   MonoColourAttrib(RVal,GVal,BVal);
                              ReDrawFlag |= RFColour;
                          } else if( CurToken )
                          {      CommandError(MsgStrs(ErrColour));
                          } else CommandError(MsgStrs(ErrNoCol));
            }
            break;

        case(BondTok):    
        case(DashTok):
            FetchToken();
            if( CurToken==NoneTok )
            {   ColourBondNone();
                ReDrawFlag |= RFColour;
            } else if( ParseColour() )
            {   ColourBondAttrib(RVal,GVal,BVal);
                ReDrawFlag |= RFColour;
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(BackboneTok):
            FetchToken();
            if( CurToken==NoneTok )
            {   ColourBackNone();
                ReDrawFlag |= RFColour;
            } else if( ParseColour() )
            {   ColourBackAttrib(RVal,GVal,BVal);
                ReDrawFlag |= RFColour;
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(SSBondTok):
            FetchToken();
            if( CurToken==NoneTok )
            {   ReDrawFlag |= RFColour;
                ColourHBondNone( False );
            } else if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                ColourHBondAttrib(False,RVal,GVal,BVal);
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(HBondTok):
            FetchToken();
            if( CurToken==NoneTok )
            {   ReDrawFlag |= RFColour;
                ColourHBondNone( True );
            } else if( CurToken==TypeTok )
            {   ReDrawFlag |= RFColour;
                ColourHBondType();
            } else if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                ColourHBondAttrib(True,RVal,GVal,BVal);
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(DotsTok):
            FetchToken();
            if( CurToken==PotentialTok )
            {   ReDrawFlag |= RFColour;
                ColourDotsPotential();
            } else if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                ColourDotsAttrib(RVal,GVal,BVal);
            } else if( CurToken == NoneTok )
            {   ReDrawFlag |= RFColour;
                DotR = DotG = DotB = 0;
                UseDotCol = DotColNone;        
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(MonitorTok):
            FetchToken();
            if( CurToken == NoneTok )
            {   ColourMonitNone();
            } else if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                ColourMonitAttrib(RVal,GVal,BVal);
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(AxesTok):
        case(BoundBoxTok):
        case(UnitCellTok):
            FetchToken();
            if( ParseColour() )
            {   ReDrawFlag |= RFColour;
                ColourBoxAttrib(RVal,GVal,BVal);
            } else if( CurToken )
            {   CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(WorldTok):
            FetchToken();
            if( CurToken==AxesTok )
            {   FetchToken();
                if( ParseColour() )
                {   ReDrawFlag |= RFColour;
                    AxeR = RVal; AxeG = GVal; AxeB = BVal;
                } else if( CurToken )
                {   CommandError(MsgStrs(ErrColour));
                } else CommandError(MsgStrs(ErrNoCol));
            } else CommandError(MsgStrs(ErrSyntax));
            break;

        case(LabelTok):
            FetchToken();
            if( CurToken==NoneTok )
            {   ReDrawFlag |= RFColour;
                UseLabelCol = False;
            } else if( ParseColour() )
            {   LabR = RVal;  LabG = GVal;  LabB = BVal;
                ReDrawFlag |= RFColour;
                UseLabelCol = True;
            } else if( CurToken )
            {      CommandError(MsgStrs(ErrColour));
            } else CommandError(MsgStrs(ErrNoCol));
            break;

        case(TraceTok): 
        case(RibbonTok):
        case(StrandsTok):
        case(CartoonTok):  flag = RibColBoth;     break;
        case(Ribbon1Tok):  flag = RibColInside;   break;
        case(Ribbon2Tok):  flag = RibColOutside;  break;
    }

    if( flag )
    {   FetchToken();
        if( CurToken==NoneTok )
        {   ReDrawFlag |= RFColour;
            ColourRibbonNone(flag);
        } else if( ParseColour() )
        {   ReDrawFlag |= RFColour;
            ColourRibbonAttrib(flag,RVal,GVal,BVal);
        } else if( CurToken )
        {      CommandError(MsgStrs(ErrColour));
        } else CommandError(MsgStrs(ErrNoCol));
    }
}

/* prints out information about all selected objects to terminal */
/* example: Describe(ATM) could print output in terms of:        */
/*          Chain: ?  Group:  ???  ??  Atom:  ???  ????          */
static void DescribeSelected( Selection type )
{

  register Chain __far *chain = (Chain __far*)NULL;
  register Group __far *group = (Group __far*)NULL;
  register RAtom __far *ptr    = (RAtom  __far*)NULL;
  AtomRef current;
  int touched    ;
  int Aselect    ;
  int Acount     ;
  int Gselect    ;
  int Gcount     ;
  int model      ;
  char buffer[80];
  char value[20];

  if(!Database)
    return;
  
  model = -1;
 
  for(chain=Database->clist;chain;chain=chain->cnext){
    Gselect =  Gcount = 0;
    touched = False;
    current.chn = chain;
    for(group=chain->glist;group;group=group->gnext){
      Aselect =  Acount = 0;
      current.grp = group;
      for(ptr=group->alist;ptr;ptr=ptr->anext) {
        current.atm = ptr;
        if( !(ptr->flag&HeteroFlag) || type != CHN) { 
          if( ptr->flag&SelectFlag ) {
            Aselect++;
            touched = True;
            if( type == ATM || type == CRD){        /* Atom or Coordinates */
            WriteString(DescribeObj(&current, type));
            WriteChar('\n');
          }
        }
        Acount++; 
      }           
    } 
    
    if( Acount == Aselect )
      Gselect++;
    Gcount++;       
    if( Aselect && (type == GRP || type == CHN)) { 
      if (!(model == group->model)) {
        model = group->model;
        if (model) {
          sprintf(value,"%d",model);
          FormatMsg(buffer,StrObjModel,value);
          WriteString(buffer); 
        }
      }
    }
    if( Aselect && type == GRP) {                  /* Group */
      WriteString(DescribeObj(&current, GRP));
      sprintf(value,"%d",Aselect); 
      FormatMsg(buffer,StrObjGroup,value);
      sprintf(value,"%d",Acount); 
      FormatMsg(buffer,0,value);
      WriteString(buffer);
    }
  }

  if( touched && type == CHN ) {                    /* Chain */      
      WriteString(DescribeObj(&current, CHN));
      if( Gselect > 0 ) {
          sprintf(value,"%d",Gselect); 
          FormatMsg(buffer,StrObjChain,value);
          sprintf(value,"%d",Gcount); 
          FormatMsg(buffer,0,value);
          WriteString(buffer);
      }
      else
        WriteString(MsgStrs(StrNoGroupSel));    
    }
  } 
}  

/* Selection for printing selected atoms || groups || chains to terminal  */
static void ExecuteSelectedCommand()
{
    WriteString(MsgStrs(StrCurSel));
    WriteString("\n");

    switch( FetchToken() )
    {
        case(AtomTok):
            DescribeSelected(ATM);
            break;
        case(CoordTok):
            DescribeSelected(CRD);
            break;
        case(GroupTok):
            DescribeSelected(GRP);
            break;
        case(ChainTok):
            DescribeSelected(CHN);
            break;
        case(0):
            DescribeSelected(GRP);  /* default option is 'group' */
            break;
        default:
            CommandError(MsgStrs(ErrBadArg));
    }
}


static void DescribeSequence( void )
{
    register Chain __far *chn;
    register Group __far *grp;
    register int chain,count;
    register int subcount;
    register char *str;
    char buffer[80];
    char value[20];
    int  model;

    if( !Database )
        return;

    WriteString(MsgStrs(StrDscbSeq));
    WriteString("\n");

    model = -1;
    for( chn=Database->clist; chn; chn=chn->cnext )
    {   chain = (Info.chaincount<2);
        count = 0;
        subcount = -1;
        for( grp=chn->glist; grp; grp=grp->gnext )
            if( grp->alist && !(grp->alist->flag&HeteroFlag) )
            {   if( !chain )
                {   if (!(model==grp->model)) 
                    {   model = grp->model;
                        if (model) 
                        {   sprintf(value,"%d",model);
                            FormatMsg(buffer,StrSeqModel,value);
                            WriteString(buffer);
                        }
                    }
                    value[0] = chn->ident;
                    value[1] = '\0';
                    FormatMsg(buffer,StrSeqChain,value);
                    WriteString(buffer);
                    chain = True;
                }

                if( !SeqFormat )
                {   if( count == 10 )
                    {   WriteChar('\n');
                        count = 1;
                    } else count++;

                    str = Residue[grp->refno];
                    WriteChar(str[0]);
                    WriteChar(str[1]);
                    WriteChar(str[2]);

                    sprintf(buffer,"%-3d ",grp->serno);
                    WriteString(buffer);
                } else
                {   if( count == 50 )
                    {   WriteChar('\n');
                        count = 1;
                        subcount = 0;
                    } else 
                    {    count++;
                        subcount++;
                    }

                    if( subcount == 10)
                    {    WriteChar(' ');
                        subcount = 0;
                    }

                    if( grp->refno < 29 )
                    {   WriteChar(ResidueChar[grp->refno]);
                    } else WriteChar('*');
                }
            }
        WriteChar('\n');
    }
    WriteChar('\n');
}

static void DescribeSymmetry( void )
{    register double temp;
    char buffer[40];

    WriteString(MsgStrs(StrDesSym));
    WriteString("\n");

    if( *Info.spacegroup )
    {   sprintf(buffer,"%s ...... %s\n",MsgStrs(StrSGroup),Info.spacegroup);
        WriteString(buffer);

        sprintf(buffer,"%s A ...... %g\n",MsgStrs(StrUCell),Info.cella);
        WriteString(buffer);
        sprintf(buffer,"%s B ...... %g\n",MsgStrs(StrUCell),Info.cellb);
        WriteString(buffer);
        sprintf(buffer,"%s C ...... %g\n",MsgStrs(StrUCell),Info.cellc);
        WriteString(buffer);

        temp = Rad2Deg*Info.cellalpha;
        sprintf(buffer,"%s alpha .. %g\n",MsgStrs(StrUCell),temp);
        WriteString(buffer);
        temp = Rad2Deg*Info.cellbeta;
        sprintf(buffer,"%s beta ... %g\n",MsgStrs(StrUCell),temp);
        WriteString(buffer);
        temp = Rad2Deg*Info.cellgamma;
        sprintf(buffer,"%s gamma .. %g\n",MsgStrs(StrUCell),temp);
        WriteString(buffer);

    } else WriteString(MsgStrs(StrSymm));
    WriteChar('\n');
}


static void DescribeWorld( void )
{   register MolSet *mptr, *mcur;
    register int i;
    double x,y,z;
    double theta,phi,psi;
    char buffer[80];
    double WRotX[3], WRotY[3], WRotZ[3];

    if( !Database || !MoleculeList )
        return;

    x = 0.0; y = 0.0; z = 0.0;

    WriteString("\n");
    WriteString(MsgStrs(StrWrdlWrdl));
       WriteString("          x         y         z\n");

    /* World position */
    x = Wo[0]/(Scale*250.0);
    y = Wo[1]/(Scale*250.0);
#ifdef INVERT
    y = -y;
#endif
    /* z = -Wo[2]/(Scale*250.0); */
    z = 0.0;
    WriteString(MsgStrs(StrWrdlPos));
    sprintf(buffer,"%-10.3f%-10.3f%-10.3f Angstroms",x,y,z);
    WriteString(buffer);
    WriteString("\n");

    /* World Rotation */
    WRotX[0] = WAxes[0][0]; WRotX[1] = WAxes[0][1]; WRotX[2] = WAxes[0][2]; 
    WRotY[0] = WAxes[1][0]; WRotY[1] = WAxes[1][1]; WRotY[2] = WAxes[1][2]; 
    WRotZ[0] = WAxes[2][0]; WRotZ[1] = WAxes[2][1]; WRotZ[2] = WAxes[2][2]; 
    RMat2RV(&theta, &phi, &psi, WRotX, WRotY, WRotZ);
    theta *= 180.;
    phi *= 180.;
    psi *= 180.;
    WriteString(MsgStrs(StrWrldRot));
    sprintf(buffer,"%-10.1f%-10.1f%-10.1f ",
        InvertY(-theta),phi,InvertY(-psi));
    WriteString(buffer);
    WriteString(MsgStrs(StrWrdlDegr));

    /* Zoom */
    if( DialValue[3]<0.0 )
    {   x = 100.0*DialValue[3];
    } else x = 100.0*MaxZoom*DialValue[3];
    WriteString(MsgStrs(StrWrldZoom));
    sprintf(buffer,"%d\n",(int)(x+100));
    WriteString(buffer);
    WriteString(MsgStrs(StrWrdlScal));
    sprintf(buffer,"%.2f Angstroms\n",Range/(Scale*250));
    WriteString(buffer);

    /* World Depth */
    i = GetWorldSize();
    WriteString(MsgStrs(StrWrdlDDph));
    sprintf(buffer,"%-.3f Angstroms or %d RU\n",
        (double)i/250.0,i);
    WriteString(buffer);
    WriteString(MsgStrs(StrWrdlCDhp));
    sprintf(buffer,"%-.3f Angstroms or %d RU\n",
        (double)WorldSize/250.0,WorldSize);
    WriteString(buffer);

    /* Clip planes */
    if( UseSlabPlane )
    {   x = GetClipCoord(ScreenCoor,CurMolSet,0);
        WriteString(MsgStrs(StrWrldSlb));
        sprintf(buffer,"%-10.3f\n",x);
        WriteString(buffer);
    }
    if( UseDepthPlane )
    {   y = GetClipCoord(ScreenCoor,CurMolSet,1);
        WriteString(MsgStrs(StrWrldDpth));
        sprintf(buffer,"Depth:    %-10.3f\n",y);
        WriteString(buffer);
    }
    WriteString("\n");

    WriteString(MsgStrs(StrWrldMol));
       WriteString(MsgStrs(StrWrldInd));
    i = 0;
    mptr = MoleculeList;
    while( mptr )
    {   i++;
        sprintf(buffer,"%-10d%s",i,mptr->title);
        WriteString(buffer);
        if( mptr->copynum>1 )
        {   WriteString(" #");
            _itoa(mptr->copynum,buffer,10);
            WriteString(buffer);
        }
        WriteString("\n");
        mptr = mptr->next;
    }
    if( i>1 )
        sprintf(buffer,"%d",i);
    else
        sprintf(buffer,"%d",i);
    WriteString(buffer);
    WriteString(MsgStrs(StrWrldMol2));
    WriteString("\n");

    WriteString(MsgStrs(StrWrldAsbPar));
    WriteString("\n");

       /* Absolute positions */
    WriteString(MsgStrs(StrWrldPos2));
       WriteString(MsgStrs(StrWrldXYZ));
    i = 1;
    mptr = MoleculeList;
    mcur = CurMolSet;
    while( mptr )
    {   SwitchMolecule(mptr);
        sprintf(buffer,"%-10d",i);
        WriteString(buffer);
        GetCurrentCoord(AbsCoor,mptr,NULL,&x,&y,&z);
        sprintf(buffer,"%-10.3f%-10.3f%-10.3f",x,y,z);
        WriteString(buffer);
        WriteString("\n");
        mptr = mptr->next;
        i++;
    }
    SwitchMolecule(mcur);
    WriteString("\n");

    /* Absolute rotations */
    phi = psi = theta = 0.0;
    
    WriteString(MsgStrs(StrWrldRot2));
    WriteString(MsgStrs(StrWrldInd2));
    i = 1;
    mptr = MoleculeList;
    mcur = CurMolSet;
    while( mptr )
    {   SwitchMolecule(mptr);
        sprintf(buffer,"%-10d",i);
        WriteString(buffer);
        RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
        theta *= 180.;
        phi *= 180.;
        psi *= 180.;
        sprintf(buffer,"%-10.1f%-10.1f%-10.1f",InvertY(-theta),phi,InvertY(-psi));
        WriteString(buffer);
        WriteString("\n");
        mptr = mptr->next;
        i++;
    }
    SwitchMolecule(mcur);
    WriteString("\n");

    WriteString(MsgStrs(StrWrldSPar));
    WriteString("\n");

    /* Screen positions */
    WriteString(MsgStrs(StrWrldPos2));
    WriteString(MsgStrs(StrWrldXYZ));
    i = 1;
    mptr = MoleculeList;
    mcur = CurMolSet;
    while( mptr )
    {   SwitchMolecule(mptr);
        sprintf(buffer,"%-10d",i);
        WriteString(buffer);
        GetCurrentCoord(ScreenCoor,mptr,NULL,&x,&y,&z);
        sprintf(buffer,"%-10.3f%-10.3f%-10.3f",x,y,z);
        WriteString(buffer);
        WriteString("\n");
        mptr = mptr->next;
        i++;
    }
    SwitchMolecule(mcur);
    WriteString("\n");


    WriteString(MsgStrs(StrWrldWPar));
    WriteString("\n");

    /* Reset World */
    ResetWorldAxes(True);
    StorePrevSelection();

    /* World positions */
    WriteString(MsgStrs(StrWrldPos2));
       WriteString(MsgStrs(StrWrldXYZ));
    i = 1;
    mptr = MoleculeList;
    mcur = CurMolSet;
    while( mptr )
    {   SwitchMolecule(mptr);
        sprintf(buffer,"%-10d",i);
        WriteString(buffer);
        GetCurrentCoord(WorldCoor,mptr,NULL,&x,&y,&z);
        sprintf(buffer,"%-10.3f%-10.3f%-10.3f",x,y,z);
        WriteString(buffer);
        WriteString("\n");
        mptr = mptr->next;
        i++;
    }
    SwitchMolecule(mcur);
    WriteString("\n");

    /* World rotations */
    phi = psi = theta = 0.0;
    
    WriteString(MsgStrs(StrWrldRot2));
    WriteString(MsgStrs(StrWrldInd2));
    i = 1;
    mptr = MoleculeList;
    mcur = CurMolSet;
    while( mptr )
    {   SwitchMolecule(mptr);
        sprintf(buffer,"%-10d",i);
        WriteString(buffer);
        RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
        theta *= 180.;
        phi *= 180.;
        psi *= 180.;
        sprintf(buffer,"%-10.1f%-10.1f%-10.1f",InvertY(-theta),phi,InvertY(-psi));
        WriteString(buffer);
        WriteString("\n");
        mptr = mptr->next;
        i++;
    }
    WriteString("\n");
    SwitchMolecule(mcur);

    /* Set World Back */
    SetWorldBack();
    SwitchMolecule(mcur);
    SelectPrevSelection(False);
}


static void ExecuteShowCommand( void )
{    
    char buffer[80];
    char value[20];
    register double temp;
    double theta,phi,psi,x,y,z;
    register int i;
    register MolSet __far *mptr, *mcur;

    if (!MoleculeList)
        return;
    InvalidateCmndLine();


    /* Send info on current molecule */
    if( MoleculeList && MoleculeList->next )
        WriteString(MsgStrs(StrMolSel));
    else
        WriteString(MsgStrs(StrShowMol));
    
    WriteString(CurMolSet->title);
    if( CurMolSet->copynum>1 )
    {   WriteString(" #");
        _itoa(CurMolSet->copynum,buffer,10);
        WriteString(buffer);
    }
    WriteString("\n");
  
    switch( FetchToken() )
    {   case(InfoTok):
                if( MoleculeList && !MoleculeList->next )
                    DescribeMolecule();
                else 
                {   mcur = CurMolSet;
                    i = 1;
                    for( mptr=MoleculeList; mptr; mptr=mptr->next )
                    {   WriteString("#");
                        _itoa(i,buffer,10);
                        WriteString(buffer);
                        WriteString(" ");
                        WriteString(mptr->title);
                        if( mptr->copynum>1 )
                        {   WriteString(" #");
                            _itoa(mptr->copynum,buffer,10);
                            WriteString(buffer);
                        }
                        WriteString("\n");
                        SwitchMolecule(mptr);
                        DescribeMolecule();
                        WriteString("\n");
                        i++;
                    }
                    SwitchMolecule(mcur);    
                }
                break;

        case(SequenceTok):
                DescribeSequence();
                break;

        case(SelectedTok):
                ExecuteSelectedCommand();
                break;   

        case(PhiPsiTok):
                WritePhiPsiAngles(NULL, False);  /* Writing to stderr/stdout */
                break;             
        
        case(RamPrintTok):
                WritePhiPsiAngles(NULL, -1);     /* Writing to stderr/stdout */
                break;             
        
        case(SymmetryTok):
                DescribeSymmetry();
                break;

        case(CentreTok):
                if( !CenterSet )
                {   WriteString(MsgStrs(StrCenWOri));
                } else if( CenterSet == CurMolSet )
                {   if( CenX || CenY || CenZ )
                    {   WriteString(MsgStrs(StrCenCen));
#ifdef INVERT
                        sprintf(buffer,"[%ld,%ld,%ld]\n", CenX, -CenY, -CenZ);
#else
                        sprintf(buffer,"[%ld,%ld,%ld]\n", CenX, CenY, -CenZ);
#endif
                        WriteString(buffer);
                    } else
                        WriteString(MsgStrs(StrCenMOri));
                } else
                {   mcur = CurMolSet;
                    SwitchMolecule(CenterSet);
                    WriteString(CenterSet->title);
                    if( CenterSet->copynum>1 )
                    {   WriteString(" #");
                        _itoa(CenterSet->copynum,buffer,10);
                        WriteString(buffer);
                    }
                    WriteString(": ");
                    if( CenX || CenY || CenZ )
                    {   WriteString(MsgStrs(StrCenCen));
#ifdef INVERT
                        sprintf(buffer,"[%ld,%ld,%ld]\n", CenX, -CenY, -CenZ);
#else
                        sprintf(buffer,"[%ld,%ld,%ld]\n", CenX, CenY, -CenZ);
#endif
                        WriteString(buffer);
                        SwitchMolecule(mcur);
                    } else
                        WriteString(MsgStrs(StrCenMOri));
                }
                break;

        case(RotateTok):

                phi = psi = theta = 0.0;
                RMat2RV(&theta, &phi, &psi, RotX, RotY, RotZ);
                theta *= 180.;
                phi *= 180.;
                psi *= 180.;

                WriteString(MsgStrs(StrAbsVal));

                if( Rint(theta) ) {
                    sprintf(value,"%d",Rint(InvertY(-theta)));
                    FormatMsg(buffer,StrRotateX,value);
                    WriteString(buffer);
                }
                if( Rint(phi) ) {
                    sprintf(value,"%d",Rint(phi));
                    FormatMsg(buffer,StrRotateY,value);
                    WriteString(buffer);
                }
                if( Rint(psi) ) {
                    sprintf(value,"%d",Rint(InvertY(-psi)));
                    FormatMsg(buffer,StrRotateZ,value);
                    WriteString(buffer);
                }

                /* Bond Rotation */
                if (RotBondList) 
                {    BondRot __far *brptr;

                     brptr = RotBondList;
                     while (brptr) 
                     {   sprintf(buffer,"bond %ld %ld pick\n", 
                            (brptr->BSrcAtom)->serno, (brptr->BDstAtom)->serno);
                         WriteString(buffer);
                         if( brptr->BRotValue ) 
                         {   sprintf(value,"%d",Rint((brptr->BRotValue)*180.));
                             FormatMsg(buffer,StrRotBond,value);
                             WriteString(buffer);
                         }
                         brptr = brptr->brnext;
                    }
                }
                break;

        case(TranslateTok):

                WriteString(MsgStrs(StrScreenVal));
                GetCurrentCoord(ScreenCoor,CurMolSet,NULL,&x,&y,&z);
                if( x )
                {   sprintf(value,"%.2f",x);
                    FormatMsg(buffer,StrTransX,value);
                    WriteString(buffer);
                }
                if( y ) 
                {   sprintf(value,"%.2f",InvertY(-y));
                    FormatMsg(buffer,StrTransY,value);
                    WriteString(buffer);
                }
                if( z )
                {   sprintf(value,"%.2f",z);
                    FormatMsg(buffer,StrTransZ,value);
                    WriteString(buffer);
                }
                break;

        case(ZoomTok):
                if( DialValue[DialZoom] != 0.0 )
                {   if( DialValue[DialZoom]<0.0 )
                    {   temp = 100.0*DialValue[DialZoom];
                    } else temp = 100.0*MaxZoom*DialValue[DialZoom];
                    sprintf(value,"%d",(int)(temp+100));
                    FormatMsg(buffer,StrZoom,value);
                    WriteString(buffer);
                 }
                 break;

        case(WorldTok):
                 DescribeWorld();
                 break;

        case(WorldDepthTok):
                 i = GetWorldSize();
                 sprintf(value,"%-.3f",(double)i/250.0);
                 FormatMsg(buffer,StrWDepthDef,value);
                 sprintf(value,"%d",i);
                 FormatMsg(buffer,0,value);
                 WriteString(buffer);

                 sprintf(value,"%-.3f",(double)WorldSize/250.0);
                 FormatMsg(buffer,StrWDepthCur,value);
                 sprintf(value,"%d",WorldSize);
                 FormatMsg(buffer,0,value);
                 WriteString(buffer);
                 break;
        default:
            WriteString(MsgStrs(StrWrongShow));
    }

}


/*Function call on "select <...<" , start and continue selection,
*                  "select <...>" , start and stop selection,
*               or "select >...>" , continue and stop selection.
* Escape the parser to make fast atom selection under the format:
* "select (< or >) x1[-x2],...,xi[-x(i+1)](> or <)" where xi represent atomno.
*/ 
static void ReadAtomSelection( int start )
{    register Long ori=0, end=0;
    register int neg, bloc=0;
    register char ch;
    register Bond __far *bptr;
    
    if( !Database )
        return;
            
    /*Empty selection at start*/
    if( start )
    {    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
            for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
                for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                    QAtom->flag &= ~SelectFlag;
        SelectCount = 0;
    }
    
    while( *TokenPtr )
    {    bloc = 0;
        neg = 0;
        ch = *TokenPtr++;

        /*first number*/
        while( *TokenPtr && isspace(ch) )
            ch= *TokenPtr++;
        if( ch == '-' )
            neg = 1;
        else if( ch != '+' )
            *TokenPtr--;
        FetchToken();
        if( CurToken==NumberTok )
        {    ori = TokenValue;
            ch= *TokenPtr++;
            while( *TokenPtr && isspace(ch) )
                ch = *TokenPtr++;
            
            /*second number*/
            if( ch=='-' )
            {    neg = 0;
                ch= *TokenPtr++;
                while( *TokenPtr && isspace(ch) )
                    ch = *TokenPtr++;
                if( ch == '-' )
                    neg = 1;
                else if( ch != '+' )
                    *TokenPtr--;
                FetchToken();
                if( CurToken==NumberTok )
                {    end = TokenValue;
                    if( neg )
                        end = -end;
                    ch = *TokenPtr++;
                    while( *TokenPtr && isspace(ch) )
                        ch = *TokenPtr++;
                     if( ch==','||ch=='>'||ch=='<' )
                        bloc = 1;
                }
            } else if( ch==','||ch=='>'||ch=='<' )
            {    end = ori;
                bloc = 1;
            }
        }

        if( bloc==1 )
        {    for( QChain=Database->clist; QChain; QChain=QChain->cnext )
                for( QGroup=QChain->glist; QGroup; QGroup=QGroup->gnext )
                    for( QAtom=QGroup->alist; QAtom; QAtom=QAtom->anext )
                        if( QAtom->serno >= ori && QAtom->serno <= end )
                        {    if( !(QAtom->flag&SelectFlag) )
                                SelectCount++;
                            QAtom->flag |= SelectFlag;
                        }
        } else
        {    if( ch!=',' && ch!='<' && ch!='>' )        /*not empty bloc*/
                CommandError(MsgStrs(ErrBlocSel));
            while( *TokenPtr && ch!=',' && ch!='<' && ch!='>' )
                ch = *TokenPtr++;
        }

        if( ch==',' )
            continue;
        else if( ch=='<' )    
            bloc = 0;
        else
            bloc = 1;        
        while( *TokenPtr )
                *TokenPtr++;
    }

    /*termination*/
    if( bloc!=0 )
    {    if( (FileDepth == -1) || !LineStack[FileDepth] )
            DisplaySelectCount( );

        if( ZoneBoth )
        {    ForEachBond
                if( (bptr->srcatom->flag&bptr->dstatom->flag) & SelectFlag )
                {   bptr->flag |= SelectFlag;
                } else bptr->flag &= ~SelectFlag;
        } else
        {    ForEachBond
               if( (bptr->srcatom->flag|bptr->dstatom->flag) & SelectFlag )
               {   bptr->flag |= SelectFlag;
               } else bptr->flag &= ~SelectFlag;
        }
    }
}


static int WriteImageFile( char *name, int type )
{
	register int result;

	result = 0;

    if( !type )
#ifdef EIGHTBIT
        type = GIFTok;
#else
        type = PPMTok;
#endif

    switch( type )
    {   case(GIFTok):	    result = WriteGIFFile(name);                break;
        case(BMPTok):	    result = WriteBMPFile(name);                break;
        case(PPMTok):	    result = WritePPMFile(name,True);           break;
        case(SUNTok):		result = WriteRastFile(name,False);         break;
        case(SUNRLETok):    result = WriteRastFile(name,True);          break;
        case(PICTTok):      result = WritePICTFile(name);               break;
        case(IRISTok):      result = WriteIRISFile(name);               break;
        case(EPSFTok):      result = WriteEPSFFile(name,True,True);     break;
        case(MonoPSTok):    result = WriteEPSFFile(name,False,True);    break;
        case(VectPSTok):    result = WriteVectPSFile(name);             break;

        case(RasMolTok):
        case(ScriptTok):    result = WriteScriptFile(name,False,False); break;
        case(KinemageTok):  result = WriteKinemageFile(name);           break;
        case(MolScriptTok): result = WriteMolScriptFile(name);          break;
        case(POVRayTok):    result = WritePOVRayFile(name);             break;
        case(PhiPsiTok):    result = WritePhiPsiAngles(name,False);     break;
        case(RamachanTok):  result = WritePhiPsiAngles(name, 1);        break;
        case(RamPrintTok):  result = WritePhiPsiAngles(name, -1);       break;  
        case(VRMLTok):      result = WriteVRMLFile(name);               break;
    }
    return result;
}


void ResumePauseCommand( void )
{
    register int ch,len;
    register FILE *fp;
    register int stat;

    CommandActive = False;
    IsPaused = False;

#if defined(MSWIN) && !defined RASTOPWIN
    /* Re-enable Drag & Drop! */
    DragAcceptFiles(CanvWin,TRUE);
#endif

    while( FileDepth >= 0 )
    {   fp = FileStack[FileDepth];
        do {
            len = 0;
            ch = getc(fp);
            while( (ch!='\n') && (ch!=EOF) && (ch!='\r') )
            {   if( len<MAXBUFFLEN )
                    CurLine[len++] = ch;
                ch = getc(fp);
            }

            LineStack[FileDepth]++;
            if( len<MAXBUFFLEN )
            {   CurLine[len] = '\0';
                stat = ExecuteCommand();
                if( stat )
                {   if( stat == QuitTok )
                    {   while( FileDepth >= 0 )
                        {   fclose(FileStack[FileDepth]);
                            free(NameStack[FileDepth]);
                            FileDepth--;
                        }
                        RasMolExit();
                    } else /* ExitTok */
                        break;
                } else if( IsPaused )
                    return;
            } else CommandError(MsgStrs(StrSLong));
        } while( ch!=EOF );
        free(NameStack[FileDepth]);
        fclose( fp );
        FileDepth--;
    }
    InvalidateCmndLine();
    WriteString(MsgStrs(StrScrDone));
}


void InterruptPauseCommand( void )
{
    WriteString(MsgStrs(StrInterScr));
    CommandActive = False;
    IsPaused = False;

#if defined(MSWIN) && !defined RASTOPWIN
    /* Re-enable Drag & Drop! */
    DragAcceptFiles(CanvWin,TRUE);
#endif

    while( FileDepth >= 0 )
    {   fclose(FileStack[FileDepth]);
        free(NameStack[FileDepth]);
        FileDepth--;
    }
}


static void ExecuteConnectCommand( void )
{
    register Bond __far *bptr;
    register int info,flag;

    FetchToken();
    if( !CurToken )
    {   flag = (MainAtomCount+HetaAtomCount<256);
    } else if( CurToken == TrueTok )
    {   flag = True;
    } else if( CurToken == FalseTok )
    {   flag = False;
    } else 
    {   CommandError(MsgStrs(ErrSyntax));
        return;
    }

    if( Database )
    {   ForEachBond
            if( bptr->col )
                Shade[Colour2Shade(bptr->col)].refcount--;
        info = (FileDepth == -1);
        CreateMoleculeBonds(info,flag,True);
        ReDrawFlag |= RFRefresh|RFColour;
        EnableWireframe(WireFlag,0,0);
    }

    CalcBondsFlag = True;
}


static void ResetLight( void )
{   if( (LightZ!=2*LightX) || (LightZ!=2*LightY) )
        FileModified = True;
    ChangeLight( 1, 0, 0, 0 );
}

static void ResetSlab( void )
{   WClip[0] =  -999999.;
    DialValue[DialSlab] = 0.0;
    if( UseSlabPlane == True )
    {   UseSlabPlane = False;
        ReDrawFlag |= RFSlab;
        /* FileModified = True; */
    }
}

static void ResetDepth( void )
{   WClip[1] =  -999999.;
    DialValue[DialBClip] = 0.0;
     if( UseDepthPlane == True )
    {   UseDepthPlane = False;
        ReDrawFlag |= RFSlab;
        /* FileModified = True; */
    }    
}


static void ResetAxes( int token )
{   register int wflag, i;

    if( token == MoleculeTok )
    {   wflag = 0;
    } else if( token == WorldTok )
    {   wflag = 1;
    } else if( !token )
    {   if( RotMode==RotMol )
            wflag = 0;
        else
            wflag = 1;
    } else 
    {   CommandError(MsgStrs(ErrSyntax));
        return;
    }
        
    if( !wflag )
    {   for( i=0; i<3; i++ )
            DialValue[i] = 0.0;
        LastTX = LastTY = LastTZ = 0;
        for( i=4; i<7; i++ )
            DialValue[i] = 0.0;
        LastRX = LastRY = LastRZ =0;
        ReDrawFlag |= RFDials;
        /* RFDials does not modify */

        if( MoleculeList && !MoleculeList->next )
        {   DialValue[DialZoom] = 0.0;
            PrepareWorldZoom(0);
            ResetTransform();
        } else
            ResetMolSet();
        CentreTransform(CurMolSet,0,0,0,False);
    } else {
        ResetWorldAxes(False);
        if( Database )
        {   double newzoom;
            newzoom = Scale/(DScale*2*getWorldRadius());
            if( (newzoom-1.0) <= 0.0 )
            {   DialValue[3] = newzoom - 1.0;
                if( DialValue[3] < -1.0 ) DialValue[3] = -1.0;
            } else
            {   DialValue[3] = (newzoom - 1.0)/MaxZoom;
                if( DialValue[3] > 1.0 ) DialValue[3] = 1.0;
            }
        // transfer centre to world origin
        CentreTransform(NULL,0,0,0,False);
        ReDrawFlag |= RFZoom;
        PrepareWorldZoom(0);
        }
    }
    FileModified = True;
}

static ResetWorldDepth( void )
{   
    SetWorldSize(GetWorldSize());
}


static void ExecuteResetCommand( void )
{
    if( ZapToken ) return;

    FetchToken();

    if( CurToken == AllTok )
    {   ResetLight();
        ResetSlab();
        ResetDepth();
        ResetBonds();
        ResetAxes( WorldTok );
        ResetWorldDepth();
    } else if( CurToken == LightSourceTok )
    {   ResetLight();
    } else if( CurToken == SlabTok )
    {   ResetSlab();
    } else if( CurToken == DepthTok )
    {   ResetDepth();
    } else if( CurToken == BondTok )
    {   ResetBonds();
    } else if( CurToken == MoleculeTok )
    {   ResetAxes( CurToken );
    } else if( CurToken == WorldTok )
    {   ResetAxes( CurToken );
    } else if( CurToken == WorldDepthTok )
    {   ResetWorldDepth();
    } else if( !CurToken )
    {   ResetAxes( CurToken );
    }   
}


static void ExecutePositionCommand( void )
{   register double xpos,ypos,zpos;
    register int done,wflag;

    xpos = 0; ypos = 0; zpos = 0;

    FetchToken();

    if( CurToken==WorldTok )
    {   if( ZapToken ) return;
        wflag = 1;
        FetchToken();
    } else wflag = 0;

    if( CurToken==XTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }
            } else
                FetchFloat(0,250);
            if( done ) TokenValue = -TokenValue;
            xpos = (double)TokenValue;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransX;
    }

    if( CurToken==YTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;
#ifdef INVERT
        done = !done;
#endif

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }
            } else FetchFloat(0,250);
            if( done ) TokenValue = -TokenValue;
            ypos = (double)TokenValue;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransY;
    }

    if( CurToken==ZTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   TokenPtr++;
                    FetchFloat(TokenValue,250);
                }
            } else FetchFloat(0,250);
            if( done ) TokenValue = -TokenValue;
            zpos = (double)TokenValue;
            zpos = -zpos;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransZ;
    }

    if( CurToken )
    {   CommandError(MsgStrs(ErrSyntax));
        return;
    }

    if( wflag )
    {   if( ReDrawFlag&RFTransX )
            WTransValue[0] = (xpos*Scale - Wo[0])/Range + WLastTX;
        if( ReDrawFlag&RFTransY )
            WTransValue[1] = (ypos*Scale - Wo[1])/Range + WLastTY;
        if( ReDrawFlag&RFTransZ )
            WTransValue[2] = (zpos*Scale /*- Wo[2]*/)/Range + WLastTZ;
        PrepareWorldTranslate();
    } else
    {   if( ReDrawFlag&RFTransX )
            DialValue[DialTX] = (xpos*Scale - MOffset[0])/Range + LastTX;
        if( ReDrawFlag&RFTransY )
            DialValue[DialTY] = (ypos*Scale - MOffset[1])/Range + LastTY;
        if( ReDrawFlag&RFTransZ )
            DialValue[DialTZ] = (zpos*Scale - MOffset[2])/Range + LastTZ;
        PrepareMoleculeTranslate();
    }
}

/* Proposal for new translate command */
/* PV: seem error-prone on the user side! */
static void ExecuteTranslateCommand( void )
{   register double dx,dy,dz;
    register int done,wflag, isReal, isInteger;

    dx = 0; dy = 0; dz = 0;
    isReal = 0;
    isInteger = 0;

    FetchToken();

    if( CurToken==WorldTok )
    {   if( ZapToken ) return;
        wflag = 1;
        FetchToken();
        } else 
        {   wflag = 0;
        if( CurToken==MoleculeTok )
        FetchToken();
    }

    if( CurToken==XTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   isReal = 1;
                    TokenPtr++;
                    FetchFloat(TokenValue,250);
                } else {
                    isInteger = 1;
                    TokenValue *= 100;
                }
            } else {
                isReal = 1;
                FetchFloat(0,250);
            }
            if( done ) TokenValue = -TokenValue;
            if(isReal)
                dx = (double)TokenValue*Scale/Range;
            else
                dx = (double)TokenValue/10000.0;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransX;
    }

    if( CurToken==YTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;
#ifdef INVERT
        done = !done;
#endif

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   if(isInteger) {
                        CommandError(MsgStrs(ErrBadOpt));
                        return;
                    }
                    isReal = 1;
                    TokenPtr++;
                    FetchFloat(TokenValue,250);
                } else {
                    if(isReal) {
                        CommandError(MsgStrs(ErrBadOpt));
                        return;
                    }
                    isInteger = 1;
                    TokenValue *= 100;
                }
            } else {
                if(isInteger) {
                    CommandError(MsgStrs(ErrBadOpt));
                    return;
                }
                isReal = 1;
                FetchFloat(0,250);
            }
            if( done ) TokenValue = -TokenValue;
            if(isReal)
                dy = (double)TokenValue*Scale/Range;
            else
                dy = (double)TokenValue/10000.0;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransY;
    }

    if( CurToken==ZTok )
    {   FetchToken();
        if( CurToken == '-' )
        {   FetchToken();
            done = True;
        } else done = False;

        if( (CurToken==NumberTok) || (CurToken=='.') )
        {   if( CurToken==NumberTok )
            {   if( *TokenPtr=='.' )
                {   if(isInteger) {
                        CommandError(MsgStrs(ErrBadOpt));
                        return;
                    }
                    isReal = 1;
                    TokenPtr++;
                    FetchFloat(TokenValue,250);
                } else {
                    if(isReal) {
                        CommandError(MsgStrs(ErrNotNum));
                        return;
                    }
                    TokenValue *= 100;
                }
            } else {
                if(isInteger) {
                    CommandError(MsgStrs(ErrBadOpt));
                    return;
                }
                isReal = 1;
                FetchFloat(0,250);
            }
            if( done ) TokenValue = -TokenValue;
            if(isReal)
                dz = (double)TokenValue*Scale/Range;
            else
                dz = (double)TokenValue/10000.0;
            dz = -dz;    //Z-inversion;
        } else
        {   CommandError(MsgStrs(ErrNotNum));
            return;
        }
        FetchToken();
        ReDrawFlag |= RFTransZ;
    }

    if( CurToken )
    {   CommandError(MsgStrs(ErrSyntax));
        return;
    }

    if( wflag )
    {   if( ReDrawFlag&RFTransX )
            WTransValue[0] = dx - Wo[0]/Range + WLastTX;
        if( ReDrawFlag&RFTransY )
            WTransValue[1] = dy - Wo[1]/Range + WLastTY;
        if( ReDrawFlag&RFTransZ )
            WTransValue[2] = dz /*- Wo[2]/Range*/ + WLastTZ;
        PrepareWorldTranslate();
    } else
    {   if( ReDrawFlag&RFTransX )
            DialValue[DialTX] = dx - MOffset[0]/Range + LastTX;
        if( ReDrawFlag&RFTransY )
            DialValue[DialTY] = dy - MOffset[1]/Range + LastTY;
        if( ReDrawFlag&RFTransZ )
            DialValue[DialTZ] = dz - MOffset[2]/Range + LastTZ;
        PrepareMoleculeTranslate();
    }
}

int ExecuteCommand( void )
{
    register char *param;
    register int option;
    register int done,wflag;
    register Long temp;
    FILE *script;

    TokenPtr = CurLine;
    if( !FetchToken() )
    {   TokenPtr = NULL;
        if( ErrorCount ) ErrorFlag = 1;
        return False;
    }

    ErrorFlag = 0;

    switch( CurToken )
    {   case(AxesTok):       ExecuteAxesCommand();       break;
        case(BoundBoxTok):   ExecuteBoundBoxCommand();   break;
        case(CentreTok):     ExecuteCentreCommand();     break;
        case(CopyTok):
        case(PasteTok):
        case(ClipboardTok):  ExecuteClipboardCommand();  break;
        case(ColourTok):     ExecuteColourCommand();     break;
        case(ConnectTok):    ExecuteConnectCommand();    break;
        case(LoadTok):       ExecuteLoadCommand();       break;
        case(WaitTok):       ExecutePauseCommand();      break;
        case(PickingTok):    ExecutePickingCommand();    break;
        case(PrintTok):      ExecutePrintCommand();      break;
        case(SetTok):        ExecuteSetCommand();        break;
        case(ShowTok):       ExecuteShowCommand();       break;
        case(TitleTok):      ExecuteTitleCommand();      break;
        case(UnitCellTok):   ExecuteUnitCellCommand();   break;
        case(ResetTok):      ExecuteResetCommand();      break;
        case(PositionTok):   ExecutePositionCommand();   break;
        /*case(TranslateTok):  ExecuteTranslateCommand();  break;*/

        case(RefreshTok):    FetchToken();
                             if( !CurToken || (CurToken==TrueTok) )
                             {   FastDraw = False;
                                 RefreshState = True;
                                 ReDrawFlag |= RFRefresh;
                                 FBClear = False;
                                 RefreshView();
                             } else
                                 CommandError(MsgStrs(ErrSyntax));
                             break;

        case(ZapTok):        if( ZapToken ) break;
                             FetchToken();
                             if( (CurToken==SelectedTok)||(CurToken==MoleculeTok) )
                             {   ZapMolecule();
                             } else if( !CurToken || CurToken==AllTok|| CurToken==WorldTok )
                             {     ZapDatabase(True);
                             } else
                                 CommandError(MsgStrs(ErrSyntax));
                             break;

        case(BondTok):    FetchToken();

                          if( CurToken == NumberTok )
                          { temp = TokenValue;
                            FetchToken();
                            if( CurToken == ',' )
                                FetchToken();
                            if( CurToken == NumberTok )
                            {   Long temp2;
                                temp2 = TokenValue;
                                FetchToken();
                                if( CurToken == UnBondTok )
                                {   if( ChangeBondOrder(temp,temp2,0,0) )
                                        ReDrawFlag |= RFInitial;
                                } else if( !CurToken || (CurToken == SingleTok) )
                                {   /* Create bond if necessary */
                                    if( ChangeBondOrder(temp,temp2,0,1) )
                                        ReDrawFlag |= RFInitial;
                                } else if( CurToken == DoubleTok )
                                {   /* Create bond if necessary */
                                    if( ChangeBondOrder(temp,temp2,0,2) )
                                        ReDrawFlag |= RFInitial;
                                } else  if( CurToken == TripleTok )
                                {   /* Create bond if necessary */
                                    if( ChangeBondOrder(temp,temp2,0,3) )
                                        ReDrawFlag |= RFInitial;
                                } else if( CurToken == '+' )
                                {   /* Create bond if necessary */
                                    if( ChangeBondOrder(temp,temp2,1,0) )
                                        ReDrawFlag |= RFInitial;
                                } else  if( CurToken == '-' )
                                {   /* Remove bond if necessary */
                                    if( ChangeBondOrder(temp,temp2,-1,0) )
                                        ReDrawFlag |= RFInitial;
                                } else if( CurToken == PickingTok ) 
                                {   /* Just pick, does not create bonds */
                                    if( CreateBondAxis(temp,TokenValue) )
                                        ReDrawFlag |= RFRefresh;
                                } else CommandError(MsgStrs(ErrBadArg));
                            } else CommandError(MsgStrs(ErrNotNum));

                          } else if( CurToken==RotateTok )
                          { FetchToken();
                            if( !CurToken || CurToken==TrueTok )
                            {   SetPickMode(PickRBond);
                            } else if( CurToken==FalseTok )
                            {   SetPickMode(PickIdent);
                            } else CommandError(MsgStrs(ErrBadArg));
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(UnBondTok):  FetchToken();
                          if( CurToken == NumberTok )
                          { temp = TokenValue;
                            FetchToken();
                            if( CurToken == ',' )
                                FetchToken();
                            if( CurToken == NumberTok )
                            {   if( ChangeBondOrder(temp,TokenValue,0,0) )
                                {  ReDrawFlag |= RFInitial;
                                } else
                                {  CommandError(MsgStrs(ErrBadArg));
                                }
                            } else CommandError(MsgStrs(ErrNotNum));

                          } else if( (!CurToken) && BondSelected )
                          { if(ChangeBondOrder(BSrcAtom->serno,BDstAtom->serno,0,0))
                            { BSrcAtom = NULL; BDstAtom = NULL;
                              ReDrawFlag |= RFRefresh;
                              BondSelected = False;
                            } else
                            { CommandError(MsgStrs(ErrBadArg));
                            }
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(MoleculeTok):
                          FetchToken();
                          if( CurToken == ShowTok )
                          {   if( !CurMolSet->visible )
                              {   CurMolSet->visible = 1;
                                  ReDrawFlag |= RFRefresh | RFColour;
                              }
                              break;
                          } else if( CurToken == HideTok )
                          {   if( CurMolSet->visible )
                              {   CurMolSet->visible = 0;
                                  ReDrawFlag |= RFRefresh | RFColour;
                              }
                              break;
                          } else if((CurToken!=NumberTok)||(TokenValue<1))
                          {    CommandError(MsgStrs(ErrBadArg));
                               break;
                          }
                          SelectMolecule(NULL,NULL,TokenValue-1);
                          break;
  
        case(SelectTok):  FetchToken();
                          if( !CurToken )
                          {   option = NormAtomFlag;
                              if( HetaGroups ) option |= HeteroFlag;
                              if( Hydrogens )  option |= HydrogenFlag;
                              StorePrevSelection();
                              SelectZone(option);
                          } else if( CurToken==AllTok )
                          {   StorePrevSelection();
                              SelectZone(AllAtomFlag);
                          } else if( CurToken==NoneTok )
                          {   StorePrevSelection();
                              SelectZone(0x00);
                          } else if( CurToken==ViewTok )
                          {   StorePrevSelection();
                              SelectArea(False,True,1,1,SRange<<1,YRange);
                          } else if( CurToken==PreviousTok )
                          {   if( (PickMode==PickAtom) ||
                                  (PickMode==PickGroup)||
                                  (PickMode==PickChain) )
                              {    SelectPrevSelection(False);
                                   DisplaySelectCount();
                              } else
                                  SelectPrevSelection(True);
                          } else if( CurToken=='<' )
                          {      ReadAtomSelection(True);
                          } else if( CurToken=='>' )
                          {      ReadAtomSelection(False);
                          } else
                          {   QueryExpr = ParseExpression(0);
                              if( QueryExpr )
                              {   if( !CurToken )
                                  {   StorePrevSelection();
                                      SelectZoneExpr(QueryExpr);
                                  } else CommandError(MsgStrs(ErrSyntax));
                                  DeAllocateExpr(QueryExpr);
                              }
                          }
                          break;

        case(RestrictTok):
                          FetchToken();
                          if( !CurToken )
                          {   option = NormAtomFlag;
                              if( HetaGroups ) option |= HeteroFlag;
                              if( Hydrogens )  option |= HydrogenFlag;
                              StorePrevSelection();
                              RestrictZone(option);
                              ReDrawFlag |= RFRefresh;
                          } else if( CurToken==AllTok )
                          {   StorePrevSelection();
                              RestrictZone(AllAtomFlag);
                              ReDrawFlag |= RFRefresh;
                          } else if( CurToken==NoneTok )
                          {   StorePrevSelection();
                              RestrictZone(0x00);
                              ReDrawFlag |= RFRefresh;
                          } else
                          {   QueryExpr = ParseExpression(0);
                              if( QueryExpr )
                              {   if( !CurToken )
                                  {   StorePrevSelection();
                                      RestrictZoneExpr(QueryExpr);
                                      ReDrawFlag |= RFRefresh;
                                  } else CommandError(MsgStrs(ErrSyntax));
                                  DeAllocateExpr(QueryExpr);
                              }
                          } 
                          break;



        case(WireframeTok):
                          FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DisableWireframe();
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              EnableWireframe(WireFlag,0,0);
                          } else if( CurToken==DashTok )
                          {   ReDrawFlag |= RFRefresh;
                              EnableWireframe(DashFlag,0,0);
                          } else if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=500 )
                              {   EnableWireframe(CylinderFlag,
                                                  (int)TokenValue,
                                                  (int)((TokenValue*4)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=500 )
                              {   EnableWireframe(CylinderFlag,
                                                  (int)TokenValue,
                                                  (int)((TokenValue*4)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(BackboneTok):
                          FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DisableBackbone();
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              EnableBackbone(WireFlag,0,0);
                          } else if( CurToken==DashTok )
                          {   ReDrawFlag |= RFRefresh;
                              EnableBackbone(DashFlag,0,0);
                          } else if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=500 )
                              {   EnableBackbone(CylinderFlag,
                                                 (int)TokenValue,
                                                 (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=500 )
                              {   EnableBackbone(CylinderFlag,
                                                 (int)TokenValue,
                                                 (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(CPKTok):
        case(SpacefillTok):
                          FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DisableSpacefill();
                          } else if( CurToken=='-' ) {
                              FetchToken();
                              if( CurToken==NumberTok ) {
                                  if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  }
                                  if( TokenValue<=100 )
                                  {   SetVanWaalRadius( SphereFlag,TokenValue);
                                      ReDrawFlag |= RFRefresh;
                                  } else CommandError(MsgStrs(ErrBigNum));
                              } else
                                  CommandError(MsgStrs(ErrBadArg));                         
                          } else if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=750 )
                              {   SetRadiusValue(MaxFun((int)TokenValue,1),
                                    SphereFlag);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=750 )
                              {   SetRadiusValue(MaxFun((int)TokenValue,1),
                                    SphereFlag);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==UserTok )
                          {   UserMaskAttrib(MaskRadiusFlag);
                              ReDrawFlag |= RFRefresh;
                          } else if( CurToken==TemperatureTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRadiusTemperature( SphereFlag );
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetVanWaalRadius( SphereFlag, 100 );
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(StarTok):
                          FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DisableSpacefill();
                          } else if( CurToken=='-' ) {
                              FetchToken();
                              if( CurToken==NumberTok ) {
                                  if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  }
                                  if( TokenValue<=100 )
                                  {   SetVanWaalRadius( StarFlag,TokenValue);
                                      ReDrawFlag |= RFRefresh;
                                  } else CommandError(MsgStrs(ErrBigNum));
                              } else
                                  CommandError(MsgStrs(ErrBadArg));                       
                          } else if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }
                              if( TokenValue<=750 )
                              {   SetRadiusValue(MaxFun((int)TokenValue,1),
                                    StarFlag);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=750 )
                              {   SetRadiusValue(MaxFun((int)TokenValue,1),
                                    StarFlag);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==UserTok )
                          {   UserMaskAttrib(MaskRadiusFlag);
                              ReDrawFlag |= RFRefresh;
                          } else if( CurToken==TemperatureTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRadiusTemperature( StarFlag );
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetVanWaalRadius( StarFlag, 100 );
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(DashTok):    FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DisableWireframe();
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              EnableWireframe(DashFlag,0,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(SSBondTok):  FetchToken();
                          if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=500 )
                              {   SetHBondStatus(False,True,(int)TokenValue,
                                                     (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=500 )
                              {   SetHBondStatus(False,True,(int)TokenValue,
                                                     (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetHBondStatus(False,False,0,0);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetHBondStatus(False,True,0,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(HBondTok):   FetchToken();
                          if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=500 )
                              {   SetHBondStatus(True,True,(int)TokenValue,
                                                     (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=500 )
                              {   SetHBondStatus(True,True,(int)TokenValue,
                                                     (int)((4*TokenValue)/5));
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetHBondStatus(True,False,0,0);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetHBondStatus(True,True,0,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(RibbonTok):  FetchToken();
                          if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,RibbonFlag,
                                                  (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,RibbonFlag,
                                                  (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(False,RibbonFlag,0);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(True,RibbonFlag,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(StrandsTok): FetchToken();
                          if( CurToken == DashTok )
                          {   option = DashStrandFlag;
                              FetchToken();
                          } else option = StrandFlag;

                          if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,option,(int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,option,(int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(False,option,0);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(True,option,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(TraceTok):   FetchToken();
                          if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(False,TraceFlag,80);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(True,TraceFlag,80);
                          } else if( CurToken==TemperatureTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetTraceTemperature();
                          } else if( CurToken==DotsTok )
                          {   FetchToken();
                              if( !CurToken )
                              {      ReDrawFlag |= RFRefresh;
                                  SetRibbonStatus(True,DotsFlag,80);
                              } else if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,250);
                                  }
                                  if( TokenValue<=500 )
                                  {   SetRibbonStatus(True,DotsFlag,
                                                 (int)TokenValue);
                                      ReDrawFlag |= RFRefresh;
                                  } else CommandError(MsgStrs(ErrBigNum));

                              } else if( CurToken=='.' )
                              {   FetchFloat(0,250);
                                  if( TokenValue<=500 )
                                  {   SetRibbonStatus(True,DotsFlag,
                                                     (int)TokenValue);
                                      ReDrawFlag |= RFRefresh;
                                  } else CommandError(MsgStrs(ErrBigNum));
                              } else CommandError(MsgStrs(ErrBadArg));
                          } else if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=500 )
                              {   SetRibbonStatus(True,TraceFlag,
                                                 (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=500 )
                              {   SetRibbonStatus(True,TraceFlag,
                                                 (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(CartoonTok): FetchToken();
                          if( CurToken==NumberTok )
                          {   if( *TokenPtr=='.' )
                              {   TokenPtr++;
                                  FetchFloat(TokenValue,250);
                              }

                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,CartoonFlag,
                                                  (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken=='.' )
                          {   FetchFloat(0,250);
                              if( TokenValue<=1000 )
                              {   SetRibbonStatus(True,CartoonFlag,
                                                  (int)TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(False,CartoonFlag,0);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetRibbonStatus(True,CartoonFlag,0);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(DotsTok):    FetchToken();
                          if( CurToken==NumberTok )
                          {   if( TokenValue<=1000 )
                              {   if( TokenValue )
                                  {   SetSurfaceStatus(True,DotFlag,(int)TokenValue);
                                  } else
                                      SetSurfaceStatus(True,DotFlag,1);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrBigNum));
                          } else if( CurToken==FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              SetSurfaceStatus(False,DotFlag,DotDensity);
                          } else if( (CurToken==TrueTok) || !CurToken )
                          {   ReDrawFlag |= RFRefresh;
                              SetSurfaceStatus(True,DotFlag,100);
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(MonitorTok): FetchToken();
                          if( CurToken == NumberTok )
                          {   temp = TokenValue;
                              FetchToken();
                              if( CurToken == ',' )
                                  FetchToken();

                              if( CurToken == NumberTok )
                              {   CreateMonitor(temp,TokenValue);
                                  ReDrawFlag |= RFRefresh;
                              } else CommandError(MsgStrs(ErrNotNum));
                          } else if( CurToken == FalseTok )
                          {   ReDrawFlag |= RFRefresh;
                              DeleteMonitors();
                          } else if( !CurToken || CurToken == TrueTok )
                          {   ReDrawFlag |= RFRefresh;
                              DrawMonitDistance = True;
                          } else CommandError(MsgStrs(ErrBadArg));
                          break;

        case(SlabTok):    if( ZapToken ) break;
                          FetchToken();
                          
                          if( CurToken==WorldTok )
                          {   FetchToken();
                              if( CurToken==OriginTok )
                              {   WClip[0] = Wo[2];
                                  if( UseSlabPlane )
                                      ReDrawFlag |= RFSlab;
                                  break;
                              } else if( CurToken == '-' )
                              {   FetchToken();
                                  done = -1;
                              } else done = 1;

                              if( (CurToken==NumberTok) || (CurToken=='.') )
                              {   if( CurToken==NumberTok )
                                  {   if( *TokenPtr=='.' )
                                      {   TokenPtr++;
                                          FetchFloat(TokenValue,250);
                                      }
                                  } else FetchFloat(0,250);

                                  /* First Time Activation */
                                  if( WClip[0]== -999999. )
                                      WClip[0] = MOffset[2]+Scale*LocalRadius;
                                  else
                                      WClip[0] -= Scale*TokenValue*done;
                                  ReDrawFlag |= RFSlab;
                                  UseSlabPlane = True;
                                  UseShadow = False;

                              } else CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  } else TokenValue *= 100;
                              } else FetchFloat(0,100);

                              if( TokenValue<=10000 )
                              {   WClip[0] = (Scale*LocalRadius*
                                      (TokenValue-5000))/5000.0 + MOffset[2];
                                  ReDrawFlag |= RFSlab;
                                  UseSlabPlane = True;
                                  UseShadow = False;
                              } else CommandError(MsgStrs(ErrBigNum));

                          } else if( CurToken==FalseTok )
                          {   if( UseSlabPlane )
                              {   ReDrawFlag |= RFSlab;
                                  UseSlabPlane = False;
                              }
                          } else if( !CurToken || (CurToken==TrueTok) )
                          {   if( !UseSlabPlane )
                              {   /* First Time Activation */
                                  if( WClip[0]== -999999. )
                                      WClip[0] = MOffset[2]+Scale*LocalRadius;
                                  ReDrawFlag |= RFSlab;
                                  UseSlabPlane = True;
                                  UseShadow = False;
                              }
                          } else CommandError(MsgStrs(ErrSyntax));
                          break;

        case(DepthTok):   if( ZapToken ) break;
                          FetchToken();
                          
                          if( CurToken==WorldTok )
                          {   FetchToken();
                              if( CurToken==OriginTok )
                              {   WClip[1] = Wo[2];
                                  if( UseDepthPlane )
                                      ReDrawFlag |= RFSlab;
                                  break;
                              } else if( CurToken == '-' )
                              {   FetchToken();
                                  done = -1;
                              } else done = 1;

                              if( (CurToken==NumberTok) || (CurToken=='.') )
                              {   if( CurToken==NumberTok )
                                  {   if( *TokenPtr=='.' )
                                      {   TokenPtr++;
                                          FetchFloat(TokenValue,250);
                                      }
                                  } else FetchFloat(0,250);

                                  /* First Time Activation */
                                  if( WClip[1]== -999999. )
                                      WClip[1] = MOffset[2]-Scale*LocalRadius;
                                  else
                                      WClip[1] -= Scale*TokenValue*done;
                                  ReDrawFlag |= RFSlab;
                                  UseDepthPlane = True;
                                  UseShadow = False;
                              } else CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  } else TokenValue *= 100;
                              } else FetchFloat(0,100);

                              if( TokenValue<=10000 )
                              {   WClip[1] = (Scale*LocalRadius*
                                      (TokenValue-5000))/5000.0 + MOffset[2];
                                  ReDrawFlag |= RFSlab;
                                  UseDepthPlane = True;
                                  UseShadow = False;
                              } else CommandError(MsgStrs(ErrBigNum));

                          } else if( CurToken==FalseTok )
                          {   if( UseDepthPlane )
                              {   ReDrawFlag |= RFSlab;
                                  UseDepthPlane = False;
                              }
                          } else if( !CurToken || (CurToken==TrueTok) )
                          {   if( !UseDepthPlane )
                              {   /* First Time Activation */
                                  if( WClip[1]== -999999. )
                                      WClip[1] = MOffset[2]-Scale*LocalRadius;
                                  ReDrawFlag |= RFSlab;
                                  UseDepthPlane = True;
                                  UseShadow = False;
                              }
                          } else CommandError(MsgStrs(ErrSyntax));
                          break;

        case(ZoomTok):    if( ZapToken ) break;
                          FetchToken();

                          if( CurToken==WorldTok )
                          {   wflag = 1;
                              FetchToken();
                          } else wflag = 0;

                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  } else TokenValue *= 100;
                              } else FetchFloat(0,100);

                              if( TokenValue<=10000 )
                              {   DialValue[DialZoom] = (TokenValue-10000)/10000.0;
                              } else if( Database )
                              {   /* Magnification */
                                  TokenValue -= 10000;
                                  temp = (Long)(MaxZoom*10000) + 1;
                                  if( TokenValue<=temp )
                                  {   DialValue[DialZoom] = (double)TokenValue/temp;
                                  } else
                                  {   CommandError(MsgStrs(ErrBigNum));
                                      break;
                                  }
                              } else
                                  break;
                          } else if( CurToken==TrueTok )
                          {   DialValue[DialZoom] = 0.5;
                          } else if( !CurToken || (CurToken==FalseTok) )
                          {   DialValue[DialZoom] = 0.0;
                          } else
                          {   CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          ReDrawFlag |= RFZoom;
                          if( wflag )
                              PrepareWorldZoom(0);
                          else
                              PrepareWorldZoom(1);
                          
                          break;

        case(ScaleTok):   if( ZapToken ) break;
                          FetchToken();

                          if( CurToken==WorldTok )
                          {   wflag = 1;
                              FetchToken();
                          } else wflag = 0;

                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,250);
                                  } else
                                      TokenValue *= 250;
                              } else
                                  FetchFloat(0,250);

                              if( Database )
                              {   double newzoom;
                                  newzoom = 1/(DScale*TokenValue);
                                  if( (newzoom-1.0) <= 0.0 )
                                  {   DialValue[3] = newzoom - 1.0;
                                      if( DialValue[3] < -1.0 ) DialValue[3] = -1.0;
                                  } else
                                  {   DialValue[3] = (newzoom - 1.0)/MaxZoom;
                                      if( DialValue[3] > 1.0 ) DialValue[3] = 1.0;
                                  }
                              } else
                                  break;
                          } else
                          {   CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          ReDrawFlag |= RFZoom;
                          if( wflag )
                              PrepareWorldZoom(0);
                          else
                              PrepareWorldZoom(1);
                          
                          break;

        case(RotateTok):  FetchToken();

                          /* =============== */
                          /*  Bond Rotation  */
                          /* =============== */
                          if( CurToken==BondTok )
                          {   /* Bond Rotation */
                              if( !BondSelected )
                              {   CommandError(MsgStrs(ErrNoBond));
                                  break;
                              }

                              FetchToken();

                              if( CurToken==FalseTok && PickMode==PickRBond )
                              {   SetPickMode(PickIdent);
                                  break;
                              } else if( CurToken==TrueTok && PickMode!=PickRBond )
                              {   SetPickMode(PickRBond);
                                  break;
                              } else if( CurToken == '-' )
                              {   FetchToken();
                                  done = True;
                              } else done = False;

                              if( (CurToken==NumberTok) || (CurToken=='.') )
                              {   if( CurToken==NumberTok )
                                  {   if( *TokenPtr=='.' )
                                      {   TokenPtr++;
                                          FetchFloat(TokenValue,100);
                                      } else TokenValue *= 100;
                                  } else FetchFloat(0,100);
                              }

                              if( TokenValue )
                              {   if( done ) TokenValue = -TokenValue;

                                  BondSelected->BRotValue += TokenValue/18000.0;
                                  while( BondSelected->BRotValue < -1.0 )
                                      BondSelected->BRotValue += 2.0;
                                  while( BRotValue > 1.0 )
                                      BondSelected->BRotValue -= 2.0;

                                  ReDrawFlag |= RFRotBond;
                                  PrepareBondRotate();

                              } else if(CurToken == ResetTok) 
                              {   BondSelected->BRotValue = 0.0;
                                  ReDrawFlag |= RFRotBond;
                                  PrepareBondRotate();

                              } else CommandError(MsgStrs(ErrNotNum));
                              break;
                          }

                          /* =================== */
                          /*  Molecule Rotation  */
                          /* =================== */

                          wflag = 0; option = -1;
                          if( CurToken==XTok )
                          {   option = 0;
                          } else if( CurToken==YTok )
                          {   option = 1;
                          } else if( CurToken==ZTok )
                          {   option = 2;
                          } else if( CurToken==MoleculeTok )
                          {   wflag = -1;
                          } else if( (CurToken==AllTok)||(CurToken==WorldTok) )
                          {   if( ZapToken ) break;
                              wflag = 1;
                          } else
                          {   CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          FetchToken();

                          if( wflag )
                          {   if( CurToken==XTok )
                              {   option = 0;
                                  FetchToken();
                              } else if( CurToken==YTok )
                              {   option = 1;
                                  FetchToken();
                              } else if( CurToken==ZTok )
                              {   option = 2;
                                  FetchToken();
                              } 
                          }

                          if( wflag && 
                          (!CurToken || CurToken==FalseTok || CurToken==TrueTok) )
                          {   if( wflag<0 )
                              {   if( CurToken==FalseTok && RotMode==RotMol )
                                  {   RotMode = RotAll;
                                  } else
                                  if( !CurToken || CurToken==TrueTok )
                                  {   if( RotMode==RotAll )
                                      {   RotMode = RotMol;
                                          /* Reset Center only if different molset
                                             and not at Origin*/
                                          if( CenterSet && (CenterSet!=CurMolSet) )
                                              CentreTransform(CurMolSet,0,0,0,False);
                                      }
                                  }
                              } else if( wflag>0 )
                              {   if( CurToken==FalseTok && RotMode==RotAll )
                                  {   RotMode = RotMol;
                                      /* Reset Center only if different molset
                                         and not at Origin*/
                                      if( CenterSet && (CenterSet!=CurMolSet) )
                                          CentreTransform(CurMolSet,0,0,0,False);
                                  } else if( !CurToken || CurToken==TrueTok )
                                  {   RotMode = RotAll;
                                  }
                              }
                              if( !CenterSet && DisplayMode )
                                  ReDrawFlag |= RFRotate;
                              break;
                          } else if( CurToken == '-' )
                          {   FetchToken();
                              done = True;
                          } else done = False;
#ifdef INVERT
                          if( option != 1 )
                              done = !done;
#endif
                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  } else TokenValue *= 100;
                              } else FetchFloat(0,100);

                              if( TokenValue )
                              {   if( done ) TokenValue = -TokenValue;
                                  if( option>=0 )   /*axe defined*/
                                  {   if( wflag<=0  )
                                      {   ReDrawFlag |= (1<<option);
                                          DialValue[option] += TokenValue/18000.0;
                                          while( DialValue[option]<-1.0 )
                                              DialValue[option] += 2.0;
                                          while( DialValue[option]>1.0 )
                                              DialValue[option] -= 2.0;
                                          PrepareMoleculeRotate();
                                      } else /*world*/
                                      {   ReDrawFlag |= (1<<option);
                                          WRotValue[option] += TokenValue/18000.0;
                                          while( WRotValue[option]<-1.0 )
                                              WRotValue[option] += 2.0;
                                          while( WRotValue[option]>1.0 )
                                              WRotValue[option] -= 2.0;
                                          PrepareWorldRotate(1);
                                      } 
                                  } else
                                  {   CommandError(MsgStrs(ErrSyntax));
                                      break;
                                  }
                              } else
                                  CommandError(MsgStrs(ErrSyntax));
                          } else if( CurToken == ResetTok ) 
                          {   if( option>=0 )   /*axe defined*/
                              {   if( wflag<=0 )
                                  {   DialValue[option] = 0.0;
                                      ReDrawFlag |= (1<<option);
                                      PrepareMoleculeRotate();
                                  } else /*world*/
                                  {   WRotValue[option] = 0.0;
                                      ReDrawFlag |= (1<<option);
                                      PrepareWorldRotate(1);
                                  } 
                              } else
                              {   CommandError(MsgStrs(ErrSyntax));
                                  break;
                              }
                          } else CommandError(MsgStrs(ErrNotNum));
                          break;

        case(TranslateTok):
                          FetchToken();

                          if( CurToken==WorldTok )
                          {   if( ZapToken ) break;
                              wflag = 1;
                              FetchToken();
                          } else 
                          {   wflag = 0;
                              if( CurToken==MoleculeTok )
                                  FetchToken();
                          }

                          if( CurToken==XTok )
                          {   option = 4;
                          } else if( CurToken==YTok )
                          {   option = 5;
                          } else if( CurToken==ZTok )
                          {   option = 6;
                          } else
                          {   CommandError(MsgStrs(ErrSyntax));
                              break;
                          }

                          FetchToken();
                          if( CurToken == '-' )
                          {   FetchToken();
                              done = True;
                          } else done = False;
#ifdef INVERT
                          if( option == 5 )
                              done = !done;
#endif
                          if( option == 6 )
                              done = !done;

                          if( (CurToken==NumberTok) || (CurToken=='.') )
                          {   if( CurToken==NumberTok )
                              {   if( *TokenPtr=='.' )
                                  {   TokenPtr++;
                                      FetchFloat(TokenValue,100);
                                  } else TokenValue *= 100;
                              } else
                                  FetchFloat(0,100);

                              ReDrawFlag |= (1<<option);
                              if( done ) TokenValue = -TokenValue;

                              if( wflag )
                              {   if( option == 4 )
                                  {   WTransValue[0] = WLastTX-Wo[0]/Range +
                                        TokenValue/10000.0;
                                  } else if( option == 5 )
                                  {   WTransValue[1] = WLastTY-Wo[1]/Range +
                                        TokenValue/10000.0;
                                  } else if( option == 6 )
                                  {   WTransValue[2] = WLastTZ +
                                        TokenValue/10000.0;
                                  }
                                  PrepareWorldTranslate();
                              } else
                              {   if( option == 4 )
                                  {   DialValue[4] = LastTX-MOffset[0]/Range +
                                        TokenValue/10000.0;
                                  } else if( option == 5 )
                                  {   DialValue[5] = LastTY-MOffset[1]/Range +
                                        TokenValue/10000.0;
                                  } else if( option == 6 )
                                  {   DialValue[6] = LastTZ-MOffset[2]/Range +
                                        TokenValue/10000.0;
                                  }
                                  PrepareMoleculeTranslate();
                              }
                          } else CommandError(MsgStrs(ErrNotNum));
                          break;

        case(StereoTok):  if( ZapToken ) break;
                          FetchToken();
                          if( !CurToken || (CurToken==TrueTok) )
                          {   SetStereoMode(True);
                          } else if( CurToken==FalseTok )
                          {   SetStereoMode(False);
                          } else if( CurToken == '-' )
                          {   if( !NextIf(NumberTok,ErrNotNum) )
                              {   StereoAngle = -TokenValue;
                                  SetStereoMode(True);
                                  CurToken = 0;
                              }
                          } else if( CurToken==NumberTok )
                          {   StereoAngle = TokenValue;
                              SetStereoMode(True);
                              CurToken = 0;
                          } else CommandError(MsgStrs(ErrSyntax));
                          break;

        case(ResizeTok):  FetchToken();
                          break;

        case('?'):
        case(HelpTok):    FetchToken();
                          if( !CurToken )
                          {   break;
                          } else if( CurToken==HTMLTok )
                          {   FetchToken();
                              if( !CurToken )
                              {   StartHelpHTML();
                              } else if( CurToken==StringTok )
                              {   strcpy(HelpHTMLSource,TokenIdent);
                                  StartHelpHTML();
                                  PrefsModified = True;
                              } else
                              {   strcpy(HelpHTMLSource,TokenStart);
                                  StartHelpHTML();
                                  PrefsModified = True;
                              }
                          } else
                          {   char topic[32];
                              char subtopic[32];
                              topic[0] = subtopic[0] = '\0';
                              strcpy(topic,TokenIdent);
                              if (FetchToken())
                                strcpy(subtopic,TokenIdent);
                              FindHelpInfo(topic,subtopic);
                          }
                          CurToken = 0;
                          break;

        case(LabelTok):   FetchToken();
                          if( !CurToken && (*LabelFormat!='\0') )
                          {   DefineLabels(LabelFormat);
                          } else if( !CurToken || (CurToken==TrueTok) )
                          {   *LabelFormat = '\0';
                              if( Info.chaincount>1 )
                              {   DefineLabels("%n%r:%c.%a%A");
                              } else if( MainGroupCount>1 )
                              {   DefineLabels("%n%r.%a%A");
                              } else DefineLabels("%e%i%A");
                          } else if( CurToken==FalseTok )
                          {   *LabelFormat = '\0';
                              DeleteLabels();
                          } else if( CurToken!=StringTok )
                          {   DefineLabels(TokenStart);
                              CurToken = 0;
                          } else 
                          {   strcpy(LabelFormat,TokenIdent);
                              if(PickMode == PickLabel)
                              {   WriteString(MsgStrs(StrPickLabel));
                                  break;
                              }
                              DefineLabels(LabelFormat);
                          }
                          ReDrawFlag |= RFRefresh;
                          break;

        case(EchoTok):    FetchToken();
                          InvalidateCmndLine();
                          if( CurToken==StringTok )
                          {   WriteString(TokenIdent);
                          } else if( CurToken )
                              WriteString(TokenStart);
                          WriteChar('\n');
                          CurToken = 0;
                          break;

        case(DefineTok):  FetchToken();
                          if( CurToken != IdentTok ) 
                          {   CommandError(MsgStrs(ErrSetName));
                              break;
                          }

                          param = (char*)malloc(TokenLength+1);
                          if( param )
                          {   memcpy(param,TokenIdent,TokenLength+1);
                              if( FetchToken() )
                              {   QueryExpr = ParseExpression(0);
                                  if( QueryExpr )
                                  {   done = DefineSetExpr(param,QueryExpr);
                                  } else done = True;
                              } else done = DefineSetExpr(param,(Expr*)NULL);
                          } else done = False;

                          if( !done )
                              CommandError(MsgStrs(ErrBadSet));
                          else
                              ResetUserSetBox( );
                          break;

        case(BackgroundTok):
                          if( ZapToken ) break;
                          FetchToken();
                          if( CurToken == TransparentTok )
                          {   UseTransparent = True;
                          } else if( CurToken == NormalTok )
                          {   UseTransparent = False;
                          } else if( ParseColour() )
                          {   ReDrawFlag |= RFColour;
                              BackR = RVal;
                              BackG = GVal;
                              BackB = BVal;
#ifndef IBMPC
                              FBClear = False;
#endif
                          } else if( CurToken )
                          {      CommandError(MsgStrs(ErrColour));
                          } else CommandError(MsgStrs(ErrNoCol));
                          break;


        case(LightSourceTok):    
                          if( ZapToken ) break;
                          ExecuteLightSourceCommand();
                          break;

        case(WriteTok):
        case(SaveTok):    if( !AllowWrite )
                              if( (FileDepth!=-1) && LineStack[FileDepth] )
                              {   CommandError(MsgStrs(ErrInScrpt));
                                  break;
                              }
                          
                          /* 'save' */
                          while( isspace(*TokenPtr) )
                              TokenPtr++;
                          if( !*TokenPtr )
                          {   if( !FileModified )
                                  break;
                              InvalidateCmndLine();
                              if( !SaveRsmMolecule((void *)0,1) )
                                  WriteString(MsgStrs(StrErrSavRSM));
                              else
                                  WriteString(MsgStrs(StrSavRSMOK));
                              break;
                          }

                          option = FetchToken();
                          wflag = 1;
                          if( option==MoleculeTok )
                          {   wflag = 0;
                              FetchToken();
                          } else if( option==WorldTok )
                              FetchToken();

                          if( (option==RasMolTok) || 
                              (option==ScriptTok) || 
                              (IsMoleculeToken(option)) || 
                              (IsImageToken(option) ))
                          {   if( !*TokenPtr || *TokenPtr==' ' )
                                  FetchToken();
                          } 
                          
                          if( !CurToken )
                          {   CommandError(MsgStrs(ErrFilNam));
                              break;
                          } else if( CurToken==StringTok )
                          {   ProcessFileName(TokenIdent);
                          } else ProcessFileName(TokenStart);                         
                          param = DataFileName;
                          CurToken = 0;

                          /* 'save {world|molecule} <filename> */
                          if( (option!=RasMolTok)      &&
                              (option!=ScriptTok)      &&
                              !IsMoleculeToken(option) &&
                              !IsImageToken(option)      )
                          {   InvalidateCmndLine();
                              if( !SaveRsmMolecule(param,wflag) )
                                  WriteString(MsgStrs(StrErrSavRSM));
                              else
                                  WriteString(MsgStrs(StrSavRSMOK));
                              break;
                          }

                          /* 'save <format> <filename> */
                          if( !IsMoleculeToken(option) )
                          {   if( ReDrawFlag ) RefreshScreen();
                              if( !WriteImageFile( param, option ) ) {
                                InvalidateCmndLine();
                                WriteString(MsgStrs(StrErrSav));
                              }
                          } else switch(option)
                          {   case(NMRPDBTok):
                              case(PDBTok):  SavePDBMolecule(param); break;
                              case(CIFTok):  SaveCIFMolecule(param); break;
                              case(MDLTok):  SaveMDLMolecule(param); break;
                              case(XYZTok):  SaveXYZMolecule(param); break;
#ifdef CEXIOLIB
                              case(CEXTok):  SaveCEXMolecule(param); break;
#endif
                              case(AlchemyTok): SaveAlchemyMolecule(param);
                                                break;
                          } break;

        case(AddTok):     FetchToken();
                          if( FileDepth<STACKSIZE )
                          {   if( !CurToken )
                              {   CommandError(MsgStrs(ErrFilNam));
                                  break;
                              } else if( CurToken==StringTok )
                              {      ProcessFileName(TokenIdent);
                              } else ProcessFileName(TokenStart);
                              CurToken = 0;

                              if( AddRSMDir )
                              if((strlen(RSMPathName)+strlen(DataFileName))<1023)
                              {   char temp[1024];
                                  strcpy(temp,RSMPathName);
                                  strcat(RSMPathName,DataFileName);
                                  strcpy(DataFileName,RSMPathName);
                                  strcpy(RSMPathName,temp);
                              }                                        
                              script = fopen(DataFileName,"rb");
                              ZapToken += 1;
                              LoadScriptFile(script,DataFileName);
                              ZapToken -= 1;
                              if( ZapToken < 0 )
                                  ZapToken = 0;
                              RotMode = RotAll;

                              if( !script )
                              {   InvalidateCmndLine();
                                  WriteString(MsgStrs(StrErrWorld));
                                  WorldScriptFormat = DataFormat;
                                  FileModified = True;
                              }

                          } else CommandError(MsgStrs(ErrScript));
                          break;

        case(SourceTok):  
        case(ScriptTok):  FetchToken();
                          if( CurToken==DefaultTok )
                          {   FetchToken();
                              if( !CurToken )
                                  *DefaultScriptName = '\0';
                              else if( CurToken==StringTok )
                                  strcpy(DefaultScriptName,TokenIdent);
                              else
                                  strcpy(DefaultScriptName,TokenStart);
                              PrefsModified = True;
                              CurToken = 0;
                          } else if( FileDepth<STACKSIZE )
                          {   if( !CurToken )
                              {   CommandError(MsgStrs(ErrFilNam));
                                  break;
                              } else if( CurToken==StringTok )
                              {      ProcessFileName(TokenIdent);
                              } else ProcessFileName(TokenStart);
                              CurToken = 0;

                              script = fopen(DataFileName,"rb");
                              LoadScriptFile(script,DataFileName);
                          } else CommandError(MsgStrs(ErrScript));
                          break;

        case(RenumTok):   FetchToken();
                          if( CurToken )
                          {   if( CurToken=='-' )
                              {    FetchToken();
                                  done = True;
                              } else done = False;

                              if( CurToken == NumberTok )
                              {   if( done )
                                  {     RenumberMolecule(-(int)TokenValue);
                                  } else RenumberMolecule((int)TokenValue); 
                              } else CommandError(MsgStrs(ErrNotNum));
                          } else RenumberMolecule(1);
                          break;

        case(StructureTok):
                          FetchToken();
                          if( !CurToken || (CurToken==FalseTok) )
                          {   DetermineStructure(False);
                          } else if( CurToken==TrueTok )
                          {   DetermineStructure(True);
                          } else CommandError(MsgStrs(ErrSyntax));
                          break;

        case(HeaderTok):
        case(CIFDataTok):
                         if( AcceptData[FileDepth] == 'Y' ) 
                         {   if( (FileDepth != -1) && LineStack[FileDepth] ) 
                             {   ScriptFileFormat = DataFormat;
                                 Recycle = &CurLine[0];
                                 switch( CurToken )
                                 {   case(HeaderTok):
                                        ProcessFile(FormatPDB,False,
                                        FileStack[FileDepth]);
                                        break;

                                     case(CIFDataTok):
                                        ProcessFile(FormatCIF,False,
                                        FileStack[FileDepth]);
                                        break;
                                 }
                                 DefaultRepresentation();
                             } else {
                                 CommandError(MsgStrs(ErrOutScrpt));
                             }
                          }
                          CurToken = 0;
                          return( ExitTok );

        case(ExitTok):    return( ExitTok );
        case(QuitTok):    return( QuitTok );
        default:          CommandError(MsgStrs(StrUnrec));
                          ErrorFlag = 1;
                          break;
    }

    if( CurToken && !ZapToken )
        if( FetchToken() )
            CommandError(MsgStrs(StrIgnore));
    TokenPtr = NULL;
    return False;
}


int ExecuteIPCCommand( char __huge *ptr )
{
    auto char buffer[256];
    register int stat,result;
    register int len,depth;
    register char *dst;

    /* Ignore IPC commands while paused! */
    if( IsPaused ) return 0;

    FileDepth = 0;
    *LineStack = 0;
    result = IPC_Ok;
    *NameStack = "IPC Error";

    /* Save command line */
    strcpy(buffer,CurLine);
   
    while( *ptr && (*ptr==' ') )
        ptr++;

    if( *ptr == '[' )
    {   while( *ptr++ == '[' )
        {   dst = CurLine;
            depth = 0;  
            len = 0;

            while( *ptr )
            {   if( *ptr == ']' )
                {   if( !depth )
                    {   ptr++; 
                        break;
                    } else depth--;
                } else if( *ptr=='[' )
                    depth++;

                if( len < MAXBUFFLEN-1 )
                {   *dst++ = *ptr++;
                    len++;
                } else ptr++;
            }
            *dst = '\0';

            if( len < MAXBUFFLEN-1 )
            {   stat = ExecuteCommand();
                if( stat == QuitTok )
                {  result = IPC_Quit;
                } else if( stat )
                   result = IPC_Exit;
            } else
            {   InvalidateCmndLine();
                WriteString(MsgStrs(StrRCLong));
            }

            while( *ptr && ((*ptr==' ')||(*ptr==';')) )
                ptr++;
        }
    } else if( *ptr )
    {   dst = CurLine;
        len = 0;
        while( *ptr )
        {   if( len < MAXBUFFLEN-1 )
            {   *dst++ = *ptr++;
                len++;
            } else break;
        }
        *dst = '\0';

        if( len < MAXBUFFLEN-1 )
        {   stat = ExecuteCommand();
            if( stat == QuitTok )
            {  result = IPC_Quit;
            } else if( stat )
               result = IPC_Exit;
        } else
        {   InvalidateCmndLine();
            WriteString(MsgStrs(StrRCLong));
        }
    }

    FileDepth = -1;
    if( CommandActive )
    {   strcpy(CurLine,buffer);
        if( !result ) result = IPC_Error;
    }
    return result;
}


/* Initiate default path in rasmol/scripts subdirectory */
static void InitDefaultScript( void ) {
    register char *dst;
    register FILE *fp;

    if(!*RasMolPath)
        return;

    dst = RasMolPath;
    dst = strcpy(DefaultScriptName,dst);
    while( *dst++ ) {};
    dst--;
    dst = strcpy(dst,"scripts");
    dst += 7;
    *dst++ = DirChar;
    strcpy(dst,"default.scr");
    if( !(fp=fopen(DefaultScriptName,"rb")) )
        *DefaultScriptName = '\0';
    else
        fclose(fp);
}


void InitialiseCommand( void )
{
    *DefaultScriptName = '\0';
    useDefaultScript = 1;
    InitDefaultScript();

    SelectCount = 0;
    TokenPtr = NULL;
    FileDepth = -1;

    IsPaused = False;
}

