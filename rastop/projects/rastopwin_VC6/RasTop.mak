# Microsoft Developer Studio Generated NMAKE File, Based on RasTop.dsp
!IF "$(CFG)" == ""
CFG=RasTop - Win32 Debug
!MESSAGE No configuration specified. Defaulting to RasTop - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RasTop - Win32 Release" && "$(CFG)" != "RasTop - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RasTop.mak" CFG="RasTop - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RasTop - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RasTop - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "RasTop - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
TargetName=RasTop
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OutDir)\$(TargetName).hlp" "$(OUTDIR)\RasTop.exe"\
 "$(OUTDIR)\RasTop.pch"

!ELSE 

ALL : "$(OutDir)\$(TargetName).hlp" "$(OUTDIR)\RasTop.exe"\
 "$(OUTDIR)\RasTop.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\abstree.obj"
	-@erase "$(INTDIR)\cif.obj"
	-@erase "$(INTDIR)\cif_ctonum.obj"
	-@erase "$(INTDIR)\cif_fract.obj"
	-@erase "$(INTDIR)\cif_stx.obj"
	-@erase "$(INTDIR)\cmndline.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\datadisplay.obj"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\infile.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\MenuColorPicker.obj"
	-@erase "$(INTDIR)\molecule.obj"
	-@erase "$(INTDIR)\outfile.obj"
	-@erase "$(INTDIR)\pannel.obj"
	-@erase "$(INTDIR)\pixutils.obj"
	-@erase "$(INTDIR)\rasdoc.obj"
	-@erase "$(INTDIR)\rasgraphics.obj"
	-@erase "$(INTDIR)\RasTop.obj"
	-@erase "$(INTDIR)\RasTop.pch"
	-@erase "$(INTDIR)\RasTop.res"
	-@erase "$(INTDIR)\rasview.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\repres.obj"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\selectbar.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_case.obj"
	-@erase "$(INTDIR)\tokens.obj"
	-@erase "$(INTDIR)\transfor.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(OUTDIR)\RasTop.exe"
	-@erase "$(OutDir)\$(TargetName).hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\RasTop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RasTop.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RasTop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\RasTop.pdb" /machine:I386 /force /out:"$(OUTDIR)\RasTop.exe"\
 /force:multiple 
LINK32_OBJS= \
	"$(INTDIR)\abstree.obj" \
	"$(INTDIR)\cif.obj" \
	"$(INTDIR)\cif_ctonum.obj" \
	"$(INTDIR)\cif_fract.obj" \
	"$(INTDIR)\cif_stx.obj" \
	"$(INTDIR)\cmndline.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\datadisplay.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\infile.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\MenuColorPicker.obj" \
	"$(INTDIR)\molecule.obj" \
	"$(INTDIR)\outfile.obj" \
	"$(INTDIR)\pannel.obj" \
	"$(INTDIR)\pixutils.obj" \
	"$(INTDIR)\rasdoc.obj" \
	"$(INTDIR)\rasgraphics.obj" \
	"$(INTDIR)\RasTop.obj" \
	"$(INTDIR)\RasTop.res" \
	"$(INTDIR)\rasview.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\repres.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\selectbar.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_case.obj" \
	"$(INTDIR)\tokens.obj" \
	"$(INTDIR)\transfor.obj"

"$(OUTDIR)\RasTop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
TargetName=RasTop
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OutDir)\$(TargetName).hlp" "$(OUTDIR)\RasTop.exe"\
 "$(OUTDIR)\RasTop.pch"

!ELSE 

ALL : "$(OutDir)\$(TargetName).hlp" "$(OUTDIR)\RasTop.exe"\
 "$(OUTDIR)\RasTop.pch"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\abstree.obj"
	-@erase "$(INTDIR)\cif.obj"
	-@erase "$(INTDIR)\cif_ctonum.obj"
	-@erase "$(INTDIR)\cif_fract.obj"
	-@erase "$(INTDIR)\cif_stx.obj"
	-@erase "$(INTDIR)\cmndline.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\datadisplay.obj"
	-@erase "$(INTDIR)\dialog.obj"
	-@erase "$(INTDIR)\infile.obj"
	-@erase "$(INTDIR)\mainfrm.obj"
	-@erase "$(INTDIR)\MenuColorPicker.obj"
	-@erase "$(INTDIR)\molecule.obj"
	-@erase "$(INTDIR)\outfile.obj"
	-@erase "$(INTDIR)\pannel.obj"
	-@erase "$(INTDIR)\pixutils.obj"
	-@erase "$(INTDIR)\rasdoc.obj"
	-@erase "$(INTDIR)\rasgraphics.obj"
	-@erase "$(INTDIR)\RasTop.obj"
	-@erase "$(INTDIR)\RasTop.pch"
	-@erase "$(INTDIR)\RasTop.res"
	-@erase "$(INTDIR)\rasview.obj"
	-@erase "$(INTDIR)\render.obj"
	-@erase "$(INTDIR)\repres.obj"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\selectbar.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\string_case.obj"
	-@erase "$(INTDIR)\tokens.obj"
	-@erase "$(INTDIR)\transfor.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\RasTop.exe"
	-@erase "$(OUTDIR)\RasTop.pdb"
	-@erase "$(OutDir)\$(TargetName).hlp"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\RasTop.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD\
 /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

