/* **************************************************************************
 * @version $Id: inputbox.h,v 1.1 2014/09/09 23:00:51 David Exp $
 *
 * File:		InputBox.h
 * Content:		main include file for CInputBox class
 * Notes:		declares C or C++ class depending on the current language
 *
 * Copyright(c) 2006 by legolas558
 *
 * https://sourceforge.net/projects/cinputbox
 *
 * This software is released under a BSD license. See LICENSE.TXT
 * You must accept the license before using this software.
 *
 * **************************************************************************/
/****************************** Original LICENSE.TXT ****************************
Copyright (c) 2006, legolas558 <legolas558@users.sourceforge.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the author nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission from the author.
    * Commercial use is allowed only by a specific written permission from the author.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************/

#include "imgcalkap.h"

#ifndef		__INPUTBOX_H
#define		__INPUTBOX_H

#include <windows.h>

#ifndef	__CPPCOMPAT_H
#define __CPPCOMPAT_H

#ifndef bool
	#define	bool	unsigned int
	#define	true	1
	#define	false	0
#endif

#ifdef	inline
#define	L_INLINE	inline
#endif

#ifndef	L_INLINE
#ifdef	_inline
#define	L_INLINE	_inline
#endif
#endif

#ifndef	L_INLINE
#ifdef	__inline
#define	L_INLINE	__inline
#endif
#endif

#ifndef	L_INLINE
#define	L_INLINE
#endif


#endif



#ifdef __cplusplus

#define	DCIB_DECL	class	CInputBox {\
private:

#define	CIB_DECL		CInputBox::

#define	CIB_OBJ

#define	CIB_OBJ1

#define	CIB_DESTRUCTOR	~CInputBox

#define	CIB_CONSTRUCTOR	CInputBox

#define	DCIB_OBJ1(t)
#define	DCIB_OBJ(t)

#define	CIB_DEREF(t, code)	t->code

#define	DCIB_PUB	public:

#else

//#include "../../cppcompat.h"

#define	DCIB_DECL		typedef struct	S_CInputBox {

#define	CInputBox		struct	S_CInputBox

#define	CIB_DECL       

#define	CIB_OBJ1		CInputBox *this

#define	CIB_OBJ			CIB_OBJ1,

#define	CIB_DESTRUCTOR	int CInputBox_free
#define	CIB_CONSTRUCTOR	int CInputBox_init

#define	DCIB_OBJ1(t)	(t)
#define	DCIB_OBJ(t)		DCIB_OBJ1(t),

#define	CIB_DEREF(t, code)	code

#define	DCIB_PUB

#endif

DCIB_DECL

HWND				hwndParent,
					hwndInputBox,
					hwndQuery,
					hwndOk,
					hwndCancel,
					hwndEditBoxLat,
					hwndEditBoxLatMin,
					hwndEditBoxLong,
					hwndEditBoxLongMin,
					hwndEditChartName,
					hwndEditChartDesc,
					hwndEditChartScale,
					hwndEditCmbProj,
					hwndEditCmbDatum,
					hwndEditCmbDepth,
					hwndSave;

LPSTR				szInputText;
WORD				wInputMaxLength, wInputLength;
bool				bRegistered,
					bResult;

HINSTANCE			hThisInstance;

DCIB_PUB

#ifndef	__cplusplus
};
#endif

void	create(CIB_OBJ HWND hwndNew);
void	show(CIB_OBJ LPCSTR lpszTitle, LPCSTR	lpszQuery);
void	hide(CIB_OBJ1);
void	submit(CIB_OBJ1);
void	close(CIB_OBJ1);
void	destroy(CIB_OBJ1);

CIB_CONSTRUCTOR(CIB_OBJ HINSTANCE hInst);
CIB_DESTRUCTOR(CIB_OBJ1);


int ShowInputBox(CIB_OBJ HWND hParentWindow, LPCTSTR lpszTitle, LPCTSTR lpszQuery, LPSTR szResult, WORD wMax);
int ShowSaveWindowWin(void);
int ShowOpenFileWindowWin(void);
#ifdef	__cplusplus
};
#endif

#endif
