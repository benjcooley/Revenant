# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=exile - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to exile - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "exile - Win32 Release" && "$(CFG)" != "exile - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "exile.mak" CFG="exile - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "exile - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "exile - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "exile - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "exile - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\exile.exe"

CLEAN : 
	-@erase "$(INTDIR)\3dimage.obj"
	-@erase "$(INTDIR)\3dscene.obj"
	-@erase "$(INTDIR)\3dsurface.obj"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animimage.obj"
	-@erase "$(INTDIR)\animsprite.obj"
	-@erase "$(INTDIR)\armor.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\battle.obj"
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bmsurface.obj"
	-@erase "$(INTDIR)\button.obj"
	-@erase "$(INTDIR)\character.obj"
	-@erase "$(INTDIR)\chunkcache.obj"
	-@erase "$(INTDIR)\colortable.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\container.obj"
	-@erase "$(INTDIR)\cursor.obj"
	-@erase "$(INTDIR)\d3dmath.obj"
	-@erase "$(INTDIR)\ddsurface.obj"
	-@erase "$(INTDIR)\decompress16.obj"
	-@erase "$(INTDIR)\decompress32.obj"
	-@erase "$(INTDIR)\decompress8.obj"
	-@erase "$(INTDIR)\decompress832.obj"
	-@erase "$(INTDIR)\directdraw.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dls.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\equip.obj"
	-@erase "$(INTDIR)\exit.obj"
	-@erase "$(INTDIR)\exlmain.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\food.obj"
	-@erase "$(INTDIR)\graphics.obj"
	-@erase "$(INTDIR)\helper.obj"
	-@erase "$(INTDIR)\holeanim.obj"
	-@erase "$(INTDIR)\holeinst.obj"
	-@erase "$(INTDIR)\imagery.obj"
	-@erase "$(INTDIR)\inventory.obj"
	-@erase "$(INTDIR)\lightninganim.obj"
	-@erase "$(INTDIR)\lightninginst.obj"
	-@erase "$(INTDIR)\lightsource.obj"
	-@erase "$(INTDIR)\logoscreen.obj"
	-@erase "$(INTDIR)\Mainwnd.obj"
	-@erase "$(INTDIR)\Mappane.obj"
	-@erase "$(INTDIR)\money.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\multianimimage.obj"
	-@erase "$(INTDIR)\multictrl.obj"
	-@erase "$(INTDIR)\multiimage.obj"
	-@erase "$(INTDIR)\multisurface.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playscreen.obj"
	-@erase "$(INTDIR)\rangedweapon.obj"
	-@erase "$(INTDIR)\resource.obj"
	-@erase "$(INTDIR)\savegame.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\Sector.obj"
	-@erase "$(INTDIR)\shadow.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\spell.obj"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\spritelist.obj"
	-@erase "$(INTDIR)\statpane.obj"
	-@erase "$(INTDIR)\statusbar.obj"
	-@erase "$(INTDIR)\Stream.obj"
	-@erase "$(INTDIR)\surface.obj"
	-@erase "$(INTDIR)\talisman.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\textbar.obj"
	-@erase "$(INTDIR)\tile.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\tool.obj"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\zcomp.obj"
	-@erase "$(OUTDIR)\exile.exe"
	-@erase "$(OUTDIR)\exile.map"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G5 /Gr /Zp16 /W3 /GX /Zi /Ox /Ot /Ow /Og /Oi /D "WIN32" /D "_WINDOWS" /D "EXILE" /YX /c
# SUBTRACT CPP /Oa
CPP_PROJ=/nologo /G5 /Gr /Zp16 /ML /W3 /GX /Zi /Ox /Ot /Ow /Og /Oi /D "WIN32"\
 /D "_WINDOWS" /D "EXILE" /Fp"$(INTDIR)/exile.pch" /YX /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/exile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 msvcrt.lib libc.lib winmm.lib ddraw.lib d3drm.lib dplay.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib implode.lib vfw32.lib libcmtd.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=msvcrt.lib libc.lib winmm.lib ddraw.lib d3drm.lib dplay.lib\
 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib implode.lib\
 vfw32.lib libcmtd.lib /nologo /subsystem:windows /profile\
 /map:"$(INTDIR)/exile.map" /debug /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/exile.exe" 
