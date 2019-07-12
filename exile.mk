COPT =-W3 -c -Gf4ys -DIS_32 -DWIN32 -Zdp -Zl -DDEBLEVEL=1
LOPT = -pdb:exile.pdb -debug -nologo -nodefaultlib -out:exile.exe
LIBS =  kernel32.lib\
        user32.lib\
        advapi32.lib\
        ddraw.lib\
        comdlg32.lib\
        gdi32.lib\
        winmm.lib\
        libcd.lib\
        d3drm.lib\
        exlmain.lib\
        font.lib\
        graphics.lib\
        bitmap.lib\
        surface.lib\
        display.lib\
        mainwnd.lib\
        3dsurface.lib\
        zcomp.lib\
        screen.lib\
        resource.lib\
        timer.lib\
        ddsurface.lib\
        bmsurface.lib\
        directdraw.lib\


exile.sym: exile.exe ; dbg2map /A /M exile.exe
exile.exe: exlmain.obj font.obj graphics.obj bitmap.obj surface.obj \
           display.obj mainwnd.obj 3dsurface.obj zcomp.obj screen.obj \
           resource.obj timer.obj ddsurface.obj bmsurface.obj directdraw.obj\
           ; link $(LOPT) $(LIBS)

exlmain.obj: exlmain.cpp; cl $(COPT) exlmain.cpp
font.obj: font.cpp; cl $(COPT) font.cpp
graphics.obj: graphics.cpp; cl $(COPT) graphics.cpp
surface.obj: surface.cpp; cl $(COPT) surface.cpp
display.obj: display.cpp; cl $(COPT) display.cpp
mainwnd.obj: mainwnd.cpp; cl $(COPT) mainwnd.cpp
3dsurface.obj: 3dsurface.cpp; cl $(COPT) 3dsurface.cpp
zcomp.obj: zcomp.cpp; cl $(COPT) zcomp.cpp
screen.obj: screen.cpp; cl $(COPT) screen.cpp
resource.obj: resource.cpp; cl $(COPT) resource.cpp
timer.obj: timer.cpp; cl $(COPT) timer.cpp
ddsurface.obj: ddsurface.cpp; cl $(COPT) ddsurface.cpp
bmsurface.obj: bmsurface.cpp; cl $(COPT) bmsurface.cpp
bitmap.obj: bitmap.cpp; cl $(COPT) bitmap.cpp
directdraw.obj: directdraw.cpp; cl $(COPT) directdraw.cpp
