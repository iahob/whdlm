#ifndef GUID_5584579D_569F_4D8A_8AD1_C712759EE1D6
#define GUID_5584579D_569F_4D8A_8AD1_C712759EE1D6

#pragma once

#include "DexterD3DHead.h"
#include "D3DDevice.h"
#include "LapseCount.h"
#include "D3DTexture.h"
#include "D3DFont.h"
#include "D3DDrawBasic.h"
#include "DirectUIEngine.h"
#include "DirectUIObject.h"
#include "D3DTextureBuffer.h"


////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

// �����쳣
#define MODAL_EXCEPTION						llong_max

////////////////////////////////////////////////////////////////////////////////// 

// ������ʾ��ʽ
enum EnumWindowShowType
{
	WindowShow_Alpha,						// �������� 
	WindowShow_SizeY,						// �仯����
	WindowShow_SizeX,						// �仯����
	WindowShow_Size,						// �仯����
	WindowShow_Normal,						// ��������
};
////////////////////////////////////////////////////////////////////////////////// 

// ���ⴰ��
class DEXTER_D3D_CLASS CDirectUIWindow : public CDirectUIObject
{
	// ��Ԫ����
	friend class CDirectUIEngine;
	friend class CDirectUIButton;

	// ��������
protected:
	bool							m_bActive;							// �����־
	bool							m_bEnable;							// ���ñ�־
	bool							m_bVisible;							// ��ʾ��־
	CString							m_StrClassName;						// ����
	CD3DColor						m_D3DColorBack;						// ������ɫ

	// ģ������
protected:
	bool							m_bBlurry;							// ģ����־
	int								m_nBlurryAlpha;						// ģ��������

	// ��������
protected:
	bool							m_bModalShake;						// ��������
	uint							m_nModalShakeTime;					// ����ʱ��
	bool							m_bModalMode;						// ����ģʽ
	int64							m_nModalValue;						// ��������
	CDirectUIWindow *				m_pModalWindowParent;				// ����������

	// ���ڶ�������
protected:
	EnumWindowShowType				m_WSWValue;							// ������ʾ��ʽ
	bool							m_WSWOngoing;						// ����������
	uint							m_WSWTime;							// ����ʱ��
	uint							m_WSWBeginTime;						// ������ʼʱ��
	float							m_WSWAlpha;							// ����͸����
	float							m_WSWRotation;						// ��ת�Ƕ�
	CSize							m_WSWSize;							// ��С����

	// �����ƶ�����
protected:
	bool							m_bMoveWindow;						// �ƶ�����
	uint							m_nMoveTime;						// �ƶ�ʱ��
	uint							m_nMoveBegin;						// �ƶ�ʱ��
	CPoint							m_PointBeginPos;					// ������ʼλ��
	CPoint							m_PointEndPos;						// �����ƶ�λ��

	// ���Ա���
protected:
	uint							m_uWindowID;						// ���ڱ�ʶ
	uint							m_uLayerIndex;						// ���ڵȼ�
	byte							m_cbWinAlpha;						// ����͸����

	// λ�ñ���
protected:
	CSize							m_WindowSize;						// ���ڴ�С
	CPoint							m_WindowPos;						// ����λ��
	CPoint							m_BenchmarkPos;						// ��׼λ��

	// ���ڻ���
protected:
	CD3DTextureBuffer				m_WindowBuffer;						// ���ڻ���

	// �ں˱���
protected:
	CDirectUIWindow *				m_pParentWindow;					// �ϲ㴰��
	CDirectUIEngine *				m_pDirectUIEngine;					// ������
	CDirectUIWindowPtrArray			m_DirectUIWindowPtrArray;			// ��������

	// ��������
public:
	// ���캯��
	CDirectUIWindow();
	// ��������
	virtual ~CDirectUIWindow();

	// ���ڱ�ʶ
public:
	// ��ȡ��ʶ
	uint GetWindowID() { return m_uWindowID; }
	// ���ñ�ʶ
	void SetWindowID(uint uWindowID) { m_uWindowID = uWindowID; }

	// ���ڵȼ�
public:
	// ���õȼ�
	void SetLayerIndex(uint uLayerIndex);
	// ��ȡ�ȼ�
	uint GetLayerIndex() { return m_uLayerIndex; }

	// ����͸����
public:
	// ��ȡ��ǰ͸����
	float GetCurrentAlpha() { return m_WSWAlpha; }
	// ����͸����
	void SetWinAlpha( byte cbWinAlpha ) { m_cbWinAlpha = cbWinAlpha; }
	// ��ȡ͸����
	byte GetWinAlpha() { return m_cbWinAlpha; }

