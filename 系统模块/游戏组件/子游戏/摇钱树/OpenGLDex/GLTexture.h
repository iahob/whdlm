#pragma once

// GL�ļ�
#include "OpenGL.h"
#include "OpenglEngineHead.h"

// ��������
typedef vector<GLuint>							verLuint;
class CVerLuint
{
protected:
	verLuint									m_verLuint;

public:
	CVerLuint(){}
	~CVerLuint(){}

public:
	verLuint*	GetObject() { return &m_verLuint; }
};

// ��Դ��Ϣ
struct tagFGLResourceInfo
{
	DWORD		dwSize;								// ��Դ��С
	LPVOID		pcbBuffer;							// ��Դ�ڴ�
};

// �����ȼ�
enum enumQualityLevel
{
	QL_LOW = 0,		// ��
	QL_IN,			// ��
	QL_HIGH,		// ��
};

// GL TGA������
class DEXTER_OPENGL_CLASS CGLTexture
{
	// ͼƬ��Ϣ
protected:
	GLuint							m_nWidth;									// ͼƬ���
	GLuint							m_nHeight;									// ͼƬ�߶�

	GLuint							m_nWidthNumber;								// ����
	GLuint							m_nHeightNumber;							// ����

	// ��ת��Ϣ
protected:
	GLfloat							m_fAngle;									// ��ת����
	GLfloat							m_fRotationX;								// ��ת����
	GLfloat							m_fRotationY;								// ��ת����

	// �Գ���Ϣ
protected:
	BOOL							m_bSymmetrical;								// �Գ���Ϣ

	// ��������
protected:
	GLboolean						m_bLoadSuccess;								// �����Ƿ�ɹ�

	// �������
protected:
	CVerLuint*						m_verTextureID;								// ����ID

	// �����ȼ�
public:
	static enumQualityLevel			m_QualityLevel;								// �����ȼ�

public:
	CGLTexture(void);
	~CGLTexture(void);

	// ͼƬ��Ϣ
public:
	// �����ж�
	GLboolean IsNull();
	// �Ƿ�����ͼƬ
	GLboolean IsImageIndex();
	// ��ȡ����
	INT GetImageIndexMax() { return m_nHeightNumber * m_nWidthNumber; }
	// ������
	INT GetWidth() { return m_nWidth; }
	// ����߶�
	INT GetHeight() { return m_nHeight; }
	// ������
	INT GetWidthIndex() { return (m_nWidthNumber > 0) ? (m_nWidth/m_nWidthNumber) : (m_nWidth); }
	// ����߶�
	INT GetHeightIndex() { return (m_nHeightNumber > 0) ? (m_nHeight/m_nHeightNumber) : (m_nHeight); }
	// ����ID
	vector< GLuint >* GetTextureID() { return m_verTextureID->GetObject(); }

	// �������
public:
	// ��������
	GLboolean Destory();

	// ���ƺ���
public:
	// ������ת
	GLvoid SetRotation(FLOAT fRotationX, FLOAT fRotationY, FLOAT fAngle);
	// ������ת
	GLvoid SetRotation(INT nRotationX, INT nRotationY, FLOAT fAngle);
	// ���öԳ�
	GLvoid SetSymmetrical(BOOL bSymmetrical);

	// ���ܺ���
public:
	// ��������
	GLboolean LoadImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// ��������
	GLboolean LoadImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );
	
	// ���ܺ���
protected:
	// ��������
	GLboolean LoadHighModeImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// ��������
	GLboolean LoadHighModeImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );
	// ��������
	GLboolean LoadLowModeImage( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType);
	// ��������
	GLboolean LoadLowModeImageIndex( CGLDevice* pOpenGL, HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, DWORD wImageType, INT nWidthNumber, INT nHeightNumber );


	// �滭����
public:
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

public:
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, COLORREF GLColor);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor);


	// �滭����
public:
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	// �滭ͼƬ
	GLboolean DrawImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

	// �滭����
public:
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest);
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource);
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight);

	// �滭����
public:
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha);
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, BYTE cbAlpha);
	// �滭ͼƬ
	GLboolean DrawColourFilterImage(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, BYTE cbAlpha);

	// �滭����
public:
	// �滭ͼƬ
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor, INT nIndex);
	// �滭ͼƬ
	GLboolean DrawImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, BYTE cbAlpha, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawColourFilterImageIndex(CGLDevice* pOpenGL, INT nXDest, INT nYDest, COLORREF GLColor, INT nIndex);
	// �滭ͼƬ
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, INT nIndex );
	// �滭ͼƬ
	GLboolean DrawColourFilterImageIndex( CGLDevice* pOpenGL, INT nXDest, INT nYDest, INT nDestWidth, INT nDestHeight, INT nXSource, INT nYSource, INT nSourceWidth, INT nSourceHeight, COLORREF GLColor, INT nIndex );

	// �ڲ�����
protected:
	// ת������
	GLint	  ConversionTexture( GLint nLength );
	// ������ͼ
	GLvoid	  TextureMap(GLdouble dXDest, GLdouble dYDest, GLdouble dDestWidth, GLdouble dDestHeight, GLdouble dXSource, GLdouble dYSource, GLdouble dSourceWidth, GLdouble dSourceHeight);
	// ������ͼ
	GLvoid	  TextureMapIndex(INT nIndex);
	// ������ͼ
	GLvoid	  DrawTexture( GLdouble dImageBeginX, GLdouble dImageBeginY, GLdouble dImageEndX, GLdouble dImageEndY, GLdouble dDestBeginX, GLdouble dDestBeginY, GLdouble dDestEndX, GLdouble dDestEndY);
	// ��ȡ��Դ
	GLboolean GetResourceInfo(HINSTANCE hInstance, const TCHAR * pszResource, const TCHAR * pszTypeName, tagFGLResourceInfo & ResourceInfo);
	// ͼƬ����
	bool Decryption( BYTE* pData, UINT& uSize );
	// ͼƬ����
	bool DecryptionOld( BYTE* pData, UINT& uSize );
	// ͼƬ����
	bool DecryptionNew( BYTE* pData, UINT& uSize );

};



