#pragma once

// Ԫ���ƶ�
struct tagElementMove
{
	int					nElementIndex;			// Ԫ������
	CPoint				PointSite;				// λ����Ϣ
};

// ת�̴���
class CWindowTurntable : public dex::CDirectUIWindow
{

	// ������Ϣ
public:
	bool										m_bSpeedUp;														// ������ʾ
	bool										m_bElementMove;													// Ԫ���ƶ�
	bool										m_nKeepTurn;													// ������ת
	bool										m_nAcceptedElement;												// ����Ԫ��
	int											m_nElementMoveCount[TURN_WIDTH_COUNT];							// �ƶ�����
	int											m_nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];			// �̶�Ԫ��

	// ��ʾ����
public:
	bool										m_bLinePrompt;													// ������ʾ
	bool										m_bNineLinePrompt;												// 9����ʾ
	bool										m_bThreeLinePrompt[LINE_INDEX_MAX];								// 3����ʾ
	bool										m_bThreeLineElement[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// 3����ʾ

	// �ο�����
public:
	CSize										m_SizeElementTexture;											// Ԫ��ͼƬ��С
	CSize										m_SizeElementArea;												// Ԫ�������С
	CPoint										m_PointDatumElement[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT];		// Ԫ�ػ�׼λ��

	// ��������
public:
	CWHArray< tagElementMove * >				m_TElementMoveInfo[TURN_WIDTH_COUNT];		// Ԫ�ض���

	// ��Դ��Ϣ
public:
	dex::CLapseCount							m_LCSoundTime;								// ��������
	dex::CD3DTexture							m_TextureElement[Element_Max];

	// �ຯ��
public:
	// ���캯��
	CWindowTurntable();
	// ��������
	virtual ~CWindowTurntable();

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

	// ���ܺ���
public:
	// ת��Ԫ��
	void TurnElement();
	// ֹͣԪ��
	void StopElement( int nElementIndex[TURN_WIDTH_COUNT][TURN_HEIGHT_COUNT], bool bStopNew );
	// ������ʾ
	bool LinePrompt();
	// ǿ��ֹͣԪ��
	bool ForcedStopElement();
	// ���ü���
	void SetSpeedUp(bool bSpeedUp) { m_bSpeedUp = bSpeedUp; }

	// �ڲ�����
public:
	// ����ͼƬλ��
	CPoint PointTextureElement( CPoint PointElement );
	// ��ն���
	void EmptyElementMove();


	// ������Ϣ
public:
	// ������Ϣ
	DECLARE_WHDUI_MESSAGE_MAP()
};
