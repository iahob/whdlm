#ifndef GUID_99C0FFA4_8318_4D0F_9806_9C037C99B641
#define GUID_99C0FFA4_8318_4D0F_9806_9C037C99B641

#pragma once

#include "D3DDevice.h"

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ��˵��
class CDirectUIEngine;
class CDirectUIWindow;

// ��������
typedef CArray<CDirectUIWindow *> CDirectUIWindowPtrArray;									// ��������
typedef CMap<CString, const tchar *, CDirectUIWindow *, CDirectUIWindow *> CMapDirectUIWindow;		// �����ֵ�


////////////////////////////////////////////////////////////////////////////////// 

// ��������
class DEXTER_D3D_CLASS CDirectUIEngine : public ID3DDirectUISink
{
	// ��Ԫ����
	friend class CDirectUIWindow;

	// ���ñ���
protected:
	CD3DDevice *					m_pD3DDevice;						// �豸����
	bool							m_bMouseTracking;					// ������

	// ״̬����
protected:
	CDirectUIWindow *				m_pWindowLeave;						// �뿪����
	CDirectUIWindow *				m_pWindowCurrent;					// ��ǰ����
	CDirectUIWindow *				m_pWindowMain;						// ������

	// �ں˱���
protected:
	CDirectUIWindowPtrArray			m_DirectUIWindowPtrArray;			// ��������
	CMapDirectUIWindow				m_MapDirectUIWindowMessge;			// ���д���

	// ��̬����
public:
	static CDirectUIEngine			m_DirectUIEngine;					// ����ָ��

	// ��������
protected:
	// ���캯��
	CDirectUIEngine();
	// ��������
	virtual ~CDirectUIEngine();

	// ���ú���
public:
	// ��ȡ�豸
	CD3DDevice * GetD3DDevice() { return m_pD3DDevice; }
	// ���õ�ǰ����
	void SetWindowCurrent(CDirectUIWindow * pWindowCurrent) { m_pWindowCurrent = pWindowCurrent; }
	// ����������
	void SetWindowMain(CDirectUIWindow * pWindowMain) { m_pWindowMain = pWindowMain; }
	
	// ��ȡ����
public:
	// ��ȡ��ǰ����
	CDirectUIWindow * GetWindowCurrent() { return m_pWindowCurrent; }

	// �豸�¼�
public:
	// �����豸
	virtual void OnInitDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// ��ʧ�豸
	virtual void OnLostDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// �����豸
	virtual void OnResetDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// �������
	virtual void OnUpdataTextureBuffer(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// ��Ⱦ����
	virtual void OnRenderDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// ���ڶ���
	virtual void OnMovieDirectUI(CD3DDevice * pD3DDevice, int nWidth, int nHeight);

	// ��������
public:
	// ��Ϣ����
	bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam);
	// ��Ϣ����
	bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam);

	// ���º���
private:
	// ���´���
	void InvalidWindow();
	// ���´���
	void InvalidWindow(int nXPos, int nYPos, int nWidth, int nHeight);

	// ��������
private:
	// ��ȡ����
	CDirectUIWindow * SwitchToWindow(int nXMousePos, int nYMousePos);
	// ��ȡ����
	CDirectUIWindow * SwitchToWindow(CDirectUIWindow * pDirectUIWindow, int nXMousePos, int nYMousePos);

	// �ڲ�����
protected:
	// ���������Ϣ
	CDirectUIWindow * SendMouseMessage( int64 uMessage, int64 nFlags, CPoint ptMouseSite );

	// ��Ϣ���ƺ���
public:
	// ������Ϣ
	__int64	SendMessage( const tchar * pszClassName, uint nMessage );
	// ������Ϣ
	__int64	SendMessage( const tchar * pszClassName, uint nMessage, __int64 nParamOne, __int64 nParamTwo );

	// ��̬����
public:
	// ��ȡ����
	static CDirectUIEngine * _Object() { return &m_DirectUIEngine; }
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif