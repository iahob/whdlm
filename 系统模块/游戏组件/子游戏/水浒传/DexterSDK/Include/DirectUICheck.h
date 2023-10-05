#ifndef GUID_193694C0_3A5E_44B9_A2D9_9AB3728E8938
#define GUID_193694C0_3A5E_44B9_A2D9_9AB3728E8938

#pragma once

#include "DirectUIWindow.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 

// ѡ���
class DEXTER_D3D_CLASS CDirectUICheck : public CDirectUIWindow
{
	// ״̬����
protected:
	bool							m_bMouseDown;						// ���±�־
	bool							m_bMouseMove;						// ������־
	bool							m_bCheck;							// ѡ��״̬

	// ��������
protected:
	WORD							m_wImageIndex;						// ��������
	CLapseCount						m_ImageLapseCount;					// ���ż���

	// ���ֱ���
protected:
	CPoint							m_ptTextOffset;						// ����ƫ��
	CString							m_StrButtonText;					// ��ť�ı�
	D3DCOLOR						m_nButtonTextColor;					// ������ɫ

	// ��Դ����
protected:
	CD3DFont						m_D3DFontButton;					// ��ť����
	CD3DTexture						m_TextureCheck;						// ��ť����
	CD3DTexture						m_TextureCheckOn;					// ��ť����
	CD3DTexture						m_TextureCheckOff;					// ��ť����

	// ��ѡ����
protected:
	uint							m_nRadioID;							// ��ѡID
	static CMapStringToPtr			m_MapRadio;							// ��ѡMAP

	// ��������
public:
	// ���캯��
	CDirectUICheck();
	// ��������
	virtual ~CDirectUICheck();

	// ϵͳ�¼�
protected:
	// ������Ϣ
	void OnWindowCreate(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);
	// ������Ϣ
	void OnWindowDestory(CD3DDevice * pD3DDevice);
	// ������Ϣ
	void OnWindowMovie();
	// �滭��Ϣ
	void OnWindowRender(CD3DDevice * pD3DDevice, uint nWidth, uint nHeight);

	// �����Ϣ
protected:
	// ����ƶ� 
	void OnMouseMove(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ����뿪 
	void OnMouseLeave();
	// ������� 
	void OnLButtonDown(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���˫�� 
	void OnLButtonDblClk(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);
	// ���̧�� 
	void OnLButtonUp(CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// ���ܺ���
public:
	// ����λͼ
	void SetCheckTexture( CD3DTexture& TextureCheckOn, CD3DTexture& TextureCheckOff );
	// �����ı�
	void SetCheckText( const tchar * pszText );
	// �����ı���ɫ
	void SetCheckTextColor( D3DCOLOR nButtonTextColor );
	// ��������ƫ��
	void SetCheckTextOffset( CPoint ptTextOffset );
	// ��������
	void SetCheckFont( CD3DFont& font );
	// ���õ�ѡKEY [ KEY ���� 0 �Ǿ��Ǹ�ѡ�� ]
	void SetRadio( uint nRadioID );
	// ����ѡ��
	void SetCheck( bool bCheck );
	// ��ȡѡ��
	bool GetCheck() { return m_bCheck; }

	// �ڲ�����
protected:
	// ɾ����ѡ
	void DeleteRadio();
	// ��������
	void UpdataButtonTexture( CD3DTexture& Texture );
	// �����ؼ�
	void RectifyControl();

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif