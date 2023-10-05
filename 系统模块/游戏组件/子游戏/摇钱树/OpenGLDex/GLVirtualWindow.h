#pragma once
#include "OpenGL.h"
#include "GLTexture.h"
#include "OpenglEngineHead.h"

// ��������
class CGLVirtualWindow;
typedef vector<CGLVirtualWindow *>				verVirtualWindow;
class CVerVirtualWindow
{
protected:
	verVirtualWindow							m_verVirtualWindow;

public:
	CVerVirtualWindow(){}
	~CVerVirtualWindow(){}

public:
	verVirtualWindow*	GetObject() { return &m_verVirtualWindow; }
};

class DEXTER_OPENGL_CLASS CGLVirtualWindow
{
	friend class CGLVirtualButton;

	// ��������
protected:
	bool							m_bActive;							// �����־
	bool							m_bEnable;							// ���ñ�־
	bool							m_bVisible;							// ��ʾ��־

	// ���Ա���
protected:
	BYTE							m_cbAlpha;							// ͸����
	UINT							m_uWindowID;						// ���ڱ�ʶ
	static UINT						m_uFocusID;							// �����ʶ

	// λ�ñ���
protected:
	CSize							m_WindowSize;						// ���ڴ�С
	CPoint							m_BenchmarkPos;						// ��׼λ��

	// �ں˱���
protected:
	CGLDevice *						m_pOpenGl;							// �豸ָ��

	// ��������
protected:
	CGLVirtualWindow *				m_pParentWindow;					// �ϲ㴰��
	CVerVirtualWindow*				m_ChildWindowArray;					// �Ӵ�������

	// һ������
public:
	static CVerVirtualWindow*		m_WindowArray;						// ��������

public:
	CGLVirtualWindow(void);
	virtual ~CGLVirtualWindow(void);
	
	// ���ڱ�ʶ
public:
	// ��ȡ��ʶ
	virtual UINT GetWindowID() { return m_uWindowID; }
	// ���ñ�ʶ
	virtual VOID SetWindowID(UINT uWindowID) { m_uWindowID=uWindowID; }

	// ���Զ���
public:
	// �ϲ㴰��
	virtual CGLVirtualWindow * GetParentWindow() { return m_pParentWindow; }

	// ������
public:
	// ɾ������
	virtual VOID DeleteWindow();
	// �����
	virtual bool ActiveWindow(CRect & rcWindow, DWORD dwStyle, UINT uWindowID, CGLDevice* pOpenGl, CGLVirtualWindow * pParentWindow);

	// ��������
public:
	// ��������
	virtual bool IsWindowActive() { return m_bActive; }
	// ��������
	virtual bool IsWindowEnable() { return m_bEnable; }
	// ��ʾ����
	virtual bool IsWindowVisible() { return m_cbAlpha > 0; }

	// ���ڿ���
public:
	// ��ʾ����
	virtual VOID ShowWindow(bool bVisible);
	// ��ֹ����
	virtual VOID EnableWindow(bool bEnable);
	// ���ý���
	virtual VOID SetFocus(bool bFocus = true);
	// ���ڹر�
	virtual VOID OnOK();

	// ����λ��
public:
	// ���ڴ�С
	virtual VOID GetClientRect(CRect & rcClient);
	// ���ڴ�С
	virtual VOID GetWindowRect(CRect & rcWindow);
	// ����λ��
	virtual VOID SetWindowPos(INT nXPos, INT nYPos, INT nWidth, INT nHeight, UINT uFlags);

	// ���ܺ���
public:
	// ��������
	virtual bool IsChildWindow(CGLVirtualWindow * pVirtualWindow);

	// �ײ�����
private:
	// �ײ㶯������
	VOID AnimationDrive();

	// ϵͳ�¼�
protected:
	// ������Ϣ
	virtual VOID OnWindowMovie() { return; }
	// ������Ϣ
	virtual VOID OnWindowCreate(CGLDevice * pGLDevice) { return; }
	// ������Ϣ
	virtual VOID OnWindowDestory() { return; }
	// λ����Ϣ
	virtual VOID OnWindowPosition() { return; }

	// ���غ���
protected:
	// ����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, INT nXMousePos, INT nYMousePos)=NULL;
	// ��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, INT nXMousePos, INT nYMousePos)=NULL;
	// �滭����
	virtual VOID OnEventDrawWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos)=NULL;


	// ��ʾ���غ���
protected:
	// ����ʾ����
	virtual VOID EarlyDisplayWindow() = NULL;
	// ��ȫ��ʾ����
	virtual VOID FullyDisplayWindow() = NULL;
	// ���رմ���
	virtual VOID EarlyClosedWindow() = NULL;
	// ��ȫ�رմ���
	virtual VOID FullyClosedWindow() = NULL;

	// �ڲ�����
private:
	// ���ô���
	virtual VOID ResetWindow();
	// �滭����
	virtual VOID OnEventDrawChildWindow(CGLDevice * pGLDevice, INT nXOriginPos, INT nYOriginPos);

	// �滭����
public:
	// �滭����
	static void DraWindows(CGLDevice * pGLDevice);

	// ��Ϣ����
public:
	// ��Ϣ����
	static GLboolean PreTranslateMessage(MSG* pMsg);
	// ��Ϣ����
	static GLboolean DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

	// ��������
private:
	// ��ȡ����
	static CGLVirtualWindow* SwitchToWindow(INT nXMousePos, INT nYMousePos);
	// ��ȡ����
	static CGLVirtualWindow* SwitchToWindow(CGLVirtualWindow * pVirtualWindow, INT nXMousePos, INT nYMousePos);

};
