#pragma once
#include "ObjectMove.h"

// �궨��
#define ARRAY_GOLD_MAX				2			// �������
#define TEXTRUE_GOLD_MAX			10			// ���ͼƬ

// ������Ϣ
#define TEXTRUE_THREE_BACK			17			// ��������
#define TEXTRUE_THREE_STAR			15			// ��������

// ������Ϣ
#define TEXTRUE_NINE_BACK			12			// ��������
#define TEXTRUE_NINE_LOGO			2			// ����LOGO
#define TEXTRUE_NINE_LIGHT			5			// ����ɨ��

// ������Ϣ
struct tagWinNumberInfo
{
	int				nNumberIndex;					// ��������
	int				nImageFrame;					// ͼƬ֡��
	float			fNumberSize;					// ���ִ�С
	CDoublePoint	PointNumber;					// �滭λ��
};

// Ӯ����
class CWindowWin : public dex::CDirectUIWindow
{

	// ��������
public:
	int										m_nBackIndex;					// ��������
	int										m_nBackTime;					// ����ʱ��
	dex::CLapseCount						m_nLCBackTime;					// ��������
	int										m_nMultipleIndex;				// ��������
	EnumMultipleType						m_nMultipleType;				// ��������

	// ������Ϣ
public:
	int										m_nTaskTarget[TASK_INDEX_MAX];	// ����Ŀ��
	bool									m_bTaskComplete[TASK_INDEX_MAX];// �������

	// ��Ҷ���
public:
	bool									m_bHeldDrop;					// ���ֵ���
	int										m_nGoldCount;					// ��Ҷ���
	CWHArray< CGoldMove * >					m_ArrayGold[ARRAY_GOLD_MAX];	// ��Ҷ���
	CWHArray< tagWinNumberInfo >			m_ArrayNumberWin;				// Ӯ����
	longlong								m_lWinScore;					// Ӯ����
	longlong								m_lTaskScore;					// �������

	// �������
public:
	CWHArray< CDoublePoint >				m_ArrayPointFlash;				// �ƹ��
	CWHArray< CLightMove * >				m_ArrayFlash;					// �������
	dex::CLapseCount						m_nLCFlashTime;					// ��������

	// ��������
public:
	CWHArray< tagThreeStarMovie * >			m_ArrayThreeStarMovie;			// ��������

	// ��������
public:
	int										m_nNineLogoIndex;				// ������ʾ

	// ��Դ��Ϣ
public:
	// ����ͼƬ
	dex::CD3DTexture						m_TextureThreeBack[TEXTRUE_THREE_BACK];	// ��������
	dex::CD3DTexture						m_TextureThreeStar[TEXTRUE_THREE_STAR]; // ��������
	dex::CD3DTexture						m_TextureThreeRay;				// ��������

	// ����ͼƬ
	dex::CD3DTexture						m_TextureNineBack[TEXTRUE_NINE_BACK];	// ��������
	dex::CD3DTexture						m_TextureNineLogo[TEXTRUE_NINE_LOGO];	// ��������
	dex::CD3DTexture						m_TextureNineLight[TEXTRUE_NINE_LIGHT];	// ����ɨ��
	dex::CD3DTexture						m_TextureNineRay;				// ��������
	dex::CD3DTexture						m_TextureNineNumber;			// ��������

	// ͨ��ͼƬ
	dex::CD3DTexture						m_TextureGold[TEXTRUE_GOLD_MAX];// ���ͼƬ
	dex::CD3DTexture						m_TextureNumber1;				// ����ͼƬ
	dex::CD3DTexture						m_TextureNumber2;				// ����ͼƬ
	dex::CD3DTexture						m_TextureNumber3;				// ����ͼƬ
	dex::CD3DTexture						m_TextureNumber4;				// ����ͼƬ
	dex::CD3DTexture						m_TextureNumber5;				// ����ͼƬ
	dex::CD3DTexture						m_TextureComma1;				// ����ͼƬ
	dex::CD3DTexture						m_TextureComma2;				// ����ͼƬ
	dex::CD3DTexture						m_TextureComma3;				// ����ͼƬ
	dex::CD3DTexture						m_TextureComma4;				// ����ͼƬ
	dex::CD3DTexture						m_TextureLight;					// �ƹ�ͼƬ
	dex::CD3DTexture						m_TextureTaskLogo;				// �����ʶ
	dex::CD3DTexture						m_TextureTaskNumber;			// ��������

	// �ຯ��
public:
	// ���캯��
	CWindowWin();
	// ��������
	virtual ~CWindowWin();

	// ��Ϣ����
public:
	// ���ڴ��� 
	void OnWindowCreate( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// ���ڶ���
	void OnWindowMovie();
	// ���ڻ滭 
	void OnWindowRender( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );

	// �滭���
public:
	// �滭����
	void DrawScore( dex::CD3DDevice * pD3DDevice, uint nWidth, uint nHeight );
	// �滭����
	void DrawScore( dex::CD3DDevice * pD3DDevice, CWHArray< tagWinNumberInfo > & ArrayNumberInfo, dex::CD3DTexture * pImageNumber, dex::CD3DTexture * pImageComma, CPoint PointOffset );
	// �滭���
	void DrawGold( dex::CD3DDevice * pD3DDevice, CWHArray< CGoldMove * > & ArrayGold );

	// ���ƺ���
public:
	// ��ӽ��
	void AppendGold( bool bHeldDrop );
	// ��������Ŀ��
	void SetTaskTarget( int nTaskTarget[TASK_INDEX_MAX] );
	// ���ý��
	void SetWinScore( int nMultipleIndex, longlong lWinScore, longlong lTaskScore, bool bTaskComplete[TASK_INDEX_MAX] );

	// �ڲ�����
public:
	// ��������
	void GenerateNumber( CWHArray< tagWinNumberInfo > & ArrayNumberInfo, CPoint PointCenter, longlong lNumber, float fSize );

	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
