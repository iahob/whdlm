#pragma once

// ��������
class CWindowHelp : public dex::CDirectUIWindow
{

	// ��Դ��Ϣ
public:
	dex::CD3DTexture						m_TextureBack;				// ����ͼƬ
	dex::CDirectUIButton					m_ButtonClose;				// �رհ�ť

	// �ຯ��
public:
	// ���캯��
	CWindowHelp();
	// ��������
	virtual ~CWindowHelp();

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
	// �ر�
	void OnClickedClose();


	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()

};