LINK32_OBJS= \
	"$(INTDIR)\3dimage.obj" \
	"$(INTDIR)\3dscene.obj" \
	"$(INTDIR)\3dsurface.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\animimage.obj" \
	"$(INTDIR)\animsprite.obj" \
	"$(INTDIR)\armor.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\battle.obj" \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bmsurface.obj" \
	"$(INTDIR)\button.obj" \
	"$(INTDIR)\character.obj" \
	"$(INTDIR)\chunkcache.obj" \
	"$(INTDIR)\colortable.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\container.obj" \
	"$(INTDIR)\cursor.obj" \
	"$(INTDIR)\d3dmath.obj" \
	"$(INTDIR)\ddsurface.obj" \
	"$(INTDIR)\decompress16.obj" \
	"$(INTDIR)\decompress32.obj" \
	"$(INTDIR)\decompress8.obj" \
	"$(INTDIR)\decompress832.obj" \
	"$(INTDIR)\directdraw.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dls.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\equip.obj" \
	"$(INTDIR)\exit.obj" \
	"$(INTDIR)\exlmain.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\food.obj" \
	"$(INTDIR)\graphics.obj" \
	"$(INTDIR)\helper.obj" \
	"$(INTDIR)\holeanim.obj" \
	"$(INTDIR)\holeinst.obj" \
	"$(INTDIR)\imagery.obj" \
	"$(INTDIR)\inventory.obj" \
	"$(INTDIR)\lightninganim.obj" \
	"$(INTDIR)\lightninginst.obj" \
	"$(INTDIR)\lightsource.obj" \
	"$(INTDIR)\logoscreen.obj" \
	"$(INTDIR)\Mainwnd.obj" \
	"$(INTDIR)\Mappane.obj" \
	"$(INTDIR)\money.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\multianimimage.obj" \
	"$(INTDIR)\multictrl.obj" \
	"$(INTDIR)\multiimage.obj" \
	"$(INTDIR)\multisurface.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playscreen.obj" \
	"$(INTDIR)\rangedweapon.obj" \
	"$(INTDIR)\resource.obj" \
	"$(INTDIR)\savegame.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\Sector.obj" \
	"$(INTDIR)\shadow.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\spell.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\spritelist.obj" \
	"$(INTDIR)\statpane.obj" \
	"$(INTDIR)\statusbar.obj" \
	"$(INTDIR)\Stream.obj" \
	"$(INTDIR)\surface.obj" \
	"$(INTDIR)\talisman.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\textbar.obj" \
	"$(INTDIR)\tile.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\tool.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\zcomp.obj"

"$(OUTDIR)\exile.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir ""
# PROP Target_Dir ""
OUTDIR=.
INTDIR=.

ALL : "$(OUTDIR)\exile.exe"

CLEAN : 
	-@erase "$(INTDIR)\3dimage.obj"
	-@erase "$(INTDIR)\3dscene.obj"
	-@erase "$(INTDIR)\3dsurface.obj"
	-@erase "$(INTDIR)\ammo.obj"
	-@erase "$(INTDIR)\animation.obj"
	-@erase "$(INTDIR)\animimage.obj"
	-@erase "$(INTDIR)\animsprite.obj"
	-@erase "$(INTDIR)\armor.obj"
	-@erase "$(INTDIR)\automap.obj"
	-@erase "$(INTDIR)\battle.obj"
	-@erase "$(INTDIR)\bitmap.obj"
	-@erase "$(INTDIR)\bmsurface.obj"
	-@erase "$(INTDIR)\button.obj"
	-@erase "$(INTDIR)\character.obj"
	-@erase "$(INTDIR)\chunkcache.obj"
	-@erase "$(INTDIR)\colortable.obj"
	-@erase "$(INTDIR)\command.obj"
	-@erase "$(INTDIR)\container.obj"
	-@erase "$(INTDIR)\cursor.obj"
	-@erase "$(INTDIR)\d3dmath.obj"
	-@erase "$(INTDIR)\ddsurface.obj"
	-@erase "$(INTDIR)\decompress16.obj"
	-@erase "$(INTDIR)\decompress32.obj"
	-@erase "$(INTDIR)\decompress8.obj"
	-@erase "$(INTDIR)\decompress832.obj"
	-@erase "$(INTDIR)\directdraw.obj"
	-@erase "$(INTDIR)\display.obj"
	-@erase "$(INTDIR)\dls.obj"
	-@erase "$(INTDIR)\editor.obj"
	-@erase "$(INTDIR)\equip.obj"
	-@erase "$(INTDIR)\exit.obj"
	-@erase "$(INTDIR)\exlmain.obj"
	-@erase "$(INTDIR)\font.obj"
	-@erase "$(INTDIR)\food.obj"
	-@erase "$(INTDIR)\graphics.obj"
	-@erase "$(INTDIR)\helper.obj"
	-@erase "$(INTDIR)\holeanim.obj"
	-@erase "$(INTDIR)\holeinst.obj"
	-@erase "$(INTDIR)\imagery.obj"
	-@erase "$(INTDIR)\inventory.obj"
	-@erase "$(INTDIR)\lightninganim.obj"
	-@erase "$(INTDIR)\lightninginst.obj"
	-@erase "$(INTDIR)\lightsource.obj"
	-@erase "$(INTDIR)\logoscreen.obj"
	-@erase "$(INTDIR)\Mainwnd.obj"
	-@erase "$(INTDIR)\Mappane.obj"
	-@erase "$(INTDIR)\money.obj"
	-@erase "$(INTDIR)\multi.obj"
	-@erase "$(INTDIR)\multianimimage.obj"
	-@erase "$(INTDIR)\multictrl.obj"
	-@erase "$(INTDIR)\multiimage.obj"
	-@erase "$(INTDIR)\multisurface.obj"
	-@erase "$(INTDIR)\object.obj"
	-@erase "$(INTDIR)\parse.obj"
	-@erase "$(INTDIR)\player.obj"
	-@erase "$(INTDIR)\playscreen.obj"
	-@erase "$(INTDIR)\rangedweapon.obj"
	-@erase "$(INTDIR)\resource.obj"
	-@erase "$(INTDIR)\savegame.obj"
	-@erase "$(INTDIR)\screen.obj"
	-@erase "$(INTDIR)\script.obj"
	-@erase "$(INTDIR)\scroll.obj"
	-@erase "$(INTDIR)\Sector.obj"
	-@erase "$(INTDIR)\shadow.obj"
	-@erase "$(INTDIR)\sound.obj"
	-@erase "$(INTDIR)\spell.obj"
	-@erase "$(INTDIR)\sprite.obj"
	-@erase "$(INTDIR)\spritelist.obj"
	-@erase "$(INTDIR)\statpane.obj"
	-@erase "$(INTDIR)\statusbar.obj"
	-@erase "$(INTDIR)\Stream.obj"
	-@erase "$(INTDIR)\surface.obj"
	-@erase "$(INTDIR)\talisman.obj"
	-@erase "$(INTDIR)\template.obj"
	-@erase "$(INTDIR)\textbar.obj"
	-@erase "$(INTDIR)\tile.obj"
	-@erase "$(INTDIR)\timer.obj"
	-@erase "$(INTDIR)\tool.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(INTDIR)\weapon.obj"
	-@erase "$(INTDIR)\zcomp.obj"
	-@erase "$(OUTDIR)\exile.exe"
	-@erase "$(OUTDIR)\exile.map"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "EXILE" /YX /GM /c
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS"\
 /D "EXILE" /Fp"$(INTDIR)/exile.pch" /YX /GM /c 
