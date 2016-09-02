CC			= g++
DEBUG		= 
WARNINGS	=  

all:gui.o

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
## This section is conditional for Linux
CFLAGS=`wx-config --cppflags`  `pkg-config --cflags opencv` -DIMGCALKAP_AS_LIB -DLIBIMGKAP -DCOMPILE_LINUX 
#imgcalkap:imgcalkap.o proc.o  points.o gtk-ui.o
#	$(CC)  imgcalkap.o points.o proc.o gtk-ui.o $(LDFLAGS) -o imgcalkap 
#
else
## This section is conditional for Win
OPENCV_DIR=c:\OpenCV2.4.8\opencv\build
WX_DIR=c:\wxWidgets-2.8.12
FREEIMG_DIR=c:\FreeImage
CFLAGS		= $(DEBUG) $(WARNINGS) -I"$(WX_DIR)\include" -DIMGCALKAP_AS_LIB \
			    -I"$(FREEIMG_DIR)\Dist" \
				-I"$(WX_DIR)/msys/lib/wx/include/msw-ansi-release-2.8/" \
				-I"$(OPENCV_DIR)\include\opencv" -I"$(OPENCV_DIR)\include" 

endif

gui.o:gui.cpp dialog.o
	$(CC) -c gui.cpp  $(CFLAGS) 

dialog.o:dialog.cpp dialog.h
	$(CC) -c dialog.cpp  $(CFLAGS) 

clean:
	rm -f *.o 
	


