# Microsoft Developer Studio Project File - Name="RasTop" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RasTop - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RasTop.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RasTop - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /G5 /MD /W3 /GX /Od /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /force /force:multiple
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "RasTop - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /FR /FD /Yd /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /incremental:no /debug /machine:I386 /pdbtype:sept /force:multiple
# SUBTRACT LINK32 /pdb:none /force

!ENDIF 

# Begin Target

# Name "RasTop - Win32 Release"
# Name "RasTop - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "rastop"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=..\..\src\rastop\cmdwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\colorbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\datadisplay.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\dialog.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\pannel.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\rasdoc.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\rasgraphics.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\RasTop.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\rasview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\scriptbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\selectbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\StdAfx.cpp
# End Source File
# End Group
# Begin Group "rasmol"

# PROP Default_Filter "c"
# Begin Source File

SOURCE=..\..\src\rasmol\abstree.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\brotate.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif_ctonum.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif_fract.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif_stx.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cmndline.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\command.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\help.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\infile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\langsel.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\molecule.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\multiple.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\outfile.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\pixutils.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\prefer.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\rasmol.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\render.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\repres.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\script.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\string_case.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\tokens.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\transfor.c
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\vector.c
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "rasmol headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\rasmol\abstree.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\brotate.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif_ctonum.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cif_fract.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\cmndline.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\command.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\font.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\graphics.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\help.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\infile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\langsel.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\molecule.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\multiple.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\outfile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\pixutils.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\prefer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\rasmol.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\render.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\repres.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\script.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\string_case.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\tokens.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\transfor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rasmol\vector.h
# End Source File
# End Group
# Begin Group "rastop headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\rastop\cmdwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\colorbar.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\datadisplay.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\dialog.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\mainfrm.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\pannel.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\rasdoc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\RasTop.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\rasview.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\resource.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\scriptbar.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\selectbar.h
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\StdAfx.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\res\cedit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\cedit.bmp
# End Source File
# Begin Source File

SOURCE=.\res\close_down.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\close_down.bmp
# End Source File
# Begin Source File

SOURCE=.\res\close_focused.bmp
# End Source File
# Begin Source File

SOURCE=.\res\close_up.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\close_up.bmp
# End Source File
# Begin Source File

SOURCE=.\res\closedown.bmp
# End Source File
# Begin Source File

SOURCE=.\res\closeup.bmp
# End Source File
# Begin Source File

SOURCE=.\res\colorpalette.bmp
# End Source File
# Begin Source File

SOURCE=.\res\command_.bmp
# End Source File
# Begin Source File

SOURCE=.\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\res\infopointeroff.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\infopointeroff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\infopointeron.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\infopointeron.bmp
# End Source File
# Begin Source File

SOURCE=.\res\light_sc.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\light_sc.bmp
# End Source File
# Begin Source File

SOURCE=.\res\picking.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\picking.cur
# End Source File
# Begin Source File

SOURCE=.\res\pickingm.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\pickingm.cur
# End Source File
# Begin Source File

SOURCE=.\res\pickingp.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\pickingp.cur
# End Source File
# Begin Source File

SOURCE=.\res\rasframe.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\rasframe.ico
# End Source File
# Begin Source File

SOURCE=.\res\rasmol_o.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\rasmol_o.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rasmoloff.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\rasmoloff.bmp
# End Source File
# Begin Source File

SOURCE=.\res\rasmolon.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\rasmolon.bmp
# End Source File
# Begin Source File

SOURCE=.\res\RasTop.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\RasTop.ico
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\RasTop.rc
# End Source File
# Begin Source File

SOURCE=.\res\RasTop.rc2
# End Source File
# Begin Source File

SOURCE=.\res\raswin.cur
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\raswin.cur
# End Source File
# Begin Source File

SOURCE=.\res\select_a.bmp
# End Source File
# Begin Source File

SOURCE=.\res\selectio.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\src\rastop\res\toolbar_.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\RasTop.reg
# End Source File
# Begin Source File

SOURCE=.\RastopLog.txt
# End Source File
# Begin Source File

SOURCE=.\RastopLog20.txt
# End Source File
# Begin Source File

SOURCE=.\RasTopLog2x.txt
# End Source File
# End Target
# End Project
# Section RasTop : {0000073A-0010-2000-4300-460001000200}
# 	2:10:CMainFrame:Palette support added
# End Section
