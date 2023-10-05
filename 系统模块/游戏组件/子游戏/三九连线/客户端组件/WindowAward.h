#pragma once


// �н�����
class CWindowAward : public dex::CDirectUIWindow
{
	// ������Ϣ
public:
	EnumMultipleType			m_nMultipleType;										// �н�����
	int							m_nMultipleIndex;										// �н���Ϣ
	tchar						m_szPlayName[40];										// �������
	longlong					m_lAwardScore;											// �н�����

	// ��Դ��Ϣ
public:
	dex::CD3DFont				m_D3DFontWidth12;										// ������Ϣ
	dex::CD3DTexture			m_TextureNameBack;										// ���ֱ���
	dex::CD3DTexture			m_TextureLineBack[MultipleType_Max];					// ���ͱ���
	dex::CD3DTexture			m_TextureAwardBack;										// ���𱳾�
	dex::CD3DTexture			m_TextureAwardNumber;									// ��������
	dex::CD3DTexture			m_TextureElementNumber;									// Ԫ������
	dex::CD3DTexture			m_TextureElementMultip;									// Ԫ�س˺�

	// �ຯ��
public:
	// ���캯��
	CWindowAward();
	// ��������
	virtual ~CWindowAward();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ������� 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);

	// ���ܺ���
public:
	// ���û���Ϣ
	bool SetAwardInfo( const tchar * pszPlayName, int nMultipleIndex, longlong lAwardScore );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
