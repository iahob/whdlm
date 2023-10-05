#pragma once

// �ߴ���
class CWindowLine : public dex::CDirectUIWindow
{
	// Ӯ����
public:
	uint							m_nMovieCount;							// ��������
	bool							m_bWinLine[LINE_INDEX_MAX];				// Ӯ��
	
	// ��Դ��Ϣ
public:
	dex::CD3DTexture				m_TextureLineWidth;						// ��
	dex::CD3DTexture				m_TextureLineHeight;					// ��
	dex::CD3DTexture				m_TextureLineOblique1;					// б1
	dex::CD3DTexture				m_TextureLineOblique2;					// б2

	// �ຯ��
public:
	// ���캯��
	CWindowLine();
	// ��������
	virtual ~CWindowLine();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// ������Ϣ
public:
	// ����Ӯ��
	void SetWinLine( bool bWinLine[LINE_INDEX_MAX] );
	// ���Ӯ��
	void ClearWinLine();

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()

};
