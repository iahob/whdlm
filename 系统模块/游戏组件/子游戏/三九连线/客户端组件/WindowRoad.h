#pragma once

// ·������
class CWindowRoad : public dex::CDirectUIWindow
{
	// ������Ϣ
public:
	CWHArray< int64 >						m_ArrayRoad;					// ·����Ϣ
	
	// ��Դ��Ϣ
public:
	dex::CD3DFont							m_D3DFont;						// ������Ϣ
	dex::CD3DTexture						m_TextureBack;					// ����ͼƬ
	dex::CD3DTexture						m_TextureElementNumber;			// Ԫ������
	dex::CD3DTexture						m_TextureElementMultip;			// Ԫ�س˺�
	dex::CDirectUIButton					m_ButtonClose;					// �رհ�ť

	// �ຯ��
public:
	// ���캯��
	CWindowRoad();
	// ��������
	virtual ~CWindowRoad();

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
	// ������� 
	void OnLButtonDown( dex::CD3DDevice * pD3DDevice, uint nFlags, CPoint ptMouseSite);


	// ��Ϣ����
public:
	// �ر�
	void OnClickedClose();

	// ������Ϣ
public:
	// ���·��
	void AppendRoad( int nMultipleIndex );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()

};