	// ���Զ���
public:
	// �ϲ㴰��
	CDirectUIWindow * GetParentWindow() { return m_pParentWindow; }
	// ������
	CDirectUIEngine * GetVirtualEngine() { return m_pDirectUIEngine; }

	// ������
public:
	// ɾ������
	void DeleteWindow();
	// �����
	bool ActiveWindow( const tchar * pszClassName, CRect & rcWindow, DWORD dwStyle, uint uWindowID, CDirectUIWindow * pParentWindow);

	// ��������
public:
	// ��������
	bool IsWindowActive() { return m_bActive; }
	// ��������
	bool IsWindowEnable() { return m_bEnable && !m_WSWOngoing; }
	// ��ʾ����
	bool IsWindowVisible() { return m_bVisible; }
	// ����ģʽ
	bool IsWindowModal() { return m_bModalMode; }

	// ʱ�����
public:
	// ��ʾʱ��
	void SetShowTime( uint nTime ) { m_WSWTime = nTime; }
	// ��ʾʱ��
	uint GetShowTime() { return m_WSWTime; }

	// ���ڵ���
public:
	// ���ñ�����ɫ
	void SetBackColor( CD3DColor D3DColorBack ) { m_D3DColorBack = D3DColorBack; }
	// ��ֹ����
	void EnableWindow(bool bEnable);
	// ģ������
	void BlurryWindow(bool bBlurry);
	// ���´���
	void InvalidWindow(bool bCoerce);
	// ���ý���
	void SetFocus();

	// ��ʾ����
public:
	// ��ʾ����
	void ShowWindow(bool bVisible, EnumWindowShowType WSWValue = WindowShow_Normal );
	// ��������
	int64 DoModal( CDirectUIWindow * pWindowParent = NULL, EnumWindowShowType WSWValue = WindowShow_Normal );
	// ��������
	void EndModal( int64 nEndValue, EnumWindowShowType WSWValue = WindowShow_Normal );

	// ����λ��
public:
	// ���ڴ�С
	void GetClientRect(CRect & rcClient);
	// ���ڴ�С
	void GetWindowRect(CRect & rcWindow);
	// ����λ��
	void SetWindowPos( int nXPos, int nYPos, int nWidth, int nHeight, uint uFlags );
	// �ƶ����ڶ���
	void MoveWindow( CPoint PointEndPos, uint nMoveTime );
	// �ƶ����ڶ���
	void MoveWindow( CPoint PointEndBegin, CPoint PointEndPos, uint nMoveTime );

	// ���ܺ���
public:
	// ��������
	bool IsChildWindow(CDirectUIWindow * pDirectUIWindow);

	// ϵͳ�¼�
protected:
	// ��ʾ����
	void OnShowMovie();

	// ��Ϣ���غ���
protected:
	// ��Ϣ����
	virtual bool DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam) { return false; }
	// ��Ϣ����
	virtual bool PreTranslateMessage(UINT uMessage, WPARAM wParam, LPARAM lParam) { return false; }

	// ��Ϣ����
protected:
	// ���ڴ��� 
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ���ڴݻ� 
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// �����ƶ� 
	void OnWindowPosition(CD3DDevice * pD3DDevice, int nPosX, int nPosY);
	// ���ڴ�С 
	void OnWindowSize(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ���»��� 
	void OnWindowUpdataTextureBuffer(CD3DDevice * pD3DDevice);
	// ���ڶ��� 
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ������ʾ 
	void OnWindowShow(CD3DDevice * pD3DDevice, bool bVisible );
	// ����ƶ� 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ����뿪 
	void OnMouseLeave();
	// ������� 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���̧�� 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���˫�� 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �Ҽ����� 
	void OnRButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �Ҽ�̧�� 
	void OnRButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �Ҽ�˫�� 
	void OnRButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �м����� 
	void OnMButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �м�̧�� 
	void OnMButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �м�˫�� 
	void OnMButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// �������� 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);
	// ����̸�� 
	void OnKeyUp(uint nChar, uint nRepCnt, uint nFlags);
	// �ַ���Ϣ 
	void OnChar(uint nChar, uint nRepCnt, uint nFlags);

	// �ڲ�����
private:
	// ���ô���
	void ResetWindow();
	// ���»���
	void OnUpdataTextureBuffer( CD3DDevice * pD3DDevice );
	// ���´���
	void OnDirectUIUpdata( CD3DDevice * pD3DDevice );
	// �滭����
	void OnDirectUIRender( CD3DDevice * pD3DDevice );
	// ���ڶ���
	void OnDirectUIMovie( CD3DDevice * pD3DDevice );
	// ���ô����ɰ�
	void SetWindowMask( CD3DDevice * pD3DDevice, CRect RectMask );

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif