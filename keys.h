#ifndef __KEYS_IMGCALKAP_INC
#define __KEYS_IMGCALKAP_INC
/* 
  The numeric key codes where copied from the output of cvWaitKey()
  should be a better way to get system independent key codes */

#ifndef COMPILE_LINUX
	#include <windows.h>
	#define KEY_RETURN 	VK_RETURN
	#define KEY_ESC 	VK_ESCAPE
	#define KEY_F1  	7340032 // VK_F1
	#define KEY_PGUP  	2162688 // VK_PRIOR
	#define KEY_PGDOWN 	2228224 // VK_NEXT
	#define KEY_INSERT 	2949120 // VK_INSERT
	#define KEY_DELETE 	3014656 // VK_DELETE
	#define KEY_HOME 	2359296 //VK_HOME
	#define KEY_CTRL_S 	VK_PAUSE
#else
	#define KEY_RETURN 	10
	#define KEY_ESC 	27
	#define KEY_F1  	0xffbe 
	#define KEY_PGUP  	0xff55
	#define KEY_PGDOWN 	0xff56
	#define KEY_INSERT  0xff63
	#define KEY_DELETE  0xffff
	#define KEY_HOME 	0xff50
	#define KEY_CTRL_S 	6
#endif

#endif