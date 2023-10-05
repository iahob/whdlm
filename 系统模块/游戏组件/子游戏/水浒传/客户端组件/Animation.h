#pragma once

#define IDM_USER_ACTION_FINISH		(WM_USER+101)						//��������

//X ���з�ʽ
enum enXCollocateMode
{ 
	enXLeft,						//�����
	enXCenter,						//�ж���
	enXRight,						//�Ҷ���
};

//Y ���з�ʽ
enum enYCollocateMode 
{ 
	enYTop,							//�϶���
	enYCenter,						//�ж���
	enYBottom,						//�¶���
};

//��������ģʽ
enum enPlayMode 
{ 
	enStatic,						//��̬
	enOnce,							//һ��
	enCycle,						//ѭ��
};


class CAnimation
{
	bool							m_bSendFinish;						//֪ͨ���

protected:
	bool							m_bInit;							//����ͼƬ
	bool							m_bShowImage;						//��ʾͼƬ
	int								m_nImageCount;						//ͼƬ����
	int								m_nImageX;							//����ͼƬ
	int								m_nImageY;							//����ͼƬ

	//λ�ñ���
public:
	CPoint							m_BenchmarkPos;						//��׼λ��
	enXCollocateMode				m_XCollocateMode;					//��ʾģʽ
	enYCollocateMode				m_YCollocateMode;					//��ʾģʽ

	bool							m_bSan;								//��
	bool							m_bSanShow;							//����
	int								m_nLapseCount;						//ʱ����
	int								m_nLapseIndex;						//ʱ����
	int								m_nCurIndex;						//��ǰ����
	int								m_nMaxIndex;						//�������
	int								m_nSendIndex;						//֪ͨ����
	enPlayMode						m_enPlayMode;						//����ģʽ
	CD3DTextureEx *				m_pTextureImage;					//ͼƬָ��

public:
	CAnimation(void);
	~CAnimation(void);

	//ɾ��ͼƬ
	VOID DeleteImamge();
	//����ͼƬ
	bool IsInitImamge() { return m_bInit; }

	//���溯��
public:
	//��������
	bool CartoonMovie();
	//�����豸
	bool InitAnimation(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageCount, int nImagePageCount);
	//�����豸
	bool InitAnimationSingle(dex::CD3DDevice * pD3DDevice, CString strImageName, CString strImageKey, int nImageX, int nImageY);
	//�滭����
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice);
	//�滭����
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nDrawIndex);
	//�滭����
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);
	//�滭����
	VOID DrawAnimation(dex::CD3DDevice * pD3DDevice, INT nXPos, INT nYPos, INT nDrawIndex, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);

	//��������
	VOID ShowAnimation(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//��������
	VOID ShowAnimationEx(bool bShow, int nLapseCount = 1, enPlayMode PlayMode = enOnce);
	//ֹͣ����
	VOID StopAnimation();
	//ֹͣ����
	VOID StopAnimationSan(bool bSan) { m_bSan = bSan;};
	//֪ͨ����
	VOID SetSendIndex(int nSendIndex);
	//��ʾ����
	bool IsShow() { return m_bShowImage; }

	//��׼λ��
	VOID SetBenchmarkPos(INT nXPos, INT nYPos, enXCollocateMode XCollocateMode = enXCenter, enYCollocateMode YCollocateMode = enYCenter);

};
