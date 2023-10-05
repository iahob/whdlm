#pragma once

#include "OpenGL.h"
#include "GLTexture.h"
#include "GLLapseCount.h"
#include "GLVirtualWindow.h"
#include "OpenglEngineHead.h"

typedef map<CString, CGLTexture*>					mapGLTexture;

class DEXTER_OPENGL_CLASS CGLVirtualButton : public CGLVirtualWindow
{
	// ״̬����
protected:
	TCHAR							m_strTexture[MAX_PATH];				// �����־
	bool							m_bMouseDown;						// ���±�־
	bool							m_bMouseMove;						// ������־
	bool							m_bButtonFocus;						// �����ʾ				
	CRect							m_rcButtonRect;						// ��ť����

	// ��������
protected:
	WORD							m_wImageIndex;						// ��������
	CGLLapseCount					m_ImageLapseCount;					// ���ż���

	// ��Դ����
protected:
	static mapGLTexture				m_mapGLTextureButton;				// ��ť����

public:
	CGLVirtualButton(void);
	~CGLVirtualButton(void);


	// ϵͳ�¼�
protected:
	// ������Ϣ
	virtual VOID OnWindowMovie();
	// ������Ϣ
	virtual VOID OnWindowCreate(CGLDevice * pGLDevice);
	// ������Ϣ
	virtual VOID OnWindowDestory();

	// ���غ���
protected:
	// ����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos);
	// ��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos);
	// �滭����
	virtual VOID OnEventDrawWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos);

	// ��ʾ���غ���
protected:
	// ����ʾ����
	virtual VOID EarlyDisplayWindow() {};
	// ��ȫ��ʾ����
	virtual VOID FullyDisplayWindow() {};
	// ���رմ���
	virtual VOID EarlyClosedWindow() {};
	// ��ȫ�رմ���
	virtual VOID FullyClosedWindow() {};

	// ���ܺ���
public:
	// ���ý���
	VOID SetButtonFocus(bool bButtonFocus) { m_bButtonFocus = bButtonFocus; }
	// ��������
	VOID SetButtonRect(CRect rcButtonRect);
	// ����λͼ
	VOID SetButtonImage(CGLDevice * pGLDevice, HINSTANCE hResInstance, const TCHAR * pszResource, const TCHAR * pszTypeName,  DWORD wImageType);

	// �ڲ�����
protected:
	// �����ؼ�
	VOID RectifyControl();
	// ����ͼƬ
	CGLTexture* FindTexture();
};
