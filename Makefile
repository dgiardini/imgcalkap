CC			= gcc
LD			= g++
DEBUG		= 
WARNINGS	= -Wall -Wno-unused-function

WINCFLAGS=

all:imgcalkap_png.h imgcalkap

UNAME := $(shell uname)
ifeq ($(UNAME),Linux)
# Conditional section for Linux
CFLAGS=-O2 `wx-config --cppflags`  `pkg-config --cflags opencv` \
			`pkg-config --cflags gtk+-2.0` \
			-DIMGCALKAP_AS_LIB -DLIBIMGKAP  -DCOMPILE_LINUX  
LDFLAGS=-O2 `pkg-config --libs opencv` `wx-config --libs` `pkg-config --libs gtk+-2.0` -lfreeimage   

else
# Conditional section for Win
OPENCV_DIR=c:\OpenCV2.4.8\opencv\build
WX_DIR=c:\wxWidgets-2.8.12
FREEIMG_DIR=c:\FreeImage
CFLAGS		= $(DEBUG) $(WARNINGS)  -I"$(WX_DIR)\include" -DIMGCALKAP_AS_LIB -DLIBIMGKAP \
			    -I"$(FREEIMG_DIR)\Dist" \
				-I"$(WX_DIR)/msys/lib/wx/include/msw-ansi-release-2.8/" \
				-I"$(OPENCV_DIR)/include/opencv" -I"$(OPENCV_DIR)/include"
				
LDFLAGS		= $(DEBUG)  $(WARNINGS)-DIMGCALKAP_AS_LIB \
			-L"$(OPENCV_DIR)\msys\bin" \
			-L"$(WX_DIR)/msys/lib" \
			-l wx_msw_core-2.8 -l wx_base-2.8 -l wx_base_net-2.8 -l wx_base_xml-2.8 \
			-l wx_msw_adv-2.8 -l wx_msw_aui-2.8 -l wx_msw_xrc-2.8 -l winspool \
			-l comctl32 \
			-l oleaut32 \
			-l gdi32 \
			-l comdlg32 \
			-l ole32 \
			-l uuid \
			-l opencv_core248 \
            -lopencv_highgui248 \
			-lopencv_imgproc248 \
			-L"$(FREEIMG_DIR)\Dist" -l freeimage \
			-mwindows 

OTHER=imgcalkap.res 
STRIP = strip imgcalkap.exe
endif

imgcalkap:imgcalkap.o proc.o points.o readkapdetails.o imgkap.o parse_dms.o base64.o mousewheel.o ./gui/gui.o $(OTHER)
	$(LD)  imgcalkap.o points.o proc.o  readkapdetails.o imgkap.o parse_dms.o base64.o mousewheel.o ./gui/gui.o ./gui/dialog.o $(OTHER) $(LDFLAGS) -o imgcalkap
	$(STRIP)

imgcalkap.o:imgcalkap.c
	$(CC) -c imgcalkap.c  $(CFLAGS) 

proc.o:proc.c
	$(CC) -c proc.c  $(CFLAGS) 

points.o:points.c
	$(CC) -c points.c  $(CFLAGS) 
	
#inputbox.o:inputbox.c
#	$(CC) -c inputbox.c  $(CFLAGS) 

readkapdetails.o:readkapdetails.c
	$(CC) -c readkapdetails.c  $(CFLAGS)

imgkap.o:imgkap.c
	$(CC) -c imgkap.c  $(CFLAGS)

parse_dms.o:parse_dms.c
	$(CC) -c parse_dms.c  $(CFLAGS)
	
imgcalkap.res:imgcalkap.rc
	windres imgcalkap.rc -O coff -o imgcalkap.res

# Program to embed the splah image into the code	
base64.o:base64.c
	$(CC) -c base64.c  $(CFLAGS) 
	
imgcalkap_png.h:base64.c imgcalkap.png
	$(CC) base64.c -DCOMPILE_B64_ENCODER -o b64enc   $(CFLAGS)   $(LDFLAGS) 
	./b64enc imgcalkap.png imgcalkap_png.h

mousewheel.o:mousewheel.c
	$(CC) -c mousewheel.c  $(CFLAGS) 
	
./gui/gui.o:./gui/gui.cpp ./gui/dialog.cpp
	cd ./gui && make

clean:
	rm -f *.o imgcalkap.res imgcalkap.exe imgcalkap imgkap.exe b64enc b64enc.exe imgcalkap_png.h freeimage.a
	cd gui && make clean
	

