#pragma once
#include "GameClientView.h"

// ��Ϸ��ͼ
class CGameFrameViewDexter : public CGameFrameView, public dex::ID3DDeviceSink
{
	// ״̬����
protected:
	bool							m_bInitD3D;							// ������־
	HMODULE							m_hModule;							// ��ԴDLL

	// ��Դ����
public:
	CGameClientView					m_GameClientView;					// ��Ϸ����

	// ��������
public:
	// ���캯��
	CGameFrameViewDexter();
	// ��������
	virtual ~CGameFrameViewDexter();

	// ���غ���
public:
	// ���ý���
	virtual VOID ResetGameView();
	// �����ؼ�
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	// �������
	virtual VOID InvalidGameView(INT nXPos, INT nYPos, INT nWidth, INT nHeight);

	// ���к���
public:
	// ������Ϣ
	virtual bool OnEventInsureMessage(word wSubCmdID, VOID * pData, word wDataSize);

	// �豸�ӿ�
protected:
	// �����豸
	virtual void OnInitDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// ��ʧ�豸
	virtual void OnLostDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// �����豸
	virtual void OnResetDevice(dex::CD3DDevice * pD3DDevice, int nWidth, int nHeight);
	// ���Ƶƹ�
	virtual void OnControlLight(dex::CD3DDevice * pD3DDevice);

	// ��Ϣӳ��
protected:
	// ��С��Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);
	// ��ʱ����Ϣ
	afx_msg void OnTimer(UINT nIDEvent);
	// ������Ϣ
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);
	// �Ҽ�̸��
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	// ���غ���
protected:
	// ��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	// Ĭ�ϻص�
	virtual LRESULT DefWindowProc(UINT nMessage, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};
