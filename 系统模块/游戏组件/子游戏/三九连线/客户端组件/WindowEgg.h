#pragma once

// ������Ϣ
#define TEXTRUE_BACK_MAX			17			// ��������
#define TEXTRUE_LIGHT_MAX			10			// ���⶯��

// ��ӵ�����
class CWindowEgg : public dex::CDirectUIWindow
{
	// ����Ϣ
public:
	CWHArray< int >						m_ArrayEggElement;				// ��ӵ�Ԫ��

	// ��������
public:
	int									m_nBackIndex;					// ��������
	int									m_nLightIndex;					// ��������
	dex::CLapseCount					m_nLCBackTime;					// ��������
	dex::CLapseCount					m_nLCLightTime;					// ��������
	
	// ������Դ
public:
	dex::CD3DTexture					m_TextureBack[TEXTRUE_BACK_MAX];	// ����ͼƬ
	dex::CD3DTexture					m_TextureLight[TEXTRUE_LIGHT_MAX];	// ����ͼƬ

	// �ຯ��
public:
	// ���캯��
	CWindowEgg();
	// ��������
	virtual ~CWindowEgg();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// ���ܺ���
public:
	// ������ӵ�����
	void SetEggElement( CWHArray< int > & ArrayEggElement );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
