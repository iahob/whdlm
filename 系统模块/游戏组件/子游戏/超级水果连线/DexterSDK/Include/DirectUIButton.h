#ifndef GUID_571D5B08_B9BC_477E_8158_717C33EEA782
#define GUID_571D5B08_B9BC_477E_8158_717C33EEA782

#pragma once

#include "DirectUIWindow.h"
#include "D3DFreeType.h"

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_begin

////////////////////////////////////////////////////////////////////////////////// 
// ���ָ�ʽ
enum EnumButtonTextAlign
{
	BTA_LEFT,
	BTA_RIGHT,
	BTA_CENTER,
};

// ͼ��ť
class DEXTER_D3D_CLASS CDirectUIButton : public CDirectUIWindow
{
	// ״̬����
protected:
	bool							m_bMouseDown;						// ���±�־
	bool							m_bMouseMove;						// ������־

	// ��������
protected:
	WORD							m_wImageIndex;						// ��������
	CLapseCount						m_ImageLapseCount;					// ���ż���

	// ������Ӧ
protected:
	uint							m_nBeginTime;						// ��Ӧ��ʼʱ��
	uint							m_nIntervalTime;					// ��Ӧ���ʱ��
	uint							m_nResponseTime;					// ��Ӧʱ��
	CLapseCount						m_ResponseLapseCount;				// ��Ӧ���ż���

	// ���ֱ���
protected:
	CPoint							m_ptTextOffset;						// ����ƫ��
	CString							m_StrButtonText;					// ��ť�ı�
	D3DCOLOR						m_nButtonBackColor;					// ������ɫ
	D3DCOLOR						m_nButtonTextColor;					// ������ɫ
	EnumButtonTextAlign				m_nButtonTextAlign;					// ���ָ�ʽ

	// ��Դ����
protected:
	CD3DFont						m_D3DFontButton;					// ��ť����
	CD3DFreeType					m_D3DFreeTypeButton;				// ��ť����
	CD3DTexture						m_D3DTextureButton;					// ��ť����

	// ��������
public:
	// ���캯��
	CDirectUIButton();
	// ��������
	virtual ~CDirectUIButton();

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
	// ��������
	void SetButtonTexture( CD3DTexture& Texture );
	// ����λͼ
	void SetButtonTexture( CD3DDevice * pD3DDevice, hinstance hResInstance, const tchar * pszResource, const tchar * pszTypeName);
	// ��������
	void SetButtonTexture( CD3DDevice * pD3DDevice, hinstance hInstance, const tchar * pszResource, const tchar * pszTypeName, const tchar * pszImageKey );
	// ���ó�����Ӧ
	void SetButtonSustained( uint nBeginTime, uint nIntervalTime );
	// �����ı�
	void SetButtonText( const tchar * pszText );
	// �����ı���ɫ
	void SetButtonTextColor( D3DCOLOR nButtonTextColor );
	// ���ñ�����ɫ
	void SetButtonBackColor( D3DCOLOR nButtonTextColor );
	// �������ָ�ʽ
	void SetButtonTextAlign( EnumButtonTextAlign nButtonTextAlign );
	// ��������ƫ��
	void SetButtonTextOffset( CPoint ptTextOffset );

	// ��ȡ����
public:
	// ��ȡ����
	CD3DFont & GetFontButton();
	// ��ȡ����
	CD3DFreeType & GetFreeTypeButton();

	// �ڲ�����
protected:
	// �����ؼ�
	void RectifyDebug();

	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////////// 

namespace_dex_end

/////////////////////////////////////////////////////////////////////////////////

#endif