MTL=midl.exe
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o NUL /win32 
RSC=rc.exe
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\RasTop.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\RasTop.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)\RasTop.pdb" /debug /machine:I386 /out:"$(OUTDIR)\RasTop.exe"\
 /pdbtype:sept /force:multiple 
LINK32_OBJS= \
	"$(INTDIR)\abstree.obj" \
	"$(INTDIR)\cif.obj" \
	"$(INTDIR)\cif_ctonum.obj" \
	"$(INTDIR)\cif_fract.obj" \
	"$(INTDIR)\cif_stx.obj" \
	"$(INTDIR)\cmndline.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\datadisplay.obj" \
	"$(INTDIR)\dialog.obj" \
	"$(INTDIR)\infile.obj" \
	"$(INTDIR)\mainfrm.obj" \
	"$(INTDIR)\MenuColorPicker.obj" \
	"$(INTDIR)\molecule.obj" \
	"$(INTDIR)\outfile.obj" \
	"$(INTDIR)\pannel.obj" \
	"$(INTDIR)\pixutils.obj" \
	"$(INTDIR)\rasdoc.obj" \
	"$(INTDIR)\rasgraphics.obj" \
	"$(INTDIR)\RasTop.obj" \
	"$(INTDIR)\RasTop.res" \
	"$(INTDIR)\rasview.obj" \
	"$(INTDIR)\render.obj" \
	"$(INTDIR)\repres.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\selectbar.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\string_case.obj" \
	"$(INTDIR)\tokens.obj" \
	"$(INTDIR)\transfor.obj"

"$(OUTDIR)\RasTop.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(CFG)" == "RasTop - Win32 Release" || "$(CFG)" == "RasTop - Win32 Debug"
SOURCE=.\datadisplay.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_DATAD=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\datadisplay.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_DATAD=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\datadisplay.obj" : $(SOURCE) $(DEP_CPP_DATAD) "$(INTDIR)"


!ENDIF 

SOURCE=.\dialog.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_DIALO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_DIALO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\dialog.obj" : $(SOURCE) $(DEP_CPP_DIALO) "$(INTDIR)"


!ENDIF 

SOURCE=.\mainfrm.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_MAINF=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\MenuColorPicker.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_MAINF=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\MenuColorPicker.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\mainfrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"


!ENDIF 

SOURCE=.\MenuColorPicker.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_MENUC=\
	".\MenuColorPicker.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MenuColorPicker.obj" : $(SOURCE) $(DEP_CPP_MENUC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_MENUC=\
	".\MenuColorPicker.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MenuColorPicker.obj" : $(SOURCE) $(DEP_CPP_MENUC) "$(INTDIR)"


!ENDIF 

SOURCE=.\pannel.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_PANNE=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\pannel.obj" : $(SOURCE) $(DEP_CPP_PANNE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_PANNE=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\pannel.obj" : $(SOURCE) $(DEP_CPP_PANNE) "$(INTDIR)"


!ENDIF 

SOURCE=.\rasdoc.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_RASDO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasdoc.obj" : $(SOURCE) $(DEP_CPP_RASDO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_RASDO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasdoc.obj" : $(SOURCE) $(DEP_CPP_RASDO) "$(INTDIR)"


!ENDIF 

SOURCE=.\rasgraphics.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_RASGR=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasgraphics.obj" : $(SOURCE) $(DEP_CPP_RASGR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_RASGR=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasgraphics.obj" : $(SOURCE) $(DEP_CPP_RASGR) "$(INTDIR)"


!ENDIF 

SOURCE=.\RasTop.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_RASTO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\RasTop.obj" : $(SOURCE) $(DEP_CPP_RASTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_RASTO=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\RasTop.obj" : $(SOURCE) $(DEP_CPP_RASTO) "$(INTDIR)"


!ENDIF 

SOURCE=.\rasview.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_RASVI=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasview.obj" : $(SOURCE) $(DEP_CPP_RASVI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_RASVI=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\dialog.h"\
	".\graphics.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\rasview.obj" : $(SOURCE) $(DEP_CPP_RASVI) "$(INTDIR)"


!ENDIF 

SOURCE=.\selectbar.cpp

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_SELEC=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\selectbar.obj" : $(SOURCE) $(DEP_CPP_SELEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_SELEC=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\datadisplay.h"\
	".\infile.h"\
	".\mainfrm.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pannel.h"\
	".\pixutils.h"\
	".\rasdoc.h"\
	".\rasmol.h"\
	".\RasTop.h"\
	".\rasview.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\selectbar.h"\
	".\StdAfx.h"\
	".\transfor.h"\
	

"$(INTDIR)\selectbar.obj" : $(SOURCE) $(DEP_CPP_SELEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "RasTop - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /Fp"$(INTDIR)\RasTop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\RasTop.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /Fp"$(INTDIR)\RasTop.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\RasTop.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\abstree.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_ABSTR=\
	".\abstree.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\abstree.obj" : $(SOURCE) $(DEP_CPP_ABSTR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_ABSTR=\
	".\abstree.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\abstree.obj" : $(SOURCE) $(DEP_CPP_ABSTR) "$(INTDIR)"


!ENDIF 

SOURCE=.\cif.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_CIF_C=\
	".\cif.h"\
	".\cmndline.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\cif.obj" : $(SOURCE) $(DEP_CPP_CIF_C) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_CIF_C=\
	".\cif.h"\
	".\cmndline.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\cif.obj" : $(SOURCE) $(DEP_CPP_CIF_C) "$(INTDIR)"


!ENDIF 

SOURCE=.\cif_ctonum.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_CIF_CT=\
	".\cif_ctonum.h"\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\cif_ctonum.obj" : $(SOURCE) $(DEP_CPP_CIF_CT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_CIF_CT=\
	".\cif_ctonum.h"\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\cif_ctonum.obj" : $(SOURCE) $(DEP_CPP_CIF_CT) "$(INTDIR)"


!ENDIF 

SOURCE=.\cif_fract.c
DEP_CPP_CIF_F=\
	".\cif_fract.h"\
	

"$(INTDIR)\cif_fract.obj" : $(SOURCE) $(DEP_CPP_CIF_F) "$(INTDIR)"


SOURCE=.\cif_stx.c
DEP_CPP_CIF_S=\
	".\cif.h"\
	".\rasmol.h"\
	

"$(INTDIR)\cif_stx.obj" : $(SOURCE) $(DEP_CPP_CIF_S) "$(INTDIR)"


SOURCE=.\cmndline.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_CMNDL=\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	

"$(INTDIR)\cmndline.obj" : $(SOURCE) $(DEP_CPP_CMNDL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_CMNDL=\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	

"$(INTDIR)\cmndline.obj" : $(SOURCE) $(DEP_CPP_CMNDL) "$(INTDIR)"


!ENDIF 

SOURCE=.\command.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_COMMA=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\string_case.h"\
	".\tokens.h"\
	".\transfor.h"\
	

"$(INTDIR)\command.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_COMMA=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\string_case.h"\
	".\tokens.h"\
	".\transfor.h"\
	

"$(INTDIR)\command.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


!ENDIF 

SOURCE=.\infile.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_INFIL=\
	".\abstree.h"\
	".\cif.h"\
	".\cif_ctonum.h"\
	".\cif_fract.h"\
	".\cmndline.h"\
	".\command.h"\
	".\infile.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\infile.obj" : $(SOURCE) $(DEP_CPP_INFIL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_INFIL=\
	".\abstree.h"\
	".\cif.h"\
	".\cif_ctonum.h"\
	".\cif_fract.h"\
	".\cmndline.h"\
	".\command.h"\
	".\infile.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\infile.obj" : $(SOURCE) $(DEP_CPP_INFIL) "$(INTDIR)"


!ENDIF 

SOURCE=.\molecule.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_MOLEC=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\molecule.obj" : $(SOURCE) $(DEP_CPP_MOLEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_MOLEC=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	".\string_case.h"\
	".\transfor.h"\
	

"$(INTDIR)\molecule.obj" : $(SOURCE) $(DEP_CPP_MOLEC) "$(INTDIR)"


!ENDIF 

SOURCE=.\outfile.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_OUTFI=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\transfor.h"\
	

"$(INTDIR)\outfile.obj" : $(SOURCE) $(DEP_CPP_OUTFI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_OUTFI=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\outfile.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\transfor.h"\
	

"$(INTDIR)\outfile.obj" : $(SOURCE) $(DEP_CPP_OUTFI) "$(INTDIR)"


!ENDIF 

SOURCE=.\pixutils.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_PIXUT=\
	".\abstree.h"\
	".\font.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\pixutils.obj" : $(SOURCE) $(DEP_CPP_PIXUT) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_PIXUT=\
	".\abstree.h"\
	".\font.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\pixutils.obj" : $(SOURCE) $(DEP_CPP_PIXUT) "$(INTDIR)"


!ENDIF 

SOURCE=.\render.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_RENDE=\
	".\abstree.h"\
	".\cif_fract.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\render.obj" : $(SOURCE) $(DEP_CPP_RENDE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_RENDE=\
	".\abstree.h"\
	".\cif_fract.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\render.obj" : $(SOURCE) $(DEP_CPP_RENDE) "$(INTDIR)"


!ENDIF 

SOURCE=.\repres.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_REPRE=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\repres.obj" : $(SOURCE) $(DEP_CPP_REPRE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_REPRE=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\infile.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\repres.obj" : $(SOURCE) $(DEP_CPP_REPRE) "$(INTDIR)"


!ENDIF 

SOURCE=.\script.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_SCRIP=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\transfor.h"\
	

"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_SCRIP=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\pixutils.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\script.h"\
	".\transfor.h"\
	

"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"


!ENDIF 

SOURCE=.\string_case.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_STRIN=\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\string_case.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_STRIN=\
	".\rasmol.h"\
	".\string_case.h"\
	

"$(INTDIR)\string_case.obj" : $(SOURCE) $(DEP_CPP_STRIN) "$(INTDIR)"


!ENDIF 

SOURCE=.\tokens.c
DEP_CPP_TOKEN=\
	".\tokens.h"\
	

"$(INTDIR)\tokens.obj" : $(SOURCE) $(DEP_CPP_TOKEN) "$(INTDIR)"


SOURCE=.\transfor.c

!IF  "$(CFG)" == "RasTop - Win32 Release"

DEP_CPP_TRANS=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\transfor.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

DEP_CPP_TRANS=\
	".\abstree.h"\
	".\cmndline.h"\
	".\command.h"\
	".\graphics.h"\
	".\molecule.h"\
	".\rasmol.h"\
	".\render.h"\
	".\repres.h"\
	".\transfor.h"\
	

"$(INTDIR)\transfor.obj" : $(SOURCE) $(DEP_CPP_TRANS) "$(INTDIR)"


!ENDIF 

SOURCE=.\hlp\RasTop.hpj
USERDEP__RASTOP="$(ProjDir)\hlp\AfxCore.rtf"	"$(ProjDir)\hlp\AfxPrint.rtf"	

!IF  "$(CFG)" == "RasTop - Win32 Release"

OutDir=.\Release
ProjDir=.
TargetName=RasTop
InputPath=.\hlp\RasTop.hpj

"$(OutDir)\$(TargetName).hlp"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\
 $(USERDEP__RASTOP)
	call "$(ProjDir)\makehelp.bat"

!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

OutDir=.\Debug
ProjDir=.
TargetName=RasTop
InputPath=.\hlp\RasTop.hpj

"$(OutDir)\$(TargetName).hlp"	 : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"\
 $(USERDEP__RASTOP)
	call "$(ProjDir)\makehelp.bat"

!ENDIF 

SOURCE=.\RasTop.rc
DEP_RSC_RASTOP_=\
	".\res\cedit.bmp"\
	".\res\close_down.bmp"\
	".\res\close_up.bmp"\
	".\res\idr_main.ico"\
	".\res\infopointeroff.bmp"\
	".\res\infopointeron.bmp"\
	".\res\picking.cur"\
	".\res\rasframe.ico"\
	".\res\rasmoloff.bmp"\
	".\res\rasmolon.bmp"\
	".\res\RasTop.ico"\
	".\res\RasTop.rc2"\
	".\res\raswin.cur"\
	".\res\Toolbar.bmp"\
	".\res\toolbar_.bmp"\
	

"$(INTDIR)\RasTop.res" : $(SOURCE) $(DEP_RSC_RASTOP_) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)



!ENDIF 