CPP_OBJS=.\.
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/exile.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 libcmtd.lib dxguid.lib winmm.lib ddraw.lib d3drm.lib dplay.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib implode.lib vfw32.lib dsound.lib /nologo /subsystem:windows /profile /map /debug /machine:I386 /nodefaultlib:"libc.lib"
# SUBTRACT LINK32 /nodefaultlib
LINK32_FLAGS=libcmtd.lib dxguid.lib winmm.lib ddraw.lib d3drm.lib dplay.lib\
 kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib implode.lib\
 vfw32.lib dsound.lib /nologo /subsystem:windows /profile\
 /map:"$(INTDIR)/exile.map" /debug /machine:I386 /nodefaultlib:"libc.lib"\
 /out:"$(OUTDIR)/exile.exe" 
LINK32_OBJS= \
	"$(INTDIR)\3dimage.obj" \
	"$(INTDIR)\3dscene.obj" \
	"$(INTDIR)\3dsurface.obj" \
	"$(INTDIR)\ammo.obj" \
	"$(INTDIR)\animation.obj" \
	"$(INTDIR)\animimage.obj" \
	"$(INTDIR)\animsprite.obj" \
	"$(INTDIR)\armor.obj" \
	"$(INTDIR)\automap.obj" \
	"$(INTDIR)\battle.obj" \
	"$(INTDIR)\bitmap.obj" \
	"$(INTDIR)\bmsurface.obj" \
	"$(INTDIR)\button.obj" \
	"$(INTDIR)\character.obj" \
	"$(INTDIR)\chunkcache.obj" \
	"$(INTDIR)\colortable.obj" \
	"$(INTDIR)\command.obj" \
	"$(INTDIR)\container.obj" \
	"$(INTDIR)\cursor.obj" \
	"$(INTDIR)\d3dmath.obj" \
	"$(INTDIR)\ddsurface.obj" \
	"$(INTDIR)\decompress16.obj" \
	"$(INTDIR)\decompress32.obj" \
	"$(INTDIR)\decompress8.obj" \
	"$(INTDIR)\decompress832.obj" \
	"$(INTDIR)\directdraw.obj" \
	"$(INTDIR)\display.obj" \
	"$(INTDIR)\dls.obj" \
	"$(INTDIR)\editor.obj" \
	"$(INTDIR)\equip.obj" \
	"$(INTDIR)\exit.obj" \
	"$(INTDIR)\exlmain.obj" \
	"$(INTDIR)\font.obj" \
	"$(INTDIR)\food.obj" \
	"$(INTDIR)\graphics.obj" \
	"$(INTDIR)\helper.obj" \
	"$(INTDIR)\holeanim.obj" \
	"$(INTDIR)\holeinst.obj" \
	"$(INTDIR)\imagery.obj" \
	"$(INTDIR)\inventory.obj" \
	"$(INTDIR)\lightninganim.obj" \
	"$(INTDIR)\lightninginst.obj" \
	"$(INTDIR)\lightsource.obj" \
	"$(INTDIR)\logoscreen.obj" \
	"$(INTDIR)\Mainwnd.obj" \
	"$(INTDIR)\Mappane.obj" \
	"$(INTDIR)\money.obj" \
	"$(INTDIR)\multi.obj" \
	"$(INTDIR)\multianimimage.obj" \
	"$(INTDIR)\multictrl.obj" \
	"$(INTDIR)\multiimage.obj" \
	"$(INTDIR)\multisurface.obj" \
	"$(INTDIR)\object.obj" \
	"$(INTDIR)\parse.obj" \
	"$(INTDIR)\player.obj" \
	"$(INTDIR)\playscreen.obj" \
	"$(INTDIR)\rangedweapon.obj" \
	"$(INTDIR)\resource.obj" \
	"$(INTDIR)\savegame.obj" \
	"$(INTDIR)\screen.obj" \
	"$(INTDIR)\script.obj" \
	"$(INTDIR)\scroll.obj" \
	"$(INTDIR)\Sector.obj" \
	"$(INTDIR)\shadow.obj" \
	"$(INTDIR)\sound.obj" \
	"$(INTDIR)\spell.obj" \
	"$(INTDIR)\sprite.obj" \
	"$(INTDIR)\spritelist.obj" \
	"$(INTDIR)\statpane.obj" \
	"$(INTDIR)\statusbar.obj" \
	"$(INTDIR)\Stream.obj" \
	"$(INTDIR)\surface.obj" \
	"$(INTDIR)\talisman.obj" \
	"$(INTDIR)\template.obj" \
	"$(INTDIR)\textbar.obj" \
	"$(INTDIR)\tile.obj" \
	"$(INTDIR)\timer.obj" \
	"$(INTDIR)\tool.obj" \
	"$(INTDIR)\weapon.obj" \
	"$(INTDIR)\zcomp.obj"

