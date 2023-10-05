#ifndef OPENGL_ENGINE_HEAD_HEAD_FILE
#define OPENGL_ENGINE_HEAD_HEAD_FILE

////////////////////////////////////////////////////////////////////////////////// 
// �����ļ�

// OPEN ��
#include <GL/GL.h>		
#include <GL/GLU.h>
#include <GL/GLAux.h>

// STL
#include <map>
#include <Vector>
using namespace std;


////��������
//#ifndef DEXTER_OPENGL_CLASS
//	#ifdef OPENGL_ENGINE_DLL
//		#define DEXTER_OPENGL_CLASS //_declspec(dllexport)
//	#else
//		#define DEXTER_OPENGL_CLASS //_declspec(dllimport)
//	#endif
//#endif
#define DEXTER_OPENGL_CLASS

////////////////////////////////////////////////////////////////////////////////// 
// �ṹ����

#define  GL_PI			((FLOAT)  3.141592654f)

#define  GLCOLOR_RGB(r,g,b)	    ((COLORREF)((((255)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_RGBA(r,g,b,a)  ((COLORREF)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_ARGB(a,r,g,b)  ((COLORREF)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define  GLCOLOR_GetR(rgb)      ((BYTE)((rgb)>>16))
#define  GLCOLOR_GetG(rgb)      ((BYTE)((rgb)>> 8))
#define  GLCOLOR_GetB(rgb)      ((BYTE)((rgb)>> 0))
#define  GLCOLOR_GetA(rgb)      ((BYTE)((rgb)>>24))

/////////////////////////////////////////////////////////////////////////////
// ��������
enum ENUM_TEXTURE_FORMATS{
	TEXTURE_FORMAT_UNKNOWN = 0,
	TEXTURE_FORMAT_PNG = 4,
};


////////////////////////////////////////////////////////////////////////////////// 
// �����ļ�

#ifndef OPENGL_ENGINE_DLL
	#include "OpenGL.h"
	#include "GLTexture.h"
	#include "GLFont.h"
	#include "GLVirtualWindow.h"
	#include "GLVirtualButton.h"
#endif

////////////////////////////////////////////////////////////////////////// 

#endif