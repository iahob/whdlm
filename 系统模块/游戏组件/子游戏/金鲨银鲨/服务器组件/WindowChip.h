#pragma once

//�������
class CWindowChip : public CVirtualWindow
{
	//��Ϣ����
public:
	LONG								m_lCellScore;			//�׷�
	LONGLONG							m_lPlayOriginalScore;	//���ԭʼ����
	LONGLONG							m_lPlayScore;			//��һ���
	LONGLONG							m_lPlayChip;			//��ҳ���

	//�������
public:
	CD3DFont							m_D3DFontText;			//D3D����
	CD3DFont							m_D3DFontBold;			//D3D����

	//����ͼƬ
public:
	CD3DTexture							m_ImageBack;			//����ͼƬ

	CVirtualButton						m_btChip100;			//�һ�100
	CVirtualButton						m_btChip1000;			//�һ�1000
	CVirtualButton						m_btChip10000;			//�һ�10000
	CVirtualButton						m_btChipMax;			//���һ�
	CVirtualButton						m_btChipEmpty;			//��նһ�

	CVirtualButton						m_btOK;					//ȷ��
	CVirtualButton						m_btClose;				//�ر�
	CVirtualButton						m_btCloseX;				//�ر�X

	//���캯��
public:
	CWindowChip(void);
	~CWindowChip(void);

	//ϵͳ�¼�
protected:
	//������Ϣ
	virtual VOID OnWindowMovie();
	//������Ϣ
	virtual VOID OnWindowCreate(CD3DDevice * pD3DDevice);

	//���غ���
protected:
	//����¼�
	virtual VOID OnEventMouse(UINT uMessage, UINT nFlags, int nXMousePos, int nYMousePos);
	//��ť�¼�
	virtual VOID OnEventButton(UINT uButtonID, UINT uMessage, int nXMousePos, int nYMousePos);
	//�滭����
	virtual VOID OnEventDrawWindow(CD3DDevice * pD3DDevice, int nXOriginPos, int nYOriginPos);

	//���ڿ���
public:
	//��ʾ����
	VOID ShowWindow(bool bVisible);

	//���ܺ���
public:
	//������Ϣ
	VOID SetChipInfo( LONG lCellScore, LONGLONG	 lPlayScore, LONGLONG lPlayChip);
	//��ť����
	VOID ButtonEnable();
	//�滭�ַ�
	VOID DrawTextString(CD3DDevice* pD3DDevice, CD3DFont * pD3DFont,  LPCTSTR pszString, CRect rcDraw, UINT nFormat, COLORREF crText, COLORREF crFrame);


};