"$(OUTDIR)\exile.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "exile - Win32 Release"
# Name "exile - Win32 Debug"

!IF  "$(CFG)" == "exile - Win32 Release"

!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\graphics.cpp
DEP_CPP_GRAPH=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\chunkcache.h"\
	".\ddsurface.h"\
	".\decompdata.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\surface.h"\
	{$(INCLUDE)}"\decompress16.h"\
	{$(INCLUDE)}"\decompress32.h"\
	{$(INCLUDE)}"\decompress8.h"\
	{$(INCLUDE)}"\decompress832.h"\
	

"$(INTDIR)\graphics.obj" : $(SOURCE) $(DEP_CPP_GRAPH) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bitmap.cpp
DEP_CPP_BITMA=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\surface.h"\
	

"$(INTDIR)\bitmap.obj" : $(SOURCE) $(DEP_CPP_BITMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\font.cpp
DEP_CPP_FONT_=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\surface.h"\
	

"$(INTDIR)\font.obj" : $(SOURCE) $(DEP_CPP_FONT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\display.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_DISPL=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Mainwnd.h"\
	".\multisurface.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_DISPL=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\display.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_DISPL=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Mainwnd.h"\
	".\multisurface.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_DISPL=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\display.obj" : $(SOURCE) $(DEP_CPP_DISPL) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\surface.cpp
DEP_CPP_SURFA=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\surface.h"\
	

"$(INTDIR)\surface.obj" : $(SOURCE) $(DEP_CPP_SURFA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\exlmain.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_EXLMA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\automap.h"\
	".\Bitmap.h"\
	".\button.h"\
	".\character.h"\
	".\chunkcache.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\logoscreen.h"\
	".\Mainwnd.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\sound.h"\
	".\spell.h"\
	".\statpane.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	".\Timer.h"\
	".\wavedata.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\statusbar.h"\
	
NODEP_CPP_EXLMA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\exlmain.obj" : $(SOURCE) $(DEP_CPP_EXLMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_EXLMA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\automap.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\chunkcache.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\logoscreen.h"\
	".\Mainwnd.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\sound.h"\
	".\spell.h"\
	".\statpane.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	".\Timer.h"\
	".\wavedata.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\statusbar.h"\
	
NODEP_CPP_EXLMA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\exlmain.obj" : $(SOURCE) $(DEP_CPP_EXLMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\screen.cpp
DEP_CPP_SCREE=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Mainwnd.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	".\Timer.h"\
	

"$(INTDIR)\screen.obj" : $(SOURCE) $(DEP_CPP_SCREE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\resource.cpp
DEP_CPP_RESOU=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\resourcehdr.h"\
	".\surface.h"\
	".\Zcomp.h"\
	

"$(INTDIR)\resource.obj" : $(SOURCE) $(DEP_CPP_RESOU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mainwnd.cpp
DEP_CPP_MAINW=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Mainwnd.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\Mainwnd.obj" : $(SOURCE) $(DEP_CPP_MAINW) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\timer.cpp
DEP_CPP_TIMER=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Screen.h"\
	".\Timer.h"\
	

"$(INTDIR)\timer.obj" : $(SOURCE) $(DEP_CPP_TIMER) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\zcomp.cpp
DEP_CPP_ZCOMP=\
	".\Zcomp.h"\
	

"$(INTDIR)\zcomp.obj" : $(SOURCE) $(DEP_CPP_ZCOMP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\3dsurface.cpp
DEP_CPP_3DSUR=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\ddsurface.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_3DSUR=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\3dsurface.obj" : $(SOURCE) $(DEP_CPP_3DSUR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\bmsurface.cpp
DEP_CPP_BMSUR=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	

"$(INTDIR)\bmsurface.obj" : $(SOURCE) $(DEP_CPP_BMSUR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ddsurface.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_DDSUR=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_DDSUR=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\ddsurface.obj" : $(SOURCE) $(DEP_CPP_DDSUR) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_DDSUR=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_DDSUR=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\ddsurface.obj" : $(SOURCE) $(DEP_CPP_DDSUR) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\directdraw.cpp
DEP_CPP_DIREC=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Mainwnd.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_DIREC=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\directdraw.obj" : $(SOURCE) $(DEP_CPP_DIREC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\colortable.cpp
DEP_CPP_COLOR=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	

"$(INTDIR)\colortable.obj" : $(SOURCE) $(DEP_CPP_COLOR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\savegame.cpp
DEP_CPP_SAVEG=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\savegame.obj" : $(SOURCE) $(DEP_CPP_SAVEG) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\logoscreen.cpp
DEP_CPP_LOGOS=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\logoscreen.h"\
	".\Mainwnd.h"\
	".\Screen.h"\
	".\surface.h"\
	".\Timer.h"\
	

"$(INTDIR)\logoscreen.obj" : $(SOURCE) $(DEP_CPP_LOGOS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animation.cpp
DEP_CPP_ANIMA=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\surface.h"\
	

"$(INTDIR)\animation.obj" : $(SOURCE) $(DEP_CPP_ANIMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sprite.cpp
DEP_CPP_SPRIT=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\sprite.h"\
	

"$(INTDIR)\sprite.obj" : $(SOURCE) $(DEP_CPP_SPRIT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animsprite.cpp
DEP_CPP_ANIMS=\
	".\animation.h"\
	".\animdata.h"\
	".\animsprite.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	

"$(INTDIR)\animsprite.obj" : $(SOURCE) $(DEP_CPP_ANIMS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\spritelist.cpp
DEP_CPP_SPRITE=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\sprite.h"\
	".\spritelist.h"\
	

"$(INTDIR)\spritelist.obj" : $(SOURCE) $(DEP_CPP_SPRITE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\object.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_OBJEC=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\button.h"\
	".\character.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_OBJEC=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_OBJEC=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_OBJEC=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\object.obj" : $(SOURCE) $(DEP_CPP_OBJEC) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Stream.cpp
DEP_CPP_STREA=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Stream.h"\
	

"$(INTDIR)\Stream.obj" : $(SOURCE) $(DEP_CPP_STREA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\animimage.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_ANIMI=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\animimage.h"\
	".\animimagebody.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\mappane.h"\
	".\object.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\surface.h"\
	

"$(INTDIR)\animimage.obj" : $(SOURCE) $(DEP_CPP_ANIMI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_ANIMI=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\animimage.h"\
	".\animimagebody.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\animimage.obj" : $(SOURCE) $(DEP_CPP_ANIMI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Mappane.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_MAPPA=\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\dls.h"\
	".\editor.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\money.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\scroll.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	

"$(INTDIR)\Mappane.obj" : $(SOURCE) $(DEP_CPP_MAPPA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_MAPPA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\dls.h"\
	".\editor.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\money.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\scroll.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_MAPPA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\Mappane.obj" : $(SOURCE) $(DEP_CPP_MAPPA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Sector.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_SECTO=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imagery.h"\
	".\mappane.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\Sector.obj" : $(SOURCE) $(DEP_CPP_SECTO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_SECTO=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\Sector.obj" : $(SOURCE) $(DEP_CPP_SECTO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\tile.cpp
DEP_CPP_TILE_=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	".\tile.h"\
	

"$(INTDIR)\tile.obj" : $(SOURCE) $(DEP_CPP_TILE_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\playscreen.cpp
DEP_CPP_PLAYS=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\animimagebody.h"\
	".\automap.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\editor.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\Mainwnd.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\scroll.h"\
	".\sector.h"\
	".\sound.h"\
	".\spell.h"\
	".\statpane.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	".\tile.h"\
	".\Timer.h"\
	".\wavedata.h"\
	{$(INCLUDE)}"\statusbar.h"\
	

"$(INTDIR)\playscreen.obj" : $(SOURCE) $(DEP_CPP_PLAYS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multisurface.cpp
DEP_CPP_MULTI=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\multisurface.h"\
	".\surface.h"\
	

"$(INTDIR)\multisurface.obj" : $(SOURCE) $(DEP_CPP_MULTI) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\editor.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_EDITO=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\button.h"\
	".\character.h"\
	".\command.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\dls.h"\
	".\editor.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\scroll.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	".\template.h"\
	".\textbar.h"\
	".\tile.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\statusbar.h"\
	
NODEP_CPP_EDITO=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_EDITO=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\command.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\dls.h"\
	".\editor.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\script.h"\
	".\scroll.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	".\template.h"\
	".\textbar.h"\
	".\tile.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\statusbar.h"\
	
NODEP_CPP_EDITO=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\editor.obj" : $(SOURCE) $(DEP_CPP_EDITO) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dls.cpp
DEP_CPP_DLS_C=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Matrix.h"\
	".\Rmatrix.h"\
	".\surface.h"\
	

"$(INTDIR)\dls.obj" : $(SOURCE) $(DEP_CPP_DLS_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\3dscene.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_3DSCE=\
	".\3dimage.h"\
	".\3dimagebody.h"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Mainwnd.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_3DSCE=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	

"$(INTDIR)\3dscene.obj" : $(SOURCE) $(DEP_CPP_3DSCE) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_3DSCE=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dimage.h"\
	".\3dimagebody.h"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Mainwnd.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_3DSCE=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\3dscene.obj" : $(SOURCE) $(DEP_CPP_3DSCE) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\button.cpp
DEP_CPP_BUTTO=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\surface.h"\
	

"$(INTDIR)\button.obj" : $(SOURCE) $(DEP_CPP_BUTTO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\parse.cpp
DEP_CPP_PARSE=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\parse.h"\
	

"$(INTDIR)\parse.obj" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\cursor.cpp
DEP_CPP_CURSO=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\bmsurface.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	

"$(INTDIR)\cursor.obj" : $(SOURCE) $(DEP_CPP_CURSO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\3dimage.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_3DIMA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dimage.h"\
	".\3dimagebody.h"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_3DIMA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\3dimage.obj" : $(SOURCE) $(DEP_CPP_3DIMA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_3DIMA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dimage.h"\
	".\3dimagebody.h"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\d3dmacs.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\directdraw.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_3DIMA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\3dimage.obj" : $(SOURCE) $(DEP_CPP_3DIMA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\character.cpp
DEP_CPP_CHARA=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\character.obj" : $(SOURCE) $(DEP_CPP_CHARA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\armor.cpp
DEP_CPP_ARMOR=\
	".\armor.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\armor.obj" : $(SOURCE) $(DEP_CPP_ARMOR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\automap.cpp
DEP_CPP_AUTOM=\
	".\3dsurface.h"\
	".\automap.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\automap.obj" : $(SOURCE) $(DEP_CPP_AUTOM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\inventory.cpp
DEP_CPP_INVEN=\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\money.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\inventory.obj" : $(SOURCE) $(DEP_CPP_INVEN) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\weapon.cpp
DEP_CPP_WEAPO=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	".\weapon.h"\
	

"$(INTDIR)\weapon.obj" : $(SOURCE) $(DEP_CPP_WEAPO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multictrl.cpp
DEP_CPP_MULTIC=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Multi.h"\
	".\multictrl.h"\
	".\Multidat.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\surface.h"\
	

"$(INTDIR)\multictrl.obj" : $(SOURCE) $(DEP_CPP_MULTIC) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\player.cpp
DEP_CPP_PLAYE=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sound.h"\
	".\Stream.h"\
	".\surface.h"\
	".\textbar.h"\
	".\wavedata.h"\
	

"$(INTDIR)\player.obj" : $(SOURCE) $(DEP_CPP_PLAYE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\equip.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_EQUIP=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\equip.obj" : $(SOURCE) $(DEP_CPP_EQUIP) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_EQUIP=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\cursor.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\equip.obj" : $(SOURCE) $(DEP_CPP_EQUIP) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\statpane.cpp
DEP_CPP_STATP=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\statpane.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\statpane.obj" : $(SOURCE) $(DEP_CPP_STATP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\spell.cpp
DEP_CPP_SPELL=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\spell.obj" : $(SOURCE) $(DEP_CPP_SPELL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\talisman.cpp
DEP_CPP_TALIS=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	".\talisman.h"\
	

"$(INTDIR)\talisman.obj" : $(SOURCE) $(DEP_CPP_TALIS) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\script.cpp
DEP_CPP_SCRIP=\
	".\command.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\script.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\script.obj" : $(SOURCE) $(DEP_CPP_SCRIP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\battle.cpp
DEP_CPP_BATTL=\
	".\battle.h"\
	".\battleblock.h"\
	".\character.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\battle.obj" : $(SOURCE) $(DEP_CPP_BATTL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\command.cpp
DEP_CPP_COMMA=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\command.h"\
	".\editor.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\script.h"\
	".\scroll.h"\
	".\sector.h"\
	".\spell.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\command.obj" : $(SOURCE) $(DEP_CPP_COMMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\chunkcache.cpp
DEP_CPP_CHUNK=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\chunkcache.h"\
	".\decompdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\surface.h"\
	{$(INCLUDE)}"\decompress16.h"\
	

"$(INTDIR)\chunkcache.obj" : $(SOURCE) $(DEP_CPP_CHUNK) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\food.cpp
DEP_CPP_FOOD_=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\food.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	

"$(INTDIR)\food.obj" : $(SOURCE) $(DEP_CPP_FOOD_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\imagery.cpp
DEP_CPP_IMAGE=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\imagery.obj" : $(SOURCE) $(DEP_CPP_IMAGE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\d3dmath.cpp
DEP_CPP_D3DMA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\d3dmath.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_D3DMA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\d3dmath.obj" : $(SOURCE) $(DEP_CPP_D3DMA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multi.cpp
DEP_CPP_MULTI_=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\Resource.h"\
	

"$(INTDIR)\multi.obj" : $(SOURCE) $(DEP_CPP_MULTI_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\container.cpp
DEP_CPP_CONTA=\
	".\container.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\container.obj" : $(SOURCE) $(DEP_CPP_CONTA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\tool.cpp
DEP_CPP_TOOL_=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	".\tool.h"\
	

"$(INTDIR)\tool.obj" : $(SOURCE) $(DEP_CPP_TOOL_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\lightsource.cpp
DEP_CPP_LIGHT=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\lightsource.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	

"$(INTDIR)\lightsource.obj" : $(SOURCE) $(DEP_CPP_LIGHT) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\helper.cpp
DEP_CPP_HELPE=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\helper.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	

"$(INTDIR)\helper.obj" : $(SOURCE) $(DEP_CPP_HELPE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\shadow.cpp
DEP_CPP_SHADO=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\shadow.h"\
	".\Stream.h"\
	

"$(INTDIR)\shadow.obj" : $(SOURCE) $(DEP_CPP_SHADO) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\exit.cpp
DEP_CPP_EXIT_=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\character.h"\
	".\equip.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\exit.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\player.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\savegame.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\exit.obj" : $(SOURCE) $(DEP_CPP_EXIT_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multiimage.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_MULTII=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\holeanim.h"\
	{$(INCLUDE)}"\lightninganim.h"\
	
NODEP_CPP_MULTII=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\multiimage.obj" : $(SOURCE) $(DEP_CPP_MULTII) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_MULTII=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\holeanim.h"\
	{$(INCLUDE)}"\lightninganim.h"\
	
NODEP_CPP_MULTII=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\multiimage.obj" : $(SOURCE) $(DEP_CPP_MULTII) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lightninginst.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_LIGHTN=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\lightninginst.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_LIGHTN=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\lightninginst.obj" : $(SOURCE) $(DEP_CPP_LIGHTN) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_LIGHTN=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\lightninginst.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_LIGHTN=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\lightninginst.obj" : $(SOURCE) $(DEP_CPP_LIGHTN) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\lightninganim.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_LIGHTNI=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\lightninginst.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\lightninganim.h"\
	
NODEP_CPP_LIGHTNI=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\lightninganim.obj" : $(SOURCE) $(DEP_CPP_LIGHTNI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_LIGHTNI=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\lightninginst.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\lightninganim.h"\
	
NODEP_CPP_LIGHTNI=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\lightninganim.obj" : $(SOURCE) $(DEP_CPP_LIGHTNI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\holeinst.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_HOLEI=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\holeinst.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_HOLEI=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\holeinst.obj" : $(SOURCE) $(DEP_CPP_HOLEI) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_HOLEI=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\dls.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\holeinst.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	
NODEP_CPP_HOLEI=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\holeinst.obj" : $(SOURCE) $(DEP_CPP_HOLEI) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\holeanim.cpp

!IF  "$(CFG)" == "exile - Win32 Release"

DEP_CPP_HOLEA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\holeinst.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\holeanim.h"\
	
NODEP_CPP_HOLEA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\holeanim.obj" : $(SOURCE) $(DEP_CPP_HOLEA) "$(INTDIR)"


!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

DEP_CPP_HOLEA=\
	"..\dxsdk\sdk\inc\d3dvec.inl"\
	".\3dscene.h"\
	".\3dsurface.h"\
	".\animation.h"\
	".\animdata.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\d3dmath.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\holeinst.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\multiimage.h"\
	".\multiimagebody.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	{$(INCLUDE)}"\d3d.h"\
	{$(INCLUDE)}"\d3dcaps.h"\
	{$(INCLUDE)}"\d3drm.h"\
	{$(INCLUDE)}"\d3drmdef.h"\
	{$(INCLUDE)}"\d3drmobj.h"\
	{$(INCLUDE)}"\d3drmwin.h"\
	{$(INCLUDE)}"\d3dtypes.h"\
	{$(INCLUDE)}"\holeanim.h"\
	
NODEP_CPP_HOLEA=\
	"..\dxsdk\sdk\inc\d3dcom.h"\
	"..\dxsdk\sdk\inc\subwtype.h"\
	

"$(INTDIR)\holeanim.obj" : $(SOURCE) $(DEP_CPP_HOLEA) "$(INTDIR)"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\decompress8.cpp
DEP_CPP_DECOM=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	{$(INCLUDE)}"\decompress16.h"\
	{$(INCLUDE)}"\decompress8.h"\
	

"$(INTDIR)\decompress8.obj" : $(SOURCE) $(DEP_CPP_DECOM) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\decompress832.cpp
DEP_CPP_DECOMP=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	{$(INCLUDE)}"\decompress832.h"\
	

"$(INTDIR)\decompress832.obj" : $(SOURCE) $(DEP_CPP_DECOMP) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\decompress16.cpp
DEP_CPP_DECOMPR=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	{$(INCLUDE)}"\decompress16.h"\
	

"$(INTDIR)\decompress16.obj" : $(SOURCE) $(DEP_CPP_DECOMPR) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\decompress32.cpp
DEP_CPP_DECOMPRE=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	{$(INCLUDE)}"\decompress32.h"\
	

"$(INTDIR)\decompress32.obj" : $(SOURCE) $(DEP_CPP_DECOMPRE) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\exile.ico

!IF  "$(CFG)" == "exile - Win32 Release"

!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\death.gif

!IF  "$(CFG)" == "exile - Win32 Release"

!ELSEIF  "$(CFG)" == "exile - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\money.cpp
DEP_CPP_MONEY=\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\money.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\money.obj" : $(SOURCE) $(DEP_CPP_MONEY) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\multianimimage.cpp
DEP_CPP_MULTIA=\
	".\animation.h"\
	".\animdata.h"\
	".\animimage.h"\
	".\animimagebody.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\multianimimage.h"\
	".\multianimimagebody.h"\
	".\object.h"\
	".\parse.h"\
	".\Resource.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\multianimimage.obj" : $(SOURCE) $(DEP_CPP_MULTIA) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ammo.cpp
DEP_CPP_AMMO_=\
	".\ammo.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\inventory.h"\
	".\lightdef.h"\
	".\mappane.h"\
	".\multisurface.h"\
	".\object.h"\
	".\parse.h"\
	".\Screen.h"\
	".\sector.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\ammo.obj" : $(SOURCE) $(DEP_CPP_AMMO_) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\template.cpp
DEP_CPP_TEMPL=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\sector.h"\
	".\Stream.h"\
	".\template.h"\
	

"$(INTDIR)\template.obj" : $(SOURCE) $(DEP_CPP_TEMPL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\sound.cpp
DEP_CPP_SOUND=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Mainwnd.h"\
	".\parse.h"\
	".\Resource.h"\
	".\sound.h"\
	".\wavedata.h"\
	

"$(INTDIR)\sound.obj" : $(SOURCE) $(DEP_CPP_SOUND) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\scroll.cpp
DEP_CPP_SCROL=\
	".\3dsurface.h"\
	".\Bitmap.h"\
	".\bitmapdata.h"\
	".\button.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\Font.h"\
	".\fontdata.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\object.h"\
	".\parse.h"\
	".\playscreen.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\scroll.h"\
	".\Stream.h"\
	".\surface.h"\
	

"$(INTDIR)\scroll.obj" : $(SOURCE) $(DEP_CPP_SCROL) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\textbar.cpp
DEP_CPP_TEXTB=\
	".\3dsurface.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Screen.h"\
	".\surface.h"\
	".\textbar.h"\
	

"$(INTDIR)\textbar.obj" : $(SOURCE) $(DEP_CPP_TEXTB) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\statusbar.cpp
DEP_CPP_STATU=\
	".\3dsurface.h"\
	".\ddsurface.h"\
	".\Display.h"\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\Multi.h"\
	".\Multidat.h"\
	".\Resource.h"\
	".\Screen.h"\
	".\surface.h"\
	{$(INCLUDE)}"\statusbar.h"\
	

"$(INTDIR)\statusbar.obj" : $(SOURCE) $(DEP_CPP_STATU) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\rangedweapon.cpp
DEP_CPP_RANGE=\
	".\Exile.h"\
	".\exiledef.h"\
	".\Exltypes.h"\
	".\graphics.h"\
	".\imageres.h"\
	".\imagery.h"\
	".\lightdef.h"\
	".\object.h"\
	".\parse.h"\
	".\Stream.h"\
	{$(INCLUDE)}"\rangedweapon.h"\
	

"$(INTDIR)\rangedweapon.obj" : $(SOURCE) $(DEP_CPP_RANGE) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
