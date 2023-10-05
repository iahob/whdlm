#pragma once

// ��Ϣ����
class CWindowMessage : public dex::CDirectUIWindow
{
	// ������Ϣ
public:
	CString								m_StrMessage;

	// ��Դ����
public:
	dex::CD3DFont						m_D3DFont;
	dex::CD3DTexture					m_TextureBack;
	dex::CDirectUIButton				m_ButtonOK;
	
	// �ຯ��
public:
	// ���캯��
	CWindowMessage( const tchar * pszMessage );
	// ��������
	virtual ~CWindowMessage();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// ��Ϣ����
public:
	// �������� 
	void OnKeyDown(uint nChar, uint nRepCnt, uint nFlags);

	// ��ť��Ϣ
public:
	// ȷ����ť	
	void OnClickedOK();



	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()

};
