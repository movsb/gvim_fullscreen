gvim_fullscreen.dll: gvim_fullscreen.cpp
	cl /LD user32.lib gdi32.lib kernel32.lib gvim_fullscreen.cpp

clean:
	del *.obj *.lib *.obj *.exp
